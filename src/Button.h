#pragma once

#include "ReturnCode.h"
#include "Gpio.h"
#include "Clock.h"

class Button
{
public:
    Button(const gpio_dt_spec spec) : _gpio(spec, true)
    {
        _gpio.Interrupt.Subscribe(OnPress, this);
    }

    ReturnCode Initialize()
    {
        auto rc = _gpio.Configure(GpioDirection::In);
        CHECK_RETURN_CODE(rc);

        return _gpio.ConfigureInterrupt();
    }

    EventHandler<> Pressed;

private:
    static constexpr TimeSpan DebounceTime = TimeSpan::FromMilliseconds(250);

    Gpio _gpio;
    TimeSpan _lastPressTime;

    ReturnCode OnPress()
    {
        auto time = Clock::GetUptime();
        auto lapsed = time - _lastPressTime;
        if(lapsed > DebounceTime)
        {
            auto rc = Pressed.Invoke();
            CHECK_RETURN_CODE(rc);

            _lastPressTime = time;
        }

        return ReturnCode::Success;
    }

    static ReturnCode OnPress(void* context)
    {
        return (static_cast<Button*>(context))->OnPress();
    }
};