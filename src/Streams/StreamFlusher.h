#pragma once

#include "OutputStream.h"

class StreamFlusher
{
public:
    StreamFlusher(OutputStream& stream) : _stream(stream)
    {
    }

    ~StreamFlusher()
    {
        _stream.Flush();
    }

private:
    OutputStream& _stream;
};
