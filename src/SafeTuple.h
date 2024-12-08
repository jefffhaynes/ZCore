#pragma once

#include <tuple>
#include "TypeHelper.h"

template<typename ...TArgs>
class SafeTuple : public std::tuple<TArgs...>
{
public:
    static_assert(sizeof...(TArgs) == 0 || (TypeHelper::IsArithmeticOrEnum<TArgs>::value && ...), "All arguments must be arithmetic or enums");


    // constructors
    constexpr SafeTuple() noexcept : std::tuple<TArgs...>() {}
    constexpr SafeTuple(const TArgs&... args) : std::tuple<TArgs...>(args...) {}
    constexpr SafeTuple(const SafeTuple&) = default;
    constexpr SafeTuple(SafeTuple&&) = default;

    // assignment operators
    constexpr SafeTuple& operator=(const SafeTuple&) = default;
};