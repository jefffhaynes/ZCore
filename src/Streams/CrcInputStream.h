#pragma once

#include <Streams/InputStream.h>
#include <ICrc.h>

class CrcInputStream : public InputStream
{
public:
    constexpr CrcInputStream(InputStream& stream, ICrc& crc) : _stream(stream), _crc(crc)
    {
    }

    constexpr ReturnCode Read(Span<uint8_t> data, uint32_t& read) override
    {
        auto rc = _stream.Read(data, read);
        _crc.Update(data.Take(read));
        return rc;
    }

private:
    InputStream& _stream;
    ICrc& _crc;
};
