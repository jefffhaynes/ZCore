#pragma once

#include <CoreMath.h>

template<typename T, uint32_t Alignment = 4>
struct Aligned
{
    static_assert(CoreMath::IsPowerOfTwo(Alignment), "Alignment must be a power of two");
    
    alignas(Alignment) T Value;
};