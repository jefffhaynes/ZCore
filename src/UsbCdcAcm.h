#pragma once

#include "Uart.h"

#include <zephyr/usb/usb_device.h>

class UsbCdcAcm : public Uart
{
    public:
        UsbCdcAcm(const struct device* device) : Uart(device)
        {
        }

        ReturnCode Initialize() override
        {
            auto error = usb_enable(nullptr);
            auto rc = ErrorConverter::Convert(error);
            CHECK_RETURN_CODE(rc);

            return Uart::Initialize();
        }
};
