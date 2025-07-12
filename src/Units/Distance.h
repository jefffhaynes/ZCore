#pragma once

#include "Unit.h"

struct Distance final : public Unit<Distance>
{
public:
    using Unit<Distance>::Unit;
    using Unit<Distance>::operator*;
    using Unit<Distance>::operator/;

    friend struct Unit<Distance>;
    friend constexpr Distance operator*(ValueType, const Distance&);
    friend constexpr Distance operator/(ValueType, const Distance&);

    static constexpr Distance FromMillimeters(ValueType millimeters) { return FromMilliunits(millimeters); }
    static constexpr Distance FromCentimeters(ValueType centimeters) { return FromCentiunits(centimeters); }
    static constexpr Distance FromMeters(ValueType meters) { return Distance(meters); }
    static constexpr Distance FromKilometers(ValueType kilometers) { return FromKilounits(kilometers); }

    constexpr ValueType ToMillimeters() const { return ToMilliunits(); }
    constexpr ValueType ToCentimeters() const { return ToCentiunits(); }
    constexpr ValueType ToMeters() const { return ToUnits(); }
    constexpr ValueType ToKilometers() const { return ToKilounits(); }
};

#include "UnitOperators.h"
