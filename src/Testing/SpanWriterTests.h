#pragma once

#include "SpanWriter.h"
#include "Array.h"

static_assert([]() constexpr
{
    int data[3] = { 0 };
    SpanWriter<int> writer(data);
    int data2[3] = { 1, 2, 3 };
    auto rc = writer.Write(data2);
    return rc == ReturnCode::Success && writer.GetWritten() == 3 && data[0] == 1 && data[1] == 2 && data[2] == 3;
}());

static_assert([]() constexpr
{
    int data[3] = { 0 };
    SpanWriter<int> writer(data);
    int data2[3] = { 1, 2, 3 };
    auto rc = writer.Write(data2);
    
    if(rc != ReturnCode::Success)
    {
        return false;
    }

    auto written = writer.GetWrittenSpan();
    auto expected = Span<int>(data);
    return written.SequenceEquals(expected);
}());

static_assert([]() constexpr
{
    int data[3] = { 0 };
    SpanWriter<int> writer(data);
    int data2[4] = { 1, 2, 3, 4 };
    auto rc = writer.Write(data2);
    return rc == ReturnCode::InvalidLength && writer.GetWritten() == 0;
}());

static_assert([]() constexpr
{
    Array<int, 3> data;
    SpanWriter<int> writer(data);
    int data2[3] = { 1, 2, 3 };
    auto rc = writer.Write(data2);
    auto expected = Span<int>(data2);
    return rc == ReturnCode::Success && writer.GetWritten() == 3 && data.SequenceEquals(expected);
}());
