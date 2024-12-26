#pragma once

#include "OutputStream.h"
#include "MemoryMarshal.h"

class StreamWriter
{
public:
    StreamWriter(OutputStream& stream) : _stream(stream)
    {
    }

    template <Safe T>
    ReturnCode Write(const T& value)
    {
        auto data = MemoryMarshal::AsConstBytes(value);
        return Write(data);
    }

    ReturnCode Write(Span<const uint8_t> data)
    {
        return _stream.Write(data);
    }

private:
    OutputStream& _stream;
};
