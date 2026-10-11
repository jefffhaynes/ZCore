#pragma once

#include "Array.h"
#include "Device.h"
#include "ErrorConverter.h"
#include "IAccelerometer.h"

// Any accelerometer with a driver behind Zephyr's sensor API.
class Accelerometer : public Device, public IAccelerometer
{
public:
    constexpr Accelerometer(const struct device* device) : Device(device)
    {
    }

    ReturnCode Sample(Vector3<Acceleration>& acceleration) override
    {
        auto rc = FailIfNotReady();
        CHECK_RETURN_CODE(rc);

        auto err = sensor_sample_fetch_chan(GetDevice(), SENSOR_CHAN_ACCEL_XYZ);

        // Some drivers only fetch everything they have at once.
        if(err == -ENOTSUP)
        {
            err = sensor_sample_fetch(GetDevice());
        }

        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        // Zephyr fills the three axes through the one pointer.
        Array<sensor_value, 3> axes;
        err = sensor_channel_get(GetDevice(), SENSOR_CHAN_ACCEL_XYZ, axes.GetData());
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        acceleration = Vector3<Acceleration>(
            ToAcceleration(axes.Get<0>()),
            ToAcceleration(axes.Get<1>()),
            ToAcceleration(axes.Get<2>()));

        return ReturnCode::Success;
    }

private:
    static Acceleration ToAcceleration(const sensor_value& value)
    {
        return Acceleration::FromMetersPerSecondSquared(sensor_value_to_float(&value));
    }
};
