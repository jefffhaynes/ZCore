#pragma once

#include "InputStream.h"
#include "MemoryMarshal.h"

class StreamReader
{
public:
    StreamReader(InputStream& stream) : _stream(stream)
    {
    }

    template <typename T>
    ReturnCode Read(T& value, TimeSpan timeout)
    {
        auto data = MemoryMarshal::AsBytes(value);
        return Read(data, timeout);
    }
    
    template <typename T>
    ReturnCode Read(T& value)
    {
        auto data = MemoryMarshal::AsBytes(value);
        return Read(data);
    }

    ReturnCode Read(Span<uint8_t> data, TimeSpan timeout)
    {
        return _stream.Read(data, timeout);
    }

    ReturnCode Read(Span<uint8_t> data)
    {
        return _stream.Read(data);
    }

private:
    InputStream& _stream;
};