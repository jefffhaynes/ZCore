#pragma once

#include <Streams/OutputStream.h>
#include <Streams/InputStream.h>

class MemoryStream : public InputStream, public OutputStream
{
public:
    constexpr MemoryStream(Span<uint8_t> span) : _span(span), _offset(0)
    {
    }

    constexpr ReturnCode Write(Span<const uint8_t> data) override
    {
        auto rc = data.CopyTo(GetRemaining());
        CHECK_RETURN_CODE(rc);

        _offset += data.GetLength();

        return ReturnCode::Success;
    }

    constexpr ReturnCode Read(Span<uint8_t> data, uint32_t& read) override
    {
        auto readBlock = GetRemaining().Take(data.GetLength());
        auto rc = readBlock.CopyTo(data);
        CHECK_RETURN_CODE(rc);

        read = readBlock.GetLength();
        _offset += read;

        return ReturnCode::Success;
    }

private:
    Span<uint8_t> _span;
    uint32_t _offset;

    constexpr Span<uint8_t> GetRemaining()
    {
        return _span.Skip(_offset);
    }
};
