#pragma once

#include <Vector2.h>
#include <Vector3.h>

template<typename T>
class Quaternion
{
public:
    constexpr Quaternion(T x, T y, T z, T w) : X(x), Y(y), Z(z), W(w)
    {
    }

    constexpr Quaternion()
    {
    }

    T X = { };
    T Y = { };
    T Z = { };
    T W = { };

    constexpr bool operator==(const Quaternion& other) const
    {
        return X == other.X && Y == other.Y && Z == other.Z && W == other.W;
    }

    constexpr Quaternion operator+(const Quaternion& other) const
    {
        return { X + other.X, Y + other.Y, Z + other.Z, W + other.W };
    }

    constexpr Quaternion operator-(const Quaternion& other) const
    {
        return { X - other.X, Y - other.Y, Z - other.Z, W - other.W };
    }

    constexpr Quaternion operator*(T value) const
    {
        return { X * value, Y * value, Z * value, W * value };
    }

    constexpr Quaternion operator/(T value) const
    {
        return { X / value, Y / value, Z / value, W / value };
    }

    constexpr Quaternion operator*(const Quaternion& other) const
    {
        return { W * other.X + X * other.W + Y * other.Z - Z * other.Y,
                 W * other.Y - X * other.Z + Y * other.W + Z * other.X,
                 W * other.Z + X * other.Y - Y * other.X + Z * other.W,
                 W * other.W - X * other.X - Y * other.Y - Z * other.Z };
    }

    constexpr Quaternion Conjugate() const
    {
        return { -X, -Y, -Z, W };
    }

    constexpr T Dot(const Quaternion& other) const
    {
        return X * other.X + Y * other.Y + Z * other.Z + W * other.W;
    }

    constexpr Quaternion Normalize() const
    {
        const auto length = std::sqrt(X * X + Y * Y + Z * Z + W * W);
        return { X / length, Y / length, Z / length, W / length };
    }

    constexpr Quaternion Inverse() const
    {
        return Conjugate() / Dot(*this);
    }

    constexpr Vector3<T> Rotate(const Vector3<T>& vector) const
    {
        const auto q = *this * Quaternion(vector.X, vector.Y, vector.Z, 0) * Inverse();
        return { q.X, q.Y, q.Z };
    }

    constexpr Vector2<T> Rotate(const Vector2<T>& vector) const
    {
        const auto q = *this * Quaternion(vector.X, vector.Y, 0, 0) * Inverse();
        return { q.X, q.Y };
    }
};