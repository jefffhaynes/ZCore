#pragma once

#include "IAdc.h"
#include "ErrorConverter.h"
#include "Range.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>

#include <cmath>

class Adc : public IAdc
{
public:
    Adc(const adc_dt_spec& adc) : _adc(adc)
    {
    };

    ReturnCode Initialize()
    {
        if(_initialized)
        {
            return ReturnCode::Success;
        }

        if(!device_is_ready(_adc.dev))
        {
            return ReturnCode::InvalidState;
        }

        auto err = adc_channel_setup_dt(&_adc);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        rc = GetMax(_max);
        CHECK_RETURN_CODE(rc);

        _initialized = true;

        return ReturnCode::Success;
    }

    ReturnCode Sample(int16_t& sample) override
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

	    struct adc_sequence sequence = {
            .buffer = &sample,
            /* buffer size in bytes, not number of samples */
            .buffer_size = sizeof(sample),
            .oversampling = 5
        };

        auto err = adc_sequence_init_dt(&_adc, &sequence);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        err = adc_read(_adc.dev, &sequence);
        return ErrorConverter::Convert(err);
    }

    ReturnCode Sample(Voltage& voltage) override
    {
        int16_t sample;
        auto rc = Sample(sample);
        CHECK_RETURN_CODE(rc);

        int32_t mv = sample;
        auto err = adc_raw_to_millivolts_dt(&_adc, &mv);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        voltage = Voltage::FromMillivolts(mv);

        return ReturnCode::Success;
    }

    ReturnCode Sample(float& value) override
    {
        Voltage voltage;
        auto rc = Sample(voltage);
        CHECK_RETURN_CODE(rc);

        value = Range<float>::ZeroToOne.Clamp(voltage / _max);

        return ReturnCode::Success;
    }

    ReturnCode GetRange(Range<Voltage>& range) override
    {
        auto rc = Initialize();
        CHECK_RETURN_CODE(rc);

        range = Range<Voltage>(Voltage::Zero, _max);

        return ReturnCode::Success;
    }

private:
    bool _initialized = false;
    const adc_dt_spec _adc;
    Voltage _max;

    ReturnCode GetMax(Voltage& max)
    {
        auto reference = (_adc.channel_cfg.reference == ADC_REF_INTERNAL) ?
            (int32_t)adc_ref_internal(_adc.dev) : _adc.vref_mv;
        
        auto err = adc_gain_invert(_adc.channel_cfg.gain, &reference);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);
        
        max = Voltage::FromMillivolts(reference);

        return ReturnCode::Success;
    }
};