#pragma once

#include "EventHandler.h"
#include "ErrorConverter.h"
#include "Debug.h"
#include "Work.h"
#include <nrfx_timer.h>

class Oscillator
{
public:
    Oscillator(nrfx_timer_t timer) : _timer(timer)
    {
        _work.Worker.Subscribe<Oscillator, &Oscillator::OnWork>(this);
    }

    ReturnCode Initialize()
    {
        if (_initialized)
        {
            return ReturnCode::Success;
        }

        // TODO figure out how to get the instance number at compile time??
        IRQ_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TIMER_INST_GET(0)), IRQ_PRIO_LOWEST,
                    NRFX_TIMER_INST_HANDLER_GET(0), 0, 0);
                    
        nrfx_timer_config_t config = 
        {
            .frequency = NRF_TIMER_BASE_FREQUENCY_GET(_timer->p_reg),
            .mode = NRF_TIMER_MODE_TIMER,
            .bit_width = NRF_TIMER_BIT_WIDTH_32,
            .p_context = this
        };

        auto err = nrfx_timer_init(&_timer, &config, OnInterrupt);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        nrfx_timer_clear(&_timer);

        rc = _work.Initialize();
        CHECK_RETURN_CODE(rc);

        _initialized = true;

        return ReturnCode::Success;
    }

    ReturnCode Start(TimeSpan period)
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        if (nrfx_timer_is_enabled(&_timer))
        {
            nrfx_timer_disable(&_timer);
        }

        auto ticks = nrfx_timer_us_to_ticks(&_timer, period.ToMicroseconds());

        nrfx_timer_extended_compare(&_timer, NRF_TIMER_CC_CHANNEL0, ticks,
                                NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

        nrfx_timer_enable(&_timer);

        return ReturnCode::Success;
    }

    ReturnCode Stop()
    {
        nrfx_timer_disable(&_timer);

        return ReturnCode::Success;
    }

    EventHandler<> Tick;

private:
    nrfx_timer_t _timer;
    bool _initialized = false;
    Work _work;

    ReturnCode OnWork()
    {
        return Tick.Invoke();
    }

    void OnInterrupt(nrf_timer_event_t event_type)
    {
        if (event_type == NRF_TIMER_EVENT_COMPARE0)
        {
            _work.Run();
        }
    }

    static void OnInterrupt(nrf_timer_event_t event_type, void* p_context)
    {
        auto* oscillator = static_cast<Oscillator*>(p_context);
        oscillator->OnInterrupt(event_type);
    }
};
