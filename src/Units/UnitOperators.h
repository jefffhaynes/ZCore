#pragma once

#include "TimeSpan.h"
#include "Frequency.h"
#include "Current.h"
#include "Resistance.h"
#include "Voltage.h"
#include "Power.h"
#include "Distance.h"
#include "Velocity.h"
#include "Acceleration.h"


/* Time */
// [[nodiscard]] constexpr TimeSpan operator*(Frequency f, TimeSpan period)
// {
//     auto seconds = static_cast<Unit<Frequency>::ValueType>(period.ToSeconds());
//     return TimeSpan::FromSeconds(f.ToHertz() * seconds); // T = f · t
// }


/* Electrical */
[[nodiscard]] constexpr Voltage operator*(Current i, Resistance r)
{
    return Voltage::FromVolts(i.ToAmperes() * r.ToOhms());       // V = I · R
}

[[nodiscard]] constexpr Voltage operator*(Resistance r, Current i)
{
    return Voltage::FromVolts(i.ToAmperes() * r.ToOhms());       // V = I · R
}

[[nodiscard]] constexpr Current operator/(Voltage v, Resistance r)
{
    return Current::FromAmperes(v.ToVolts() / r.ToOhms());          // I = V / R
}

[[nodiscard]] constexpr Resistance operator/(Voltage v, Current i)
{
    return Resistance::FromOhms(v.ToVolts() / i.ToAmperes());       // R = V / I
}

[[nodiscard]] constexpr Power operator*(Voltage v, Current i)
{
    return Power::FromWatts(v.ToVolts() * i.ToAmperes());          // P = V · I
}

[[nodiscard]] constexpr Power operator*(Current i, Voltage v)
{
    return Power::FromWatts(v.ToVolts() * i.ToAmperes());          // P = V · I
}

[[nodiscard]] constexpr Current operator/(Power p, Voltage v)
{
    return Current::FromAmperes(p.ToWatts() / v.ToVolts());        // I = P / V
}

[[nodiscard]] constexpr Voltage operator/(Power p, Current i)
{
    return Voltage::FromVolts(p.ToWatts() / i.ToAmperes());        // V = P / I
}


/* Kinematic */

[[nodiscard]] constexpr Distance operator*(Velocity v, TimeSpan t)
{
    auto seconds = static_cast<Unit<Velocity>::ValueType>(t.ToSeconds());
    return Distance::FromMeters(v.ToMetersPerSecond() * seconds); // d = v · t
}

[[nodiscard]] constexpr Velocity operator/(Distance d, TimeSpan t)
{
    auto seconds = static_cast<Unit<Distance>::ValueType>(t.ToSeconds());
    return Velocity::FromMetersPerSecond(d.ToMeters() / seconds); // v = d / t
}

[[nodiscard]] constexpr Acceleration operator/(Velocity v, TimeSpan t)
{
    auto seconds = static_cast<Unit<Velocity>::ValueType>(t.ToSeconds());
    return Acceleration::FromMetersPerSecondSquared(v.ToMetersPerSecond() / seconds); // a = v / t
}

[[nodiscard]] constexpr Velocity operator*(Acceleration a, TimeSpan t)
{
    auto seconds = static_cast<Unit<Acceleration>::ValueType>(t.ToSeconds());
    return Velocity::FromMetersPerSecond(a.ToMetersPerSecondSquared() * seconds); // v = a · t
}

[[nodiscard]] constexpr Distance operator/(Velocity v, Acceleration a)
{
    return Distance::FromMeters(v.ToMetersPerSecond() / a.ToMetersPerSecondSquared()); // d = v / a
}

