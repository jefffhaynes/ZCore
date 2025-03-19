#pragma once

#include <Span.h>

class ICrc
{
public:
    virtual void Reset() = 0;
    virtual void Update(uint8_t data) = 0;
    virtual void Update(Span<const uint8_t> data) = 0;
    virtual constexpr ~ICrc() = default;
};
