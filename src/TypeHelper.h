#pragma once

class TypeHelper
{
public:
    template<typename T>
    struct IsArithmeticOrEnum : std::integral_constant<bool, std::is_arithmetic_v<T> || std::is_enum_v<T>> {};
};