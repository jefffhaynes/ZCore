#pragma once

#include "Array.h"

namespace ArrayTests
{
    constexpr Array<const int, 5> testArray = {1, 2, 3, 4, 5};

    static_assert(testArray.GetLength() == 5, "GetLength method failed");
    static_assert(testArray.TryCompare(0, 1), "TryCompare failed");

    static_assert([]{
        Array<int, 5> mutableArray = {1, 2, 3, 4, 5};
        if(mutableArray.Set(0, 2) != ReturnCode::Success)
        {
            return false;
        }

        return mutableArray.TryCompare(0, 2);
    }(), "Set method failed");

    static_assert([]{
        int value = 0;
        if(testArray.Get(0, value) != ReturnCode::Success)
        {
            return false;
        }

        return value == 1;
    }(), "Get method failed");

    static_assert([]{
        int value = 0;
        return testArray.Get(5, value) == ReturnCode::OutOfRange;
    }(), "Get method failed");

    static_assert([]{
        int value = 0;
        if(!testArray.TryGet(0, value))
        {
            return false;
        }

        return value == 1;
    }(), "TryGet method failed");

    static_assert([]{
        int value = 0;
        return testArray.TryGet(5, value) == false;
    }(), "TryGet method failed");

    static_assert([]{
        auto slice = testArray.Take(3);
        if(slice.GetLength() != 3)
        {
            return false;
        }

        return slice.TryCompare(0, 1);
    }(), "Take method failed");

    static_assert([]{
        auto slice = testArray.Skip(3);
        if(slice.GetLength() != 2)
        {
            return false;
        }

        return slice.TryCompare(0, 4);
    }(), "Skip method failed");

    static_assert([]{
        auto* hello = "Hello";
        auto* world = "World";
        Array<const char*, 2> mutableArray = {hello};

        if(mutableArray.Contains(world))
        {
            return false;
        }
        
        if(mutableArray.GetCount() != 1)
        {
            return false;
        }

        if(mutableArray.Add(world) != ReturnCode::Success)
        {
            return false;
        }

        if(!mutableArray.Contains(world))
        {
            return false;
        }

        if(mutableArray.Add(world) != ReturnCode::OutOfMemory)
        {
            return false;
        }

        if(mutableArray.GetCount() != 2)
        {
            return false;
        }

        mutableArray.Remove(hello);

        if(mutableArray.GetCount() != 1)
        {
            return false;
        }

        return true;
    }(), "Add/Remove/GetCount/Contains method failed");
}