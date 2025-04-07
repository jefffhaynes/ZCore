#pragma once

#include "OutputStream.h"

class ForkedOutputStream : public OutputStream
{
public:
    constexpr ForkedOutputStream(OutputStream& stream, OutputStream& tap) : _stream(stream), _tap(tap)
    {
    }

    ReturnCode Write(Span<const uint8_t> data) override
    {
        auto rc = _stream.Write(data);
        CHECK_RETURN_CODE(rc);

        return _tap.Write(data);
    }

private:
    OutputStream& _stream;
    OutputStream& _tap;
};
