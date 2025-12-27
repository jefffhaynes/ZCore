#pragma once

#include "Timer.h"

class PollingSensor
{
public:
    ReturnCode Start()
    {
        if(_timer.IsRunning())
        {
            return ReturnCode::Success;
        }
        
        // make sure when we leave this method, we have a good sample
        auto rc = OnRefresh();
        CHECK_RETURN_CODE(rc);

        return _timer.Start(_period, TimerMode::Repeating);
    }

    ReturnCode Stop()
    {
        return _timer.Stop();
    }

protected:
    constexpr PollingSensor(TimeSpan period) : 
        _timer(TimerOptions::Scheduled), _period(period)
    {
        _timer.Expired.Subscribe(OnExpired, this);
    }

    virtual ReturnCode OnRefresh() = 0;

private:
    Timer _timer;
    TimeSpan _period;

    static ReturnCode OnExpired(void* context)
    {
        auto* sensor = static_cast<PollingSensor*>(context);
        return sensor->OnRefresh();
    }
};