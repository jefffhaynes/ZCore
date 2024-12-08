#pragma once

#include "InputStream.h"

class NullInputStream : public InputStream
{
public:
    ReturnCode Read(Span<uint8_t> data, uint32_t& read) override
    {
        read = 0;
        return ReturnCode::Success;
    }
};