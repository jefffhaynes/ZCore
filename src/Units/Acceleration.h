#pragma once

#include "Unit.h"

struct Acceleration : public Unit<Acceleration>
{
public:
    using Unit<Acceleration>::Unit;
    using Unit<Acceleration>::operator*;
    using Unit<Acceleration>::operator/;

    friend struct Unit<Acceleration>;
    friend constexpr Acceleration operator*(ValueType, const Acceleration&);
    friend constexpr Acceleration operator/(ValueType, const Acceleration&);

    static constexpr Acceleration FromMetersPerSecondSquared(ValueType metersPerSecondSquared) 
    { 
        return Acceleration(metersPerSecondSquared);
    }

    static constexpr Acceleration FromGees(ValueType gees) 
    { 
        return FromMetersPerSecondSquared(gees * 9.81f);
    }

    constexpr ValueType ToMetersPerSecondSquared() const { return ToUnits(); }
    constexpr ValueType ToGees() const { return ToMetersPerSecondSquared() / 9.81f; }


    constexpr Acceleration Magnitude() const { return Acceleration(std::abs(ToUnits())); }

    static const Acceleration Zero;
    static const Acceleration EarthGravity;

private:
    explicit constexpr Acceleration(ValueType value) : Unit<Acceleration>(value)
    {
    }
};


constexpr Acceleration Acceleration::Zero = Acceleration::FromMetersPerSecondSquared(0);
constexpr Acceleration Acceleration::EarthGravity = Acceleration::FromGees(1);