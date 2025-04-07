#pragma once

#include "InputStream.h"
#include "OutputStream.h"

class ForkedInputStream : public InputStream
{
public:
    constexpr ForkedInputStream(InputStream& stream, OutputStream& tap) : _stream(stream), _tap(tap)
    {
    }

    ReturnCode Read(Span<uint8_t> data, uint32_t& read) override
    {
        auto rc = _stream.Read(data, read);
        CHECK_RETURN_CODE(rc);

        return _tap.Write(data.Take(read));
    }

private:
    InputStream& _stream;
    OutputStream& _tap;
};
