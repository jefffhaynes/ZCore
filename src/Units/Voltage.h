#pragma once

#include "Unit.h"
#include <cmath>

struct Voltage final : public Unit<Voltage>
{
public:
    using Unit<Voltage>::Unit;
    using Unit<Voltage>::operator*;
    using Unit<Voltage>::operator/;

    friend struct Unit<Voltage>;
    friend constexpr Voltage operator*(ValueType, const Voltage&);
    friend constexpr Voltage operator/(ValueType, const Voltage&);

    static constexpr Voltage FromMillivolts(ValueType millivolts) { return Voltage(FromMilliunits(millivolts)); }
    static constexpr Voltage FromVolts(ValueType volts) { return Voltage(volts); }

    constexpr ValueType ToMillivolts() const { return ToMilliunits(); }
    constexpr ValueType ToVolts() const { return ToUnits(); }

    constexpr Voltage Magnitude() const { return Voltage(std::abs(ToUnits())); }

    static const Voltage Zero;

private:
    explicit constexpr Voltage(ValueType value) : Unit<Voltage>(value)
    {
    }
};

constexpr Voltage Voltage::Zero = Voltage::FromVolts(0);