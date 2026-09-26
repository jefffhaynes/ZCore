#pragma once

#include "Crc.h"
#include "Streams/MemoryStream.h"
#include "Streams/BoundedInputStream.h"
#include "Streams/CrcInputStream.h"
#include "Streams/NullInputStream.h"

// Waiting out a timeout would reach the kernel clock, so these would not compile.
namespace InputStreamTests
{
    static_assert([]{
        uint8_t bytes[] = { 1, 2, 3 };
        MemoryStream stream{Span<uint8_t>(bytes)};
        uint8_t data[4] = {};

        return !stream.IsEndOfStream()
            && stream.Read(Span<uint8_t>(data), TimeSpan::FromSeconds(5)) == ReturnCode::InvalidLength
            && stream.IsEndOfStream();
    }(), "MemoryStream failed at its end");

    static_assert([]{
        uint8_t bytes[] = { 1, 2, 3, 4 };
        MemoryStream memory{Span<uint8_t>(bytes)};
        BoundedInputStream bounded(memory, 2);
        uint8_t data[2] = {};
        uint8_t more[1] = {};

        return bounded.Read(Span<uint8_t>(data)) == ReturnCode::Success && data[1] == 2
            && bounded.IsEndOfStream() && !memory.IsEndOfStream()
            && bounded.Read(Span<uint8_t>(more)) == ReturnCode::InvalidLength;
    }(), "BoundedInputStream failed at its end");

    static_assert([]{
        uint8_t bytes[] = { 1 };
        MemoryStream memory{Span<uint8_t>(bytes)};
        Crc8<> crc;
        CrcInputStream stream(memory, crc);
        uint8_t data[2] = {};

        return stream.Read(Span<uint8_t>(data)) == ReturnCode::InvalidLength && stream.IsEndOfStream();
    }(), "CrcInputStream failed at its end");

    static_assert([]{
        NullInputStream stream;
        uint8_t data[1] = {};

        return stream.IsEndOfStream() && stream.Read(Span<uint8_t>(data)) == ReturnCode::InvalidLength;
    }(), "NullInputStream failed");

    static_assert([]{
        NullInputStream stream;
        return stream.Read(Span<uint8_t>()) == ReturnCode::Success;
    }(), "Empty read failed at the end");
}
