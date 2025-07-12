#pragma once

#include "Unit.h"

struct Illuminance final : public Unit<Illuminance, false>
{
public:
    using Unit<Illuminance, false>::Unit;
    using Unit<Illuminance, false>::operator*;
    using Unit<Illuminance, false>::operator/;

    friend struct Unit<Illuminance, false>;
    friend constexpr Illuminance operator*(ValueType k, const Illuminance& u);
    friend constexpr Illuminance operator/(ValueType k, const Illuminance& u);

    static constexpr Illuminance FromLux(ValueType lux) { return Illuminance(lux); }

    constexpr float ToLux() const { return ToUnits(); }
};

#include "UnitOperators.h"
