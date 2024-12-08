#pragma once

#include "OutputStream.h"

class NullOutputStream : public OutputStream
{
public:
    ReturnCode Write(Span<const uint8_t> data) override
    {
        return ReturnCode::Success;
    }
};