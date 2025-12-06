#pragma once

#include "Unit.h"
#include "TimeSpan.h"

struct Frequency final : public Unit<Frequency, false>
{
public:
    using Unit<Frequency, false>::Unit;
    using Unit<Frequency, false>::operator*;
    using Unit<Frequency, false>::operator/;

    friend struct Unit<Frequency, false>;
    friend constexpr Frequency operator*(ValueType, const Frequency&);

    static constexpr Frequency FromMillihertz(ValueType millihertz) { return FromMilliunits(millihertz); }
    static constexpr Frequency FromHertz(ValueType hertz) { return Frequency(hertz); }
    static constexpr Frequency FromKilohertz(ValueType kilohertz) { return FromKilounits(kilohertz); }
    static constexpr Frequency FromMegahertz(ValueType megahertz) { return FromMegaunits(megahertz); }

    static constexpr Frequency FromPeriod(const TimeSpan& period)
    {
        return Frequency(static_cast<ValueType>(1) / static_cast<ValueType>(period.ToSeconds()));
    }

    constexpr ValueType ToMillihertz() const { return ToMilliunits(); }
    constexpr ValueType ToHertz() const { return ToUnits(); }
    constexpr ValueType ToKilohertz() const { return ToKilounits(); }
    constexpr ValueType ToMegahertz() const { return ToMegaunits(); }

    constexpr TimeSpan ToPeriod() const
    {
        return TimeSpan::FromSeconds(static_cast<ValueType>(1) / ToUnits());
    }

    static const Frequency Zero;
};

constexpr Frequency Frequency::Zero = Frequency::FromUnits(0);

#include "UnitOperators.h"
