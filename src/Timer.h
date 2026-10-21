#pragma once

#include <zephyr/kernel.h>
#include "Work.h"
#include "TimerOptions.h"
#include "TimerMode.h"
#include "TimeSpan.h"
#include "EventHandler.h"
#include "Flags.h"
#include "TimeHelper.h"

class Timer
{
public:
    constexpr Timer(TimerOptions options = TimerOptions::None) : _options(options), _running(false)
    {
    }

    // Scheduled on the given queue instead of the system work queue.
    constexpr explicit Timer(WorkQueueBase& queue) : _options(TimerOptions::Scheduled), _running(false), _work(&queue)
    {
    }

    ~Timer()
    {
        auto rc = Stop();
        Debug::WriteIfError(rc);
    }

    constexpr ReturnCode Initialize()
    {
        if(_timer.expiry_fn != nullptr)
        {
            return ReturnCode::Success;
        }

        if(IsScheduled())
        {
            auto rc = _work.Initialize();
            CHECK_RETURN_CODE(rc);

            rc = _work.Worker.Subscribe(OnWork, this);
            CHECK_RETURN_CODE(rc);
        }

        k_timer_init(&_timer, OnExpiration, nullptr);
        k_timer_user_data_set(&_timer, this);

        return ReturnCode::Success;
    }

    constexpr ReturnCode Start(TimeSpan period, TimerMode mode = TimerMode::Single)
    {
        auto rc = Stop();
        CHECK_RETURN_CODE(rc);

        rc = Initialize();
        CHECK_RETURN_CODE(rc);

        auto timeout = TimeHelper::ToTimeout(period);

        if(mode == TimerMode::Single)
        {
            k_timer_start(&_timer, timeout, TimeHelper::NoWait());
        }
        else
        {
            k_timer_start(&_timer, timeout, timeout);
        }

        _running = true;

        return ReturnCode::Success;
    }

    constexpr ReturnCode Stop()
    {
        if(!_running)
        {
            return ReturnCode::Success;
        }
        
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        k_timer_stop(&_timer);

        // A scheduled timer's ISR may already have queued an expiry, which would still run after
        // the stop, and a handler that restarts the timer (a blinker) would keep it going. It's
        // left to run and do nothing: cancelling it from inside its own handler would make the
        // kernel refuse every submit, and so the next expiry, until the handler returned.
        _generation++;
        _running = false;

        return ReturnCode::Success;
    }

    constexpr bool IsRunning() const
    {
        return _running;
    }

    EventHandler<> Expired;

private:
    TimerOptions _options;
    k_timer _timer = {};
    bool _running;
    Work _work;
    uint32_t _generation = 0;
    uint32_t _expiredGeneration = 0;

    constexpr bool IsScheduled()
    {
        return Flags::HasFlag(_options, TimerOptions::Scheduled);
    }

    constexpr ReturnCode OnExpiration()
    {
        if(!IsScheduled())
        {
            return Expire();
        }

        _expiredGeneration = _generation;
        return _work.Run();
    }

    constexpr ReturnCode OnWork()
    {
        // Stopped or restarted since this expiry was queued.
        if(_expiredGeneration != _generation)
        {
            return ReturnCode::Success;
        }

        return Expire();
    }

    constexpr ReturnCode Expire()
    {
        if(_timer.period.ticks == 0)
        {
            // single shot
            _running = false;
        }

        return Expired.Invoke();
    }

    static void OnExpiration(k_timer* timer)
    {
        auto* ctx = k_timer_user_data_get(timer);
        static_cast<Timer*>(ctx)->OnExpiration();
    }

    static ReturnCode OnWork(void* context)
    {
        return (static_cast<Timer*>(context))->OnWork();
    }
};