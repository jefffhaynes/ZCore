#pragma once

#include "Unit.h"

struct Force final : public Unit<Force>
{
public:
    using Unit<Force>::Unit;
    using Unit<Force>::operator*;
    using Unit<Force>::operator/;

    friend struct Unit<Force>;
    friend constexpr Force operator*(ValueType, const Force&);

    static constexpr Force FromNewtons(ValueType newtons) { return Force(newtons); }
    static constexpr Force FromKilonewtons(ValueType kilonewtons) { return FromKilounits(kilonewtons); }
    static constexpr Force FromPoundsForce(ValueType poundsForce) { return FromUnits(poundsForce * PoundsForceToNewtons); } 

    constexpr ValueType ToNewtons() const { return ToUnits(); }
    constexpr ValueType ToKilonewtons() const { return ToKilounits(); }
    constexpr ValueType ToPoundsForce() const { return ToUnits() * NewtonsToPoundsForce; }

private:
    static constexpr ValueType PoundsForceToNewtons = static_cast<ValueType>(4.4482216152605);
    static constexpr ValueType NewtonsToPoundsForce = static_cast<ValueType>(1.0) / PoundsForceToNewtons;
};

#include "UnitOperators.h"
