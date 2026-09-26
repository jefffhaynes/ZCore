#pragma once

#include "InputStream.h"

class BoundedInputStream : public InputStream
{
public:
    constexpr BoundedInputStream(InputStream& stream, uint32_t length) : _stream(stream), _length(length), _read(0)
    {
    }

    constexpr ReturnCode Read(Span<uint8_t> data, uint32_t& read) override
    {
        auto remaining = GetAvailable();
        auto readLength = std::min(remaining, data.GetLength());
        auto readBlock = data.Take(readLength);

        auto rc = _stream.Read(readBlock, read);
        CHECK_RETURN_CODE(rc);

        _read += read;

        return ReturnCode::Success;
    }

    using InputStream::Read;

    constexpr uint32_t GetAvailable()
    {
        return _length - _read;
    }

    constexpr bool IsEndOfStream() override
    {
        return GetAvailable() == 0 || _stream.IsEndOfStream();
    }

private:
    InputStream& _stream;
    uint32_t _length;
    uint32_t _read;
};
