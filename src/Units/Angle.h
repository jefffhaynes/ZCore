#pragma once

#include "Unit.h"
#include <numbers>

struct Angle final : public Unit<Angle>
{
public:
    using Unit<Angle>::Unit;
    using Unit<Angle>::operator*;
    using Unit<Angle>::operator/;
    
    friend struct Unit<Angle>;
    friend constexpr Angle operator*(ValueType, const Angle&);
    friend constexpr Angle operator/(ValueType, const Angle&);

    static constexpr Angle FromRadians(ValueType radians) { return Angle(radians); }
    static constexpr Angle FromDegrees(ValueType degrees) { return Angle(degrees * RadiansPerDegree); }

    constexpr ValueType ToRadians() const { return ToUnits(); }
    constexpr ValueType ToDegrees() const { return ToUnits() * DegreesPerRadian; }

    // constexpr Angle operator%(const Angle& other) const
    // {
    //     return Angle(std::fmod(ToUnits(), other.ToUnits()));
    // }

    constexpr ValueType Sin() const { return std::sin(ToRadians()); }
    constexpr ValueType Cos() const { return std::cos(ToRadians()); }
    constexpr ValueType Tan() const { return std::tan(ToRadians()); }

    static const Angle Pi;
    static const Angle TwoPi;

private:
    static constexpr ValueType RadiansPerDegree = std::numbers::pi_v<ValueType> / 180.0f;
    static constexpr ValueType DegreesPerRadian = 180.0f / std::numbers::pi_v<ValueType>;
};

constexpr Angle Angle::Pi = Angle::FromRadians(std::numbers::pi_v<Angle::ValueType>);
constexpr Angle Angle::TwoPi = Angle::FromRadians(2 * std::numbers::pi_v<Angle::ValueType>);

#include "UnitOperators.h"
