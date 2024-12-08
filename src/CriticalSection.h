#pragma once

#include "InterruptControl.h"

class CriticalSection
{
public:
    CriticalSection()
    {
        _key = InterruptControl::Disable();
    }

    ~CriticalSection()
    {
        InterruptControl::Enable(_key);
    }

private:
    int _key;
};