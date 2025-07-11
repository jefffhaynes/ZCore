#pragma once

#include "Unit.h"

struct Current final : public Unit<Current>
{
public:
    using Unit<Current>::Unit;
    using Unit<Current>::operator*;
    using Unit<Current>::operator/;

    friend struct Unit<Current>;
    friend constexpr Current operator*(ValueType, const Current&);
    friend constexpr Current operator/(ValueType, const Current&);

    static constexpr Current FromMicroamperes(ValueType microamperes) { return FromMicrounits(microamperes); }
    static constexpr Current FromMilliamperes(ValueType milliamperes) { return FromMilliunits(milliamperes); }
    static constexpr Current FromAmperes(ValueType amperes) { return Current(amperes); }

    constexpr ValueType ToAmperes() const { return ToUnits(); }
    constexpr ValueType ToMilliamperes() const { return ToMilliunits(); }
    constexpr ValueType ToMicroamperes() const { return ToMicrounits(); }
};

#include "UnitOperators.h"
