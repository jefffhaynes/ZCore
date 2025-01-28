#pragma once

#include <ErrorConverter.h>
#include <EventAddress.h>
#include <TaskAddress.h>
#include <helpers/nrfx_gppi.h>


class InterconnectChannel
{
public:
    ReturnCode Connect(EventAddress event, TaskAddress task)
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        nrfx_gppi_channel_endpoints_setup(_channel, event.GetAddress(), task.GetAddress());
        
        return ReturnCode::Success;
    }

    ReturnCode Connect(EventAddress event)
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        nrfx_gppi_event_endpoint_setup(_channel, event.GetAddress());
        
        return ReturnCode::Success;
    }

    ReturnCode Connect(TaskAddress task)
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        nrfx_gppi_task_endpoint_setup(_channel, task.GetAddress());
        
        return ReturnCode::Success;
    }

    ReturnCode Enable()
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        nrfx_gppi_channels_enable(BIT(_channel));
        
        return ReturnCode::Success;
    }

    ReturnCode Disable()
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        nrfx_gppi_channels_disable(BIT(_channel));
        
        return ReturnCode::Success;
    }

private:
    uint8_t _channel;
    bool _initialized = false;

    ReturnCode Initialize()
    {
        if (_initialized)
        {
            return ReturnCode::Success;
        }

        auto err = nrfx_gppi_channel_alloc(&_channel);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        _initialized = true;

        return ReturnCode::Success;
    }
};