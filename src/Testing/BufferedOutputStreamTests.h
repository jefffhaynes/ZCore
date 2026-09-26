#pragma once

#include "Streams/BufferedOutputStream.h"
#include "Streams/NullOutputStream.h"
#include "Streams/MemoryStream.h"
#include "Streams/StreamWriter.h"
#include "Array.h"

static_assert([]{
    NullOutputStream nullStream;
    Array<uint8_t, 10> buffer;
    BufferedOutputStream stream(nullStream, buffer);
    StreamWriter writer(stream);
    auto rc = writer.Write(5);
    return rc == ReturnCode::Success;
}(), "BufferedOutputStream failed");

static_assert([]{
    Array<uint8_t, 3> data;
    MemoryStream target(data.AsSpan());
    BufferedOutputStream stream(target, Span<uint8_t>());
    uint8_t bytes[] = { 1, 2, 3 };
    return stream.Write(Span<const uint8_t>(bytes)) == ReturnCode::Success && data[0] == 1 && data[2] == 3;
}(), "BufferedOutputStream failed with no buffer");
