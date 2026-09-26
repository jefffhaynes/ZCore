#pragma once

#include "Device.h"
#include "QDecOptions.h"
#include "ErrorConverter.h"
#include "EventHandler.h"
#include "Flags.h"
#include <Units/Angle.h>
#include <Work.h>

#include <Debug.h>

// The rotation channel means different things per driver, so the
// implementation follows the driver:
//  - nRF (qdec_nrfx): data-ready fires on motion, and the channel is the
//    rotation since the last read.
//  - STM32 (qdec_stm32, a timer in encoder mode): there is no trigger, and the
//    channel is the absolute angle within one revolution, so it is polled and
//    unwrapped.
#if defined(CONFIG_QDEC_NRFX)
#include <hal/nrf_qdec.h>
#elif defined(CONFIG_QDEC_STM32)
#include "Timer.h"
#else
#error "QDec.h needs a quadrature decoder driver it supports (nRF QDEC or STM32 timer encoder)"
#endif

class QDec : public Device
{
public:
    QDec(const struct device* device, QDecOptions options = QDecOptions::None) : Device(device),
        _options(options)
#if defined(CONFIG_QDEC_NRFX)
        , Trigger(SENSOR_TRIG_DATA_READY, SENSOR_CHAN_ROTATION, this)
#endif
    {
        _work.Worker.Subscribe<QDec, &QDec::OnWork>(this);
#if defined(CONFIG_QDEC_STM32)
        _pollTimer.Expired.Subscribe<QDec, &QDec::OnPoll>(this);
#endif
    }

    EventHandler<Angle> ValueUpdated;

    ReturnCode Initialize()
    {
        auto rc = FailIfNotReady();
        CHECK_RETURN_CODE(rc);

#if defined(CONFIG_QDEC_NRFX)
        // Zephyr's driver leaves the debounce filter off.
        auto* registers = GetRegisters(GetDevice());
        if (registers != nullptr)
        {
            nrf_qdec_dbfen_enable(registers);
        }

        auto err = sensor_trigger_set(GetDevice(), &Trigger, OnDataReady);
        return ErrorConverter::Convert(err);
#else
        rc = ReadPosition(_position);
        CHECK_RETURN_CODE(rc);

        return _pollTimer.Start(PollInterval, TimerMode::Repeating);
#endif
    }

    Angle GetValue() const
    {
        return _value;
    }

private:
    QDecOptions _options;
    Angle _value;
    Work _work;

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

    ReturnCode Accumulate(Angle rotation)
    {
        _value += rotation;

        bool isScheduled = Flags::HasFlag(_options, QDecOptions::Scheduled);
        return isScheduled ? _work.Run() : OnWork();
    }

    ReturnCode OnWork()
    {
        return ValueUpdated.Invoke(_value);
    }

#if defined(CONFIG_QDEC_NRFX)
    struct SensorTriggerWithContext : public sensor_trigger
    {
        SensorTriggerWithContext(sensor_trigger_type t, sensor_channel c, void* ctx = nullptr)
            : sensor_trigger{t, c}, Context(ctx) {}

        void* Context;
    };

    const struct SensorTriggerWithContext Trigger;

    ReturnCode OnDataReady()
    {
        Angle value;
        auto rc = Read(value);
        CHECK_RETURN_CODE(rc);

        return Accumulate(value);
    }

    static void OnDataReady(const struct device *dev, const struct sensor_trigger *trigger)
    {
        auto triggerWithContext = static_cast<const SensorTriggerWithContext*>(trigger);
        auto instance = static_cast<QDec*>(triggerWithContext->Context);

        instance->OnDataReady();
    }

    // The instance's registers, looked up in devicetree: the driver keeps them
    // in a private config struct whose layout changes between SDK releases.
    static NRF_QDEC_Type* GetRegisters(const struct device* device)
    {
#define ZCORE_QDEC_REGISTERS(node)                                          \
        if (device == DEVICE_DT_GET(node))                                  \
        {                                                                   \
            return reinterpret_cast<NRF_QDEC_Type*>(DT_REG_ADDR(node));     \
        }

        DT_FOREACH_STATUS_OKAY(nordic_nrf_qdec, ZCORE_QDEC_REGISTERS)

#undef ZCORE_QDEC_REGISTERS

        return nullptr;
    }
#else
    // Short enough that the shaft turns less than half a revolution between
    // reads; any more and the direction of travel is ambiguous.
    static constexpr TimeSpan PollInterval = TimeSpan::FromMilliseconds(10);

    Timer _pollTimer;
    Angle _position;

    ReturnCode ReadPosition(Angle& position)
    {
        auto err = sensor_sample_fetch(GetDevice());
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        return Read(position);
    }

    ReturnCode OnPoll()
    {
        Angle position;
        auto rc = ReadPosition(position);
        CHECK_RETURN_CODE(rc);

        // Take the shorter way round from the last position.
        auto degrees = (position - _position).ToDegrees();

        if (degrees > 180)
        {
            degrees -= 360;
        }
        else if (degrees < -180)
        {
            degrees += 360;
        }

        _position = position;

        return degrees == 0 ? ReturnCode::Success : Accumulate(Angle::FromDegrees(degrees));
    }
#endif
};
