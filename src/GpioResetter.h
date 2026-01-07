#pragma once

#include "IGpio.h"

class GpioResetter
{
public:
    constexpr GpioResetter(IGpio& gpio, bool initialState) : _gpio(gpio), _initialState(initialState)
    {
        _gpio.Set(_initialState);
    }

    constexpr ~GpioResetter()
    {
        _gpio.Set(!_initialState);
    }

private:
    IGpio& _gpio;
    bool _initialState;
};