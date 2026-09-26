#pragma once

#include "OutputStream.h"
#include "MemoryMarshal.h"
#include "BinaryPrimitives.h"

// Writes values to a stream. Numbers and enums are written in the writer's
// byte order, little-endian unless given; structs are copied as raw bytes.
class StreamWriter
{
public:
    constexpr StreamWriter(OutputStream& stream, Endianness endianness = Endianness::LittleEndian)
        : _stream(stream), _endianness(endianness)
    {
    }

    template <Arithmetic T>
    constexpr ReturnCode Write(T value)
    {
        uint8_t bytes[sizeof(T)] = {};
        auto rc = BinaryPrimitives::Write(Span<uint8_t>(bytes), value, _endianness);
        CHECK_RETURN_CODE(rc);

        return _stream.Write(Span<const uint8_t>(bytes));
    }

    template <Enum T>
    constexpr ReturnCode Write(T value)
    {
        auto underlying = static_cast<std::underlying_type_t<T>>(value);
        return Write(underlying);
    }

    template <ComplexSafe T>
    constexpr ReturnCode Write(T& value)
    {
        auto span = MemoryMarshal::AsConstBytes(value);
        return _stream.Write(span);
    }

    constexpr ReturnCode Write(Span<const uint8_t> data)
    {
        return _stream.Write(data);
    }

private:
    OutputStream& _stream;
    Endianness _endianness;
};
