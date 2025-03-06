#pragma once

#include "EventHandler.h"
#include "ErrorConverter.h"
#include "Debug.h"
#include "Work.h"
#include "EventAddress.h"
#include "TaskAddress.h"
#include "TimeSpan.h"
#include <nrfx_timer.h>


class DelayLine
{
public:
    DelayLine(nrfx_timer_t timer) : _timer(timer)
    {
    }

    ReturnCode Initialize(TimeSpan period)
    {
        // TODO figure out how to get the instance number at compile time??
        IRQ_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_TIMER_INST_GET(1)), IRQ_PRIO_LOWEST,
                    NRFX_TIMER_INST_HANDLER_GET(1), 0, 0);
                    
        nrfx_timer_config_t config = 
        {
            .frequency = NRF_TIMER_BASE_FREQUENCY_GET(_timer->p_reg),
            .mode = NRF_TIMER_MODE_TIMER,
            .bit_width = NRF_TIMER_BIT_WIDTH_32,
            .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,
            .p_context = this
        };

        auto err = nrfx_timer_init(&_timer, &config, nullptr);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        auto ticks = nrfx_timer_ns_to_ticks(&_timer, period.ToNanoseconds());
        auto flags = Flags::Combine(NRF_TIMER_SHORT_COMPARE0_STOP_MASK, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK);
        nrfx_timer_extended_compare(&_timer, NRF_TIMER_CC_CHANNEL0, ticks, 
            flags, false);

        return ReturnCode::Success;
    }

    EventAddress GetTickEventAddress()
    {
        auto addressValue = nrfx_timer_compare_event_address_get(&_timer, NRF_TIMER_CC_CHANNEL0);
        return EventAddress(addressValue);
    }

    
    TaskAddress GetStartTaskAddress()
    {
        auto addressValue = nrfx_timer_task_address_get(&_timer, NRF_TIMER_TASK_START);
        return TaskAddress(addressValue);
    }

private:
    nrfx_timer_t _timer;

    static uint32_t nrfx_timer_ns_to_ticks(nrfx_timer_t const * p_instance, uint64_t time_ns)
    {
        uint32_t prescaler = nrfy_timer_prescaler_get(p_instance->p_reg);
        auto freq_base_ghz = NRFX_TIMER_BASE_FREQUENCY_GET(p_instance) / 1000000000.0f;
        uint64_t ticks = (((uint64_t)(time_ns * freq_base_ghz)) >> prescaler);
        return (uint32_t)ticks;
    }
};
