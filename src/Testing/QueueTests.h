#pragma once

#include "Queue.h"
#include "Array.h"

namespace QueueTests
{
    // check initial state
    static_assert([] {
        Array<int, 11> buffer;
        Queue<int> queue(buffer);
        
        if(queue.GetCount() != 0)
        {
            return false;
        }

        if(!queue.IsEmpty())
        {
            return false;
        }

        if(queue.IsFull())
        {
            return false;
        }

        return true;
    }());

    static_assert([] {
        Array<int, 11> buffer;
        Queue<int> queue(buffer);
        
        return constexpr_for<0, 10, 1>([&](auto round)
        {
            int items[6] = {0, 1, 2, 3, 4, 5};
            auto rc = queue.Enqueue(items);
            
            if(rc != ReturnCode::Success)
            {
                return false;
            }

            if (queue.GetCount() != 6)
            {
                return false;
            }
            
            int result[6] = {0};
            rc = queue.Dequeue(result);
            
            if(rc != ReturnCode::Success)
            {
                return false;
            }
            
            for(int32_t i = 0; i < 6; i++)
            {
                if(result[i] != i)
                {
                    return false;
                }
            }
        
            return true;
        });
    }());
}