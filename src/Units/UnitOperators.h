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
[[nodiscard]] constexpr Frequency::ValueType operator*(Frequency f, TimeSpan period)
{
    auto seconds = static_cast<Frequency::ValueType>(period.ToSeconds());
    return f.ToHertz() * seconds; 
}

[[nodiscard]] constexpr Frequency::ValueType operator*(TimeSpan t, Frequency f)
{
    return f * t;
}


/* Electrical */
[[nodiscard]] constexpr Voltage operator*(Current i, Resistance r)
{
    return Voltage::FromVolts(i.ToAmperes() * r.ToOhms());       // V = I · R
}

[[nodiscard]] constexpr Voltage operator*(Resistance r, Current i)
{
    return i * r;       // V = I · R
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
    return i * v; // P = V · I
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
    auto seconds = static_cast<Velocity::ValueType>(t.ToSeconds());
    return Distance::FromMeters(v.ToMetersPerSecond() * seconds); // d = v · t
}

[[nodiscard]] constexpr Velocity operator/(Distance d, TimeSpan t)
{
    auto seconds = static_cast<Distance::ValueType>(t.ToSeconds());
    return Velocity::FromMetersPerSecond(d.ToMeters() / seconds); // v = d / t
}

[[nodiscard]] constexpr Acceleration operator/(Velocity v, TimeSpan t)
{
    auto seconds = static_cast<Velocity::ValueType>(t.ToSeconds());
    return Acceleration::FromMetersPerSecondSquared(v.ToMetersPerSecond() / seconds); // a = v / t
}

[[nodiscard]] constexpr Velocity operator*(Acceleration a, TimeSpan t)
{
    auto seconds = static_cast<Acceleration::ValueType>(t.ToSeconds());
    return Velocity::FromMetersPerSecond(a.ToMetersPerSecondSquared() * seconds); // v = a · t
}

[[nodiscard]] constexpr TimeSpan operator/(Velocity v, Acceleration a)
{
    return TimeSpan::FromSeconds(v.ToMetersPerSecond() /
                                 a.ToMetersPerSecondSquared());   // t = v / a
}
