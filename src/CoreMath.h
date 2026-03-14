#pragma once

#include <stdint.h>

// here because we need constexpr for unit tests
// switch to std when available

class CoreMath
{
public:
    template <typename T>
    static constexpr T Abs(T value)
    {
        T zero = value * 0;
        return value < zero ? -value : value;
    }

    static constexpr float Exp(float x)
    {
        const int terms = 12;

        // this threshold is based on the number of terms in the series
        // and the epsilon value used to calculate the series
        if (x < -3.0f)
        {
            return 0;
        }

        auto sum = 1.0f;

        for (int k = terms; k > 0; --k) 
        {
            sum = 1.0f + (x / static_cast<float>(k)) * sum;
        }

        return sum;
    }

    template<typename T>
    static constexpr T Clamp(T value, T min, T max)
    {
        return value < min ? min : (value > max ? max : value);
    }

    static constexpr bool IsPowerOfTwo(uint32_t value)
    {
        return value != 0 && (value & (value - 1)) == 0;
    }
};

