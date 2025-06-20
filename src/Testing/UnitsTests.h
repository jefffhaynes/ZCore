#pragma once

#include "Units/Current.h"
#include "Units/Illuminance.h"
#include "Units/Power.h"
#include "Units/Resistance.h"
#include "Units/SignalStrength.h"
#include "Units/Temperature.h"
#include "Units/Voltage.h"
#include "Units/Distance.h"
#include "Units/Velocity.h"
#include "Units/Acceleration.h"

#include <stdint.h>
#include <limits>
#include <cmath>

constexpr float Epsilon = std::numeric_limits<float>::epsilon();

namespace CurrentTests
{
    static_assert(Current::FromMicroamperes(1000000) == Current::FromAmperes(1), "Current conversion from microamperes to amperes failed");
    static_assert(Current::FromMicroamperes(1000) == Current::FromMilliamperes(1), "Current conversion from microamperes to milliamperes failed");

    static_assert(Current::FromMilliamperes(1000) == Current::FromAmperes(1), "Current conversion from milliamperes to amperes failed");
    static_assert(AreAlmostEqual(Current::FromMilliamperes(1).ToMicroamperes(), 1000), "Current conversion from milliamperes to microamperes failed");

    static_assert(Current::FromAmperes(1) == Current::FromMilliamperes(1000), "Current conversion from amperes to milliamperes failed");
    static_assert(Current::FromAmperes(1).ToMicroamperes() == 1000000, "Current conversion from amperes to microamperes failed");

    static_assert(Current::FromAmperes(1).ToAmperes() == 1, "Current conversion to amperes failed");
    static_assert(Current::FromMilliamperes(1000).ToAmperes() == 1, "Current conversion to amperes failed");
    static_assert(Current::FromMicroamperes(1000000).ToAmperes() == 1, "Current conversion to amperes failed");

    static_assert(Current::FromAmperes(1).ToMilliamperes() == 1000, "Current conversion to milliamperes failed");
    static_assert(Current::FromMilliamperes(1).ToMilliamperes() == 1, "Current conversion to milliamperes failed");
    static_assert(Current::FromMicroamperes(1000).ToMilliamperes() == 1, "Current conversion to milliamperes failed");

    static_assert(Current::FromAmperes(1).ToMicroamperes() == 1000000, "Current conversion to microamperes failed");
    static_assert(AreAlmostEqual(Current::FromMilliamperes(1).ToMicroamperes(), 1000), "Current conversion to microamperes failed");
    static_assert(Current::FromMicroamperes(1).ToMicroamperes() == 1, "Current conversion to microamperes failed");
}

namespace IlluminanceTests
{
    static_assert(Illuminance::FromLux(100).ToLux() == 100, "Illuminance conversion to lux failed");
    static_assert(Illuminance::FromLux(0).ToLux() == 0, "Illuminance conversion to lux failed for zero value");

    static_assert(Illuminance::FromLux(1) == Illuminance::FromLux(1), "Equality operator failed");
    static_assert(Illuminance::FromLux(1) != Illuminance::FromLux(2), "Inequality operator failed");
    static_assert(Illuminance::FromLux(1) < Illuminance::FromLux(2), "Less than operator failed");
    static_assert(Illuminance::FromLux(2) > Illuminance::FromLux(1), "Greater than operator failed");
    static_assert(Illuminance::FromLux(2) >= Illuminance::FromLux(2), "Greater than operator failed");
    static_assert(Illuminance::FromLux(2) <= Illuminance::FromLux(2), "Greater than operator failed");

    static_assert(Illuminance::FromLux(-2).Magnitude() == Illuminance::FromLux(2), "Magnitude method failed");
}

namespace PowerTests
{
    static_assert(Power::FromMilliwatts(1000) == Power::FromWatts(1), "Power conversion from milliwatts to watts failed");
    static_assert(AreAlmostEqual(Power::FromMilliwatts(100).ToWatts(), 0.1), "Power conversion from milliwatts to watts failed");

