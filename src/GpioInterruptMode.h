#pragma once

#include <zephyr/drivers/gpio.h>

enum class GpioInterruptMode
{
    OnActive = GPIO_INT_EDGE_TO_ACTIVE,
    OnInactive = GPIO_INT_EDGE_TO_INACTIVE,
    Both = GPIO_INT_EDGE_BOTH,
};