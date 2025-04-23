#pragma once

#include "CoreMath.h"
#include "TimeSpan.h"

template<typename T = float>
class FixedAverager
{
public:
    constexpr FixedAverager(float smoothing = 1.0f) : _scale(GetScale(smoothing))
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

        auto scaledValue = value * _scale;
        auto scaledState = _state * (1 - _scale);
        _state = scaledValue + scaledState;

        return _state;
    }

    constexpr T GetValue()
    {
        return _state;
    }

    constexpr void Reset()
    {
        _initialized = false;
    }

    constexpr void SetSmoothing(float smoothing)
    {
        _scale = GetScale(smoothing);
    }

private:
    bool _initialized = false;
    float _scale;
    T _state = T();

    static constexpr float GetScale(float smoothing)
    {
        if(smoothing <= 0.0f)
        {
            return 1.0f;
        }

        return 1 - CoreMath::Exp(-1.0f/smoothing);
    }
};