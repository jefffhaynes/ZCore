#pragma once

#include "SpanReader.h"

static_assert(SpanReader<int>(Span<int>()).Read(0).GetLength() == 0);
static_assert(SpanReader<int>(Span<int>()).Read(1).GetLength() == 0);

static_assert([]() constexpr
{
    int data[] = { 1, 2, 3 };
    SpanReader<int> reader(data);
    auto span = reader.Read(2);
    auto expected = Span<int>(data).Take(2);
    return span.SequenceEquals(expected);
}());

static_assert([]() constexpr
{
    int data[] = { 1, 2, 3 };
    SpanReader<int> reader(data);
    auto span = reader.Read(3);
    auto expected = Span<int>(data);
    return span.SequenceEquals(expected);
}());

static_assert([]() constexpr
{
    int data[] = { 1, 2, 3 };
    SpanReader<int> reader(data);
    auto span = reader.Read(4);
    auto expected = Span<int>(data);
    return span.SequenceEquals(expected);
}());