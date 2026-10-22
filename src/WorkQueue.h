#pragma once

#include <zephyr/kernel.h>
#include "ErrorConverter.h"

class WorkQueueBase
{
public:
    ReturnCode Submit(k_work& work)
    {
        auto err = k_work_submit_to_queue(&_queue, &work);
        return ErrorConverter::Convert(err);
    }

protected:
    k_work_q _queue = {};
};

// A work queue on its own thread, for work that can't wait behind the system work queue. Pass it
// to Work, Timer or Gpio and their handlers run here instead. Its thread never stops, so it has to
// live as long as the program.
template<uint32_t StackSize>
class WorkQueue : public WorkQueueBase
{
public:
    static constexpr int DefaultPriority = K_PRIO_PREEMPT(7);

    // Zephyr's priority: the lower runs first, and below 0 is cooperative.
    explicit WorkQueue(int priority = DefaultPriority, const char* name = nullptr) : _priority(priority), _name(name)
    {
    }

    // A copy would be a second k_work_q and k_thread for the same running queue.
    WorkQueue(const WorkQueue&) = delete;
    WorkQueue& operator=(const WorkQueue&) = delete;
    WorkQueue(WorkQueue&&) = delete;
    WorkQueue& operator=(WorkQueue&&) = delete;

    ReturnCode Start()
    {
        if(_started)
        {
            return ReturnCode::InvalidState;
        }

        k_work_queue_config config = {};
        config.name = _name;

        k_work_queue_init(&_queue);
        k_work_queue_start(&_queue, _stack, K_KERNEL_STACK_SIZEOF(_stack), _priority, &config);

        _started = true;

        return ReturnCode::Success;
    }

private:
    K_KERNEL_STACK_MEMBER(_stack, StackSize);
    int _priority;
    const char* _name;
    bool _started = false;
};
