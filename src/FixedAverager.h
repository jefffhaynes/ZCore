#pragma once

#include "CoreMath.h"

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

    constexpr void SetSmoothing(float smoothing)
    {
        _scale = GetScale(smoothing);
    }

    constexpr void Reset()
    {
        _initialized = false;
    }

private:
    bool _initialized = false;
    float _scale;
    T _state = T();

    static constexpr float GetScale(float smoothing)
    {
        return CoreMath::Exp(-smoothing);
    }
};