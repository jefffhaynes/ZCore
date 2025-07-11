#pragma once

#include "Unit.h"
#include <algorithm>

class Temperature : public Unit<Temperature>
{
public:
    using Unit<Temperature>::Unit;
    using Unit<Temperature>::operator*;
    using Unit<Temperature>::operator/;

    friend struct Unit<Temperature>;
    friend constexpr Temperature operator*(ValueType, const Temperature&);
    friend constexpr Temperature operator/(ValueType, const Temperature&);

    static constexpr Temperature FromKelvin(ValueType kelvin) { return Temperature(kelvin); }
    static constexpr Temperature FromCelsius(ValueType celsius) { return Temperature(celsius + CelsiusOffset); }

    constexpr ValueType ToKelvin() const { return ToUnits(); }
    constexpr ValueType ToCelsius() const { return ToUnits() - CelsiusOffset; }

    static const Temperature AbsoluteZero;

private:
    static constexpr ValueType CelsiusOffset = 273.15;

    explicit constexpr Temperature(ValueType value) : Unit<Temperature>(value)
    {
    }
};

inline constexpr Temperature Temperature::AbsoluteZero = Temperature::FromKelvin(0);
