#pragma once

#include "TimeSpan.h"
#include "Frequency.h"
#include "Current.h"
#include "Resistance.h"
#include "Voltage.h"
#include "Power.h"
#include "SignalStrength.h"
#include "Distance.h"
#include "Velocity.h"
#include "Acceleration.h"
#include "Mass.h"
#include "Force.h"
#include "Angle.h"
#include "Illuminance.h"
#include "Temperature.h"


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

/* Time UDLs */

[[nodiscard]] constexpr TimeSpan operator"" _ns(long double nanoseconds)
{
    return TimeSpan::FromNanoseconds(nanoseconds);
}

[[nodiscard]] constexpr TimeSpan operator"" _us(long double microseconds)
{
    return TimeSpan::FromMicroseconds(microseconds);
}

[[nodiscard]] constexpr TimeSpan operator"" _ms(long double milliseconds)
{
    return TimeSpan::FromMilliseconds(milliseconds);
}

[[nodiscard]] constexpr TimeSpan operator"" _s(long double seconds)
{
    return TimeSpan::FromSeconds(seconds);
}

[[nodiscard]] constexpr TimeSpan operator"" _min(long double minutes)
{
    return TimeSpan::FromMinutes(minutes);
}

[[nodiscard]] constexpr TimeSpan operator"" _hrs(long double hours)
{
    return TimeSpan::FromHours(hours);
}

