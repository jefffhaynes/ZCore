#pragma once

#include "TimeSpan.h"
#include <zephyr/kernel.h>

class TimeHelper
{
    public:
        static k_timeout_t ToTimeout(TimeSpan duration)
        {
            return Z_TIMEOUT_US(static_cast<int64_t>(duration.ToMicroseconds()));
        }

        static k_timeout_t Forever()
        {
            return K_FOREVER;
        }

        static k_timeout_t NoWait()
        {
            return Z_TIMEOUT_NO_WAIT;
        }
};