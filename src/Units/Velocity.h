#pragma once

#include "Unit.h"
#include "TimeSpan.h"
#include "Distance.h"
#include <cmath>

class Distance;

struct Velocity : public Unit
{
public:
    using Unit::Unit;
    constexpr Velocity(const Unit& unit) : Unit(unit) {}

    static constexpr Velocity FromCentimetersPerSecond(float centimetersPerSecond) { return FromCentiunits(centimetersPerSecond); }
    static constexpr Velocity FromMetersPerSecond(float metersPerSecond) { return Velocity(metersPerSecond); }

    constexpr float ToCentimetersPerSecond() const { return ToCentiunits(); }
    constexpr float ToMetersPerSecond() const { return ToUnits(); }

    constexpr Velocity operator+(const Velocity& other) const
    {
        return Velocity(ToUnits() + other.ToUnits());
    }

    constexpr Velocity operator-(const Velocity& other) const 
    {
        return Velocity(ToUnits() - other.ToUnits());
    }

    constexpr Velocity operator*(float value) const
    {
        return Velocity(ToUnits() * value);
    }

    constexpr Distance operator*(const TimeSpan& time) const
    {
        return Distance::FromMeters(ToUnits() * static_cast<float>(time.ToSeconds()));
    }

    constexpr float operator/(const Velocity& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Velocity operator/(float value) const
    {
        return Velocity(ToUnits() / value);
    }

    constexpr bool operator==(const Velocity& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Velocity& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Velocity& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Velocity& other) const
    {
        return ToUnits() > other.ToUnits();
    }

    constexpr bool operator<=(const Velocity& other) const
    {
        return ToUnits() <= other.ToUnits();
    }

    constexpr bool operator>=(const Velocity& other) const
    {
        return ToUnits() >= other.ToUnits();
    }

    constexpr Velocity& operator+=(const Velocity& other)
    {
        *this = *this + other;
        return *this;
    }

    constexpr Velocity& operator-=(const Velocity& other)
    {
        *this = *this - other;
        return *this;
    }

    constexpr Velocity& operator*=(float value)
    {
        *this = *this * value;
        return *this;
    }

    constexpr Velocity& operator/=(float value)
    {
        *this = *this / value;
        return *this;
    }

    constexpr Velocity Magnitude() const { return Velocity(std::abs(ToUnits())); }

    friend constexpr Velocity operator/(float a, Velocity const& b);
    friend constexpr Velocity operator*(float a, Velocity const& b);

    static const Velocity Zero;

private:
    constexpr Velocity(float units) : Unit(units)
    {
    }
};

inline constexpr Velocity operator/(float a, Velocity const& b) 
{
    return Velocity::FromUnits(a / b.ToUnits());
}

inline constexpr Velocity operator*(float a, Velocity const& b) 
{
    return Velocity::FromUnits(a * b.ToUnits());
}


constexpr Velocity Velocity::Zero = Velocity::FromMetersPerSecond(0);