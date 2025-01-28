#pragma once

#include "ErrorConverter.h"
#include "TaskAddress.h"
#include <nrfx_gpiote.h>

class GpioTask
{
public:
    GpioTask(nrfx_gpiote_t gpiote, uint32_t pin) : _gpiote(gpiote), _pin(pin)
    {
    }

    ReturnCode Initialize()
    {
        if (_initialized)
        {
            return ReturnCode::Success;
        }

        uint8_t channel;
        auto err = nrfx_gpiote_channel_alloc(&_gpiote, &channel);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        const nrfx_gpiote_output_config_t output_config = NRFX_GPIOTE_DEFAULT_OUTPUT_CONFIG;

        const nrfx_gpiote_task_config_t task_config = 
        {
            .task_ch = channel,
            .polarity = NRF_GPIOTE_POLARITY_TOGGLE,
            .init_val = NRF_GPIOTE_INITIAL_VALUE_LOW,
	    };

        err = nrfx_gpiote_output_configure(&_gpiote, _pin, &output_config, &task_config);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        nrfx_gpiote_out_task_enable(&_gpiote, _pin);

        _initialized = true;

        return ReturnCode::Success;
    }

    TaskAddress GetSetTaskAddress()
    {
        auto addressValue = nrfx_gpiote_set_task_address_get(&_gpiote, _pin);
        return TaskAddress(addressValue);
    }

    TaskAddress GetClearTaskAddress()
    {
        auto addressValue = nrfx_gpiote_clr_task_address_get(&_gpiote, _pin);
        return TaskAddress(addressValue);
    }

private:
    nrfx_gpiote_t _gpiote;
    uint32_t _pin;
    bool _initialized = false;
};

