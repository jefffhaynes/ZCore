#pragma once

#include "AveragerHelper.h"

template<typename T = float>
class FixedAverager
{
public:
    constexpr FixedAverager(TimeSpan updateInterval = AveragerHelper::DefaultTimeConstant, 
                            TimeSpan timeConstant = AveragerHelper::DefaultTimeConstant) : 
                            _alpha(AveragerHelper::GetAlpha(updateInterval, timeConstant))
    {
    }

    constexpr T Update(T value)
    {
        if(!_initialized)
        {
            _state = value;
            _initialized = true;
            return _state;
        }
        
        _state = _state + _alpha * (value - _state);

        return _state;
    }

    constexpr T GetValue() const
    {
        return _state;
    }

    constexpr void Reset()
    {
        _initialized = false;
    }

    constexpr void SetSmoothing(TimeSpan updateInterval, TimeSpan timeConstant)
    {
        _alpha = AveragerHelper::GetAlpha(updateInterval, timeConstant);
    }

private:
    bool _initialized = false;
    float _alpha;
    T _state = T();
};