#pragma once

#include "Unit.h"
#include "TimeSpan.h"
#include "Distance.h"
#include <cmath>

class Distance;

struct Velocity : public Unit<Velocity>
{
public:
    using Unit<Velocity>::Unit;
    using Unit<Velocity>::operator*;
    using Unit<Velocity>::operator/;
    
    friend struct Unit<Velocity>;
    friend constexpr Velocity operator*(ValueType, const Velocity&);
    friend constexpr Velocity operator/(ValueType, const Velocity&);

    static constexpr Velocity FromCentimetersPerSecond(ValueType centimetersPerSecond) { return FromCentiunits(centimetersPerSecond); }
    static constexpr Velocity FromMetersPerSecond(ValueType metersPerSecond) { return Velocity(metersPerSecond); }

    constexpr ValueType ToCentimetersPerSecond() const { return ToCentiunits(); }
    constexpr ValueType ToMetersPerSecond() const { return ToUnits(); }

    constexpr Distance operator*(const TimeSpan& time) const
    {
        return Distance::FromMeters(ToUnits() * static_cast<ValueType>(time.ToSeconds()));
    }

    static const Velocity Zero;
};

constexpr Velocity Velocity::Zero = Velocity::FromMetersPerSecond(0);