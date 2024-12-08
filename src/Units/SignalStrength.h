#pragma once

#include "Unit.h"
#include <cmath>

struct SignalStrength : public Unit
{
public:
    using Unit::Unit;
    
    static constexpr SignalStrength FromDecibels(float decibels) { return SignalStrength(decibels); }
    static constexpr SignalStrength FromPower(float power) { return SignalStrength(PowerScale * std::log10(power)); }
    static constexpr SignalStrength FromAmplitude(float amplitude) { return SignalStrength(AmplitudeScale * std::log10(amplitude)); }

    constexpr float ToDecibels() const { return ToUnits(); }
    constexpr float ToPower() const { return std::pow(10, ToUnits() / PowerScale); }
    constexpr float ToAmplitude() const { return std::pow(10, ToUnits() / AmplitudeScale); }

    constexpr SignalStrength operator+(const SignalStrength& other) const
    {
        return SignalStrength(ToUnits() + other.ToUnits());
    }

    constexpr SignalStrength operator-(const SignalStrength& other) const 
    {
        return SignalStrength(ToUnits() - other.ToUnits());
    }

    constexpr SignalStrength operator*(float value) const
    {
        return SignalStrength(ToUnits() * value);
    }

    constexpr float operator/(const SignalStrength& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr SignalStrength operator/(float value) const
    {
        return SignalStrength(ToUnits() / value);
    }

    constexpr bool operator==(const SignalStrength& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const SignalStrength& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const SignalStrength& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const SignalStrength& other) const
    {
        return ToUnits() > other.ToUnits();
    }
    
    friend constexpr SignalStrength operator/(float a, SignalStrength const& b);
    friend constexpr SignalStrength operator*(float a, SignalStrength const& b);

private:
    static constexpr float PowerScale = 10;
    static constexpr float AmplitudeScale = 20;

    constexpr SignalStrength(float units) : Unit(units)
    {
    }
};

inline constexpr SignalStrength operator/(float a, SignalStrength const& b) 
{
    return SignalStrength::FromUnits(a / b.ToUnits());
}

inline constexpr SignalStrength operator*(float a, SignalStrength const& b) 
{
    return SignalStrength::FromUnits(a * b.ToUnits());
}