[[nodiscard]] constexpr TimeSpan operator"" _days(long double days)
{
    return TimeSpan::FromDays(days);
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


/* Electrical UDLs */

[[nodiscard]] constexpr Frequency operator"" _Hz(long double hertz)
{
    return Frequency::FromHertz(static_cast<Frequency::ValueType>(hertz));
}

[[nodiscard]] constexpr Frequency operator"" _kHz(long double kilohertz)
{
    return Frequency::FromKilohertz(static_cast<Frequency::ValueType>(kilohertz));
}

[[nodiscard]] constexpr Frequency operator"" _MHz(long double megahertz)
{
    return Frequency::FromMegahertz(static_cast<Frequency::ValueType>(megahertz));
}

[[nodiscard]] constexpr Voltage operator"" _mV(long double millivolts)
{
    return Voltage::FromMillivolts(static_cast<Voltage::ValueType>(millivolts));
}

[[nodiscard]] constexpr Voltage operator"" _V(long double volts)
{
    return Voltage::FromVolts(static_cast<Voltage::ValueType>(volts));
}

[[nodiscard]] constexpr Current operator"" _mA(long double milliamperes)
{
    return Current::FromMilliamperes(static_cast<Current::ValueType>(milliamperes));
}

[[nodiscard]] constexpr Current operator"" _A(long double amperes)
{
    return Current::FromAmperes(static_cast<Current::ValueType>(amperes));
}

[[nodiscard]] constexpr Current operator"" _uA(long double microamperes)
{
    return Current::FromMicroamperes(static_cast<Current::ValueType>(microamperes));
}

[[nodiscard]] constexpr Resistance operator"" _Ohm(long double ohms)
{
    return Resistance::FromOhms(static_cast<Resistance::ValueType>(ohms));
}

[[nodiscard]] constexpr Resistance operator"" _kOhm(long double kiloohms)
{
    return Resistance::FromKiloohms(static_cast<Resistance::ValueType>(kiloohms));
}

[[nodiscard]] constexpr Resistance operator"" _MOhm(long double megaohms)
{
    return Resistance::FromMegaohms(static_cast<Resistance::ValueType>(megaohms));
}

[[nodiscard]] constexpr Resistance operator"" _Ω(long double ohms)
{
    return Resistance::FromOhms(static_cast<Resistance::ValueType>(ohms));
}

[[nodiscard]] constexpr Resistance operator"" _kΩ(long double ohms)
{
    return Resistance::FromKiloohms(static_cast<Resistance::ValueType>(ohms));
}

[[nodiscard]] constexpr Resistance operator"" _MΩ(long double ohms)
{
    return Resistance::FromMegaohms(static_cast<Resistance::ValueType>(ohms));
}

[[nodiscard]] constexpr Power operator"" _mW(long double milliwatts)
{
    return Power::FromMilliwatts(static_cast<Power::ValueType>(milliwatts));
}

[[nodiscard]] constexpr Power operator"" _W(long double watts)
{
    return Power::FromWatts(static_cast<Power::ValueType>(watts));
}

[[nodiscard]] constexpr Power operator"" _dBm(long double decibelMilliwatts)
{
    return Power::FromDecibelMilliwatts(static_cast<Power::ValueType>(decibelMilliwatts));
}

[[nodiscard]] constexpr SignalStrength operator"" _dB(long double decibels)
{
    return SignalStrength::FromDecibels(static_cast<SignalStrength::ValueType>(decibels));
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



/* Kinematic UDLs */

[[nodiscard]] constexpr Distance operator"" _mm(long double millimeters)
{
    return Distance::FromMillimeters(static_cast<Distance::ValueType>(millimeters));
}

[[nodiscard]] constexpr Distance operator"" _cm(long double centimeters)
{
    return Distance::FromCentimeters(static_cast<Distance::ValueType>(centimeters));
}

[[nodiscard]] constexpr Distance operator"" _km(long double kilometers)
{
    return Distance::FromKilometers(static_cast<Distance::ValueType>(kilometers));
}

[[nodiscard]] constexpr Distance operator"" _m(long double meters)
{
    return Distance::FromMeters(static_cast<Distance::ValueType>(meters));
}

[[nodiscard]] constexpr Velocity operator"" _mps(long double metersPerSecond)
{
    return Velocity::FromMetersPerSecond(static_cast<Velocity::ValueType>(metersPerSecond));
}

[[nodiscard]] constexpr Acceleration operator"" _mps2(long double metersPerSecondSquared)
{
    return Acceleration::FromMetersPerSecondSquared(static_cast<Acceleration::ValueType>(metersPerSecondSquared));
}


/* Mass and Mechanics */

[[nodiscard]] constexpr Force operator*(Mass m, Acceleration a)
{
    return Force::FromNewtons(m.ToKilograms() * a.ToMetersPerSecondSquared()); // F = m · a
}

[[nodiscard]] constexpr Force operator*(Acceleration a, Mass m)
{
    return m * a; // F = m · a
}

[[nodiscard]] constexpr Acceleration operator/(Force f, Mass m)
{
    return Acceleration::FromMetersPerSecondSquared(f.ToNewtons() / m.ToKilograms()); // a = F / m
}

[[nodiscard]] constexpr Mass operator/(Force f, Acceleration a)
{
    return Mass::FromKilograms(f.ToNewtons() / a.ToMetersPerSecondSquared()); // m = F / a
}


/* Mass UDLs */

[[nodiscard]] constexpr Mass operator"" _g(long double grams)
{
    return Mass::FromGrams(static_cast<Mass::ValueType>(grams));
}

[[nodiscard]] constexpr Mass operator"" _kg(long double kilograms)
{
    return Mass::FromKilograms(static_cast<Mass::ValueType>(kilograms));
}

[[nodiscard]] constexpr Force operator"" _newt(long double newtons) // C++ forbids "_N" as a suffix
{
    return Force::FromNewtons(static_cast<Force::ValueType>(newtons));
}

[[nodiscard]] constexpr Force operator"" _kN(long double kilonewtons)
{
    return Force::FromKilonewtons(static_cast<Force::ValueType>(kilonewtons));
}

[[nodiscard]] constexpr Force operator"" _lbf(long double poundsForce)
{
    return Force::FromPoundsForce(static_cast<Force::ValueType>(poundsForce));
}


/* Angle UDLs */

[[nodiscard]] constexpr Angle operator"" _deg(long double degrees)
{
    return Angle::FromDegrees(static_cast<Angle::ValueType>(degrees));
}

[[nodiscard]] constexpr Angle operator"" _rad(long double radians)
{
    return Angle::FromRadians(static_cast<Angle::ValueType>(radians));
}


/* Illuminance UDLs */

[[nodiscard]] constexpr Illuminance operator"" _lx(long double lux)
{
    return Illuminance::FromLux(static_cast<Illuminance::ValueType>(lux));
}


/* Temperature UDLs */

[[nodiscard]] constexpr Temperature operator"" _degK(long double kelvin)
{
    return Temperature::FromKelvin(static_cast<Temperature::ValueType>(kelvin));
}

[[nodiscard]] constexpr Temperature operator"" _degC(long double celsius)
{
    return Temperature::FromCelsius(static_cast<Temperature::ValueType>(celsius));
}

[[nodiscard]] constexpr Temperature operator"" _degF(long double fahrenheit)
{
    return Temperature::FromFahrenheit(static_cast<Temperature::ValueType>(fahrenheit));
}