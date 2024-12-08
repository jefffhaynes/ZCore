#pragma once

#include "WorkQueueBase.h"
#include "Aligned.h"

template<uint32_t StackSize, uint32_t Priority = 5>
class WorkQueue : public WorkQueueBase
{
public:
    WorkQueue() : _initialized(false)
    {
    }

    ReturnCode Initialize() override
    {
        if(_initialized)
        {
            return ReturnCode::Success;
        }

        k_work_q_start(GetQueue(), _stack, K_THREAD_STACK_SIZEOF(_stack), Priority);

        _initialized = true;
        
        return ReturnCode::Success;
    }

private:
    bool _initialized;
    Aligned<z_thread_stack_element, Z_KERNEL_STACK_OBJ_ALIGN> _stack[Z_KERNEL_STACK_SIZE_ADJUST(StackSize)];
};