    static_assert(Power::FromWatts(1) == Power::FromMilliwatts(1000), "Power conversion from watts to milliwatts failed");
    static_assert(Power::FromWatts(1).ToMilliwatts() == 1000, "Power conversion from watts to milliwatts failed");

    // Tests for 'ToWatts' and 'ToMilliwatts' methods
    static_assert(Power::FromWatts(1).ToWatts() == 1, "Power conversion to watts failed");
    static_assert(Power::FromMilliwatts(1000).ToWatts() == 1, "Power conversion to watts failed");

    static_assert(Power::FromWatts(1).ToMilliwatts() == 1000, "Power conversion to milliwatts failed");
    static_assert(Power::FromMilliwatts(100).ToMilliwatts() == 100, "Power conversion to milliwatts failed");
}

namespace ResistanceTests
{
    static_assert(Resistance::FromOhms(1000) == Resistance::FromKiloohms(1), "Resistance conversion from ohms to kiloohms failed");
    static_assert(Resistance::FromOhms(1000000) == Resistance::FromMegaohms(1), "Resistance conversion from ohms to megaohms failed");

    static_assert(Resistance::FromKiloohms(1) == Resistance::FromOhms(1000), "Resistance conversion from kiloohms to ohms failed");
    static_assert(Resistance::FromKiloohms(1000) == Resistance::FromMegaohms(1), "Resistance conversion from kiloohms to megaohms failed");

    static_assert(Resistance::FromMegaohms(1) == Resistance::FromOhms(1000000), "Resistance conversion from megaohms to ohms failed");
    static_assert(AreAlmostEqual(Resistance::FromMegaohms(1).ToKiloohms(), 1000), "Resistance conversion from megaohms to kiloohms failed");

    // Tests for 'ToOhms', 'ToKiloohms' and 'ToMegaohms' methods
    static_assert(Resistance::FromOhms(1).ToOhms() == 1, "Resistance conversion to ohms failed");
    static_assert(Resistance::FromKiloohms(1).ToOhms() == 1000, "Resistance conversion to ohms failed");
    static_assert(Resistance::FromMegaohms(1).ToOhms() == 1000000, "Resistance conversion to ohms failed");

    static_assert(Resistance::FromOhms(1000).ToKiloohms() == 1, "Resistance conversion to kiloohms failed");
    static_assert(Resistance::FromKiloohms(1).ToKiloohms() == 1, "Resistance conversion to kiloohms failed");
    static_assert(AreAlmostEqual(Resistance::FromMegaohms(1).ToKiloohms(), 1000), "Resistance conversion to kiloohms failed");

    static_assert(Resistance::FromOhms(1000000).ToMegaohms() == 1, "Resistance conversion to megaohms failed");
    static_assert(Resistance::FromKiloohms(1000).ToMegaohms() == 1, "Resistance conversion to megaohms failed");
    static_assert(Resistance::FromMegaohms(1).ToMegaohms() == 1, "Resistance conversion to megaohms failed");
}

namespace SignalStrengthTests
{
    // Test conversion methods
    static_assert(std::abs(SignalStrength::FromDecibels(10).ToDecibels() - 10) < Epsilon, "Decibels conversion failed");
    static_assert(std::abs(SignalStrength::FromPower(10).ToPower() - 10) < Epsilon, "Power conversion failed");
    static_assert(std::abs(SignalStrength::FromAmplitude(10).ToAmplitude() - 10) < Epsilon, "Amplitude conversion failed");

    // Test operators involving two SignalStrength instances
    static_assert(SignalStrength::FromDecibels(10) == SignalStrength::FromDecibels(10), "Equality operator failed");
    static_assert(SignalStrength::FromDecibels(10) != SignalStrength::FromDecibels(20), "Inequality operator failed");
    static_assert(SignalStrength::FromDecibels(10) < SignalStrength::FromDecibels(20), "Less than operator failed");
    static_assert(SignalStrength::FromDecibels(20) > SignalStrength::FromDecibels(10), "Greater than operator failed");
    static_assert(std::abs((SignalStrength::FromDecibels(10) + SignalStrength::FromDecibels(20)).ToDecibels() - 30) < Epsilon, "Addition operator failed");
    static_assert(std::abs((SignalStrength::FromDecibels(20) - SignalStrength::FromDecibels(10)).ToDecibels() - 10) < Epsilon, "Subtraction operator failed");
    static_assert(std::abs((SignalStrength::FromDecibels(20) / SignalStrength::FromDecibels(10)) - 2) < Epsilon, "Division operator failed");

