#pragma once

#include "Unit.h"
#include <cmath>

struct Illuminance : public Unit<Illuminance>
{
public:
    using Unit<Illuminance>::Unit;
    using Unit<Illuminance>::operator*;
    using Unit<Illuminance>::operator/;

    friend struct Unit<Illuminance>;
    friend constexpr Illuminance operator*(ValueType k, const Illuminance& u);
    friend constexpr Illuminance operator/(ValueType k, const Illuminance& u);

    static constexpr Illuminance FromLux(ValueType lux) { return Illuminance(lux); }

    constexpr float ToLux() const { return ToUnits(); }

    constexpr Illuminance Magnitude() const { return Illuminance(std::abs(ToUnits())); }
    
    static const Illuminance Zero;

private:
    explicit constexpr Illuminance(ValueType value) : Unit<Illuminance>(value)
    {
    }
};

constexpr Illuminance Illuminance::Zero = Illuminance::FromLux(0);