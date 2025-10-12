#pragma once

#include <zephyr/kernel.h>
#include "TimeSpan.h"
#include "TimeHelper.h"

class Event
{
public:
    Event()
    {
        k_event_init(&_event);
    }

    void Post(uint32_t flags)
    {
        k_event_post(&_event, flags);
    }

    void Set(uint32_t flags)
    {
        k_event_set(&_event, flags);
    }

    void Clear(uint32_t flags)
    {
        k_event_clear(&_event, flags);
    }

    ReturnCode Wait(bool reset, uint32_t flags, TimeSpan timeout)
    {
        auto result = k_event_wait(&_event, flags, reset, TimeHelper::ToTimeout(timeout));
        return result == flags ? ReturnCode::Success : ReturnCode::Timeout;
    }

    ReturnCode Wait(bool reset, uint32_t flags)
    {
        auto result = k_event_wait(&_event, flags, reset, TimeHelper::Forever());
        return result == flags ? ReturnCode::Success : ReturnCode::Timeout;
    }

    ReturnCode WaitAll(bool reset, uint32_t flags, TimeSpan timeout)
    {
        auto result = k_event_wait_all(&_event, flags, reset, TimeHelper::ToTimeout(timeout));
        return result == flags ? ReturnCode::Success : ReturnCode::Timeout;
    }

    ReturnCode WaitAll(bool reset, uint32_t flags)
    {
        auto result = k_event_wait_all(&_event, flags, reset, TimeHelper::Forever());
        return result == flags ? ReturnCode::Success : ReturnCode::Timeout;
    }

private:
    struct k_event _event;
};
