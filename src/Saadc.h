#pragma once

#include "TaskAddress.h"
#include "ErrorConverter.h"
#include "EventHandler.h"
#include "Array.h"
#include "Interconnect.h"
#include <nrfx_saadc.h>

// REMOVE
#include <helpers/nrfx_gppi.h>



// #pragma GCC push_options
// #pragma GCC optimize ("O0")

// TODO MAKE A TEMPLATE EXCEPT THIS COMPLETELY BREAKS THE CALLBACK FOR SOME REASON
class Saadc
{
public:
    static const uint32_t ChannelCount = 4;

    ReturnCode Initialize()
    {
        if (_initialized)
        {
            return ReturnCode::Success;
        }

        IRQ_CONNECT(DT_IRQN(DT_NODELABEL(adc)),
                    DT_IRQ(DT_NODELABEL(adc), priority),
                    nrfx_isr, nrfx_saadc_irq_handler, 0);
        
        auto err = nrfx_saadc_init(DT_IRQ(DT_NODELABEL(adc), priority));
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        _channels[0].channel_config.gain = NRF_SAADC_GAIN1_6;
        _channels[1].channel_config.gain = NRF_SAADC_GAIN1_6;
        _channels[2].channel_config.gain = NRF_SAADC_GAIN1_6;
        _channels[3].channel_config.gain = NRF_SAADC_GAIN1_6;
        

        err = nrfx_saadc_channels_config(_channels, ChannelCount);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);
        

        nrfx_saadc_adv_config_t config = NRFX_SAADC_DEFAULT_ADV_CONFIG;
        config.internal_timer_cc = 0;
        config.start_on_end = false;
        // config.oversampling = NRF_SAADC_OVERSAMPLE_32X;

        auto channel_mask = nrfx_saadc_channels_configured_get();
        err = nrfx_saadc_advanced_mode_set(channel_mask,
                                            NRF_SAADC_RESOLUTION_12BIT,
                                            &config,
                                            OnEvent);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);
                                                
        err = nrfx_saadc_buffer_set(_samples, ChannelCount);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        err = nrfx_saadc_mode_trigger();
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);


        _initialized = true;

        return ReturnCode::Success;
    }


    EventAddress GetEndEventAddress()
    {
        auto addressValue = nrf_saadc_event_address_get(NRF_SAADC, NRF_SAADC_EVENT_END);
        return EventAddress(addressValue);
    }

    EventAddress GetResultDoneEventAddress()
    {
        auto addressValue = nrf_saadc_event_address_get(NRF_SAADC, NRF_SAADC_EVENT_RESULTDONE);
        return EventAddress(addressValue);
    }

    TaskAddress GetSampleTaskAddress()
    {
        auto addressValue = nrf_saadc_task_address_get(NRF_SAADC, NRF_SAADC_TASK_SAMPLE);
        return TaskAddress(addressValue);
    }

    TaskAddress GetStartTaskAddress()
    {
        auto addressValue = nrf_saadc_task_address_get(NRF_SAADC, NRF_SAADC_TASK_START);
        return TaskAddress(addressValue);
    }

    static EventHandler<FixedSpan<float, ChannelCount>> Sample;

private:
    nrfx_saadc_channel_t _channels[ChannelCount] =
    {
        NRFX_SAADC_DEFAULT_CHANNEL_SE(NRF_SAADC_INPUT_AIN0, 0),
        NRFX_SAADC_DEFAULT_CHANNEL_SE(NRF_SAADC_INPUT_AIN1, 1),
        NRFX_SAADC_DEFAULT_CHANNEL_SE(NRF_SAADC_INPUT_AIN2, 2),
        NRFX_SAADC_DEFAULT_CHANNEL_SE(NRF_SAADC_INPUT_AIN3, 3)
    };

    static int16_t _samples[ChannelCount];

    bool _initialized = false;

    static void OnEvent(nrfx_saadc_evt_t const * p_event)
    {
        switch (p_event->type)
        {
            case NRFX_SAADC_EVT_READY:
                break;                        
                
            case NRFX_SAADC_EVT_BUF_REQ:
                nrfx_saadc_buffer_set(_samples, ChannelCount);
                break;

            case NRFX_SAADC_EVT_DONE:
            {
                Array<float, ChannelCount> samples;
                for (uint32_t i = 0; i < ChannelCount; i++)
                {
                    auto value = NRFX_SAADC_SAMPLE_GET(NRF_SAADC_RESOLUTION_12BIT, p_event->data.done.p_buffer, i);
                    const float max = nrf_saadc_value_max_get(NRF_SAADC_RESOLUTION_12BIT);
                    samples[i] = value / max;
                }

                Sample.Invoke(samples.AsFixedSpan());
            }
            default:
                break;
        }
    }

};

inline int16_t Saadc::_samples[Saadc::ChannelCount];
inline EventHandler<FixedSpan<float, Saadc::ChannelCount>> Saadc::Sample;


// #pragma GCC pop_options