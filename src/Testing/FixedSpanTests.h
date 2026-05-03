#pragma once

#include "FixedSpan.h"

namespace FixedSpanTests
{
    constexpr int testData[] = { 1, 2, 3, 4 };
    constexpr FixedSpan<const int, 4> testSpan = FixedSpan<const int, 4>::FromArray(testData);

    static_assert(testSpan.GetData() == testData, "GetData failed");
    static_assert(testSpan.GetLength() == 4, "GetLength failed");
    static_assert(testSpan[0] == 1, "operator[] failed");
    static_assert(testSpan.Get<2>() == 3, "Get<Index> failed");

    constexpr auto offsetSpan = FixedSpan<const int, 2>::FromArray<1>(testData);
    static_assert(offsetSpan[0] == 2 && offsetSpan[1] == 3, "Offset FromArray failed");

    constexpr auto takenSpan = testSpan.Take<2>();
    static_assert(takenSpan.GetLength() == 2, "Take failed");
    static_assert(takenSpan.Get<1>() == 2, "Take contents failed");

    static_assert([]{
        int data[] = { 1, 2, 3, 4 };
        auto span = FixedSpan<int, 4>::FromArray(data);
        if(span.Set(1, 20) != ReturnCode::Success)
        {
            return false;
        }

        span.Set<2>(30);

        int value = 0;
        if(!span.TryGet(2, value))
        {
            return false;
        }

        return data[1] == 20 && value == 30;
    }(), "Set/Get/TryGet failed");

    static_assert([]{
        int data[] = { 1, 2, 3, 4 };
        auto span = FixedSpan<int, 4>::FromArray(data);
        return span.Set(4, 10) == ReturnCode::OutOfRange && !span.TrySet(4, 10);
    }(), "Out-of-range handling failed");

    static_assert([]{
        int sourceData[] = { 1, 2, 3, 4 };
        int destinationData[] = { 0, 0, 0, 0 };

        auto source = FixedSpan<int, 4>::FromArray(sourceData);
        auto destination = FixedSpan<int, 4>::FromArray(destinationData);

        if(source.CopyTo(destination) != ReturnCode::Success)
        {
            return false;
        }

        return destinationData[0] == 1 && destinationData[3] == 4;
    }(), "CopyTo failed");
}