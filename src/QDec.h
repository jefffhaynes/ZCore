#pragma once

#include "Device.h"
#include "QDecOptions.h"
#include "ErrorConverter.h"
#include "EventHandler.h"
#include "Flags.h"
#include <Units/Angle.h>
#include <Work.h>

#include <Debug.h>

class QDec : public Device
{
public:
    QDec(const struct device* device, QDecOptions options = QDecOptions::None) : Device(device),
        _options(options), Trigger(SENSOR_TRIG_DATA_READY, SENSOR_CHAN_ROTATION, this)
    {
        _work.Worker.Subscribe<QDec, &QDec::OnWork>(this);
        NRF_QDEC->DBFEN = true;
    }

    EventHandler<Angle> ValueUpdated;

    ReturnCode Initialize()
    {
        auto rc = FailIfNotReady();
        CHECK_RETURN_CODE(rc);

        auto err = sensor_trigger_set(GetDevice(), &Trigger, OnDataReady);
        return ErrorConverter::Convert(err);
    }

    Angle GetValue() const
    {
        return _value;
    }
    
private:
    QDecOptions _options;
    Work _work;
    Angle _value;

    struct SensorTriggerWithContext : public sensor_trigger
    {
        SensorTriggerWithContext(sensor_trigger_type t, sensor_channel c, void* ctx = nullptr)
            : sensor_trigger{t, c}, Context(ctx) {}

        void* Context;
    };

    const struct SensorTriggerWithContext Trigger;

    ReturnCode Read(Angle& angle)
    {
        struct sensor_value raw;
        auto err = sensor_channel_get(GetDevice(), SENSOR_CHAN_ROTATION, &raw);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        auto degrees = raw.val1 + raw.val2 / 1000000.0f;
        angle = Angle::FromDegrees(degrees);

        return ReturnCode::Success;
    }

    ReturnCode OnDataReady()
    {
        Angle value;
        auto rc = Read(value);
        CHECK_RETURN_CODE(rc);

        _value += value;

        bool isScheduled = Flags::HasFlag(_options, QDecOptions::Scheduled);
        return isScheduled ? _work.Run() : OnWork();
    }

    ReturnCode OnWork()
    {
        return ValueUpdated.Invoke(_value);
    }

    static void OnDataReady(const struct device *dev, const struct sensor_trigger *trigger)
    {
        auto triggerWithContext = static_cast<const SensorTriggerWithContext*>(trigger);
        auto instance = static_cast<QDec*>(triggerWithContext->Context);

        instance->OnDataReady();
    }
};
