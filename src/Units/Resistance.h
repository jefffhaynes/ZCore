#pragma once

#include "Unit.h"

struct Resistance final : public Unit<Resistance>
{
public:
    using Unit<Resistance>::Unit;
    using Unit<Resistance>::operator*;
    using Unit<Resistance>::operator/;

    friend struct Unit<Resistance>;
    friend constexpr Resistance operator*(ValueType k, const Resistance& u);
    friend constexpr Resistance operator/(ValueType k, const Resistance& u);

    static constexpr Resistance FromOhms(ValueType ohms) { return Resistance(ohms); }
    static constexpr Resistance FromKiloohms(ValueType kiloohms) { return FromKilounits(kiloohms); }
    static constexpr Resistance FromMegaohms(ValueType megaohms) { return FromMegaunits(megaohms); }

    constexpr ValueType ToOhms() const { return ToUnits(); }
    constexpr ValueType ToKiloohms() const { return ToKilounits(); }
    constexpr ValueType ToMegaohms() const { return ToMegaunits(); }
};

#include "UnitOperators.h"
