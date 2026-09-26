#pragma once

#include "BinaryPrimitives.h"

namespace BinaryPrimitivesTests
{
    static_assert(BinaryPrimitives::ReverseEndianness(uint8_t{0xAB}) == 0xAB, "ReverseEndianness failed for 8 bits");
    static_assert(BinaryPrimitives::ReverseEndianness(uint16_t{0x1234}) == 0x3412, "ReverseEndianness failed for 16 bits");
    static_assert(BinaryPrimitives::ReverseEndianness(uint32_t{0x12345678}) == 0x78563412, "ReverseEndianness failed for 32 bits");
    static_assert(BinaryPrimitives::ReverseEndianness(uint64_t{0x0102030405060708}) == 0x0807060504030201, "ReverseEndianness failed for 64 bits");
    static_assert(BinaryPrimitives::ReverseEndianness(int16_t{-2}) == int16_t{-257}, "ReverseEndianness failed for a signed value");

    // integers only, as in .NET: reversing a bool or a float means nothing
    template<typename T>
    concept Reversible = requires(T value) { BinaryPrimitives::ReverseEndianness(value); };

    static_assert(Reversible<uint32_t> && Reversible<int8_t> && !Reversible<bool> && !Reversible<float>,
        "ReverseEndianness accepts the wrong types");

    // Writing `value` in `endianness` produces exactly `expected`.
    template<typename T, uint32_t N>
    constexpr bool Writes(T value, Endianness endianness, const uint8_t (&expected)[N])
    {
        uint8_t bytes[N] = {};
        return BinaryPrimitives::Write(Span<uint8_t>(bytes), value, endianness) == ReturnCode::Success
            && Span<const uint8_t>(bytes).SequenceEquals(Span<const uint8_t>(expected));
    }

    // Reading `bytes` in `endianness` produces exactly `expected`.
    template<typename T, uint32_t N>
    constexpr bool Reads(const uint8_t (&bytes)[N], Endianness endianness, T expected)
    {
        T value{};
        return BinaryPrimitives::Read(Span<const uint8_t>(bytes), value, endianness) == ReturnCode::Success
            && value == expected;
    }

    constexpr uint8_t Big32[] = { 0x12, 0x34, 0x56, 0x78 };
    constexpr uint8_t Little32[] = { 0x78, 0x56, 0x34, 0x12 };

    static_assert(Writes(uint32_t{0x12345678}, Endianness::BigEndian, Big32), "Write failed big-endian");
    static_assert(Writes(uint32_t{0x12345678}, Endianness::LittleEndian, Little32), "Write failed little-endian");
    static_assert(Reads(Big32, Endianness::BigEndian, uint32_t{0x12345678}), "Read failed big-endian");
    static_assert(Reads(Big32, Endianness::LittleEndian, uint32_t{0x78563412}), "Read failed little-endian");

    constexpr uint8_t Big16[] = { 0x01, 0x02 };
    static_assert(Writes(uint16_t{0x0102}, Endianness::BigEndian, Big16) && Reads(Big16, Endianness::BigEndian, uint16_t{0x0102}),
        "16-bit round trip failed");

    constexpr uint8_t Big64[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    static_assert(Writes(uint64_t{0x0102030405060708}, Endianness::BigEndian, Big64)
        && Reads(Big64, Endianness::BigEndian, uint64_t{0x0102030405060708}), "64-bit round trip failed");

    constexpr uint8_t BigMinusTwo[] = { 0xFF, 0xFF, 0xFF, 0xFE };
    constexpr uint8_t LittleMinusTwo16[] = { 0xFE, 0xFF };
    static_assert(Writes(int32_t{-2}, Endianness::BigEndian, BigMinusTwo) && Reads(BigMinusTwo, Endianness::BigEndian, int32_t{-2}),
        "Signed 32-bit round trip failed");
    static_assert(Writes(int16_t{-2}, Endianness::LittleEndian, LittleMinusTwo16)
        && Reads(LittleMinusTwo16, Endianness::LittleEndian, int16_t{-2}), "Signed 16-bit round trip failed");

    // floating point goes through its IEEE 754 bit pattern
    constexpr uint8_t BigOne[] = { 0x3F, 0x80, 0x00, 0x00 };
    constexpr uint8_t BigMinusTwoPointFive[] = { 0xC0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static_assert(Writes(1.0f, Endianness::BigEndian, BigOne) && Reads(BigOne, Endianness::BigEndian, 1.0f),
        "float round trip failed");
    static_assert(Writes(-2.5, Endianness::BigEndian, BigMinusTwoPointFive)
        && Reads(BigMinusTwoPointFive, Endianness::BigEndian, -2.5), "double round trip failed");

    constexpr uint8_t One[] = { 0x01 };
    constexpr uint8_t Two[] = { 0x02 };
    constexpr uint8_t Zero[] = { 0x00 };
    static_assert(Writes(true, Endianness::BigEndian, One) && Reads(Two, Endianness::BigEndian, true)
        && Reads(Zero, Endianness::BigEndian, false), "bool failed");

    // the named forms, after .NET
    static_assert([]{
        uint8_t bytes[4] = {};
        uint32_t value = 0;
        return BinaryPrimitives::WriteBigEndian(Span<uint8_t>(bytes), uint32_t{0x12345678}) == ReturnCode::Success
            && bytes[0] == 0x12 && bytes[3] == 0x78
            && BinaryPrimitives::ReadLittleEndian(Span<const uint8_t>(bytes), value) == ReturnCode::Success
            && value == 0x78563412
            && BinaryPrimitives::TryWriteLittleEndian(Span<uint8_t>(bytes), uint32_t{0x12345678}) && bytes[0] == 0x78
            && BinaryPrimitives::TryReadBigEndian(Span<const uint8_t>(bytes), value) && value == 0x78563412;
    }(), "Named forms failed");

    // too short: an error, and neither side is touched
    static_assert([]{
        uint8_t bytes[3] = { 0xAA, 0xBB, 0xCC };
        uint32_t value = 7;
        return BinaryPrimitives::Write(Span<uint8_t>(bytes), uint32_t{0}, Endianness::BigEndian) == ReturnCode::InvalidLength
            && bytes[0] == 0xAA && bytes[2] == 0xCC
            && BinaryPrimitives::Read(Span<const uint8_t>(bytes), value, Endianness::BigEndian) == ReturnCode::InvalidLength
            && value == 7
            && !BinaryPrimitives::TryReadBigEndian(Span<const uint8_t>(bytes), value)
            && !BinaryPrimitives::TryWriteLittleEndian(Span<uint8_t>(bytes), value);
    }(), "Short buffers failed");

    // only the first sizeof(T) bytes are used
    static_assert(Reads(Big64, Endianness::BigEndian, uint16_t{0x0102}), "Read failed from a longer span");
}
