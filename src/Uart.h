#pragma once

#include <Device.h>
#include <Streams/OutputStream.h>
#include <Streams/InputStreamWithDataReceived.h>
#include <ErrorConverter.h>
#include <Queue.h>
#include <EventHandler.h>
#include <Work.h>
#include <CriticalSection.h>

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

// #pragma GCC push_options
// #pragma GCC optimize ("O0")

class Uart : public Device, public OutputStream, public InputStreamWithDataReceived
{
public:
    Uart(const struct device* device) : Device(device), _rxQueue(_rxBuffer), _txQueue(_txBuffer)
    {
    }

    ReturnCode Initialize()
    {
        if (_initialized)
        {
            return ReturnCode::Success;
        }

        auto rc = OnInitialize();
        CHECK_RETURN_CODE(rc);

        auto error = uart_irq_callback_user_data_set(GetDevice(), OnInterrupt, this);
        rc = ErrorConverter::Convert(error);
        CHECK_RETURN_CODE(rc);

        rc = _work.Initialize();
        CHECK_RETURN_CODE(rc);

        rc = _work.Worker.Subscribe<Uart, &Uart::OnWork>(this);
        CHECK_RETURN_CODE(rc);

        uart_irq_rx_enable(GetDevice());

        _initialized = true;

        return ReturnCode::Success;
    }

    ReturnCode SetBaudRate(uint32_t baudRate)
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);
        
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
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        CriticalSection cs;
        return _rxQueue.Dequeue(data, read);
    }

    using InputStream::Read;

    ReturnCode Write(Span<const uint8_t> data) override
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        CriticalSection cs;
        rc = _txQueue.Enqueue(data);
        CHECK_RETURN_CODE(rc);

        uart_irq_tx_enable(GetDevice());

        return ReturnCode::Success;
    }

    constexpr uint32_t GetAvailable() override
    {
        return _rxQueue.GetCount();
    }

protected:
    virtual ReturnCode OnInitialize()
    {
        return ReturnCode::Success;
    }

private:
    bool _initialized = false;
    Array<uint8_t, 512> _rxBuffer;
    Array<uint8_t, 512> _txBuffer;
    Queue<uint8_t> _rxQueue;
    Queue<uint8_t> _txQueue;
    Work _work;


    void OnInterrupt()
    {
        if (!uart_irq_update(GetDevice()))
        {
            return;
        }
    
        if (uart_irq_rx_ready(GetDevice()))
        {
            Array<uint8_t, 64> buffer;
            auto read = uart_fifo_read(GetDevice(), buffer.GetData(), buffer.GetLength());
            
            if (read > 0)
            {
                auto data = buffer.Take(read);
                auto rc = _rxQueue.Enqueue(data);
                Debug::WriteIfError(rc);
                _work.Run();
            }
        }

        if (uart_irq_tx_ready(GetDevice()))
        {
            Array<uint8_t, 32> buffer;
            uint32_t read;
            auto rc = _txQueue.Dequeue(buffer, read);
            if (rc == ReturnCode::Success)
            {
                auto err = uart_fifo_fill(GetDevice(), buffer.GetData(), read);
                rc = ErrorConverter::Convert(err);

                if (rc != ReturnCode::Success)
                {
                    _txQueue.Enqueue(buffer.Take(read));
                }

                if (_txQueue.IsEmpty())
                {
                    uart_irq_tx_disable(GetDevice());
                }
            }
        }
    }

    ReturnCode OnWork()
    {
        return _rxQueue.GetCount() > 0 ? DataReceived.Invoke() : ReturnCode::Success;
    }

    static void OnInterrupt(const struct device *device, void *ctx)
    {
        auto uart = static_cast<Uart*>(ctx);
        uart->OnInterrupt();
    }
};

// #pragma GCC pop_options