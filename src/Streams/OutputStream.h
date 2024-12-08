#pragma once

#include "Span.h"

class OutputStream
{
public:
    virtual ReturnCode Write(Span<const uint8_t> data) = 0;
    
    virtual ReturnCode Flush()
    {
        return ReturnCode::Success;
    }
};