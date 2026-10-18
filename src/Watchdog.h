#pragma once

#include <zephyr/drivers/watchdog.h>

#include "Device.h"
#include "ErrorConverter.h"
#include "TimeSpan.h"

// A hardware watchdog, through Zephyr's watchdog API: resets the SoC unless
// Feed() comes within the timeout. A core halted by the debugger pauses it.
// On some SoCs (nRF, for one) it can't be stopped or reconfigured once
// started, and it keeps counting through a soft reset, so whatever runs next
// has to feed it too: MCUboot, while it swaps, with CONFIG_BOOT_WATCHDOG_FEED,
// and the app once Start() has run again.
class Watchdog : public Device
{
public:
    constexpr Watchdog(const struct device* device, TimeSpan timeout) :
        Device(device), _timeout(timeout)
    {
    }

    ReturnCode Start()
    {
        if(_channel >= 0)
        {
            return ReturnCode::Success;
        }

        auto rc = FailIfNotReady();
        CHECK_RETURN_CODE(rc);

        wdt_timeout_cfg config = {};
        config.window.max = static_cast<uint32_t>(_timeout.ToMilliseconds());
        config.flags = WDT_FLAG_RESET_SOC;

        auto channel = wdt_install_timeout(GetDevice(), &config);
        rc = ErrorConverter::Convert(channel);
        CHECK_RETURN_CODE(rc);

        auto err = wdt_setup(GetDevice(), WDT_OPT_PAUSE_HALTED_BY_DBG);
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        _channel = channel;

        return Feed();
    }

    // A no-op until Start(), so a feed point may run before the watchdog does.
    ReturnCode Feed()
    {
        if(_channel < 0)
        {
            return ReturnCode::Success;
        }

        auto err = wdt_feed(GetDevice(), _channel);
        return ErrorConverter::Convert(err);
    }

private:
    TimeSpan _timeout;
    int _channel = -1;
};
