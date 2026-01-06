#pragma once

#include "Clock.h"
#include "AveragerHelper.h"

template<typename T = float>
class Averager
{
public:
    constexpr Averager(TimeSpan timeConstant = AveragerHelper::DefaultTimeConstant) : _timeConstant(timeConstant)
    {
    }

    constexpr T Update(T value, TimeSpan time = Clock::GetUptime())
    {
        auto alpha = GetAlpha(time);
        _state = _state + alpha * (value - _state);

        _lastTime = time;

        return _state;
    }

    constexpr T GetValue()
    {
        return _state;
    }

    constexpr void SetTimeConstant(TimeSpan timeConstant)
    {
        _timeConstant = timeConstant;
    }

    constexpr TimeSpan GetTimeConstant()
    {
        return _timeConstant;
    }

    constexpr void Reset()
    {
        _initialized = false;
    }

private:
    bool _initialized = false;
    TimeSpan _timeConstant;
    TimeSpan _lastTime;
    T _state = T();

    constexpr float GetAlpha(TimeSpan time)
    {
        if(!_initialized)
        {
            _initialized = true;
            return 1;
        }

        auto delta = time - _lastTime;
        return AveragerHelper::GetAlpha(delta, _timeConstant);
    }
};