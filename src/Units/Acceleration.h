#pragma once

#include "Unit.h"

struct Acceleration final : public Unit<Acceleration>
{
public:
    using Unit<Acceleration>::Unit;
    using Unit<Acceleration>::operator*;
    using Unit<Acceleration>::operator/;

    friend struct Unit<Acceleration>;
    friend constexpr Acceleration operator*(ValueType, const Acceleration&);

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

    static const Acceleration EarthGravity;
};

constexpr Acceleration Acceleration::EarthGravity = Acceleration::FromGees(1);

#include "UnitOperators.h"
