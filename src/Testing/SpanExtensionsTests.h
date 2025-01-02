#pragma once

#include "SpanExtensions.h"

static_assert([]() constexpr
{
    int data[] = { 2, 1, 3 };
    auto span = Span<int>(data);
    auto sum = SpanExtensions::Sum(span);
    return sum == 6;
}());

static_assert([]() constexpr
{
    int data[] = { -1, 1 };
    auto span = Span<int>(data);
    auto sum = SpanExtensions::Sum(span);
    return sum == 0;
}());

static_assert([]() constexpr
{
    int data[] = { 2, 1, 3 };
    auto span = Span<int>(data);
    auto min = SpanExtensions::Min(span);
    return min == 1;
}());

static_assert([]() constexpr
{
    int data[] = { 1, -1, 1, -1 };
    auto span = Span<int>(data);

    uint32_t index;
    auto min = SpanExtensions::Min(span, index);
    return min == -1 && index == 1;
}());

static_assert([]() constexpr
{
    int data[] = { -1, 0, 1 };
    auto span = Span<int>(data);
    float mean;
    auto min = SpanExtensions::MinAndMean(span, mean);
    return min == -1 && mean == 0;
}());

static_assert([]() constexpr
{
    int data[] = { -2, 1, 2, -1 };
    auto span = Span<int>(data);
    float mean;
    auto min = SpanExtensions::MinAndMean(span, mean);
    return min == -2 && mean == 0;
}());

static_assert([]() constexpr
{
    int data[] = { 1, -1, 1, -1 };
    auto span = Span<int>(data);
    float mean;
    uint32_t index;
    auto min = SpanExtensions::MinAndMean(span, mean, index);
    return min == -1 && mean == 0 && index == 1;
}());

static_assert([]() constexpr
{
    int data[] = { 2, 1, 3 };
    auto span = Span<int>(data);
    auto max = SpanExtensions::Max(span);
    return max == 3;
}());

static_assert([]() constexpr
{
    int data[] = { 2, 1, 3, 3 };
    auto span = Span<int>(data);
    uint32_t index;
    auto max = SpanExtensions::Max(span, index);
    return max == 3 && index == 2;
}());

static_assert([]() constexpr
{
    int data[] = { 2, 1, 3 };
    auto span = Span<int>(data);
    float mean;
    auto max = SpanExtensions::MaxAndMean(span, mean);
    return max == 3 && mean == 2;
}());

static_assert([]() constexpr
{
    int data[] = { 2, 1, 3 };
    auto span = Span<int>(data);
    float mean;
    uint32_t index;
    auto max = SpanExtensions::MaxAndMean(span, mean, index);
    return max == 3 && mean == 2 && index == 2;
}());

static_assert([]() constexpr
{
    int data[] = { 2, 1, 3 };
    auto span = Span<int>(data);
    auto mean = SpanExtensions::Mean(span);
    return mean == 2;
}());


static_assert([]() constexpr
{
    int16_t data[] = { 2, 1, 3 };
    auto span = Span<int16_t>(data);
    auto mean = SpanExtensions::Mean(span);
    return mean == 2;
}());

static_assert([]() constexpr
{
    int data[] = { 1, 2, 3 };
    auto span = Span<int>(data);
    auto mean = SpanExtensions::Mean(span);
    auto stdDev = SpanExtensions::StandardDeviation(span, mean);
    return std::abs(stdDev - 0.816496580927726) < 0.0001;
}());