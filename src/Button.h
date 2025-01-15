#pragma once

#include "ReturnCode.h"
#include "Gpio.h"
#include "Timer.h"
#include "Work.h"


class Button
{
public:
    Button(const gpio_dt_spec spec) : _gpio(spec, true)
    {
        _gpio.Interrupt.Subscribe<Button, &Button::OnToggle>(this);
        _debounceTimer.Expired.Subscribe<Button, &Button::OnDebounceTimeout>(this);
        _clickTimer.Expired.Subscribe<Button, &Button::OnClickTimeout>(this);
        _holdTimer.Expired.Subscribe<Button, &Button::OnHoldTimeout>(this);
    }

    ReturnCode Initialize()
    {
        auto rc = _gpio.Configure(GpioDirection::In);
        CHECK_RETURN_CODE(rc);

        return _gpio.ConfigureInterrupt();
    }

    ReturnCode IsPressed(bool& pressed)
    {
        return _gpio.IsSet(pressed);
    }

    EventHandler<> Pressed;
    EventHandler<> SingleClicked;
    EventHandler<> DoubleClicked;
    EventHandler<> Held;

private:
    static constexpr TimeSpan DebounceTimeout = TimeSpan::FromMilliseconds(20);
    static constexpr TimeSpan ClickTimeout = TimeSpan::FromMilliseconds(750);
    static constexpr TimeSpan HoldTimeout = TimeSpan::FromMilliseconds(3000);

    Gpio _gpio;
    Timer _debounceTimer;
    Timer _clickTimer;
    Timer _holdTimer;
    uint32_t _clickCount = 0;


    ReturnCode OnPressed()
    {
        if (!_clickTimer.IsRunning())
        {
            _clickCount = 0;
        }

        auto rc = _clickTimer.Start(ClickTimeout);
        CHECK_RETURN_CODE(rc);

        rc = _holdTimer.Start(HoldTimeout);
        CHECK_RETURN_CODE(rc);

        _clickCount++;

        return Pressed.Invoke();
    }

    ReturnCode OnReleased()
    {
        return _holdTimer.Stop();
    }

    ReturnCode OnToggle()
    {
        if (_debounceTimer.IsRunning())
        {
            return ReturnCode::Success;
        }

        return _debounceTimer.Start(DebounceTimeout);
    }

    ReturnCode OnDebounceTimeout()
    {
        bool pressed;
        auto rc = IsPressed(pressed);
        CHECK_RETURN_CODE(rc);

        return pressed ? OnPressed() : OnReleased();
    }

    ReturnCode OnClickTimeout()
    {
        bool pressed;
        auto rc = IsPressed(pressed);
        CHECK_RETURN_CODE(rc);

        if (pressed)
        {
            return ReturnCode::Success;
        }

        switch (_clickCount)
        {
            case 1:
                rc = SingleClicked.Invoke(); break;
            case 2:
                rc = DoubleClicked.Invoke(); break;
            default:
                rc = ReturnCode::Success;
        }

        _clickCount = 0;

        return rc;
    }

    ReturnCode OnHoldTimeout()
    {
        bool pressed;
        auto rc = IsPressed(pressed);
        CHECK_RETURN_CODE(rc);

        if (!pressed)
        {
            return ReturnCode::Success;
        }

        return Held.Invoke();
    }
};