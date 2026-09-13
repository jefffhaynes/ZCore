#pragma once

#include <zephyr/kernel.h>
#include "TimeSpan.h"
#include "ErrorConverter.h"
#include "TimeHelper.h"
#include "Debug.h"

class Semaphore
{
public:
    Semaphore(uint32_t count = 0, uint32_t maxCount = 1)
    {
        // Fails for maxCount == 0 or count > maxCount. A constructor can't
        // return it, so surface it here rather than as a mystery later.
        auto err = k_sem_init(&_semaphore, count, maxCount);
        Debug::WriteIfError(ErrorConverter::Convert(err));
    }

    // k_sem holds a wait queue whose list head points at itself once
    // initialised, so a copied or moved Semaphore would carry dangling
    // pointers. Construct in place and keep it there.
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore(Semaphore&&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;

    ReturnCode Wait(TimeSpan timeout)
    {
        auto rc = k_sem_take(&_semaphore, TimeHelper::ToTimeout(timeout));

        if(rc == -EAGAIN)
        {
            return ReturnCode::Timeout;
        }

        return ErrorConverter::Convert(rc);
    }

    ReturnCode Wait()
    {
        auto rc = k_sem_take(&_semaphore, TimeHelper::Forever());
        return ErrorConverter::Convert(rc);
    }

    void Release()
    {
        k_sem_give(&_semaphore);
    }

private:
    k_sem _semaphore;
};
