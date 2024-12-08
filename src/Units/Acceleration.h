#pragma once

#include "Unit.h"

struct Acceleration : public Unit
{
public:
    using Unit::Unit;
    constexpr Acceleration(const Unit& unit) : Unit(unit) {}

    static constexpr Acceleration FromMetersPerSecondSquared(float metersPerSecondSquared) 
    { 
        return Acceleration(metersPerSecondSquared);
    }

    constexpr float ToMetersPerSecondSquared() const { return ToUnits(); }

    constexpr Acceleration operator+(const Acceleration& other) const
    {
        return Acceleration(ToUnits() + other.ToUnits());
    }

    constexpr Acceleration operator-(const Acceleration& other) const 
    {
        return Acceleration(ToUnits() - other.ToUnits());
    }

    constexpr Acceleration operator*(float value) const
    {
        return Acceleration(ToUnits() * value);
    }

    constexpr float operator/(const Acceleration& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Acceleration operator/(float value) const
    {
        return Acceleration(ToUnits() / value);
    }

    constexpr bool operator==(const Acceleration& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Acceleration& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Acceleration& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Acceleration& other) const
    {
        return ToUnits() > other.ToUnits();
    }

    constexpr bool operator<=(const Acceleration& other) const
    {
        return ToUnits() <= other.ToUnits();
    }

    constexpr bool operator>=(const Acceleration& other) const
    {
        return ToUnits() >= other.ToUnits();
    }

    constexpr Acceleration& operator+=(const Acceleration& other)
    {
        *this = *this + other;
        return *this;
    }

    constexpr Acceleration& operator-=(const Acceleration& other)
    {
        *this = *this - other;
        return *this;
    }

    constexpr Acceleration& operator*=(float value)
    {
        *this = *this * value;
        return *this;
    }

    constexpr Acceleration& operator/=(float value)
    {
        *this = *this / value;
        return *this;
    }

    constexpr Acceleration Magnitude() const { return Acceleration(std::abs(ToUnits())); }

    friend constexpr Acceleration operator/(float a, Acceleration const& b);
    friend constexpr Acceleration operator*(float a, Acceleration const& b);

    static const Acceleration Zero;

private:
    constexpr Acceleration(float units) : Unit(units)
    {
    }
};

inline constexpr Acceleration operator/(float a, Acceleration const& b) 
{
    return Acceleration::FromUnits(a / b.ToUnits());
}

inline constexpr Acceleration operator*(float a, Acceleration const& b) 
{
    return Acceleration::FromUnits(a * b.ToUnits());
}


constexpr Acceleration Acceleration::Zero = Acceleration::FromMetersPerSecondSquared(0);