#pragma once

#include "InputStream.h"
#include "MemoryMarshal.h"
#include "NullOutputStream.h"

class StreamReader
{
public:
    StreamReader(InputStream& stream) : _stream(stream)
    {
    }

    template <Safe T>
    ReturnCode Read(T& value, TimeSpan timeout)
    {
        auto data = MemoryMarshal::AsBytes(value);
        return Read(data, timeout);
    }
    
    template <Safe T>
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

    ReturnCode Advance(uint32_t length, TimeSpan timeout)
    {
        NullOutputStream nullSink;
        return _stream.CopyTo(nullSink, length, timeout);
    }

    ReturnCode AdvanceToEnd()
    {
        NullOutputStream nullSink;
        return _stream.CopyTo(nullSink);
    }

private:
    InputStream& _stream;
};