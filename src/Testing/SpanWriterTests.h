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

static_assert([]() constexpr
{
    Array<int, 3> data;
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
    Array<uint8_t, 4> data;
    SpanWriter<uint8_t> writer(data);
    
    int value = 0x12345678;
    auto rc = writer.Write(value);
    
    if(rc != ReturnCode::Success)
    {
        return false;
    }

    auto written = writer.GetWrittenSpan();
    uint8_t expected[] = { 0x78, 0x56, 0x34, 0x12 };
    return written.SequenceEquals(Span<uint8_t>(expected));
}());

static_assert([]() constexpr
{
    Array<uint8_t, 8> data;
    SpanWriter<uint8_t> writer(data, Endianness::BigEndian);

    auto rc = writer.Write(uint8_t{0x8E});
    rc = rc == ReturnCode::Success ? writer.Write(uint8_t{0xA1}) : rc;
    rc = rc == ReturnCode::Success ? writer.Write(uint16_t{12}) : rc;
    rc = rc == ReturnCode::Success ? writer.Write(uint32_t{0x12345678}) : rc;

    uint8_t expected[] = { 0x8E, 0xA1, 0x00, 0x0C, 0x12, 0x34, 0x56, 0x78 };
    return rc == ReturnCode::Success && writer.GetWrittenSpan().SequenceEquals(Span<uint8_t>(expected));
}());

enum class TestCommand : uint16_t
{
    Reset = 0x0102
};

static_assert([]() constexpr
{
    Array<uint8_t, 6> data;
    SpanWriter<uint8_t> writer(data, Endianness::BigEndian);

    auto rc = writer.Write(1.0f);
    rc = rc == ReturnCode::Success ? writer.Write(TestCommand::Reset) : rc;

    uint8_t expected[] = { 0x3F, 0x80, 0x00, 0x00, 0x01, 0x02 };
    return rc == ReturnCode::Success && writer.GetWrittenSpan().SequenceEquals(Span<uint8_t>(expected));
}());

static_assert([]() constexpr
{
    Array<uint8_t, 3> data;
    data.Fill(0xAA);
    SpanWriter<uint8_t> writer(data);
    auto rc = writer.Write(uint32_t{0x12345678});
    return rc == ReturnCode::InvalidLength && writer.GetWritten() == 0 && data[0] == 0xAA && data[2] == 0xAA;
}());

static_assert([]() constexpr
{
    int data[2] = { 0 };
    int expected[] = { 300, -2 };
    SpanWriter<int> writer(data);

    auto rc = writer.Write(300);
    rc = rc == ReturnCode::Success ? writer.Write(-2) : rc;

    return rc == ReturnCode::Success && writer.GetWrittenSpan().SequenceEquals(expected)
        && writer.Write(1) == ReturnCode::InvalidLength && writer.GetWritten() == 2;
}(), "A single value went through a byte");

static_assert([]() constexpr
{
    Array<char, 2> data;
    SpanWriter<char> writer(data);

    auto rc = writer.Write('o');
    rc = rc == ReturnCode::Success ? writer.Write('k') : rc;

    return rc == ReturnCode::Success && String(writer.GetWrittenSpan()) == String("ok")
        && writer.Write('!') == ReturnCode::InvalidLength;
}(), "Writing characters failed");
