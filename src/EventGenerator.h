#pragma once

#include "ErrorConverter.h"
#include "TaskAddress.h"
#include "EventAddress.h"
#include <nrfx_egu.h>


class EventGenerator
{
public:
    EventGenerator()
    {
    }

    ReturnCode Initialize()
    {
        nrfx_egu_t egu_inst = NRFX_EGU_INSTANCE(0);
        auto err = nrfx_egu_init(&egu_inst, NRFX_EGU_DEFAULT_CONFIG_IRQ_PRIORITY, nullptr, nullptr);
        return ErrorConverter::Convert(err);
    }

    EventAddress GetEventAddress(nrf_egu_event_t event = NRF_EGU_EVENT_TRIGGERED0)
    {
        nrfx_egu_t egu_inst = NRFX_EGU_INSTANCE(0);
        auto addressValue = nrfx_egu_event_address_get(&egu_inst, event);
        return EventAddress(addressValue);
    }

    ReturnCode Trigger(nrf_egu_event_t event = NRF_EGU_EVENT_TRIGGERED0)
    {
        nrfx_egu_t egu_inst = NRFX_EGU_INSTANCE(0);
        nrfx_egu_trigger(&egu_inst, event);
        return ReturnCode::Success;
    }
};