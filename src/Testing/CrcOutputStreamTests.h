#pragma once

#include "Crc.h"
#include "Streams/CrcOutputStream.h"
#include "Streams/NullOutputStream.h"
#include "Array.h"

static_assert([]{
    NullOutputStream nullStream;
    Crc8<> crc;
    CrcOutputStream stream(nullStream, crc);
    Array<const uint8_t, 5> data = { (uint8_t) 0x01, (uint8_t) 0x02, (uint8_t) 0x03, (uint8_t) 0x04, (uint8_t) 0x05 };
    auto rc = stream.Write(data);
    return rc == ReturnCode::Success && crc.GetValue() == 0xBC;
}(), "CrcOutputStream failed");