    // Test operators involving a SignalStrength instance and a double
    static_assert(std::abs((SignalStrength::FromDecibels(10) * 2).ToDecibels() - 20) < Epsilon, "Multiplication operator with double failed");
    static_assert(std::abs((SignalStrength::FromDecibels(20) / 2).ToDecibels() - 10) < Epsilon, "Division operator with double failed");

    // Test friend operators
    static_assert(std::abs((20 / SignalStrength::FromDecibels(10)).ToDecibels() - 2) < Epsilon, "Friend division operator failed");
    static_assert(std::abs((2 * SignalStrength::FromDecibels(10)).ToDecibels() - 20) < Epsilon, "Friend multiplication operator failed");
}

namespace TemperatureTests
{
    static_assert(AreAlmostEqual(Temperature::FromCelsius(0).ToKelvin(), 273.15), "Temperature conversion from Celsius to Kelvin failed");
    static_assert(std::abs(Temperature::FromKelvin(273.15).ToCelsius() - 0) < Epsilon, "Temperature conversion from Kelvin to Celsius failed");

    // Testing Absolute Zero
    static_assert(Temperature::AbsoluteZero == Temperature::FromKelvin(0), "AbsoluteZero constant incorrect");

    // Operators involving two Temperature instances
    static_assert(Temperature::FromKelvin(100) == Temperature::FromKelvin(100), "Equality operator failed");
    static_assert(Temperature::FromKelvin(100) != Temperature::FromKelvin(200), "Inequality operator failed");
    static_assert(Temperature::FromKelvin(100) < Temperature::FromKelvin(200), "Less than operator failed");
    static_assert(Temperature::FromKelvin(200) > Temperature::FromKelvin(100), "Greater than operator failed");
    static_assert(Temperature::FromKelvin(100) <= Temperature::FromKelvin(100), "Less than or equal operator failed");
    static_assert(Temperature::FromKelvin(100) <= Temperature::FromKelvin(200), "Less than or equal operator failed");
    static_assert(Temperature::FromKelvin(100) >= Temperature::FromKelvin(100), "Greater than or equal operator failed");
    static_assert(Temperature::FromKelvin(200) >= Temperature::FromKelvin(100), "Greater than or equal operator failed");

    static_assert(std::abs((Temperature::FromKelvin(100) + Temperature::FromKelvin(200)).ToKelvin() - 300) < Epsilon, "Addition operator failed");
    static_assert(std::abs((Temperature::FromKelvin(200) - Temperature::FromKelvin(100)).ToKelvin() - 100) < Epsilon, "Subtraction operator failed");
    static_assert(std::abs((Temperature::FromKelvin(200) / Temperature::FromKelvin(100)) - 2) < Epsilon, "Division operator failed");

    // Operators involving a Temperature instance and a double
    static_assert(std::abs((Temperature::FromKelvin(100) * 2).ToKelvin() - 200) < Epsilon, "Multiplication operator with double failed");
    static_assert(std::abs((Temperature::FromKelvin(200) / 2).ToKelvin() - 100) < Epsilon, "Division operator with double failed");

    // Assignment operators
    static_assert([]() constexpr {
        auto t1 = Temperature::FromKelvin(100);
        auto t2 = Temperature::FromKelvin(200);
        t1 += t2;
        return std::abs(t1.ToKelvin() - 300) < Epsilon;
    }(), "Addition assignment operator failed");

    static_assert([]() constexpr {
        auto t1 = Temperature::FromKelvin(200);
        auto t2 = Temperature::FromKelvin(100);
        t1 -= t2;
        return std::abs(t1.ToKelvin() - 100) < Epsilon;
    }(), "Subtraction assignment operator failed");

