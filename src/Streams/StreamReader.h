#pragma once

#include "InputStream.h"
#include "MemoryMarshal.h"
#include "BinaryPrimitives.h"
#include "NullOutputStream.h"

// Reads values from a stream. Numbers and enums are read in the reader's byte
// order, little-endian unless given; structs are copied as raw bytes.
class StreamReader
{
public:
    StreamReader(InputStream& stream, Endianness endianness = Endianness::LittleEndian)
        : _stream(stream), _endianness(endianness)
    {
    }

    template <Arithmetic T>
    ReturnCode Read(T& value, TimeSpan timeout)
    {
        uint8_t bytes[sizeof(T)];
        auto rc = Read(Span<uint8_t>(bytes), timeout);
        CHECK_RETURN_CODE(rc);

        return BinaryPrimitives::Read(Span<const uint8_t>(bytes), value, _endianness);
    }

    template <Arithmetic T>
    ReturnCode Read(T& value)
    {
        uint8_t bytes[sizeof(T)];
        auto rc = Read(Span<uint8_t>(bytes));
        CHECK_RETURN_CODE(rc);

        return BinaryPrimitives::Read(Span<const uint8_t>(bytes), value, _endianness);
    }

    template <Enum T>
    ReturnCode Read(T& value, TimeSpan timeout)
    {
        std::underlying_type_t<T> underlying;
        auto rc = Read(underlying, timeout);
        CHECK_RETURN_CODE(rc);

        value = static_cast<T>(underlying);
        return ReturnCode::Success;
    }

    template <Enum T>
    ReturnCode Read(T& value)
    {
        std::underlying_type_t<T> underlying;
        auto rc = Read(underlying);
        CHECK_RETURN_CODE(rc);

        value = static_cast<T>(underlying);
        return ReturnCode::Success;
    }

    template <ComplexSafe T>
    ReturnCode Read(T& value, TimeSpan timeout)
    {
        auto data = MemoryMarshal::AsBytes(value);
        return Read(data, timeout);
    }

    template <ComplexSafe T>
    ReturnCode Read(T& value)
    {
        auto data = MemoryMarshal::AsBytes(value);
        return Read(data);
    }

    ReturnCode Read(Span<uint8_t> data, TimeSpan timeout)
    {
        return _stream.Read(data, timeout);
    }

    ReturnCode Read(Span<uint8_t> data)
    {
        return _stream.Read(data);
    }

    ReturnCode ReadString(Span<char> data, TimeSpan timeout)
    {
        for (uint32_t i = 0; i < data.GetLength(); i++)
        {
            uint8_t c;
            auto rc = Read(c, timeout);
            CHECK_RETURN_CODE(rc);

            rc = data.Set(i, (char) c);
            CHECK_RETURN_CODE(rc);

            if (c == 0)
            {
                break;
            }
        }

        return ReturnCode::Success;
    }

    ReturnCode Advance(uint32_t length, TimeSpan timeout)
    {
        NullOutputStream nullSink;
        return _stream.CopyTo(nullSink, length, timeout);
    }

    ReturnCode AdvanceToEnd()
    {
        NullOutputStream nullSink;
        return _stream.CopyTo(nullSink);
    }

    constexpr InputStream& GetStream()
    {
        return _stream;
    }

private:
    InputStream& _stream;
    Endianness _endianness;
};