#pragma once

#include <Device.h>
#include <Streams/OutputStream.h>
#include <ErrorConverter.h>
#include <Queue.h>
#include <EventHandler.h>
#include <Work.h>
#include <InterruptLock.h>

#include <zephyr/drivers/uart.h>

// used to set the baud rate
struct uarte_nrfx_config2 {
	NRF_UARTE_Type *uarte_regs; /* Instance address */
	uint32_t flags;
	bool disable_rx;
	const struct pinctrl_dev_config *pcfg;
#ifdef UARTE_ANY_ASYNC
	nrfx_timer_t timer;
#endif
};


class Uart : public Device, public OutputStream, public InputStreamWithDataReceived
{
public:
    Uart(const struct device* device) : Device(device), _queue(_buffer)
    {
    }

    virtual ReturnCode Initialize()
    {
        auto error = uart_irq_callback_user_data_set(GetDevice(), OnInterrupt, this);
        auto rc = ErrorConverter::Convert(error);
        CHECK_RETURN_CODE(rc);

        rc = _work.Initialize();
        CHECK_RETURN_CODE(rc);

        rc = _work.Worker.Subscribe<Uart, &Uart::OnWork>(this);
        CHECK_RETURN_CODE(rc);

        uart_irq_rx_enable(GetDevice());

        return ReturnCode::Success;
    }

    ReturnCode SetBaudRate(uint32_t baudRate)
    {
        auto* device = GetDevice();
        auto* config = static_cast<const struct uarte_nrfx_config2*>(device->config);
        auto* reg = static_cast<NRF_UARTE_Type*>(config->uarte_regs);

        uint64_t baseValue = static_cast<uint64_t>(baudRate) * (1ULL << 32) / 16000000;

        // Round the result to the nearest valid register boundary
        uint32_t roundedValue = (baseValue + 0x800) & 0xFFFFF000;

        reg->BAUDRATE = roundedValue;

        return ReturnCode::Success;
    }

    ReturnCode Read(Span<uint8_t> data, uint32_t& read) override
    {
        InterruptLock lock;
        return _queue.Dequeue(data, read);
    }

    using InputStream::Read;

    ReturnCode Write(Span<const uint8_t> data) override
    {
        for (auto byte : data)
        {
            uart_poll_out(GetDevice(), byte);
        }
        
        // auto err = uart_fifo_fill(GetDevice(), data.GetData(), data.GetLength());
        // return ErrorConverter::Convert(err);

        return ReturnCode::Success;
    }

    constexpr uint32_t GetAvailable()
    {
        return _queue.GetCount();
    }

private:
    Array<uint8_t, 512> _buffer;
    Queue<uint8_t> _queue;
    Work _work;


    void OnInterrupt()
    {
        Array<uint8_t, 64> buffer;
        auto read = uart_fifo_read(GetDevice(), buffer.GetData(), buffer.GetLength());
        
        if (read > 0)
        {
            auto data = buffer.Take(read);
            _queue.Enqueue(data);
            _work.Run();
        }
    }

    ReturnCode OnWork()
    {
        return _queue.GetCount() > 0 ? DataReceived.Invoke() : ReturnCode::Success;
    }

    static void OnInterrupt(const struct device *dev, void *user_data)
    {
        auto usb_dev = static_cast<Uart*>(user_data);
        usb_dev->OnInterrupt();
    }
};