    static_assert([]() constexpr {
        auto t1 = Temperature::FromKelvin(100);
        t1 *= 2;
        return std::abs(t1.ToKelvin() - 200) < Epsilon;
    }(), "Multiplication assignment operator failed");

    static_assert([]() constexpr {
        auto t1 = Temperature::FromKelvin(200);
        t1 /= 2;
        return std::abs(t1.ToKelvin() - 100) < Epsilon;
    }(), "Division assignment operator failed");

    // Friend operators
    static_assert(std::abs((200 / Temperature::FromKelvin(100)).ToKelvin() - 2) < Epsilon, "Friend division operator failed");
    static_assert(std::abs((2 * Temperature::FromKelvin(100)).ToKelvin() - 200) < Epsilon, "Friend multiplication operator failed");
}

namespace VoltageTests
{
   // Conversion methods
    static_assert(std::abs(Voltage::FromMillivolts(1000).ToVolts() - 1) < Epsilon, "Voltage conversion from millivolts to volts failed");
    static_assert(std::abs(Voltage::FromVolts(1).ToMillivolts() - 1000) < Epsilon, "Voltage conversion from volts to millivolts failed");

    // Operators involving two Voltage instances
    static_assert(Voltage::FromVolts(1) == Voltage::FromVolts(1), "Equality operator failed");
    static_assert(Voltage::FromVolts(1) != Voltage::FromVolts(2), "Inequality operator failed");
    static_assert(Voltage::FromVolts(1) < Voltage::FromVolts(2), "Less than operator failed");
    static_assert(Voltage::FromVolts(2) > Voltage::FromVolts(1), "Greater than operator failed");
    static_assert(Voltage::FromVolts(2) >= Voltage::FromVolts(2), "Greater than operator failed");
    static_assert(Voltage::FromVolts(3) >= Voltage::FromVolts(2), "Greater than operator failed");
    static_assert(Voltage::FromVolts(2) <= Voltage::FromVolts(2), "Greater than operator failed");
    static_assert(Voltage::FromVolts(2) <= Voltage::FromVolts(3), "Greater than operator failed");

    static_assert(std::abs((Voltage::FromVolts(1) + Voltage::FromVolts(2)).ToVolts() - 3) < Epsilon, "Addition operator failed");
    static_assert(std::abs((Voltage::FromVolts(2) - Voltage::FromVolts(1)).ToVolts() - 1) < Epsilon, "Subtraction operator failed");
    static_assert(std::abs((Voltage::FromVolts(2) / Voltage::FromVolts(1)) - 2) < Epsilon, "Division operator failed");

    // Operators involving a Voltage instance and a double
    static_assert(std::abs((Voltage::FromVolts(2) * 2).ToVolts() - 4) < Epsilon, "Multiplication operator with double failed");
    static_assert(std::abs((Voltage::FromVolts(4) / 2).ToVolts() - 2) < Epsilon, "Division operator with double failed");

    // Assignment operators
    static_assert([]() constexpr {
        auto v1 = Voltage::FromVolts(1);
        auto v2 = Voltage::FromVolts(2);
        v1 += v2;
        return std::abs(v1.ToVolts() - 3) < Epsilon;
    }(), "Addition assignment operator failed");

    static_assert([]() constexpr {
        auto v1 = Voltage::FromVolts(1);
        auto v2 = Voltage::FromVolts(2);
        v1 -= v2;
        return std::abs(v1.ToVolts() + 1) < Epsilon;
    }(), "Subtraction assignment operator failed");

    static_assert([]() constexpr {
        auto v1 = Voltage::FromVolts(2);
        v1 *= 2;
        return std::abs(v1.ToVolts() - 4) < Epsilon;
    }(), "Multiplication assignment operator failed");

    static_assert([]() constexpr {
        auto v1 = Voltage::FromVolts(4);
        v1 /= 2;
        return std::abs(v1.ToVolts() - 2) < Epsilon;
    }(), "Division assignment operator failed");

    // Other methods
    static_assert(std::abs(Voltage::FromVolts(-2).Magnitude().ToVolts() - 2) < Epsilon, "Magnitude method failed");

