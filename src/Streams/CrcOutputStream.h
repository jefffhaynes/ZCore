#pragma once

#include <Streams/OutputStream.h>
#include <ICrc.h>

class CrcOutputStream : public OutputStream
{
public:
    constexpr CrcOutputStream(OutputStream& stream, ICrc& crc) : _stream(stream), _crc(crc)
    {
    }

    constexpr ReturnCode Write(Span<const uint8_t> data) override
    {
        _crc.Update(data);
        return _stream.Write(data);
    }

private:
    OutputStream& _stream;
    ICrc& _crc;
};