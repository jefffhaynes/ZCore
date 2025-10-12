#pragma once

#include <zephyr/kernel.h>
#include "TimeSpan.h"
#include "ErrorConverter.h"
#include "TimeHelper.h"

class Mutex
{
public:
    Mutex()
    {
        k_mutex_init(&_mutex);
    }

    ReturnCode Lock()
    {
        auto err = k_mutex_lock(&_mutex, TimeHelper::Forever());
        return ErrorConverter::Convert(err);
    }

    ReturnCode Lock(TimeSpan timeout)
    {
        auto err = k_mutex_lock(&_mutex, TimeHelper::ToTimeout(timeout));
        return ErrorConverter::Convert(err);
    }

    ReturnCode Unlock()
    {
        auto err = k_mutex_unlock(&_mutex);
        return ErrorConverter::Convert(err);
    }

private:
    struct k_mutex _mutex;
};