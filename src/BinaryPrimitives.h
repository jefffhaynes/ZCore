#pragma once

#include <bit>
#include <concepts>
#include <stdint.h>
#include <type_traits>
#include "Concepts.h"
#include "Span.h"
#include "ReturnCode.h"

// The order a multi-byte value's bytes are stored or sent in.
enum class Endianness
{
    LittleEndian,
    BigEndian
};

// Reads and writes numbers as bytes in a given byte order, whatever the CPU's
// own, after .NET's System.Buffers.Binary.BinaryPrimitives. Where .NET names
// the type (ReadUInt32BigEndian), this is a template (ReadBigEndian<uint32_t>).
// Floating-point values go through their bit patterns. Usable in constant
// evaluation.
class BinaryPrimitives
{
public:
    // The value with its bytes in the opposite order.
    template<std::integral T> requires (!std::same_as<T, bool>)
    static constexpr T ReverseEndianness(T value)
    {
        auto bits = static_cast<Bits<T>>(value);
        Bits<T> reversed = 0;

        for (uint32_t i = 0; i < sizeof(T); i++)
        {
            reversed = static_cast<Bits<T>>((reversed << 8) | ((bits >> (i * 8)) & 0xFF));
        }

        return static_cast<T>(reversed);
    }

    // Reads the first sizeof(T) bytes of `source`.
    template<Arithmetic T>
    static constexpr ReturnCode Read(Span<const uint8_t> source, T& value, Endianness endianness)
    {
        static_assert(sizeof(T) == sizeof(Bits<T>), "Unsupported value size");

        if (source.GetLength() < sizeof(T))
        {
            return ReturnCode::InvalidLength;
        }

        Bits<T> bits = 0;

        for (uint32_t i = 0; i < sizeof(T); i++)
        {
            auto byte = source.GetData()[GetByteIndex<T>(i, endianness)];
            bits = static_cast<Bits<T>>(bits | (static_cast<Bits<T>>(byte) << (i * 8)));
        }

        value = FromBits<T>(bits);

        return ReturnCode::Success;
    }

    // Writes the value to the first sizeof(T) bytes of `destination`.
    template<Arithmetic T>
    static constexpr ReturnCode Write(Span<uint8_t> destination, T value, Endianness endianness)
    {
        static_assert(sizeof(T) == sizeof(Bits<T>), "Unsupported value size");

        if (destination.GetLength() < sizeof(T))
        {
            return ReturnCode::InvalidLength;
        }

        auto bits = ToBits(value);

        for (uint32_t i = 0; i < sizeof(T); i++)
        {
            destination.GetData()[GetByteIndex<T>(i, endianness)] = static_cast<uint8_t>(bits >> (i * 8));
        }

        return ReturnCode::Success;
    }

    template<Arithmetic T>
    static constexpr ReturnCode ReadBigEndian(Span<const uint8_t> source, T& value)
    {
        return Read(source, value, Endianness::BigEndian);
    }

    template<Arithmetic T>
    static constexpr ReturnCode ReadLittleEndian(Span<const uint8_t> source, T& value)
    {
        return Read(source, value, Endianness::LittleEndian);
    }

    template<Arithmetic T>
    static constexpr ReturnCode WriteBigEndian(Span<uint8_t> destination, T value)
    {
        return Write(destination, value, Endianness::BigEndian);
    }

    template<Arithmetic T>
    static constexpr ReturnCode WriteLittleEndian(Span<uint8_t> destination, T value)
    {
        return Write(destination, value, Endianness::LittleEndian);
    }

    template<Arithmetic T>
    static constexpr bool TryReadBigEndian(Span<const uint8_t> source, T& value)
    {
        return ReadBigEndian(source, value) == ReturnCode::Success;
    }

    template<Arithmetic T>
    static constexpr bool TryReadLittleEndian(Span<const uint8_t> source, T& value)
    {
        return ReadLittleEndian(source, value) == ReturnCode::Success;
    }

    template<Arithmetic T>
    static constexpr bool TryWriteBigEndian(Span<uint8_t> destination, T value)
    {
        return WriteBigEndian(destination, value) == ReturnCode::Success;
    }

    template<Arithmetic T>
    static constexpr bool TryWriteLittleEndian(Span<uint8_t> destination, T value)
    {
        return WriteLittleEndian(destination, value) == ReturnCode::Success;
    }

private:
    // The unsigned integer the size of T, which holds its bit pattern.
    template<typename T>
    using Bits = std::conditional_t<sizeof(T) == 1, uint8_t,
                 std::conditional_t<sizeof(T) == 2, uint16_t,
                 std::conditional_t<sizeof(T) == 4, uint32_t, uint64_t>>>;

    // Where the byte of significance `i` (0 = least) sits.
    template<typename T>
    static constexpr uint32_t GetByteIndex(uint32_t i, Endianness endianness)
    {
        return endianness == Endianness::LittleEndian ? i : sizeof(T) - 1 - i;
    }

    template<typename T>
    static constexpr Bits<T> ToBits(T value)
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            return value ? 1 : 0;
        }
        else
        {
            return std::bit_cast<Bits<T>>(value);
        }
    }

    // Any nonzero byte reads as true; a bool has no other valid bit patterns.
    template<typename T>
    static constexpr T FromBits(Bits<T> bits)
    {
        if constexpr (std::is_same_v<T, bool>)
        {
            return bits != 0;
        }
        else
        {
            return std::bit_cast<T>(bits);
        }
    }
};
