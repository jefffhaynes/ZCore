#pragma once

#include "Unit.h"

struct Current : public Unit
{
public:
    using Unit::Unit;
    constexpr Current(const Unit& unit) : Unit(unit) {}
    
    static constexpr Current FromMicroamperes(float microamperes) { return Current(FromMicrounits(microamperes)); }
    static constexpr Current FromMilliamperes(float milliamperes) { return Current(FromMilliunits(milliamperes)); }
    static constexpr Current FromAmperes(float amperes) { return Current(amperes); }

    constexpr float ToAmperes() const { return ToUnits(); }
    constexpr float ToMilliamperes() const { return ToMilliunits(); }
    constexpr float ToMicroamperes() const { return ToMicrounits(); }


    constexpr Current operator+(const Current& other) const
    {
        return Current(ToUnits() + other.ToUnits());
    }

    constexpr Current operator-(const Current& other) const 
    {
        return Current(ToUnits() - other.ToUnits());
    }

    constexpr Current operator*(float value) const
    {
        return Current(ToUnits() * value);
    }

    constexpr float operator/(const Current& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Current operator/(float value) const
    {
        return Current(ToUnits() / value);
    }

    constexpr bool operator==(const Current& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Current& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Current& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Current& other) const
    {
        return ToUnits() > other.ToUnits();
    }

    constexpr bool operator<=(const Current& other) const
    {
        return ToUnits() <= other.ToUnits();
    }

    constexpr bool operator>=(const Current& other) const
    {
        return ToUnits() >= other.ToUnits();
    }

    constexpr Current& operator+=(const Current& other)
    {
        *this = *this + other;
        return *this;
    }

    constexpr Current& operator-=(const Current& other)
    {
        *this = *this - other;
        return *this;
    }

    constexpr Current& operator*=(float value)
    {
        *this = *this * value;
        return *this;
    }

    constexpr Current& operator/=(float value)
    {
        *this = *this / value;
        return *this;
    }

    friend constexpr Current operator/(float a, Current const& b);
    friend constexpr Current operator*(float a, Current const& b);

private:
    constexpr Current(float units) : Unit(units)
    {
    }
};

inline constexpr Current operator/(float a, Current const& b) 
{
    return Current::FromUnits(a / b.ToUnits());
}

inline constexpr Current operator*(float a, Current const& b) 
{
    return Current::FromUnits(a * b.ToUnits());
}
