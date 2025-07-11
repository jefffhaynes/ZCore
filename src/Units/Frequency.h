#pragma once

#include "Unit.h"
#include "TimeSpan.h"

struct Frequency : public UnsignedUnit<Frequency>
{
public:
    using UnsignedUnit<Frequency>::Unit;
    using UnsignedUnit<Frequency>::operator*;
    using UnsignedUnit<Frequency>::operator/;

    friend struct Unit<Frequency, false>;
    friend constexpr Frequency operator*(ValueType, const Frequency&);
    friend constexpr Frequency operator/(ValueType, const Frequency&);

    static constexpr Frequency FromMillihertz(ValueType millihertz) { return FromMilliunits(millihertz); }
    static constexpr Frequency FromHertz(ValueType hertz) { return Frequency(hertz); }
    static constexpr Frequency FromKilohertz(ValueType kilohertz) { return FromKilounits(kilohertz); }
    static constexpr Frequency FromMegahertz(ValueType megahertz) { return FromMegaunits(megahertz); }

    static constexpr Frequency FromPeriod(const TimeSpan& period)
    {
        return Frequency(1 / period.ToSeconds());
    }

    constexpr ValueType ToMillihertz() const { return ToMilliunits(); }
    constexpr ValueType ToHertz() const { return ToUnits(); }
    constexpr ValueType ToKilohertz() const { return ToKilounits(); }
    constexpr ValueType ToMegahertz() const { return ToMegaunits(); }

    constexpr TimeSpan ToPeriod() const
    {
        return TimeSpan::FromSeconds(1 / ToUnits());
    }

    static const Frequency Zero;

private:
    explicit constexpr Frequency(ValueType value) : UnsignedUnit<Frequency>(value)
    {
    }
};

constexpr Frequency Frequency::Zero = Frequency::FromUnits(0);