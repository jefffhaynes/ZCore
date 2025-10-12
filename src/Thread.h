#pragma once

#include <zephyr/kernel.h>
#include "TimeSpan.h"
#include "ErrorConverter.h"
#include "TimeHelper.h"
#include "Aligned.h"
#include <tuple>


template<uint32_t StackSize = 0, typename... Args>
class Thread
{
typedef ReturnCode (*ThreadEntryPoint)(Args... args);

public:
    Thread(ThreadEntryPoint entryPoint) : _entryPoint(entryPoint)
    {
    }

    ReturnCode Start(Args... args)
    {
        _args = std::forward_as_tuple(args...);

        auto id = k_thread_create(&_thread, 
            _stack.Value, K_THREAD_STACK_SIZEOF(_stack.Value), 
            EntryPoint, this, nullptr, nullptr, 
            K_PRIO_PREEMPT(7), 0, TimeHelper::NoWait());

        return id == nullptr ? ReturnCode::InvalidOperation : ReturnCode::Success;
    }

    ReturnCode Join()
    {
        return Join(TimeHelper::Forever());
    }

    ReturnCode Join(TimeSpan timeout)
    {
        return Join(TimeHelper::ToTimeout(timeout));
    }

    static void Sleep(TimeSpan duration)
    {
        k_sleep(TimeHelper::ToTimeout(duration));
    }

private:
    Aligned<z_thread_stack_element[Z_KERNEL_STACK_SIZE_ADJUST(StackSize)], Z_KERNEL_STACK_OBJ_ALIGN> _stack;

    ThreadEntryPoint _entryPoint;
    std::tuple<Args...> _args;

    struct k_thread _thread;

    static_assert(sizeof...(Args) < 3);

    void EntryPoint()
    {
        if constexpr (sizeof...(Args) == 0)
        {
            _entryPoint();
        }
        else if constexpr (sizeof...(Args) == 1)
        {
            _entryPoint(std::get<0>(_args));
        }
        else if constexpr (sizeof...(Args) == 2)
        {
            _entryPoint(std::get<0>(_args), std::get<1>(_args));
        }
    }

    static void EntryPoint(void* context, void* unused1, void* unused2)
    {
        auto* thread = static_cast<Thread*>(context);
        thread->EntryPoint();
    }

    ReturnCode Join(k_timeout_t timeout)
    {
        auto err = k_thread_join(&_thread, timeout);
        return ErrorConverter::Convert(err);
    }
};