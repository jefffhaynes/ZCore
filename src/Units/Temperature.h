#pragma once

#include "Unit.h"
#include <algorithm>

class Temperature : public UnsignedUnit<Temperature>
{
public:
    using UnsignedUnit<Temperature>::Unit;
    using UnsignedUnit<Temperature>::operator*;
    using UnsignedUnit<Temperature>::operator/;

    friend struct Unit<Temperature, false>;
    friend constexpr Temperature operator*(ValueType, const Temperature&);
    friend constexpr Temperature operator/(ValueType, const Temperature&);

    static constexpr Temperature FromKelvin(ValueType kelvin) { return Temperature(kelvin); }
    static constexpr Temperature FromCelsius(ValueType celsius) { return Temperature(celsius + CelsiusOffset); }

    constexpr ValueType ToKelvin() const { return ToUnits(); }
    constexpr ValueType ToCelsius() const { return ToUnits() - CelsiusOffset; }

    static const Temperature AbsoluteZero;

private:
    static constexpr ValueType CelsiusOffset = 273.15;
};

inline constexpr Temperature Temperature::AbsoluteZero = Temperature::FromKelvin(0);
