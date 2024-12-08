#pragma once

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

// Helper constexpr function to compare floating point values
constexpr bool AreAlmostEqual(float a, float b, float epsilon = 0.0001)
{
    return std::abs(a - b) < epsilon;
}
