#pragma once

#include "Clock.h"
#include "AveragerHelper.h"
#include "Units/Unit.h"
#include <numbers>

// One Euro Filter — adaptive low-pass filter that suppresses jitter at rest while
// remaining responsive to fast movements.
//
// The filter tracks the signal's derivative. When the derivative is small (near
// stationary) the cutoff stays at minCutoff, giving heavy smoothing. As the signal
// moves faster the cutoff rises by beta * |derivative|, reducing lag during
// intentional motion.
//
// Parameters:
//   minCutoff  - cutoff frequency (Hz) when the signal is stationary; lower = smoother
//   beta       - derivative gain (Hz per unit/s); higher = faster response to motion
//   dCutoff    - cutoff frequency (Hz) for the internal derivative smoother (default 1 Hz)
//
// Casiez et al., "1€ Filter: A Simple Speed-based Low-pass Filter for Noisy Input
// in Interactive Systems", CHI 2012.
template<typename T = float>
class OneEuroFilter
{
public:
    constexpr OneEuroFilter(float minCutoff = 1.0f, float beta = 2.0f, float dCutoff = 1.0f)
        : _minCutoff(minCutoff), _beta(beta), _dCutoff(dCutoff)
    {
    }

    constexpr T Update(T value, TimeSpan time = Clock::GetUptime())
    {
        if (!_initialized)
        {
            _initialized = true;
            _lastTime = time;
            _x = value;
            _prevInput = value;
            return value;
        }

        auto dt = static_cast<float>((time - _lastTime).ToSeconds());
        _lastTime = time;

        if (dt <= 0.0f)
            return _x;

        // Derivative of the raw input — use previous input, not filter state, so that
        // consecutive identical samples (quantized ADC) produce zero derivative rather
        // than keeping the filter open while the state converges.
        T rawDx = (value - _prevInput) * (1.0f / dt);
        _prevInput = value;
        auto dAlpha = AlphaFromCutoff(_dCutoff, dt);
        _dx = _dx + (rawDx - _dx) * dAlpha;

        // Raise the cutoff proportionally to the derivative magnitude
        float cutoff = _minCutoff + _beta * MagnitudeAsFloat(_dx);

        // Apply the adaptive low-pass to the signal
        auto alpha = AlphaFromCutoff(cutoff, dt);
        _x = _x + (value - _x) * alpha;

        return _x;
    }

    constexpr T GetValue() const { return _x; }
    constexpr void Reset() { _initialized = false; }

    constexpr void SetMinCutoff(float minCutoff) { _minCutoff = minCutoff; }
    constexpr void SetBeta(float beta) { _beta = beta; }
    constexpr float GetMinCutoff() const { return _minCutoff; }
    constexpr float GetBeta() const { return _beta; }

private:
    float _minCutoff;
    float _beta;
    float _dCutoff;
    bool _initialized = false;
    TimeSpan _lastTime;
    T _x{};
    T _dx{};
    T _prevInput{};

    static constexpr float AlphaFromCutoff(float cutoff, float dt)
    {
        constexpr float TwoPi = 2.0f * std::numbers::pi_v<float>;
        float tau = 1.0f / (TwoPi * cutoff);
        return AveragerHelper::GetAlpha(TimeSpan::FromSeconds(dt), TimeSpan::FromSeconds(tau));
    }

    template<typename U>
    static constexpr float MagnitudeAsFloat(const U& value)
    {
        if constexpr (IsUnit<U>)
            return static_cast<float>(value.Magnitude().ToRadians());
        else
            return value < 0.0f ? -value : value;
    }
};
