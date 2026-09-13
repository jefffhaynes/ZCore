#pragma once

#include "Device.h"
#include "Array.h"
#include "MemoryMarshal.h"
#include "ErrorConverter.h"

#include <zephyr/drivers/i2c.h>

class I2CDevice : public Device
{
public:
    I2CDevice(const struct device* device, uint8_t address) : Device(device), _address(address)
    {
    }

    ReturnCode Read(Span<uint8_t> buffer, bool stop = true) const
    {
        struct i2c_msg msg;

        msg.buf = buffer.GetData();
        msg.len = buffer.GetLength();
        msg.flags = I2C_MSG_READ;

        if(stop)
        {
            msg.flags |= I2C_MSG_STOP;
        }

        auto err = i2c_transfer(GetDevice(), &msg, 1, _address);
        return ErrorConverter::Convert(err);
    }

    ReturnCode Write(Span<const uint8_t> buffer, bool stop = true) const
    {
        struct i2c_msg msg;

        msg.buf = const_cast<uint8_t*>(buffer.GetData());
        msg.len = buffer.GetLength();
        msg.flags = I2C_MSG_WRITE;

        if(stop)
        {
            msg.flags |= I2C_MSG_STOP;
        }

        auto err = i2c_transfer(GetDevice(), &msg, 1, _address);
        return ErrorConverter::Convert(err);
    }

    ReturnCode Read(uint8_t reg, Span<uint8_t> buffer) const
    {
        auto rc = Write(reg);
        CHECK_RETURN_CODE(rc);

        return Read(buffer);
    }

    // Register write: the register address followed by `buffer`, in one
    // transaction. Payloads up to BufferSize - 1 bytes.
    ReturnCode Write(uint8_t reg, Span<const uint8_t> buffer) const
    {
        Array<uint8_t, BufferSize> addressAndBuffer;

        auto rc = addressAndBuffer.Set(0, reg);
        CHECK_RETURN_CODE(rc);

        // CopyTo rejects a payload that doesn't fit after the address byte.
        auto span = addressAndBuffer.Skip(1);
        rc = buffer.CopyTo(span);
        CHECK_RETURN_CODE(rc);

        // Only the address and the payload, not the whole scratch array.
        return Write(addressAndBuffer.Take(1 + buffer.GetLength()));
    }

    ReturnCode Read(uint8_t reg, uint8_t& value) const
    {
        auto err = i2c_reg_read_byte(GetDevice(), _address, reg, &value);
        return ErrorConverter::Convert(err);
    }

    ReturnCode Write(uint8_t reg, uint8_t value) const
    {
        auto err = i2c_reg_write_byte(GetDevice(), _address, reg, value);
        return ErrorConverter::Convert(err);
    }

    ReturnCode Write(uint8_t value) const
    {
        auto data = MemoryMarshal::AsConstBytes(value);
        return Write(data);
    }

    ReturnCode BurstRead(uint8_t reg, Span<uint8_t> buffer) const
    {
        auto err = i2c_burst_read(GetDevice(), _address, reg, buffer.GetData(), buffer.GetLength());
        return ErrorConverter::Convert(err);
    }

private:
    static const uint32_t BufferSize = 32;
    uint8_t _address;
};
