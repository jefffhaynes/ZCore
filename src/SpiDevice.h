#pragma once

#include <zephyr/drivers/spi.h>
#include "Span.h"
#include "ErrorConverter.h"
#include "Task.h"

class SpiDevice 
{
public:
    SpiDevice(const spi_dt_spec spec) : _spec(spec)
    {
    }

    bool IsReady()
    {
        return spi_is_ready_dt(&_spec);
    }

    ReturnCode Transfer(Span<const uint8_t> txBuffer, Span<uint8_t> rxBuffer)
    {
        struct spi_buf txBuf = {
            .buf = const_cast<uint8_t*>(txBuffer.GetData()),
            .len = txBuffer.GetLength()
        };

        struct spi_buf rxBuf = {
            .buf = rxBuffer.GetData(),
            .len = rxBuffer.GetLength()
        };

        struct spi_buf_set txBufSet = {
            .buffers = &txBuf,
            .count = 1
        };

        struct spi_buf_set rxBufSet = {
            .buffers = &rxBuf,
            .count = 1
        };

        auto err = spi_transceive_dt(&_spec, &txBufSet, &rxBufSet);
        return ErrorConverter::Convert(err);
    }

    Task TransferAsync(Span<const uint8_t> txBuffer, Span<uint8_t> rxBuffer)
    {
#if defined(CONFIG_SPI_ASYNC)
        struct spi_buf txBuf = 
        {
            .buf = const_cast<uint8_t*>(txBuffer.GetData()),
            .len = txBuffer.GetLength()
        };

        struct spi_buf rxBuf = 
        {
            .buf = rxBuffer.GetData(),
            .len = rxBuffer.GetLength()
        };

        struct spi_buf_set txBufSet = 
        {
            .buffers = &txBuf,
            .count = 1
        };

        struct spi_buf_set rxBufSet = 
        {
            .buffers = &rxBuf,
            .count = 1
        };

        Task task;
        TaskStateHandle handle;
        auto rc = _tcs.GetTask(task, handle);
        CHECK_RETURN_CODE(rc);

        auto err = spi_transceive_cb(_spec.bus, &_spec.config, &txBufSet, &rxBufSet, OnTransferComplete, &handle);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        return task;
#else
        return ReturnCode::NotSupported;
#endif
    }

private:
    const struct spi_dt_spec _spec;
    TaskCompletionSource<> _tcs;

    static void OnTransferComplete(const struct device *dev, int result, void *data)
    {
        auto handle = static_cast<TaskStateHandle*>(data);
        auto state = handle->Get();
        state.SetComplete(ErrorConverter::Convert(result));
    }
};