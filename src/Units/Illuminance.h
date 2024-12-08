#pragma once

#include "Unit.h"
#include <cmath>

struct Illuminance : public Unit
{
public:
    using Unit::Unit;
    
    static constexpr Illuminance FromLux(float lux) { return Illuminance(lux); }

    constexpr float ToLux() const { return ToUnits(); }

    constexpr Illuminance operator+(const Illuminance& other) const
    {
        return Illuminance(ToUnits() + other.ToUnits());
    }

    constexpr Illuminance operator-(const Illuminance& other) const 
    {
        return Illuminance(ToUnits() - other.ToUnits());
    }

    constexpr Illuminance operator*(float value) const
    {
        return Illuminance(ToUnits() * value);
    }

    constexpr float operator/(const Illuminance& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Illuminance operator/(float value) const
    {
        return Illuminance(ToUnits() / value);
    }

    constexpr bool operator==(const Illuminance& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Illuminance& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Illuminance& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Illuminance& other) const
    {
        return ToUnits() > other.ToUnits();
    }

    constexpr bool operator<=(const Illuminance& other) const
    {
        return ToUnits() <= other.ToUnits();
    }

    constexpr bool operator>=(const Illuminance& other) const
    {
        return ToUnits() >= other.ToUnits();
    }
    
    constexpr Illuminance& operator+=(const Illuminance& other)
    {
        *this = *this + other;
        return *this;
    }

    constexpr Illuminance& operator-=(const Illuminance& other)
    {
        *this = *this - other;
        return *this;
    }

    constexpr Illuminance& operator*=(float value)
    {
        *this = *this * value;
        return *this;
    }

    constexpr Illuminance& operator/=(float value)
    {
        *this = *this / value;
        return *this;
    }

    constexpr Illuminance Magnitude() const
    {
        return Illuminance(std::abs(ToUnits()));
    }

    friend constexpr Illuminance operator/(float a, Illuminance const& b);
    friend constexpr Illuminance operator*(float a, Illuminance const& b);
    
    static const Illuminance Zero;

private:
    constexpr Illuminance(float units) : Unit(units)
    {
    }
};

inline constexpr Illuminance operator/(float a, Illuminance const& b) 
{
    return Illuminance::FromUnits(a / b.ToUnits());
}

inline constexpr Illuminance operator*(float a, Illuminance const& b) 
{
    return Illuminance::FromUnits(a * b.ToUnits());
}

constexpr Illuminance Illuminance::Zero = Illuminance::FromLux(0);