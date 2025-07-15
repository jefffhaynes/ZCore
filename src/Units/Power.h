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
    static constexpr Power FromDecibelMilliwatts(ValueType decibelMilliwatts)
    {
        return Power::FromMilliwatts(std::pow(10, decibelMilliwatts / 10));
    }

    constexpr ValueType ToWatts() const { return ToUnits(); }
    constexpr ValueType ToMilliwatts() const { return ToMilliunits(); }
    constexpr ValueType ToDecibelMilliwatts() const
    {
        return 10 * std::log10(ToMilliwatts());
    }
};

#include "UnitOperators.h"

