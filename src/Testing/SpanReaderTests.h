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

// big-endian by construction, as in network byte order
static_assert([]() constexpr
{
    uint8_t data[] = { 0x8E, 0xA1, 0x00, 0x0C, 0x12, 0x34, 0x56, 0x78 };
    SpanReader<uint8_t> reader(data, Endianness::BigEndian);
    uint8_t first = 0;
    uint8_t second = 0;
    uint16_t length = 0;
    uint32_t value = 0;

    return reader.Read(first) == ReturnCode::Success && reader.Read(second) == ReturnCode::Success
        && reader.Read(length) == ReturnCode::Success && reader.Read(value) == ReturnCode::Success
        && first == 0x8E && second == 0xA1 && length == 12 && value == 0x12345678;
}());

// values can be read from const bytes
static_assert([]() constexpr
{
    constexpr uint8_t data[] = { 0x00, 0x00, 0x00, 0x02 };
    SpanReader<const uint8_t> reader(data, Endianness::BigEndian);
    TestEnum value = TestEnum::Value1;
    auto rc = reader.Read(value);
    return rc == ReturnCode::Success && value == TestEnum::Value2;
}());

static_assert([]() constexpr
{
    uint8_t data[] = { 0x00, 0x00, 0x80, 0x3F };
    SpanReader<uint8_t> reader(data);
    float value = 0;
    auto rc = reader.Read(value);
    return rc == ReturnCode::Success && value == 1.0f;
}());

// a value past the end is an error and leaves the position alone
static_assert([]() constexpr
{
    uint8_t data[] = { 1, 2, 3 };
    SpanReader<uint8_t> reader(data);
    uint32_t tooBig = 0;
    uint16_t value = 0;
    return reader.Read(tooBig) == ReturnCode::InvalidLength
        && reader.Read(value) == ReturnCode::Success && value == 0x0201;
}());

// a span past the end is an error and leaves the position alone
static_assert([]() constexpr
{
    uint8_t data[] = { 1, 2, 3 };
    SpanReader<uint8_t> reader(data);
    Array<uint8_t, 4> tooBig;
    uint16_t value = 0;
    return reader.Read(tooBig.AsSpan()) == ReturnCode::InvalidLength
        && reader.Read(value) == ReturnCode::Success && value == 0x0201;
}());
