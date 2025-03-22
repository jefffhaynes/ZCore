#pragma once

#include "SpanReader.h"

static_assert(SpanReader<int>(Span<int>()).ReadSpan(0).GetLength() == 0);
static_assert(SpanReader<int>(Span<int>()).ReadSpan(1).GetLength() == 0);

static_assert([]() constexpr
{
    int data[] = { 1, 2, 3 };
    SpanReader<int> reader(data);
    auto span = reader.ReadSpan(2);
    auto expected = Span<int>(data).Take(2);
    return span.SequenceEquals(expected);
}());

static_assert([]() constexpr
{
    int data[] = { 1, 2, 3 };
    SpanReader<int> reader(data);
    auto span = reader.ReadSpan(3);
    auto expected = Span<int>(data);
    return span.SequenceEquals(expected);
}());

static_assert([]() constexpr
{
    int data[] = { 1, 2, 3 };
    SpanReader<int> reader(data);
    auto span = reader.ReadSpan(4);
    auto expected = Span<int>(data);
    return span.SequenceEquals(expected);
}());

static_assert([]() constexpr
{
    uint8_t data[] = { 1, 2, 3, 4 };
    SpanReader<uint8_t> reader(data);
    int value = 0;
    auto rc = reader.Read(value);
    return rc == ReturnCode::Success && value == 0x04030201;
}());

enum class TestEnum
{
    Value1 = 1,
    Value2 = 2,
    Value3 = 3
};

static_assert([]() constexpr
{
    uint8_t data[] = { 1, 0, 0, 0 };
    SpanReader<uint8_t> reader(data);
    TestEnum value = TestEnum::Value1;
    auto rc = reader.Read(value);
    return rc == ReturnCode::Success && value == TestEnum::Value1;
}());

static_assert([]() constexpr
{
    uint8_t data[] = { 1, 2, 3, 4 };
    SpanReader<uint8_t> reader(data);
    
    Array<uint8_t, 4> value;
    auto rc = reader.Read(value.AsSpan());
    return rc == ReturnCode::Success && value[0] == 0x01 && value[1] == 0x02 && value[2] == 0x03 && value[3] == 0x04;
}());