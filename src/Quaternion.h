#pragma once

#include <cmath>
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

    // About `rotation`'s axis by its length in radians (a rotation vector).
    static constexpr Quaternion FromRotationVector(const Vector3<T>& rotation)
    {
        const auto angle = rotation.Length();

        // sin(angle / 2) / angle tends to 1/2.
        const auto scale = angle > T(1e-6) ? std::sin(angle / 2) / angle : T(0.5);

        return { rotation.X * scale, rotation.Y * scale, rotation.Z * scale, std::cos(angle / 2) };
    }

    static const Quaternion Identity;

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

template<typename T>
constexpr Quaternion<T> Quaternion<T>::Identity = Quaternion<T>(0, 0, 0, 1);
