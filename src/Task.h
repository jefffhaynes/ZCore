#pragma once

#include "Event.h"
#include "Pool.h"
#include "InterruptControl.h"

class TaskState 
{
public:
    TaskState() 
    {
    }

    void Reset() 
    {
        _complete = false;
        _event.Clear(CompletionBit);
    }

    void SetComplete(ReturnCode code) 
    {
        _code = code;
        _complete = true;
        
        if (InterruptControl::IsIsr()) 
        {
            _event.Post(CompletionBit);
        } 
        else 
        {
            _event.Set(CompletionBit);
        }
    }

    bool IsComplete() const 
    {
        return _complete;
    }

    ReturnCode Await() 
    {
        if (!_complete) 
        {
            _event.Wait(false, CompletionBit);
        }

        return _code;
    }

    ReturnCode Await(TimeSpan timeout)
    {
        if (!_complete) 
        {
            auto rc = _event.Wait(false, CompletionBit, timeout);
            CHECK_RETURN_CODE(rc);
        }

        return _code;
    }

private:
    static constexpr uint32_t CompletionBit = 1 << 0;

    Event _event;
    bool _complete = false;
    ReturnCode _code;
};

template <uint32_t MaxOperations>
class TaskCompletionSource;

using TaskStateHandle = PooledHandle<TaskState>;


class Task
{
public:
    Task() = default;
    explicit Task(TaskStateHandle&& handle) : _handle(std::move(handle))
    {
    }

    Task(ReturnCode code) : _code(code)
    {
    }

    ReturnCode Await()
    {
        if (!_handle.IsValid()) 
        {
            // born completed
            return _code;
        }

        return _handle.Get().Await();
    }

    bool IsComplete()
    {
        if (!_handle.IsValid()) 
        {
            // born completed
            return true;
        }

        return _handle.Get().IsComplete();
    }

private:
    ReturnCode _code;
    TaskStateHandle _handle;
};

template <uint32_t MaxOperations = 4>
class TaskCompletionSource 
{
public:
    ReturnCode GetTask(Task& task, TaskStateHandle& handle) 
    {
        auto rc = _ops.Acquire(handle);
        CHECK_RETURN_CODE(rc);

        handle.Get().Reset();

        task = Task(std::move(handle));

        return ReturnCode::Success;
    }

private:
    Pool<TaskState, MaxOperations> _ops;
};
