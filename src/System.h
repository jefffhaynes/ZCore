#pragma once

#include <zephyr/sys/reboot.h>


class System
{
public:
    static void Reset()
    {
        sys_reboot(SYS_REBOOT_WARM);
    }
};