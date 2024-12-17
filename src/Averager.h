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
        return 1 - CoreMath::Exp(-delta / _tau, 6);
    }
};