#pragma once

#include "Streams/StreamWriter.h"
#include "Streams/MemoryStream.h"

namespace StreamWriterTests
{
    static_assert([]{
        Array<uint8_t, 6> data;
        MemoryStream stream(data.AsSpan());
        StreamWriter writer(stream);

        return writer.Write(uint32_t{0x12345678}) == ReturnCode::Success
            && writer.Write(uint16_t{0xABCD}) == ReturnCode::Success
            && data[0] == 0x78 && data[3] == 0x12 && data[4] == 0xCD && data[5] == 0xAB;
    }(), "StreamWriter failed little-endian");

    enum class TestCommand : uint16_t
    {
        Reset = 0x0102
    };

    static_assert([]{
        Array<uint8_t, 10> data;
        MemoryStream stream(data.AsSpan());
        StreamWriter writer(stream, Endianness::BigEndian);

        uint8_t expected[] = { 0x12, 0x34, 0x56, 0x78, 0x3F, 0x80, 0x00, 0x00, 0x01, 0x02 };
        return writer.Write(uint32_t{0x12345678}) == ReturnCode::Success
            && writer.Write(1.0f) == ReturnCode::Success
            && writer.Write(TestCommand::Reset) == ReturnCode::Success
            && data.AsSpan().SequenceEquals(Span<uint8_t>(expected));
    }(), "StreamWriter failed big-endian");

    static_assert([]{
        Array<uint8_t, 3> data;
        data.Fill(0xAA);
        MemoryStream stream(data.AsSpan());
        StreamWriter writer(stream);

        return writer.Write(uint32_t{0x12345678}) == ReturnCode::InvalidLength
            && data[0] == 0xAA && data[2] == 0xAA;
    }(), "StreamWriter failed on overflow");

    static_assert([]{
        Array<uint8_t, 2> data;
        MemoryStream stream(data.AsSpan());
        StreamWriter writer(stream);
        uint8_t bytes[] = { 1, 2 };
        Span<uint8_t> source(bytes);

        return writer.Write(source) == ReturnCode::Success && data[0] == 1 && data[1] == 2;
    }(), "StreamWriter failed for a mutable span");
}
