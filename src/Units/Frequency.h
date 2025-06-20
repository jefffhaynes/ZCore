#pragma once

#include "Unit.h"
#include "TimeSpan.h"

struct Frequency : public Unit
{
public:
    using Unit::Unit;
    constexpr Frequency(const Unit& unit) : Unit(unit) {}

    static constexpr Frequency FromMillihertz(float millihertz) { return FromMilliunits(millihertz); }
    static constexpr Frequency FromHertz(float hertz) { return Frequency(hertz); }
    static constexpr Frequency FromKilohertz(float kilohertz) { return FromKilounits(kilohertz); }
    static constexpr Frequency FromMegahertz(float megahertz) { return FromMegaunits(megahertz); }

    static constexpr Frequency FromPeriod(const TimeSpan& period)
    {
        return Frequency(1 / period.ToSeconds());
    }

    constexpr float ToMillihertz() const { return ToMilliunits(); }
    constexpr float ToHertz() const { return ToUnits(); }
    constexpr float ToKilohertz() const { return ToKilounits(); }
    constexpr float ToMegahertz() const { return ToMegaunits(); }

    constexpr TimeSpan ToPeriod() const
    {
        return TimeSpan::FromSeconds(1 / ToUnits());
    }

    constexpr Frequency operator+(const Frequency& other) const
    {
        return Frequency(ToUnits() + other.ToUnits());
    }

    constexpr Frequency operator-(const Frequency& other) const 
    {
        return Frequency(ToUnits() - other.ToUnits());
    }

    constexpr Frequency operator*(float value) const
    {
        return Frequency(ToUnits() * value);
    }

    constexpr float operator*(const TimeSpan& time) const
    {
        return ToHertz() * static_cast<float>(time.ToSeconds());
    }

    constexpr float operator/(const Frequency& other) const
    {
        return ToUnits() / other.ToUnits();
    }

    constexpr Frequency operator/(float value) const
    {
        return Frequency(ToUnits() / value);
    }

    constexpr bool operator==(const Frequency& other) const
    {
        return ToUnits() == other.ToUnits();
    }

    constexpr bool operator!=(const Frequency& other) const
    {
        return ToUnits() != other.ToUnits();
    }

    constexpr bool operator<(const Frequency& other) const
    {
        return ToUnits() < other.ToUnits();
    }

    constexpr bool operator>(const Frequency& other) const
    {
        return ToUnits() > other.ToUnits();
    }
    
    constexpr Frequency& operator+=(const Frequency& other)
    {
        *this = *this + other;
        return *this;
    }

    constexpr Frequency& operator-=(const Frequency& other)
    {
        *this = *this - other;
        return *this;
    }

    constexpr Frequency& operator*=(float value)
    {
        *this = *this * value;
        return *this;
    }

    constexpr Frequency& operator/=(float value)
    {
        *this = *this / value;
        return *this;
    }
    
    friend constexpr Frequency operator/(float a, Frequency const& b);
    friend constexpr Frequency operator*(float a, Frequency const& b);

    static const Frequency Zero;

private:
    constexpr Frequency(float units) : Unit(units)
    {
    }
};

inline constexpr Frequency operator/(float a, Frequency const& b) 
{
    return Frequency::FromUnits(a / b.ToUnits());
}

inline constexpr Frequency operator*(float a, Frequency const& b) 
{
    return Frequency::FromUnits(a * b.ToUnits());
}

constexpr Frequency Frequency::Zero = Frequency::FromHertz(0);