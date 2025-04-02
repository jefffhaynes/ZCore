#pragma once

#include "TimeSpan.h"
#include "ErrorConverter.h"

#include <zephyr/kernel.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/nrf_clock_control.h>

class Clock
{
public:
    static TimeSpan GetUptime()
    {
        auto microseconds = k_ticks_to_us_floor64(k_uptime_ticks());
        return TimeSpan::FromMicroseconds(microseconds);
    }

    static void Sleep(TimeSpan duration)
    {
        auto microseconds = duration.ToMicroseconds();
        k_usleep(microseconds);
    }

    static ReturnCode EnableExternalOscillator()
    {
        auto* clk_mgr = z_nrf_clock_control_get_onoff(CLOCK_CONTROL_NRF_SUBSYS_HF);

        if (clk_mgr == nullptr) 
        {
            return ReturnCode::NotSupported;
        }
        
        struct onoff_client clk_cli;
        sys_notify_init_spinwait(&clk_cli.notify);
    
        auto err = onoff_request(clk_mgr, &clk_cli);
        auto rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);
    
        do 
        {
            int res;
            err = sys_notify_fetch_result(&clk_cli.notify, &res);
            rc = ErrorConverter::Convert(err);

            if (rc == ReturnCode::Success)
            {
                rc = ErrorConverter::Convert(res);
                CHECK_RETURN_CODE(rc);
            }

        } while (rc == ReturnCode::Busy);
    
        return rc;
    }
};
