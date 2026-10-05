#pragma once

#include "FixedSpan.h"
#include "Array.h"

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

    static_assert([]{
        int destinationData[] = { 0, 0, 0, 0 };
        auto destination = FixedSpan<int, 4>::FromArray(destinationData);

        return testSpan.CopyTo(destination) == ReturnCode::Success && testSpan.AsSpan().SequenceEquals(destinationData);
    }(), "CopyTo from a const span failed");

    static_assert([]{
        int sum = 0;

        for(auto value : testSpan)
        {
            sum += value;
        }

        return sum == 10;
    }(), "Iterating a const span failed");

    static_assert([]{
        int data[] = { 1, 2, 3, 4 };
        int doubled[] = { 2, 4, 6, 8 };
        auto span = FixedSpan<int, 4>::FromArray(data);

        for(auto& value : span)
        {
            value *= 2;
        }

        return span.SequenceEquals(doubled);
    }(), "Iterating didn't reach the elements themselves");

    static_assert([]{
        uint32_t count = 0;

        for(auto value : offsetSpan)
        {
            count += value == 2 || value == 3 ? 1 : 0;
        }

        return count == 2;
    }(), "Iterating ran outside the span");

    constexpr int sameData[] = { 1, 2, 3, 4 };
    constexpr int otherData[] = { 1, 2, 3, 5 };
    constexpr int longerData[] = { 1, 2, 3, 4, 5 };

    static_assert(testSpan.SequenceEquals(sameData) && !testSpan.SequenceEquals(otherData),
        "SequenceEquals failed against an array");
    static_assert(!testSpan.SequenceEquals(longerData) && !testSpan.SequenceEquals(Span<const int>()),
        "SequenceEquals matched a different length");
    static_assert(testSpan.SequenceEquals(Span<const int>(sameData))
        && !testSpan.SequenceEquals(Span<const int>(sameData).Take(3)), "SequenceEquals failed against a Span");
    static_assert(testSpan.SequenceEquals(FixedSpan<const int, 4>::FromArray(sameData))
        && !testSpan.SequenceEquals(FixedSpan<const int, 4>::FromArray(otherData))
        && !testSpan.SequenceEquals(FixedSpan<const int, 3>::FromArray(sameData))
        && testSpan.Take<2>().SequenceEquals(FixedSpan<const int, 2>::FromArray(longerData)),
        "SequenceEquals failed against a FixedSpan");

    static_assert([]{
        int data[] = { 1, 2, 3, 4 };
        Array<int, 4> array(1, 2, 3, 4);
        auto span = FixedSpan<int, 4>::FromArray(data);

        return span.SequenceEquals(testSpan) && testSpan.SequenceEquals(span) && span.SequenceEquals(array)
            && span.SequenceEquals(data);
    }(), "SequenceEquals failed between const and mutable spans");
}
