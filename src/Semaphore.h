#pragma once

#include <zephyr/kernel.h>
#include "TimeSpan.h"
#include "ErrorConverter.h"

class Semaphore
{
public:
    Semaphore(uint32_t count = 0, uint32_t maxCount = 1)
    {
        k_sem_init(&_semaphore, count, maxCount);
    }

    ReturnCode Wait(TimeSpan timeout)
    {
        auto milliseconds = static_cast<uint32_t>(timeout.ToMilliseconds());
        auto rc = k_sem_take(&_semaphore, K_MSEC(milliseconds));

        if(rc == -EAGAIN)
        {
            return ReturnCode::Timeout;
        }

        return ErrorConverter::Convert(rc);
    }

    ReturnCode Wait()
    {
        auto rc = k_sem_take(&_semaphore, K_FOREVER);
        return ErrorConverter::Convert(rc);
    }
    
    void Release()
    {
        k_sem_give(&_semaphore);
    }

private:
    k_sem _semaphore;
};

