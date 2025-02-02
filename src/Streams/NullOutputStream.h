#pragma once

#include "OutputStream.h"

class NullOutputStream : public OutputStream
{
public:
    constexpr ReturnCode Write(Span<const uint8_t> data) override
    {
        return ReturnCode::Success;
    }
};