#pragma once

#include <zephyr/kernel.h>

class InterruptControl
{
public:
    static int Disable()
    {
        return irq_lock();
    }

    static void Enable(int key)
    {
        irq_unlock(key);
    }
};