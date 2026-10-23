#pragma once

#include <type_traits>
#include <zephyr/kernel.h>
#include "FixedSpan.h"
#include "TimeSpan.h"
#include "ErrorConverter.h"
#include "TimeHelper.h"

template<typename T, uint32_t Count>
class MessageQueue
{
    static_assert(std::is_trivially_copyable_v<T>, "MessageQueue's item must be trivially copyable");

public:
    MessageQueue()
    {
        k_msgq_init(&_queue, (char*) _buffer, sizeof(T), Count);
    }

    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;
    MessageQueue(MessageQueue&&) = delete;
    MessageQueue& operator=(MessageQueue&&) = delete;

    ReturnCode Put(const T& item, TimeSpan timeout = TimeSpan::Zero())
    {
        auto err = k_msgq_put(&_queue, &item, TimeHelper::ToTimeout(timeout));
        return Convert(err);
    }

    ReturnCode Get(T& item, TimeSpan timeout = TimeSpan::Zero())
    {
        auto err = k_msgq_get(&_queue, &item, TimeHelper::ToTimeout(timeout));
        return Convert(err);
    }

    ReturnCode Peek(T& item)
    {
        auto err = k_msgq_peek(&_queue, &item);
        return err == -ENOMSG ? ReturnCode::NotFound : ErrorConverter::Convert(err);
    }

private:
    static ReturnCode Convert(int err)
    {
        if(err == -ENOMSG || err == -EAGAIN)
        {
            return ReturnCode::Timeout;
        }

        return ErrorConverter::Convert(err);
    }

    struct k_msgq _queue;
    T _buffer[Count];
};
