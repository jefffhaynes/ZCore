#pragma once

#include "Unit.h"

struct Mass final : public Unit<Mass>
{
public:
    using Unit<Mass>::Unit;
    using Unit<Mass>::operator*;
    using Unit<Mass>::operator/;

    friend struct Unit<Mass>;
    friend constexpr Mass operator*(ValueType, const Mass&);
    friend constexpr Mass operator/(ValueType, const Mass&);

    static constexpr Mass FromGrams(ValueType grams) { return Mass(grams); }
    static constexpr Mass FromKilograms(ValueType kilograms) { return FromKilounits(kilograms); }
    static constexpr Mass FromPounds(ValueType pounds) { return FromUnits(pounds * PoundsToKilograms); }

    constexpr ValueType ToGrams() const { return ToUnits(); }
    constexpr ValueType ToKilograms() const { return ToKilounits(); }
    constexpr ValueType ToPounds() const { return ToUnits() * KilogramsToPounds; }

private:
    static constexpr ValueType PoundsToKilograms = static_cast<ValueType>(0.45359237);
    static constexpr ValueType KilogramsToPounds = static_cast<ValueType>(1.0) / PoundsToKilograms;
};

#include "UnitOperators.h"
