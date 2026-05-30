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

        _lastUpdate = time;
        
        // Proportional term
        auto Pout = _kp * error;

        // Integral term
        auto dtSeconds = static_cast<float>(dt.ToSeconds());
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
    Range<float> _integralLimits = Range<float>(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
    float _integral;
    float _previousError;
};