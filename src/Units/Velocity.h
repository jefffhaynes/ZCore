#pragma once

#include "Unit.h"
#include <cmath>

class Distance;

struct Velocity final : public Unit<Velocity>
{
public:
    using Unit<Velocity>::Unit;
    using Unit<Velocity>::operator*;
    using Unit<Velocity>::operator/;
    
    friend struct Unit<Velocity>;
    friend constexpr Velocity operator*(ValueType, const Velocity&);
    friend constexpr Velocity operator/(ValueType, const Velocity&);

    static constexpr Velocity FromCentimetersPerSecond(ValueType centimetersPerSecond) 
    {
        return FromCentiunits(centimetersPerSecond);
    }

    static constexpr Velocity FromMetersPerSecond(ValueType metersPerSecond) 
    {
        return Velocity(metersPerSecond);
    }

    constexpr ValueType ToCentimetersPerSecond() const { return ToCentiunits(); }
    constexpr ValueType ToMetersPerSecond() const { return ToUnits(); }
};

#include "UnitOperators.h"
