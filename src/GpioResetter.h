#pragma once

#include "IGpio.h"

class GpioResetter
{
public:
    constexpr GpioResetter(IGpio& gpio, bool set = true) : _gpio(gpio), _set(set)
    {
        _gpio.Set(_set);
    }

    constexpr ~GpioResetter()
    {
        _gpio.Set(!_set);
    }

private:
    IGpio& _gpio;
    bool _set;
};