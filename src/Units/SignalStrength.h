#pragma once

#include "Unit.h"
#include <cmath>

struct SignalStrength : public Unit<SignalStrength>
{
public:
    using Unit<SignalStrength>::Unit;
    using Unit<SignalStrength>::operator*;
    using Unit<SignalStrength>::operator/;

    friend struct Unit<SignalStrength>;
    friend constexpr SignalStrength operator*(ValueType, const SignalStrength&);
    friend constexpr SignalStrength operator/(ValueType, const SignalStrength&);

    static constexpr SignalStrength FromDecibels(ValueType decibels) { return SignalStrength(decibels); }
    static constexpr SignalStrength FromPower(ValueType power) { return SignalStrength(PowerScale * std::log10(power)); }
    static constexpr SignalStrength FromAmplitude(ValueType amplitude) { return SignalStrength(AmplitudeScale * std::log10(amplitude)); }

    constexpr ValueType ToDecibels() const { return ToUnits(); }
    constexpr ValueType ToPower() const { return std::pow(10, ToUnits() / PowerScale); }
    constexpr ValueType ToAmplitude() const { return std::pow(10, ToUnits() / AmplitudeScale); }

    static const SignalStrength NegativeInfinity;
    static const SignalStrength PositiveInfinity;
    
private:
    static constexpr ValueType PowerScale = 10;
    static constexpr ValueType AmplitudeScale = 20;
};

constexpr SignalStrength SignalStrength::NegativeInfinity = SignalStrength(-std::numeric_limits<ValueType>::infinity());
constexpr SignalStrength SignalStrength::PositiveInfinity = SignalStrength(std::numeric_limits<ValueType>::infinity());
