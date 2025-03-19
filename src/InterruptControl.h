#pragma once

#include <zephyr/kernel.h>

class InterruptControl
{
public:
    static int Lock()
    {
        return irq_lock();
    }

    static void Unlock(int key)
    {
        irq_unlock(key);
    }

    static void Disable(int irq)
    {
        irq_disable(irq);
    }

    static void Enable(int irq)
    {
        irq_enable(irq);
    }
};