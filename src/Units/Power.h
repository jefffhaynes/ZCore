#pragma once

#include "Unit.h"

struct Power : public Unit
{
public:
    using Unit::Unit;
    constexpr Power(const Unit& unit) : Unit(unit) {}

    static constexpr Power FromMilliwatts(float milliwatts) { return Power(FromMilliunits(milliwatts)); }
    static constexpr Power FromWatts(float watts) { return Power(watts); }

    constexpr float ToWatts() const { return ToUnits(); }
    constexpr float ToMilliwatts() const { return ToMilliunits(); }

    constexpr Power operator+(const Power& other) const
    {
        return Power(ToUnits() + other.ToUnits());
    }

    constexpr Power operator-(const Power& other) const 
    {
        return Power(ToUnits() - other.ToUnits());
    }

    constexpr Power operator*(float value) const
    {
        return Power(ToUnits() * value);
    }

    constexpr float operator/(const Power& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Power operator/(float value) const
    {
        return Power(ToUnits() / value);
    }

    constexpr bool operator==(const Power& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Power& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Power& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Power& other) const
    {
        return ToUnits() > other.ToUnits();
    }
    
    constexpr Power& operator+=(const Power& other)
    {
        *this = *this + other;
        return *this;
    }

    constexpr Power& operator-=(const Power& other)
    {
        *this = *this - other;
        return *this;
    }

    constexpr Power& operator*=(float value)
    {
        *this = *this * value;
        return *this;
    }

    constexpr Power& operator/=(float value)
    {
        *this = *this / value;
        return *this;
    }
    
    friend constexpr Power operator/(float a, Power const& b);
    friend constexpr Power operator*(float a, Power const& b);

private:
    constexpr Power(float units) : Unit(units)
    {
    }
};

inline constexpr Power operator/(float a, Power const& b) 
{
    return Power::FromUnits(a / b.ToUnits());
}

inline constexpr Power operator*(float a, Power const& b) 
{
    return Power::FromUnits(a * b.ToUnits());
}
