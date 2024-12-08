#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>

#include "ReturnCode.h"

class Device
{
protected:
    constexpr Device(const struct device* device) : _device(device)
    {
    }

    constexpr const struct device* GetDevice() const
    {
        return _device;
    }

    ReturnCode FailIfNotReady() const
    {
        return device_is_ready(_device) ? ReturnCode::Success : ReturnCode::InvalidState;
    }

private:
    const struct device* _device;
};