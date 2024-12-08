#pragma once

#include "InterruptControl.h"

class InterruptLock
{
public:
    InterruptLock()
    {
        _key = InterruptControl::Disable();
    }

    ~InterruptLock()
    {
        InterruptControl::Enable(_key);
    }

private:
    int _key;
};