#pragma once

#include "Mutex.h"

class Lock
{
public:
    Lock(Mutex& mutex) : _mutex(mutex)
    {
        mutex.Lock();
    }

    ~Lock()
    {
        _mutex.Unlock();
    }

private:
    Mutex& _mutex;
};