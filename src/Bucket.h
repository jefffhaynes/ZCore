#pragma once

#include "Array.h"
#include "TimeSpan.h"

template<typename T, uint32_t Size>
class Bucket
{
public:
    typedef ReturnCode (*ReplenishCallback)(FixedSpan<T, Size> items, TimeSpan timeout);

    constexpr Bucket(ReplenishCallback replenish) : _replenish(replenish)
    {
    }

    constexpr ReturnCode Get(Span<T> items, uint32_t& count, TimeSpan timeout)
    {
        count = 0;

        while(count < items.GetLength())
        {
            auto rc = Replenish(timeout);
            CHECK_RETURN_CODE(rc);

            auto remaining = items.GetLength() - count;
            auto block = _available.Take(remaining);
            
            rc = block.CopyTo(items, count);
            CHECK_RETURN_CODE(rc);
            
            auto length = block.GetLength();
            count += length;
            _available = _available.Skip(length);
        }
    
        return ReturnCode::Success;
    }

private:
    ReplenishCallback _replenish;
    Array<T, Size> _items;
    Span<T> _available;

    constexpr ReturnCode Replenish(TimeSpan timeout)
    {
        if(!_available.IsEmpty())
        {
            return ReturnCode::Success;
        }

        auto rc = _replenish(_items, timeout);
        CHECK_RETURN_CODE(rc);

        _available = _items.AsSpan();

        return ReturnCode::Success;
    }
};