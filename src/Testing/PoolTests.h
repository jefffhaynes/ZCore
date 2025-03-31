#pragma once

#include "Pool.h"


static_assert([]() {
    Pool<int, 1> pool;

    {
        PooledHandle<int> handle;
        auto rc = pool.Acquire(handle);

        if (rc != ReturnCode::Success)
        {
            return false;
        }

        rc = pool.Acquire(handle);

        if (rc != ReturnCode::OutOfMemory)
        {
            return false;
        }
    }

    PooledHandle<int> handle;
    auto rc = pool.Acquire(handle);

    if (rc != ReturnCode::Success)
    {
        return false;
    }

    return true;
}());