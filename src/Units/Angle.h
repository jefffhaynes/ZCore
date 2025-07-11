#pragma once

#include "Unit.h"
#include <cmath>
#include <numbers>

struct Angle : public Unit<Angle>
{
public:
    using Unit<Angle>::Unit;
    using Unit<Angle>::operator*;
    using Unit<Angle>::operator/;
    
    friend struct Unit<Angle>;
    friend constexpr Angle operator*(ValueType, const Angle&);
    friend constexpr Angle operator/(ValueType, const Angle&);

    static constexpr Angle FromRadians(ValueType radians) { return Angle(radians); }
    static constexpr Angle FromDegrees(ValueType degrees) { return Angle(degrees * std::numbers::pi_v<ValueType> / 180.0f); }

    constexpr ValueType ToRadians() const { return ToUnits(); }
    constexpr ValueType ToDegrees() const { return ToUnits() * 180.0f / std::numbers::pi_v<ValueType>; }


    constexpr Angle operator-() const
    {
        return Angle(-ToUnits());
    }

    constexpr Angle operator%(const Angle& other) const
    {
        return Angle(std::fmod(ToUnits(), other.ToUnits()));
    }

    constexpr Angle Magnitude() const { return Angle(std::abs(ToUnits())); }

    constexpr ValueType Sin() const { return std::sin(ToRadians()); }
    constexpr ValueType Cos() const { return std::cos(ToRadians()); }
    constexpr ValueType Tan() const { return std::tan(ToRadians()); }

    static const Angle Zero;
    static const Angle Pi;
    static const Angle TwoPi;

private:
    explicit constexpr Angle(ValueType value) : Unit<Angle>(value)
    {
    }
};

constexpr Angle Angle::Zero = Angle::FromRadians(0);
constexpr Angle Angle::Pi = Angle::FromRadians(std::numbers::pi);
constexpr Angle Angle::TwoPi = Angle::FromRadians(2 * std::numbers::pi);