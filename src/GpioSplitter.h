#pragma once

#include "IGpio.h"

class GpioSplitter : public IGpio
{
public:
    GpioSplitter(IGpio& gpio1, IGpio& gpio2) : _gpio1(gpio1), _gpio2(gpio2)
    {
    }

    ReturnCode Set(bool value) override
    {
        auto rc = _gpio1.Set(value);
        CHECK_RETURN_CODE(rc);

        return _gpio2.Set(value);
    }

private:
    IGpio& _gpio1;
    IGpio& _gpio2;
};