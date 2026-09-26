#pragma once

#include "Streams/StreamReader.h"
#include "Streams/MemoryStream.h"

namespace StreamReaderTests
{
    enum class TestCommand : uint16_t
    {
        Reset = 0x0102
    };

    static_assert([]{
        uint8_t bytes[] = { 0x01, 0x02 };
        MemoryStream stream{Span<uint8_t>(bytes)};
        StreamReader reader(stream);
        uint16_t value = 0;

        return reader.Read(value) == ReturnCode::Success && value == 0x0201;
    }(), "StreamReader failed little-endian");

    static_assert([]{
        uint8_t bytes[] = { 0x12, 0x34, 0x56, 0x78, 0x01, 0x02, 0x3F, 0x80, 0x00, 0x00 };
        MemoryStream stream{Span<uint8_t>(bytes)};
        StreamReader reader(stream, Endianness::BigEndian);
        uint32_t value = 0;
        TestCommand command = {};
        float number = 0;

        return reader.Read(value) == ReturnCode::Success && value == 0x12345678
            && reader.Read(command, TimeSpan::FromSeconds(1)) == ReturnCode::Success && command == TestCommand::Reset
            && reader.Read(number) == ReturnCode::Success && number == 1.0f;
    }(), "StreamReader failed big-endian");

    static_assert([]{
        uint8_t bytes[] = { 1, 2, 3 };
        MemoryStream stream{Span<uint8_t>(bytes)};
        StreamReader reader(stream);
        uint32_t value = 7;

        return reader.Read(value, TimeSpan::FromSeconds(5)) == ReturnCode::InvalidLength && value == 7;
    }(), "StreamReader failed at the end of the stream");
}
