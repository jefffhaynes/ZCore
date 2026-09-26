#pragma once

#include "Uart.h"

#if !defined(CONFIG_USB_DEVICE_STACK_NEXT) || !defined(CONFIG_USBD_CDC_ACM_CLASS)
#error "UsbCdcAcm.h needs the USB device_next stack with CDC ACM (CONFIG_USB_DEVICE_STACK_NEXT, CONFIG_USBD_CDC_ACM_CLASS)"
#endif

// A CDC ACM function of the USB device_next stack, used as a UART (devicetree
// compatible "zephyr,cdc-acm-uart"). The USB device itself is brought up
// outside this class: by Zephyr at boot with CONFIG_CDC_ACM_SERIAL_INITIALIZE_AT_BOOT
// (VID/PID and strings from the CONFIG_CDC_ACM_SERIAL_* options), or by the
// application's own usbd context when it has other USB functions too.
class UsbCdcAcm : public Uart
{
    public:
        UsbCdcAcm(const struct device* device) : Uart(device)
        {
        }

        // USB has no line rate; whatever the host sets is advisory only.
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
