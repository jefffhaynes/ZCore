#pragma once

#include "Unit.h"

struct Power : public Unit<Power>
{
public:
    using Unit<Power>::Unit;
    using Unit<Power>::operator*;
    using Unit<Power>::operator/;

    friend struct Unit<Power>;
    friend constexpr Power operator*(ValueType k, const Power& u);
    friend constexpr Power operator/(ValueType k, const Power& u);

    static constexpr Power FromMilliwatts(ValueType milliwatts) { return FromMilliunits(milliwatts); }
    static constexpr Power FromWatts(ValueType watts) { return Power(watts); }

    constexpr ValueType ToWatts() const { return ToUnits(); }
    constexpr ValueType ToMilliwatts() const { return ToMilliunits(); }
};
