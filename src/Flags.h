#pragma once

#include <type_traits>

class Flags
{
public:
    template<typename T>
    static constexpr bool HasFlag(T flags, T flag)
    {
        using UT = std::underlying_type_t<T>;
        return (static_cast<UT>(flags) & static_cast<UT>(flag)) != 0;
    }

    template<typename T>
    static constexpr T Combine(T a, T b)
    {
        using UT = std::underlying_type_t<T>;
        return static_cast<T>(static_cast<UT>(a) | static_cast<UT>(b));
    }
};