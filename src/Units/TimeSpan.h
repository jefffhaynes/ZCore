#pragma once

#include "Unit.h"

struct TimeSpan final : public Unit<TimeSpan, true, double>
{
public:
    using Unit<TimeSpan, true, ValueType>::Unit;
    using Unit<TimeSpan, true, ValueType>::operator*;
    using Unit<TimeSpan, true, ValueType>::operator/;

    friend struct Unit<TimeSpan, true, ValueType>;
    friend constexpr TimeSpan operator*(ValueType, const TimeSpan&);
    friend constexpr TimeSpan operator/(ValueType, const TimeSpan&);

    static constexpr TimeSpan FromNanoseconds(ValueType nanoseconds) { return FromNanounits(nanoseconds); }
    static constexpr TimeSpan FromMicroseconds(ValueType microseconds) { return FromMicrounits(microseconds); }
    static constexpr TimeSpan FromMilliseconds(ValueType milliseconds) { return FromMilliunits(milliseconds); }
    static constexpr TimeSpan FromSeconds(ValueType seconds) { return FromUnits(seconds); }
    static constexpr TimeSpan FromMinutes(ValueType minutes) { return FromUnits(minutes * SecondsPerMinute); }
    static constexpr TimeSpan FromHours(ValueType hours) { return FromUnits(hours * SecondsPerHour); }
    static constexpr TimeSpan FromDays(ValueType days) { return FromUnits(days * SecondsPerDay); }

    constexpr ValueType ToNanoseconds() const { return ToNanounits(); }
    constexpr ValueType ToMicroseconds() const { return ToMicrounits(); }
    constexpr ValueType ToMilliseconds() const { return ToMilliunits(); }
    constexpr ValueType ToSeconds() const { return ToUnits(); }
    constexpr ValueType ToMinutes() const { return ToUnits() / SecondsPerMinute; }
    constexpr ValueType ToHours() const { return ToUnits() / SecondsPerHour; }
    constexpr ValueType ToDays() const { return ToUnits() / SecondsPerDay; }

private:
    static constexpr ValueType SecondsPerMinute = 60;
    static constexpr ValueType SecondsPerHour = 3600;
    static constexpr ValueType SecondsPerDay = 86400;
};
