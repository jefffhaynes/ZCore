#pragma once

#include "IGpio.h"
#include "ErrorConverter.h"
#include "EventHandler.h"
#include "Work.h"
#include "GpioDirection.h"
#include "CallbackContainer.h"
#include "GpioInterruptMode.h"
#include "TaskAddress.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>


class Gpio : public IGpio
{
public:
    Gpio(const gpio_dt_spec spec, bool scheduled) : _spec(spec),
        _scheduled(scheduled), _container(this), _configured(false)
    {
    }

    Gpio(const gpio_dt_spec spec) : Gpio(spec, false)
    {
    }

    ReturnCode Configure(GpioDirection direction)
    {
        if(_configured)
        {
            return ReturnCode::InvalidState;
        }

        if(_scheduled)
        {
            auto rc = _work.Initialize();
            CHECK_RETURN_CODE(rc);

            rc = _work.Worker.Subscribe(OnWork, this);
            CHECK_RETURN_CODE(rc);
        }

        auto flags = Convert(direction);
        int err = gpio_pin_configure_dt(&_spec, flags);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        _configured = true;

        return ReturnCode::Success;
    }

    ReturnCode ConfigureInterrupt(GpioInterruptMode mode = GpioInterruptMode::OnActive)
    {
        if(!_configured)
        {
            return ReturnCode::InvalidState;
        }

        int err = gpio_pin_interrupt_configure_dt(&_spec, (int) mode);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);
        
        gpio_init_callback(&_container.CallbackContext, OnInterrupt, BIT(_spec.pin));
        err = gpio_add_callback(_spec.port, &_container.CallbackContext);
        return ErrorConverter::Convert(err);
    }

    ReturnCode Set(bool value) override
    {
        if(!_configured)
        {
            auto rc = Configure(GpioDirection::OutInactive);
            CHECK_RETURN_CODE(rc);
        }

        auto err = gpio_pin_set_dt(&_spec, (int) value);
        return ErrorConverter::Convert(err);
    }

    ReturnCode IsSet(bool& set)
    {
        if(!_configured)
        {
            auto rc = Configure(GpioDirection::In);
            CHECK_RETURN_CODE(rc);
        }

        set = gpio_pin_get_dt(&_spec);

        return ReturnCode::Success;
    }

    EventHandler<> Interrupt;

private:
    const struct gpio_dt_spec _spec;
    bool _scheduled;
    CallbackContainer<gpio_callback> _container;
    Work _work;
    bool _configured;
    
    ReturnCode OnWork()
    {
        return Interrupt.Invoke();
    }

    ReturnCode OnInterrupt()
    {
        return _scheduled ? _work.Run() : Interrupt.Invoke();
    }

    static ReturnCode OnWork(void* context)
    {
        return (static_cast<Gpio*>(context))->OnWork();
    }
    
    static void OnInterrupt(const struct device *device, struct gpio_callback *cb, uint32_t pin)
    {
        // this is crappy but we don't have a choice b/c linux bros think they're super clever
        auto* container = CONTAINER_OF(cb, CallbackContainer<gpio_callback>, CallbackContext);
        auto* gpio = static_cast<Gpio*>(container->Context);
        gpio->OnInterrupt();
    }

    static gpio_flags_t Convert(GpioDirection direction)
    {
        switch(direction)
        {
            case GpioDirection::In: return GPIO_INPUT;
            case GpioDirection::Out: return GPIO_OUTPUT;
            case GpioDirection::OutLow: return GPIO_OUTPUT_LOW;
            case GpioDirection::OutHigh: return GPIO_OUTPUT_HIGH;
            case GpioDirection::OutInactive: return GPIO_OUTPUT_INACTIVE;
            case GpioDirection::OutActive: return GPIO_OUTPUT_ACTIVE;
            case GpioDirection::Disconnected: return GPIO_DISCONNECTED;
        }

        return GPIO_DISCONNECTED;
    }
};
