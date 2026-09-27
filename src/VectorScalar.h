#pragma once

// What a vector scales by: its element type, or a unit's value type (e.g. Temperature).
template<typename T>
struct VectorScalar
{
    using Type = T;
};

template<typename T> requires requires { typename T::ValueType; }
struct VectorScalar<T>
{
    using Type = typename T::ValueType;
};
