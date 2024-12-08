#pragma once

#include "Timer.h"

class PollingSensor
{
public:
    constexpr PollingSensor() : _timer(TimerOptions::Scheduled)
    {
        _timer.Expired.Subscribe(OnExpired, this);
    }

    ReturnCode Start()
    {
        if(_timer.IsRunning())
        {
            return ReturnCode::Success;
        }
        
        // make sure when we leave this method, we have a good sample
        auto rc = OnRefresh();
        CHECK_RETURN_CODE(rc);

        return _timer.Start(GetPeriod(), TimerMode::Repeating);
    }

    ReturnCode Stop()
    {
        return _timer.Stop();
    }

protected:
    virtual TimeSpan GetPeriod() = 0;
    virtual ReturnCode OnRefresh() = 0;

private:
    Timer _timer;

    static ReturnCode OnExpired(void* context)
    {
        auto* sensor = static_cast<PollingSensor*>(context);
        return sensor->OnRefresh();
    }
};