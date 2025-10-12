#pragma once

#include "Crc.h"
#include "Streams/CrcInputStream.h"
#include "Streams/MemoryStream.h"

static_assert([]{
    Array<uint8_t, 5> testArray = { (uint8_t) 0x01, (uint8_t) 0x02, (uint8_t) 0x03, (uint8_t) 0x04, (uint8_t) 0x05 };
    MemoryStream testStream(testArray.AsSpan());
    Crc8<> crc;
    CrcInputStream stream(testStream, crc);

    Array<uint8_t, 5> data;
    uint32_t read;
    auto rc = stream.Read(data, read);
    return rc == ReturnCode::Success && crc.GetValue() == 0xBC;
}(), "CrcInputStream failed");