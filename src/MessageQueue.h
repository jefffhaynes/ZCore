#pragma once

#include <zephyr/kernel.h>
#include "FixedSpan.h"
#include "TimeSpan.h"
#include "ErrorConverter.h"
#include "TimeHelper.h"

template<typename T, uint32_t Count>
class MessageQueue
{
public:
    MessageQueue()
    {
        k_msgq_init(&_queue, (char*) _buffer, sizeof(T), Count);
    }

    constexpr ReturnCode Put(const T& item, TimeSpan timeout = TimeSpan::Zero)
    {
        auto err = k_msgq_put(&_queue, &item, TimeHelper::ToTimeout(timeout));
        return ErrorConverter::Convert(err);
    }

    constexpr ReturnCode Get(T& item, TimeSpan timeout = TimeSpan::Zero)
    {
        auto err = k_msgq_get(&_queue, &item, TimeHelper::ToTimeout(timeout));
        return ErrorConverter::Convert(err);
    }

    constexpr ReturnCode Peek(T& item)
    {
        auto err = k_msgq_peek(&_queue, &item);
        return ErrorConverter::Convert(err);
    }

private:
    struct k_msgq _queue;
    T _buffer[Count];
};