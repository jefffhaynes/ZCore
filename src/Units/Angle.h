#pragma once

#include "Unit.h"
#include <cmath>
#include <numbers>

struct Angle : public Unit
{
public:
    using Unit::Unit;
    constexpr Angle(const Unit& unit) : Unit(unit) {}

    static constexpr Angle FromRadians(float radians) { return Angle(radians); }
    static constexpr Angle FromDegrees(float degrees) { return Angle(degrees * std::numbers::pi / 180); }

    constexpr float ToRadians() const { return ToUnits(); }
    constexpr float ToDegrees() const { return ToUnits() * 180 / std::numbers::pi; }

    constexpr Angle operator+(const Angle& other) const
    {
        return Angle(ToUnits() + other.ToUnits());
    }

    constexpr Angle operator-(const Angle& other) const 
    {
        return Angle(ToUnits() - other.ToUnits());
    }

    constexpr Angle operator-() const
    {
        return Angle(-ToUnits());
    }

    constexpr Angle operator*(float value) const
    {
        return Angle(ToUnits() * value);
    }

    constexpr float operator/(const Angle& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Angle operator/(float value) const
    {
        return Angle(ToUnits() / value);
    }

    constexpr Angle operator%(const Angle& other) const
    {
        return Angle(std::fmod(ToUnits(), other.ToUnits()));
    }

    constexpr bool operator==(const Angle& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Angle& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Angle& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Angle& other) const
    {
        return ToUnits() > other.ToUnits();
    }

    constexpr bool operator<=(const Angle& other) const
    {
        return ToUnits() <= other.ToUnits();
    }

    constexpr bool operator>=(const Angle& other) const
    {
        return ToUnits() >= other.ToUnits();
    }

    constexpr Angle& operator+=(const Angle& other)
    {
        *this = *this + other;
        return *this;
    }

    constexpr Angle& operator-=(const Angle& other)
    {
        *this = *this - other;
        return *this;
    }

    constexpr Angle Magnitude() const { return Angle(std::abs(ToUnits())); }

    friend constexpr Angle operator/(float a, Angle const& b);
    friend constexpr Angle operator*(float a, Angle const& b);

    constexpr float Sin() const { return std::sin(ToRadians()); }
    constexpr float Cos() const { return std::cos(ToRadians()); }
    constexpr float Tan() const { return std::tan(ToRadians()); }

    static const Angle Zero;
    static const Angle Pi;
    static const Angle TwoPi;

private:
    constexpr Angle(float units) : Unit(units)
    {
    }
};

inline constexpr Angle operator/(float a, Angle const& b) 
{
    return Angle::FromUnits(a / b.ToUnits());
}

inline constexpr Angle operator*(float a, Angle const& b) 
{
    return Angle::FromUnits(a * b.ToUnits());
}


constexpr Angle Angle::Zero = Angle::FromRadians(0);
constexpr Angle Angle::Pi = Angle::FromRadians(std::numbers::pi);
constexpr Angle Angle::TwoPi = Angle::FromRadians(2 * std::numbers::pi);