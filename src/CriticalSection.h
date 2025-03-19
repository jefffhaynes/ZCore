#pragma once

#include "InterruptControl.h"

class CriticalSection
{
public:
    CriticalSection()
    {
        _key = InterruptControl::Lock();
    }

    ~CriticalSection()
    {
        InterruptControl::Unlock(_key);
    }

private:
    int _key;
};