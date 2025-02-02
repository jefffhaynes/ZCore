#pragma once

#include "Streams/BufferedOutputStream.h"
#include "Streams/NullOutputStream.h"
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
