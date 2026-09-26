#pragma once

#include "Uart.h"

#if !defined(CONFIG_USB_DEVICE_STACK_NEXT) || !defined(CONFIG_USBD_CDC_ACM_CLASS)
#error "UsbCdcAcm.h needs the USB device_next stack with CDC ACM (CONFIG_USB_DEVICE_STACK_NEXT, CONFIG_USBD_CDC_ACM_CLASS)"
#endif

// The USB device is brought up by CONFIG_CDC_ACM_SERIAL_INITIALIZE_AT_BOOT or the app.
class UsbCdcAcm : public Uart
{
    public:
        UsbCdcAcm(const struct device* device) : Uart(device)
        {
        }

        ReturnCode SetBaudRate(uint32_t baudRate) override
        {
            return ReturnCode::NotSupported;
        }

    protected:
        ReturnCode OnInitialize() override
        {
            return FailIfNotReady();
        }
};
