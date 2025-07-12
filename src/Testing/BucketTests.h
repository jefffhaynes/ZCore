#pragma once

#include "Bucket.h"

namespace BucketTests
{
    static_assert([] {
        Bucket<int, 8>::ReplenishCallback callback = [](FixedSpan<int, 8> items, TimeSpan timeout) constexpr
        {
            for(uint32_t i = 0; i < items.GetLength(); i++)
            {
                items.Set(i, i);
            }

            return ReturnCode::Success;
        };

        Bucket<int, 8> bucket(callback);

        int buffer[6] = {0};
        uint32_t count = 0;
        auto rc = bucket.Get(buffer, count, TimeSpan::Zero());
        
        if(rc != ReturnCode::Success)
        {
            return false;
        }
        
        if(count != 6)
        {
            return false;
        }

        for(uint32_t i = 0; i < count; i++)
        {
            if(buffer[i] != (int) i)
            {
                return false;
            }
        }

        rc = bucket.Get(buffer, count, TimeSpan::Zero());

        if(rc != ReturnCode::Success)
        {
            return false;
        }

        if(count != 6)
        {
            return false;
        }

        if(buffer[0] != 6)
        {
            return false;
        }

        if(buffer[5] != 3)
        {
            return false;
        }

        return true;
    }());

    static_assert([] {
        Bucket<float, 4>::ReplenishCallback callback = [](FixedSpan<float, 4> items, TimeSpan timeout) constexpr
        {
            for(uint32_t i = 0; i < items.GetLength(); i++)
            {
                items.Set(i, i);
            }

            return ReturnCode::Success;
        };

        Bucket<float, 4> bucket(callback);

        // get buffer that is larger than the bucket
        float buffer2[10] = {0};
        uint32_t count = 0;
        auto rc = bucket.Get(buffer2, count, TimeSpan::Zero());

        if(rc != ReturnCode::Success)
        {
            return false;
        }

        if(count != 10)
        {
            return false;
        }
        
        if(buffer2[0] != 0)
        {
            return false;
        }

        if(buffer2[4] != 0)
        {
            return false;
        }

        if(buffer2[5] != 1)
        {
            return false;
        }

        return true;
    }());
}