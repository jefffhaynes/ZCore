#pragma once

#include "Unit.h"
#include <cmath>

struct Distance : public Unit<Distance>
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

    constexpr Distance Magnitude() const { return Distance(std::abs(ToUnits())); }

    static const Distance Zero;

private:
    explicit constexpr Distance(ValueType value) : Unit<Distance>(value)
    {
    }
};

constexpr Distance Distance::Zero = Distance::FromMeters(0);