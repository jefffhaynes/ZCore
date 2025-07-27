#pragma once

#include "TaskAddress.h"
#include "ErrorConverter.h"
#include "EventHandler.h"
#include "Array.h"
#include "Interconnect.h"
#include "PingPongBuffer.h"
#include <nrfx_saadc.h>


// #pragma GCC push_options
// #pragma GCC optimize ("O0")

#define NRFX_SAADC_DEFAULT_CHANNEL_SE_CONFIG(_pin_p, _index, _gain, _acq_time)                  \
{                                                                      \
    .channel_config =                                                  \
    {                                                                  \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CH_CONFIG_RES,                  \
                         (.resistor_p = NRF_SAADC_RESISTOR_DISABLED,   \
                          .resistor_n = NRF_SAADC_RESISTOR_DISABLED,), \
                         ())                                           \
        .gain       = _gain,                                 \
        .reference  = NRF_SAADC_REFERENCE_INTERNAL,                    \
        .acq_time   = _acq_time,                      \
        NRFX_COND_CODE_1(NRF_SAADC_HAS_CONV_TIME,                      \
                         (.conv_time = NRFX_SAADC_DEFAULT_CONV_TIME,), \
                         ())                                           \
        .mode       = NRF_SAADC_MODE_SINGLE_ENDED,                     \
        .burst      = NRF_SAADC_BURST_DISABLED,                        \
    },                                                                 \
    .pin_p          = (nrf_saadc_input_t)_pin_p,                       \
    .pin_n          = NRF_SAADC_INPUT_DISABLED,                        \
    .channel_index  = _index,                                          \
}

// TODO template for different channel counts (BREAKS CALLBACK FOR SOME REASON)
class Saadc
{
public:
    static const uint32_t ChannelCount = 4;
    static const uint32_t SampleCount = ChannelCount;

    static ReturnCode Initialize()
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

        err = nrfx_saadc_channels_config(_channels, ChannelCount);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);
        
        nrfx_saadc_adv_config_t config = NRFX_SAADC_DEFAULT_ADV_CONFIG;
        config.internal_timer_cc = 0;
        config.start_on_end = false;

        auto channel_mask = nrfx_saadc_channels_configured_get();
        err = nrfx_saadc_advanced_mode_set(channel_mask,
                                           NRF_SAADC_RESOLUTION_12BIT,
                                           &config,
                                           OnEvent);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);
                                       
        rc = SetActiveBuffer();
        CHECK_RETURN_CODE(rc);

        err = nrfx_saadc_mode_trigger();
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);


        _initialized = true;

        return ReturnCode::Success;
    }


    static EventAddress GetEndEventAddress()
    {
        auto addressValue = nrf_saadc_event_address_get(NRF_SAADC, NRF_SAADC_EVENT_END);
        return EventAddress(addressValue);
    }

    static EventAddress GetResultDoneEventAddress()
    {
        auto addressValue = nrf_saadc_event_address_get(NRF_SAADC, NRF_SAADC_EVENT_RESULTDONE);
        return EventAddress(addressValue);
    }

    static TaskAddress GetSampleTaskAddress()
    {
        auto addressValue = nrf_saadc_task_address_get(NRF_SAADC, NRF_SAADC_TASK_SAMPLE);
        return TaskAddress(addressValue);
    }

    static TaskAddress GetStartTaskAddress()
    {
        auto addressValue = nrf_saadc_task_address_get(NRF_SAADC, NRF_SAADC_TASK_START);
        return TaskAddress(addressValue);
    }

    static EventHandler<FixedSpan<float, ChannelCount>> Sample;

private:
    static constexpr nrfx_saadc_channel_t _channels[ChannelCount] =
    {
        NRFX_SAADC_DEFAULT_CHANNEL_SE_CONFIG(NRF_SAADC_INPUT_AIN0, 0, NRF_SAADC_GAIN1_3, NRF_SAADC_ACQTIME_3US),
        NRFX_SAADC_DEFAULT_CHANNEL_SE_CONFIG(NRF_SAADC_INPUT_AIN1, 1, NRF_SAADC_GAIN1_3, NRF_SAADC_ACQTIME_3US),
        NRFX_SAADC_DEFAULT_CHANNEL_SE_CONFIG(NRF_SAADC_INPUT_AIN2, 2, NRF_SAADC_GAIN1_3, NRF_SAADC_ACQTIME_3US),
        NRFX_SAADC_DEFAULT_CHANNEL_SE_CONFIG(NRF_SAADC_INPUT_AIN3, 3, NRF_SAADC_GAIN1_3, NRF_SAADC_ACQTIME_3US)
    };

    static bool _initialized;
    static PingPongBuffer<int16_t, SampleCount> _buffer;

    static ReturnCode SetActiveBuffer()
    {
        auto buffer = _buffer.GetActive();
        auto err = nrfx_saadc_buffer_set(buffer.GetData(), buffer.GetLength());
        return ErrorConverter::Convert(err);
    }

    static void OnEvent(nrfx_saadc_evt_t const * p_event)
    {
        switch (p_event->type)
        {
            case NRFX_SAADC_EVT_READY:
                break;                        
                
            case NRFX_SAADC_EVT_BUF_REQ:
                {
                    _buffer.Swap();
                    SetActiveBuffer();
                    break;
                }

            case NRFX_SAADC_EVT_DONE:
            {
                const float max = nrf_saadc_value_max_get(NRF_SAADC_RESOLUTION_12BIT);
                const float scale = 1.0f / max;

                Array<float, ChannelCount> channels;
                for (uint32_t i = 0; i < ChannelCount; i++)
                {
                    auto value = NRFX_SAADC_SAMPLE_GET(NRF_SAADC_RESOLUTION_12BIT, 
                        p_event->data.done.p_buffer, i);
                    channels[i] = value * scale;
                }

                Sample.Invoke(channels.AsFixedSpan());
            }
            default:
                break;
        }
    }

    constexpr Saadc() = default;
};

inline bool Saadc::_initialized = false;
inline PingPongBuffer<int16_t, Saadc::SampleCount> Saadc::_buffer;
inline EventHandler<FixedSpan<float, Saadc::ChannelCount>> Saadc::Sample;


// #pragma GCC pop_options