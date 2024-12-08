#pragma once

#include "Unit.h"
#include <cmath>

struct Distance : public Unit
{
public:
    using Unit::Unit;
    constexpr Distance(const Unit& unit) : Unit(unit) {}

    static constexpr Distance FromMillimeters(float millimeters) { return Distance(FromMilliunits(millimeters)); }
    static constexpr Distance FromCentimeters(float centimeters) { return Distance(FromCentiunits(centimeters)); }
    static constexpr Distance FromMeters(float meters) { return Distance(meters); }
    static constexpr Distance FromKilometers(float kilometers) { return Distance(FromKilounits(kilometers)); }

    constexpr float ToMillimeters() const { return ToMilliunits(); }
    constexpr float ToCentimeters() const { return ToCentiunits(); }
    constexpr float ToMeters() const { return ToUnits(); }
    constexpr float ToKilometers() const { return ToKilounits(); }

    constexpr Distance operator+(const Distance& other) const
    {
        return Distance(ToUnits() + other.ToUnits());
    }

    constexpr Distance operator-(const Distance& other) const 
    {
        return Distance(ToUnits() - other.ToUnits());
    }

    constexpr Distance operator*(float value) const
    {
        return Distance(ToUnits() * value);
    }

    constexpr float operator/(const Distance& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Distance operator/(float value) const
    {
        return Distance(ToUnits() / value);
    }

    constexpr bool operator==(const Distance& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Distance& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Distance& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Distance& other) const
    {
        return ToUnits() > other.ToUnits();
    }

    constexpr bool operator<=(const Distance& other) const
    {
        return ToUnits() <= other.ToUnits();
    }

    constexpr bool operator>=(const Distance& other) const
    {
        return ToUnits() >= other.ToUnits();
    }

    constexpr Distance& operator+=(const Distance& other)
    {
        *this = *this + other;
        return *this;
    }

    constexpr Distance& operator-=(const Distance& other)
    {
        *this = *this - other;
        return *this;
    }

    constexpr Distance& operator*=(float value)
    {
        *this = *this * value;
        return *this;
    }

    constexpr Distance& operator/=(float value)
    {
        *this = *this / value;
        return *this;
    }

    constexpr Distance Magnitude() const { return Distance(std::abs(ToUnits())); }

    friend constexpr Distance operator/(float a, Distance const& b);
    friend constexpr Distance operator*(float a, Distance const& b);

    static const Distance Zero;

private:
    constexpr Distance(float units) : Unit(units)
    {
    }
};

inline constexpr Distance operator/(float a, Distance const& b) 
{
    return Distance::FromUnits(a / b.ToUnits());
}

inline constexpr Distance operator*(float a, Distance const& b) 
{
    return Distance::FromUnits(a * b.ToUnits());
}


constexpr Distance Distance::Zero = Distance::FromMeters(0);