#pragma once

#include "InterruptControl.h"

class InterruptLock
{
public:
    InterruptLock(int irq) : _irq(irq)
    {
        InterruptControl::Disable(_irq);
    }

    ~InterruptLock()
    {
        InterruptControl::Enable(_irq);
    }

private:
    int _irq;
};