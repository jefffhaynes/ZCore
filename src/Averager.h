#pragma once

#include "Clock.h"
#include "CoreMath.h"

template<typename T = float>
class Averager
{
public:
    constexpr Averager(TimeSpan tau = TimeSpan::FromSeconds(1)) : _tau(tau)
    {
    }

    constexpr T Update(T value, TimeSpan time = Clock::GetUptime())
    {
        auto scale = GetScale(time);
        auto scaledValue = value * scale;
        auto scaledState = _state * (1 - scale);
        _state = scaledValue + scaledState;
        _lastTime = time;

        return _state;
    }

    constexpr T GetValue()
    {
        return _state;
    }

private:
    static constexpr float MaxExponent = 5;
    static constexpr float MinExponent = -5;

    bool _initialized = false;
    TimeSpan _tau;
    TimeSpan _lastTime;
    T _state = T();

    constexpr float GetScale(TimeSpan time)
    {
        if(!_initialized)
        {
            _initialized = true;
            return 1;
        }

        auto delta = time - _lastTime;

        if (_tau == TimeSpan::Zero)
        {
            // zero time constant means instant change
            return 1;
        }

        auto exponent = -delta / _tau;

        if (exponent < MinExponent)
        {
            // large time has lapsed, instant change
            return 1;
        }

        if (exponent > MaxExponent)
        {
            // small time has lapsed, no change
            return 0;
        }

        return 1 - CoreMath::Exp(exponent);
    }
};