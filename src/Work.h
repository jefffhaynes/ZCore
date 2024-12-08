#pragma once

#include <zephyr/kernel.h>
#include "EventHandler.h"
#include "ErrorConverter.h"
#include "CallbackContainer.h"
#include "Debug.h"

class WorkQueueBase;

class Work
{
    friend WorkQueueBase;

public:
    constexpr Work() : _container(this), _initialized(false)
    {
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
        
        auto err = k_work_submit(&_container.CallbackContext);
        return ErrorConverter::Convert(err);
    }

    EventHandler<> Worker;

private:
    CallbackContainer<k_work> _container;
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