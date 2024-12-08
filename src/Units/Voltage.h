#pragma once

#include "Unit.h"
#include <cmath>

struct Voltage : public Unit
{
public:
    using Unit::Unit;
    constexpr Voltage(const Unit& unit) : Unit(unit) {}

    static constexpr Voltage FromMillivolts(float millivolts) { return Voltage(FromMilliunits(millivolts)); }
    static constexpr Voltage FromVolts(float volts) { return Voltage(volts); }

    constexpr float ToMillivolts() const { return ToMilliunits(); }
    constexpr float ToVolts() const { return ToUnits(); }

    constexpr Voltage operator+(const Voltage& other) const
    {
        return Voltage(ToUnits() + other.ToUnits());
    }

    constexpr Voltage operator-(const Voltage& other) const 
    {
        return Voltage(ToUnits() - other.ToUnits());
    }

    constexpr Voltage operator*(float value) const
    {
        return Voltage(ToUnits() * value);
    }

    constexpr float operator/(const Voltage& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Voltage operator/(float value) const
    {
        return Voltage(ToUnits() / value);
    }

    constexpr bool operator==(const Voltage& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Voltage& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Voltage& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Voltage& other) const
    {
        return ToUnits() > other.ToUnits();
    }

    constexpr bool operator<=(const Voltage& other) const
    {
        return ToUnits() <= other.ToUnits();
    }

    constexpr bool operator>=(const Voltage& other) const
    {
        return ToUnits() >= other.ToUnits();
    }

    constexpr Voltage& operator+=(const Voltage& other)
    {
        *this = *this + other;
        return *this;
    }

    constexpr Voltage& operator-=(const Voltage& other)
    {
        *this = *this - other;
        return *this;
    }

    constexpr Voltage& operator*=(float value)
    {
        *this = *this * value;
        return *this;
    }

    constexpr Voltage& operator/=(float value)
    {
        *this = *this / value;
        return *this;
    }

    constexpr Voltage Magnitude() const { return Voltage(std::abs(ToUnits())); }

    friend constexpr Voltage operator/(float a, Voltage const& b);
    friend constexpr Voltage operator*(float a, Voltage const& b);

    static const Voltage Zero;

private:
    constexpr Voltage(float units) : Unit(units)
    {
    }
};

inline constexpr Voltage operator/(float a, Voltage const& b) 
{
    return Voltage::FromUnits(a / b.ToUnits());
}

inline constexpr Voltage operator*(float a, Voltage const& b) 
{
    return Voltage::FromUnits(a * b.ToUnits());
}


constexpr Voltage Voltage::Zero = Voltage::FromVolts(0);