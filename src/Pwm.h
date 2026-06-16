
#include "TimeSpan.h"
#include "Range.h"
#include "ErrorConverter.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/pwm.h>

class Pwm
{
public:
    constexpr Pwm(pwm_dt_spec pwm)
        : _pwm(pwm), _period(TimeSpan::FromNanoseconds(pwm.period)), _dutyCycle(0.0f)
    {
    }

    constexpr TimeSpan Period() const { return _period; }
    constexpr float DutyCycle() const { return _dutyCycle; }

    ReturnCode SetPeriod(const TimeSpan& period)
    {
        _period = period;
        return Apply();
    }

    ReturnCode SetDutyCycle(float dutyCycle)
    {
        _dutyCycle = Range<float>::ZeroToOne.Clamp(dutyCycle);
        return Apply();
    }

    // Absolute pulse width within the current period (e.g. servo-style timing).
    ReturnCode SetPulseWidth(const TimeSpan& pulse)
    {
        auto fraction = pulse.ToNanoseconds() / _period.ToNanoseconds();
        return SetDutyCycle(static_cast<float>(fraction));
    }

private:
    ReturnCode Apply()
    {
        auto period = _period.ToNanoseconds();
        auto pulse = _dutyCycle * period;
        auto err = pwm_set_dt(&_pwm, static_cast<uint32_t>(period), static_cast<uint32_t>(pulse));
        return ErrorConverter::Convert(err);
    }

    pwm_dt_spec _pwm;
    TimeSpan _period;
    float _dutyCycle;
};
