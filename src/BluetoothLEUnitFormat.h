#pragma once

#include <stdint.h>

enum class BluetoothLEUnitFormat : uint8_t
{
    Boolean = 0x01,
    TwoBit = 0x02,
    Nibble = 0x03,
    UInt8 = 0x04,
    UInt12 = 0x05,
    UInt16 = 0x06,
    UInt24 = 0x07,
    UInt32 = 0x08,
    UInt48 = 0x09,
    UInt64 = 0x0A,
    UInt128 = 0x0B,
    Int8 = 0x0C,
    Int12 = 0x0D,
    Int16 = 0x0E,
    Int24 = 0x0F,
    Int32 = 0x10,
    Int48 = 0x11,
    Int64 = 0x12,
    Int128 = 0x13,
    Float32 = 0x14,
    Float64 = 0x15,
    DUInt16 = 0x18,
    UTF8String = 0x19,
    UTF16String = 0x1A,
    Struct = 0x1B
};