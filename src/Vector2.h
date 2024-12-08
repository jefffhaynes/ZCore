#pragma once

#include <cmath>

template<typename T>
class Vector2
{
public:
    constexpr Vector2(T x, T y) : X(x), Y(y)
    {
    }

    constexpr Vector2()
    {
    }

    T X = { };
    T Y = { };

    constexpr bool operator==(const Vector2& other) const
    {
        return X == other.X && Y == other.Y;
    }

    constexpr Vector2 operator+(const Vector2& other) const
    {
        return { X + other.X, Y + other.Y };
    }

    constexpr Vector2 operator-(const Vector2& other) const
    {
        return { X - other.X, Y - other.Y };
    }

    constexpr Vector2 operator*(T value) const
    {
        return { X * value, Y * value };
    }

    constexpr Vector2 operator/(T value) const
    {
        return { X / value, Y / value };
    }

    static const Vector2<T> Zero;
};

template<typename T>
constexpr Vector2<T> Vector2<T>::Zero = Vector2<T>();