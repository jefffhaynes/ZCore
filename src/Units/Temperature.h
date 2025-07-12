#pragma once

#include "Unit.h"

class Temperature final : public Unit<Temperature, false>
{
public:
    using Unit<Temperature, false>::Unit;
    using Unit<Temperature, false>::operator*;
    using Unit<Temperature, false>::operator/;

    friend struct Unit<Temperature, false>;
    friend constexpr Temperature operator*(ValueType, const Temperature&);
    friend constexpr Temperature operator/(ValueType, const Temperature&);

    static constexpr Temperature FromKelvin(ValueType kelvin) { return Temperature(kelvin); }
    static constexpr Temperature FromCelsius(ValueType celsius) { return Temperature(celsius + CelsiusOffset); }
    static constexpr Temperature FromFahrenheit(ValueType fahrenheit)
    {
        // convert fahrenheit to celsius first
        auto celsius = (fahrenheit - FahrenheitOffset) * FahrenheitToCelsiusFactor;
        return FromCelsius(celsius);
    }

    constexpr ValueType ToKelvin() const { return ToUnits(); }
    constexpr ValueType ToCelsius() const { return ToUnits() - CelsiusOffset; }
    constexpr ValueType ToFahrenheit() const 
    { 
        auto celsius = ToCelsius();
        return celsius * CelsiusToFahrenheitFactor + FahrenheitOffset;
    }

    static const Temperature AbsoluteZero;

private:
    static constexpr ValueType CelsiusOffset = 273.15;
    static constexpr ValueType FahrenheitOffset = 32.0;
    static constexpr ValueType FahrenheitToCelsiusFactor = 5.0 / 9.0;
    static constexpr ValueType CelsiusToFahrenheitFactor = 9.0 / 5.0;
};

inline constexpr Temperature Temperature::AbsoluteZero = Temperature::FromKelvin(0);

#include "UnitOperators.h"
