#pragma once

#include "CoreMath.h"
#include <cmath>
#include <limits>

template <auto Start, auto End, auto Inc, class F>
constexpr bool constexpr_for(F&& f)
{
    if constexpr (Start < End)
    {
        if (!f(std::integral_constant<decltype(Start), Start>()))
            return false;  // Early exit on failure
        return constexpr_for<Start + Inc, End, Inc>(f);
    }
    return true;  // Success if loop completes fully
}

template <typename T>
constexpr bool AreAlmostEqual(T a, T b, T epsilon)
{
    return CoreMath::Abs(a - b) < epsilon;
}

constexpr bool AreAlmostEqual(float a, float b)
{
    return CoreMath::Abs(a - b) < 0.0001f;
}
