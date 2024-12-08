#pragma once

#include "Unit.h"

struct Resistance : public Unit
{
public:
    using Unit::Unit;

    static constexpr Resistance FromOhms(float ohms) { return Resistance(ohms); }
    static constexpr Resistance FromKiloohms(float kiloohms) { return Resistance(FromKilounits(kiloohms)); }
    static constexpr Resistance FromMegaohms(float megaohms) { return Resistance(FromMegaunits(megaohms)); }

    constexpr float ToOhms() const { return ToUnits(); }
    constexpr float ToKiloohms() const { return ToKilounits(); }
    constexpr float ToMegaohms() const { return ToMegaunits(); }


    constexpr Resistance operator+(const Resistance& other) const
    {
        return Resistance(ToUnits() + other.ToUnits());
    }

    constexpr Resistance operator-(const Resistance& other) const 
    {
        return Resistance(ToUnits() - other.ToUnits());
    }

    constexpr Resistance operator*(float value) const
    {
        return Resistance(ToUnits() * value);
    }

    constexpr float operator/(const Resistance& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Resistance operator/(float value) const
    {
        return Resistance(ToUnits() / value);
    }

    constexpr bool operator==(const Resistance& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Resistance& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Resistance& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Resistance& other) const
    {
        return ToUnits() > other.ToUnits();
    }

    friend constexpr Resistance operator/(float a, Resistance const& b);
    friend constexpr Resistance operator*(float a, Resistance const& b);

private:
    constexpr Resistance(float units) : Unit(units)
    {
    }
};

inline constexpr Resistance operator/(float a, Resistance const& b) 
{
    return Resistance::FromUnits(a / b.ToUnits());
}

inline constexpr Resistance operator*(float a, Resistance const& b) 
{
    return Resistance::FromUnits(a * b.ToUnits());
}
