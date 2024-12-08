#pragma once

#include "Unit.h"
#include <algorithm>

class Temperature : public Unit
{
public:
    using Unit::Unit;

    static constexpr Temperature FromKelvin(float kelvin) { return Temperature(kelvin); }
    static constexpr Temperature FromCelsius(float celsius) { return Temperature(celsius + CelsiusOffset); }

    constexpr float ToKelvin() const { return ToUnits(); }
    constexpr float ToCelsius() const { return ToUnits() - CelsiusOffset; }

    static const Temperature AbsoluteZero;
  
    constexpr Temperature operator+(const Temperature& other) const
    {
        return Temperature(ToUnits() + other.ToUnits());
    }

    constexpr Temperature operator-(const Temperature& other) const 
    {
        return Temperature(ToUnits() - other.ToUnits());
    }

    constexpr Temperature operator*(float value) const
    {
        return Temperature(ToUnits() * value);
    }

    constexpr float operator/(const Temperature& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Temperature operator/(float value) const
    {
        return Temperature(ToUnits() / value);
    }

    constexpr bool operator==(const Temperature& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Temperature& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Temperature& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Temperature& other) const
    {
        return ToUnits() > other.ToUnits();
    }

    constexpr bool operator<=(const Temperature& other) const
    {
        return ToUnits() <= other.ToUnits();
    }

    constexpr bool operator>=(const Temperature& other) const
    {
        return ToUnits() >= other.ToUnits();
    }

    constexpr Temperature& operator+=(const Temperature& other)
    {
        *this = *this + other;
        return *this;
    }

    constexpr Temperature& operator-=(const Temperature& other)
    {
        *this = *this - other;
        return *this;
    }

    constexpr Temperature& operator*=(float value)
    {
        *this = *this * value;
        return *this;
    }

    constexpr Temperature& operator/=(float value)
    {
        *this = *this / value;
        return *this;
    }

    friend constexpr Temperature operator/(float a, Temperature const& b);
    friend constexpr Temperature operator*(float a, Temperature const& b);

private:
    static constexpr float CelsiusOffset = 273.15;

    constexpr Temperature(float units) : Unit(units)
    {
    }
};

inline constexpr Temperature Temperature::AbsoluteZero = Temperature::FromKelvin(0);

inline constexpr Temperature operator/(float a, Temperature const& b) 
{
    return Temperature::FromUnits(a / b.ToUnits());
}

inline constexpr Temperature operator*(float a, Temperature const& b) 
{
    return Temperature::FromUnits(a * b.ToUnits());
}