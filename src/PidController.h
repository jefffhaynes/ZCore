#pragma once

#include "TimeSpan.h"
#include "Clock.h"
#include "Range.h"

class PidController
{
public:
    constexpr PidController(float kp = 0, float ki = 0, float kd = 0, float setPoint = 0)
        : _kp(kp), _ki(ki), _kd(kd), _setPoint(setPoint)
    {
    }

    constexpr float Update(float measuredValue, TimeSpan time = Clock::GetUptime())
    {
        // Calculate error between set point and measured value
        auto error = _setPoint - measuredValue;

        if (!_initialized)
        {
            _previousError = error;
            _lastUpdate = time;
            _integral = 0;
            _initialized = true;
            return 0;
        }

        // Calculate time since last update
        auto dt = time - _lastUpdate;
        auto dtSeconds = static_cast<float>(dt.ToSeconds());

        // Proportional term
        auto Pout = _kp * error;

        // A second update inside the same clock tick (or a clock step backwards)
        // gives dt == 0. The derivative would then be +/-inf or NaN, and even a
        // zero gain can't neutralise that (inf * 0 is NaN), so the caller would
        // receive NaN. Treat it as a repeated sample: proportional plus the
        // integral so far, no time-dependent terms, no state update.
        if (!(dtSeconds > 0.0f))
        {
            return Pout + _ki * _integral;
        }

        _lastUpdate = time;

        // Integral term
        _integral += dtSeconds * error;

        _integral = _integralLimits.Clamp(_integral);

        auto Iout = _ki * _integral;

        // Derivative term
        auto derivative = (error - _previousError) / dtSeconds;
        auto Dout = derivative * _kd;

        // Total output
        auto output = Pout + Iout + Dout;

        // Save error for next iteration
        _previousError = error;

        return output;
    }

    constexpr void SetP(float p)
    {
        _kp = p;
    }

    constexpr void SetI(float i)
    {
        _ki = i;
    }

    constexpr void SetD(float d)
    {
        _kd = d;
    }

    constexpr void SetSetPoint(float setPoint)
    {
        _setPoint = setPoint;
    }

    constexpr float GetSetPoint() const
    {
        return _setPoint;
    }

    constexpr void SetIntegralLimits(Range<float> limits)
    {
        _integralLimits = limits;
    }

    constexpr void Reset()
    {
        _initialized = false;
    }

private:
    bool _initialized = false;
    TimeSpan _lastUpdate;

    float _kp;
    float _ki;
    float _kd;
    float _setPoint;
    Range<float> _integralLimits = Range<float>(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
    float _integral;
    float _previousError;
};