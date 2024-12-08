#pragma once

#include <zephyr/kernel.h>
#include "TimeSpan.h"

class Clock
{
public:
    static TimeSpan GetUptime()
    {
        auto microseconds = k_ticks_to_us_floor64(k_uptime_ticks());
        return TimeSpan::FromMicroseconds(microseconds);
    }

    static void Sleep(TimeSpan duration)
    {
        auto microseconds = duration.ToMicroseconds();
        k_usleep(microseconds);
    }
};