    // Friend operators
    static_assert(std::abs((2 / Voltage::FromVolts(2)).ToVolts() - 1) < Epsilon, "Friend division operator failed");
    static_assert(std::abs((2 * Voltage::FromVolts(2)).ToVolts() - 4) < Epsilon, "Friend multiplication operator failed");
}

namespace DistanceTests
{
    // Test FromMeters Static Method
    static_assert(Distance::FromMeters(5.0f).ToMeters() == 5.0f, "FromMeters method failed");

    // Test Arithmetic Operators (+, -, *, /)
    static_assert((Distance::FromMeters(5.0f) + Distance::FromMeters(3.0f)).ToMeters() == 8.0f, "Operator+ failed");
    static_assert((Distance::FromMeters(5.0f) - Distance::FromMeters(3.0f)).ToMeters() == 2.0f, "Operator- failed");
    static_assert((Distance::FromMeters(5.0f) * 2.0f).ToMeters() == 10.0f, "Operator* (scalar multiplication) failed");
    static_assert((Distance::FromMeters(10.0f) / 2.0f).ToMeters() == 5.0f, "Operator/ (scalar division) failed");

    // Test Division and Multiplication with float
    static_assert((5.0f / Distance::FromMeters(2.0f)).ToMeters() == 2.5f, "Operator/ (division with float) failed");
    static_assert((5.0f * Distance::FromMeters(2.0f)).ToMeters() == 10.0f, "Operator* (multiplication with float) failed");

    // Test Comparison Operators (==, !=, <, >)
    static_assert(Distance::FromMeters(5.0f) == Distance::FromMeters(5.0f), "Operator== failed");
    static_assert(Distance::FromMeters(5.0f) != Distance::FromMeters(6.0f), "Operator!= failed");
    static_assert(Distance::FromMeters(4.0f) < Distance::FromMeters(5.0f), "Operator< failed");
    static_assert(Distance::FromMeters(6.0f) > Distance::FromMeters(5.0f), "Operator> failed");

    // Test Compound Assignment Operators (+=, -=, *=, /=)
    static_assert([] {
        Distance d = Distance::FromMeters(5.0f);
        d += Distance::FromMeters(3.0f);
        return d.ToMeters() == 8.0f;
    }(), "Operator+= failed");
}

namespace VelocityTests
{
    // Test FromMetersPerSecond Static Method
    static_assert(Velocity::FromMetersPerSecond(5.0f).ToMetersPerSecond() == 5.0f, "FromMetersPerSecond method failed");

    // Test Arithmetic Operators (+, -, *, /)
    static_assert((Velocity::FromMetersPerSecond(5.0f) + Velocity::FromMetersPerSecond(3.0f)).ToMetersPerSecond() == 8.0f, "Operator+ failed");
    static_assert((Velocity::FromMetersPerSecond(5.0f) - Velocity::FromMetersPerSecond(3.0f)).ToMetersPerSecond() == 2.0f, "Operator- failed");
    static_assert((Velocity::FromMetersPerSecond(5.0f) * 2.0f).ToMetersPerSecond() == 10.0f, "Operator* (scalar multiplication) failed");
    static_assert((Velocity::FromMetersPerSecond(10.0f) / 2.0f).ToMetersPerSecond() == 5.0f, "Operator/ (scalar division) failed");

    // Test Division and Multiplication with float
    static_assert((5.0f / Velocity::FromMetersPerSecond(2.0f)).ToMetersPerSecond() == 2.5f, "Operator/ (division with float) failed");
    static_assert((5.0f * Velocity::FromMetersPerSecond(2.0f)).ToMetersPerSecond() == 10.0f, "Operator* (multiplication with float) failed");

    // Test Comparison Operators (==, !=, <, >)
    static_assert(Velocity::FromMetersPerSecond(5.0f) == Velocity::FromMetersPerSecond(5.0f), "Operator== failed");
    static_assert(Velocity::FromMetersPerSecond(5.0f) != Velocity::FromMetersPerSecond(6.0f), "Operator!= failed");
    static_assert(Velocity::FromMetersPerSecond(4.0f) < Velocity::FromMetersPerSecond(5.0f), "Operator< failed");
    static_assert(Velocity::FromMetersPerSecond(6.0f) > Velocity::FromMetersPerSecond(5.0f), "Operator> failed");

