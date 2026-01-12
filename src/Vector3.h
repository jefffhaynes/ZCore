#pragma once

#include <Units/Angle.h>
#include <cmath>

template<typename T>
struct Vector3
{
    constexpr Vector3(T x, T y, T z) : X(x), Y(y), Z(z)
    {
    }

    constexpr Vector3()
    {
    }

    T X = { };
    T Y = { };
    T Z = { };

    constexpr bool operator==(const Vector3& other) const
    {
        return X == other.X && Y == other.Y && Z == other.Z;
    }

    constexpr Vector3 operator+(const Vector3& other) const
    {
        return { X + other.X, Y + other.Y, Z + other.Z };
    }

    constexpr Vector3 operator-(const Vector3& other) const
    {
        return { X - other.X, Y - other.Y, Z - other.Z };
    }

    constexpr Vector3 operator*(float value) const
    {
        return { X * value, Y * value, Z * value };
    }

    constexpr Vector3 operator*(int value) const
    {
        return { X * value, Y * value, Z * value };
    }

    constexpr Vector3 operator/(float value) const
    {
        return { X / value, Y / value, Z / value };
    }

    constexpr Vector3 operator/(int value) const
    {
        return { X / value, Y / value, Z / value };
    }

    constexpr T Dot(const Vector3& other) const
    {
        return X * other.X + Y * other.Y + Z * other.Z;
    }

    constexpr Vector3 Cross(const Vector3& other) const
    {
        return { Y * other.Z - Z * other.Y, Z * other.X - X * other.Z, X * other.Y - Y * other.X };
    }

    constexpr Vector3 Rotate(const Angle& angle, const Vector3& axis) const
    {
        const auto s = std::sin(angle.ToRadians());
        const auto c = std::cos(angle.ToRadians());
        const auto t = 1 - c;

        const auto x = X * (t * axis.X * axis.X + c) + Y * (t * axis.X * axis.Y - s * axis.Z) + Z * (t * axis.X * axis.Z + s * axis.Y);
        const auto y = X * (t * axis.X * axis.Y + s * axis.Z) + Y * (t * axis.Y * axis.Y + c) + Z * (t * axis.Y * axis.Z - s * axis.X);
        const auto z = X * (t * axis.X * axis.Z - s * axis.Y) + Y * (t * axis.Y * axis.Z + s * axis.X) + Z * (t * axis.Z * axis.Z + c);

        return { x, y, z };
    }

    constexpr Vector3 RotateX(const Angle& angle) const
    {
        return Rotate(angle, XAxis);
    }

    constexpr Vector3 RotateY(const Angle& angle) const
    {
        return Rotate(angle, YAxis);
    }

    constexpr Vector3 RotateZ(const Angle& angle) const
    {
        return Rotate(angle, ZAxis);
    }

    static const Vector3<T> Zero;
    static const Vector3<T> XAxis;
    static const Vector3<T> YAxis;
    static const Vector3<T> ZAxis;
};

template<typename T>
constexpr Vector3<T> Vector3<T>::Zero = Vector3<T>();

template<typename T>
constexpr Vector3<T> Vector3<T>::XAxis = Vector3<T>(1, 0, 0);

template<typename T>
constexpr Vector3<T> Vector3<T>::YAxis = Vector3<T>(0, 1, 0);

template<typename T>
constexpr Vector3<T> Vector3<T>::ZAxis = Vector3<T>(0, 0, 1);
