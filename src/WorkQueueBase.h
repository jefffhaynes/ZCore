#pragma once

#include <zephyr.h>
#include "Work.h"

class WorkQueueBase
{
public:
    WorkQueueBase()
    {
        _queue.flags = 0;
    }
    
    virtual ReturnCode Initialize() = 0;

    ReturnCode Enqueue(Work& work)
    {
        k_work_submit_to_queue(&_queue, &work._work);

        return ReturnCode::Success;
    }

protected:
    k_work_q* GetQueue() { return &_queue; }

private:
    k_work_q _queue;
};