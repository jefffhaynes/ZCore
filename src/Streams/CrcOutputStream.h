#pragma once

#include <Streams/OutputStream.h>
#include <ICrc.h>

class CrcOutputStream : public OutputStream
{
public:
    constexpr CrcOutputStream(ICrc& crc) : _crc(crc)
    {
    }

    constexpr ReturnCode Write(Span<const uint8_t> data) override
    {
        _crc.Update(data);
        return ReturnCode::Success;
    }

private:
    ICrc& _crc;
};