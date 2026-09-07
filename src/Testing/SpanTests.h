#pragma once

#include "Span.h"

namespace SpanTests
{
    constexpr int testArray[5] = {1, 2, 3, 4, 5};
    constexpr Span<const int> testSpan(testArray);

    static_assert(testSpan.GetData() == testArray, "GetData method failed");
    static_assert(testSpan.GetLength() == 5, "GetLength method failed");
    static_assert(!testSpan.IsEmpty(), "IsEmpty method failed");

    constexpr Span<const int> emptySpan;
    static_assert(emptySpan.IsEmpty(), "IsEmpty method failed for empty span");

    constexpr Span<const int> takenSpan = testSpan.Take(3);
    static_assert(takenSpan.GetLength() == 3, "Take method failed");
    static_assert(takenSpan.TryCompare(0, 1), "Take method failed");

    constexpr Span<const int> skippedSpan = testSpan.Skip(3);
    static_assert(skippedSpan.GetLength() == 2, "Skip method failed");
    static_assert(skippedSpan.TryCompare(0, 4), "Skip method failed");

    static_assert([]{
        int mutableArray[5] = {1, 2, 3, 4, 5};
        Span<int> mutableSpan(mutableArray);
        if(mutableSpan.Set(0, 2) != ReturnCode::Success)
        {
            return false;
        }

        return mutableSpan.TryCompare(0, 2);
    }(), "Set method failed");

    static_assert([]{
        int values[5] = {1, 2, 3, 4, 5};
        Span<int> span(values);
        span.ShiftLeft(2);
        return span.TryCompare(0, 3) && span.TryCompare(1, 4) && span.TryCompare(2, 5)
            && span.TryCompare(3, 4) && span.TryCompare(4, 5);
    }(), "ShiftLeft method failed");

    static_assert([]{
        int values[3] = {1, 2, 3};
        Span<int> span(values);
        span.ShiftLeft(0);
        span.ShiftLeft(3);  // whole span discarded - storage untouched
        span.ShiftLeft(7);  // beyond length - no-op
        return span.TryCompare(0, 1) && span.TryCompare(1, 2) && span.TryCompare(2, 3);
    }(), "ShiftLeft boundary handling failed");

    static_assert([]{
        int value = 0;
        if(testSpan.Get(0, value) != ReturnCode::Success)
        {
            return false;
        }

        return value == 1;
    }(), "Get method failed");

    static_assert([]{
        int value = 0;
        return testSpan.Get(5, value) == ReturnCode::OutOfRange;
    }(), "Get method failed");

    static_assert([]{
        int value = 0;
        if(!testSpan.TryGet(0, value))
        {
            return false;
        }

        return value == 1;
    }(), "TryGet method failed");

    static_assert([]{
        int value = 0;
        return !testSpan.TryGet(5, value);
    }(), "TryGet method failed");

    static_assert([]{
        int dest[5] = {0};
        Span<int> destSpan(dest);
        if(testSpan.CopyTo(destSpan) != ReturnCode::Success)
        {
            return false;
        }

        return destSpan.TryCompare(0, 1);
    }(), "CopyTo method failed");

    static_assert([]{
        int dest[4] = {0};
        Span<int> destSpan(dest);
        return testSpan.CopyTo(destSpan) == ReturnCode::InvalidLength;
    }(), "CopyTo method failed");

    static_assert([]{
        int dest[6] = {0};
        Span<int> destSpan(dest);
        if(testSpan.CopyTo(destSpan, 1) != ReturnCode::Success)
        {
            return false;
        }

        return destSpan.TryCompare(1, 1);
    }(), "CopyTo method failed");

    static_assert([]{
        int dest[5] = {1, 2, 3, 4, 5};
        Span<int> destSpan(dest);
        return testSpan.SequenceEquals(destSpan);
    }(), "SequenceEquals method failed");

    constexpr int TimesTwo(int value)
    {
        return value * 2;
    }

    static_assert([]{
        int dest[5] = {0};
        Span<int> destSpan(dest);
        if(testSpan.CopyTo(destSpan, TimesTwo) != ReturnCode::Success)
        {
            return false;
        }

        return destSpan.TryCompare(0, 2);
    }(), "CopyTo method failed");
    
    static_assert([]{
        int dest[4] = {0};
        Span<int> destSpan(dest);
        return testSpan.CopyTo(destSpan, TimesTwo) == ReturnCode::InvalidLength;
    }(), "CopyTo method failed");

    // skip test
    static_assert([]{
        int dest[5] = {0};
        Span<int> destSpan(dest);
        if(testSpan.Skip(2).CopyTo(destSpan) != ReturnCode::Success)
        {
            return false;
        }

        return destSpan.TryCompare(0, 3);
    }(), "Skip method failed");

    // take test
    static_assert([]{
        int dest[5] = {0};
        Span<int> destSpan(dest);
        if(testSpan.Take(2).CopyTo(destSpan) != ReturnCode::Success)
        {
            return false;
        }

        return destSpan.TryCompare(0, 1);
    }(), "Take method failed");

    // iterator tests
    static_assert([]{
        int sum = 0;
        for(auto& value : testSpan)
        {
            sum += value;
        }

        return sum == 15;
    }(), "Iterator failed");

    // iterator overrun test
    static_assert([]{
        auto it = testSpan.begin();
        while(it != testSpan.end())
        { 
            ++it;
        }

        ++it;
        
        return it == testSpan.end();
    }(), "Iterator overrun failed");
}