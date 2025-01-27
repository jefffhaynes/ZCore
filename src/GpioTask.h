#pragma once

#include "ErrorConverter.h"
#include "TaskAddress.h"
#include <nrfx_gpiote.h>

class GpioTask
{
public:
    GpioTask(nrfx_gpiote_t gpiote, uint32_t pin, nrf_gpiote_polarity_t polarity) : 
        _gpiote(gpiote), _pin(pin), _polarity(polarity)
    {
    }

    ReturnCode Initialize()
    {
        if (_initialized)
        {
            return ReturnCode::Success;
        }

        if (!nrfx_gpiote_init_check(&_gpiote))
        {
            // IRQ_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_GPIOTE_INST_GET(0)), IRQ_PRIO_LOWEST,
            //     NRFX_GPIOTE_INST_HANDLER_GET(0), 0, 0);

            // IRQ_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_GPIOTE_INST_GET(1)), IRQ_PRIO_LOWEST,
            //     NRFX_GPIOTE_INST_HANDLER_GET(1), 0, 0);

            auto err = nrfx_gpiote_init(&_gpiote, DT_IRQ(DT_NODELABEL(gpiote0), priority));
            auto rc = ErrorConverter::Convert(err);
            CHECK_RETURN_CODE(rc);
        }

        uint8_t channel;
        auto err = nrfx_gpiote_channel_alloc(&_gpiote, &channel);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        const nrfx_gpiote_output_config_t output_config =
        {
            .drive = NRF_GPIO_PIN_S0S1,
            .input_connect = NRF_GPIO_PIN_INPUT_DISCONNECT,
            .pull = NRF_GPIO_PIN_NOPULL,
        };

        const nrfx_gpiote_task_config_t task_config =
        {
            .task_ch = channel,
            .polarity = _polarity,
            .init_val = NRF_GPIOTE_INITIAL_VALUE_LOW,
        };

        err = nrfx_gpiote_output_configure(&_gpiote, _pin, &output_config, &task_config);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        nrfx_gpiote_out_task_enable(&_gpiote, _pin);

        _initialized = true;

        return ReturnCode::Success;
    }

    TaskAddress GetTaskAddress()
    {
        auto addressValue = nrfx_gpiote_out_task_address_get(&_gpiote, _pin);
        return TaskAddress(addressValue);
    }

private:
    nrfx_gpiote_t _gpiote;
    uint32_t _pin;
    nrf_gpiote_polarity_t _polarity;
    bool _initialized = false;
};