    // Unit conversion
    static_assert(Velocity::FromMetersPerSecond(1) * TimeSpan::FromSeconds(1) == Distance::FromMeters(1), "Velocity * TimeSpan failed");
}

namespace AccelerationTests
{
    // Test FromMetersPerSecondSquared Static Method
    static_assert(Acceleration::FromMetersPerSecondSquared(5.0f).ToMetersPerSecondSquared() == 5.0f, "FromMetersPerSecondSquared method failed");

    // Test Arithmetic Operators (+, -, *, /)
    static_assert((Acceleration::FromMetersPerSecondSquared(5.0f) + Acceleration::FromMetersPerSecondSquared(3.0f)).ToMetersPerSecondSquared() == 8.0f, "Operator+ failed");
    static_assert((Acceleration::FromMetersPerSecondSquared(5.0f) - Acceleration::FromMetersPerSecondSquared(3.0f)).ToMetersPerSecondSquared() == 2.0f, "Operator- failed");
    static_assert((Acceleration::FromMetersPerSecondSquared(5.0f) * 2.0f).ToMetersPerSecondSquared() == 10.0f, "Operator* (scalar multiplication) failed");
    static_assert((Acceleration::FromMetersPerSecondSquared(10.0f) / 2.0f).ToMetersPerSecondSquared() == 5.0f, "Operator/ (scalar division) failed");

    // Test Division and Multiplication with float
    static_assert((5.0f / Acceleration::FromMetersPerSecondSquared(2.0f)).ToMetersPerSecondSquared() == 2.5f, "Operator/ (division with float) failed");
    static_assert((5.0f * Acceleration::FromMetersPerSecondSquared(2.0f)).ToMetersPerSecondSquared() == 10.0f, "Operator* (multiplication with float) failed");

    // Test Comparison Operators (==, !=, <, >)
    static_assert(Acceleration::FromMetersPerSecondSquared(5.0f) == Acceleration::FromMetersPerSecondSquared(5.0f), "Operator== failed");
    static_assert(Acceleration::FromMetersPerSecondSquared(5.0f) != Acceleration::FromMetersPerSecondSquared(6.0f), "Operator!= failed");
    static_assert(Acceleration::FromMetersPerSecondSquared(4.0f) < Acceleration::FromMetersPerSecondSquared(5.0f), "Operator< failed");
    static_assert(Acceleration::FromMetersPerSecondSquared(6.0f) > Acceleration::FromMetersPerSecondSquared(5.0f), "Operator> failed");

    // Test Compound Assignment Operators (+=, -=, *=, /=)
    static_assert([] {
        Acceleration a = Acceleration::FromMetersPerSecondSquared(5.0f);
        a += Acceleration::FromMetersPerSecondSquared(3.0f);
        return a.ToMetersPerSecondSquared() == 8.0f;
    }(), "Operator+= failed");
    static_assert([] {
        Acceleration a = Acceleration::FromMetersPerSecondSquared(5.0f);
        a -= Acceleration::FromMetersPerSecondSquared(3.0f);
        return a.ToMetersPerSecondSquared() == 2.0f;
    }(), "Operator-= failed");
    static_assert([] {
        Acceleration a = Acceleration::FromMetersPerSecondSquared(5.0f);
        a *= 2.0f;
        return a.ToMetersPerSecondSquared() == 10.0f;
    }(), "Operator*= failed");
    static_assert([] {
        Acceleration a = Acceleration::FromMetersPerSecondSquared(10.0f);
        a /= 2.0f;
        return a.ToMetersPerSecondSquared() == 5.0f;
    }(), "Operator/= failed");

    // Test Magnitude
    static_assert(Acceleration::FromMetersPerSecondSquared(-5.0f).Magnitude().ToMetersPerSecondSquared() == 5.0f, "Magnitude method failed");

    // Test Zero Constant
    static_assert(Acceleration::Zero.ToMetersPerSecondSquared() == 0.0f, "Zero constant failed");
}