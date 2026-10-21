#pragma once

#include <zephyr/kernel.h>
#include "EventHandler.h"
#include "ErrorConverter.h"
#include "CallbackContainer.h"
#include "Debug.h"
#include "WorkQueue.h"

class Work
{
public:
    constexpr Work() : Work(nullptr)
    {
    }

    // Without a queue the work runs on the system work queue.
    constexpr explicit Work(WorkQueueBase* queue) : _container(this), _queue(queue), _initialized(false)
    {
    }

    ~Work()
    {
        auto rc = CancelSync();
        Debug::WriteIfError(rc);
    }

    constexpr ReturnCode Initialize()
    {
        if(_initialized)
        {
            return ReturnCode::Success;
        }

        k_work_init(&_container.CallbackContext, OnWork);

        _initialized = true;

        return ReturnCode::Success;
    }

    ReturnCode Run()
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        if(_queue != nullptr)
        {
            return _queue->Submit(_container.CallbackContext);
        }

        auto err = k_work_submit(&_container.CallbackContext);
        return ErrorConverter::Convert(err);
    }

    // Takes the work off the queue if it's waiting there. Unlike CancelSync it doesn't wait for a
    // run that has started, so the work queue itself can call it.
    ReturnCode Cancel()
    {
        if(!_initialized)
        {
            return ReturnCode::Success;
        }

        k_work_cancel(&_container.CallbackContext);
        return ReturnCode::Success;
    }

    ReturnCode CancelSync()
    {
        if(!_initialized)
        {
            return ReturnCode::Success;
        }

        k_work_cancel_sync(&_container.CallbackContext, &_sync);
        return ReturnCode::Success;
    }

    // Waits until the work has run, if it's waiting or running. Not from the queue it runs on.
    ReturnCode Flush()
    {
        if(!_initialized)
        {
            return ReturnCode::Success;
        }

        // Its own, so a flush can't share one with a CancelSync on another thread.
        k_work_sync sync = {};
        k_work_flush(&_container.CallbackContext, &sync);
        return ReturnCode::Success;
    }

    EventHandler<> Worker;

private:
    CallbackContainer<k_work> _container;
    WorkQueueBase* _queue;
    k_work_sync _sync = {};
    bool _initialized;

    void OnWork()
    {
        auto rc = Worker.Invoke();
        Debug::WriteIfError(rc);
    }

    static void OnWork(k_work* work)
    {
        // this is crappy but we don't have a choice b/c linux bros think they're super clever
        auto container = CONTAINER_OF(work, CallbackContainer<k_work>, CallbackContext);
        static_cast<Work*>(container->Context)->OnWork();
    }
};