#pragma once

#include "Uart.h"

#include <zephyr/usb/usb_device.h>

class UsbCdcAcm : public Uart
{
    public:
        UsbCdcAcm(const struct device* device) : Uart(device)
        {
        }

    protected:
        ReturnCode OnInitialize() override
        {
            auto error = usb_enable(nullptr);
            return ErrorConverter::Convert(error);
        }
};
