#pragma once

#include "OutputStream.h"
#include "MemoryMarshal.h"
#include "BinaryPrimitives.h"

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

    constexpr ReturnCode Write(Span<const uint8_t> data)
    {
        return _stream.Write(data);
    }

private:
    OutputStream& _stream;
    Endianness _endianness;
};
