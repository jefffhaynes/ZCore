#pragma once

#include "InputStream.h"
#include "MemoryMarshal.h"
#include "BinaryPrimitives.h"
#include "NullOutputStream.h"

class StreamReader
{
public:
    constexpr StreamReader(InputStream& stream, Endianness endianness = Endianness::LittleEndian)
        : _stream(stream), _endianness(endianness)
    {
    }

    template <Arithmetic T>
    constexpr ReturnCode Read(T& value, TimeSpan timeout)
    {
        uint8_t bytes[sizeof(T)] = {};
        auto rc = Read(Span<uint8_t>(bytes), timeout);
        CHECK_RETURN_CODE(rc);

        return BinaryPrimitives::Read(Span<const uint8_t>(bytes), value, _endianness);
    }

    template <Arithmetic T>
    constexpr ReturnCode Read(T& value)
    {
        uint8_t bytes[sizeof(T)] = {};
        auto rc = Read(Span<uint8_t>(bytes));
        CHECK_RETURN_CODE(rc);

        return BinaryPrimitives::Read(Span<const uint8_t>(bytes), value, _endianness);
    }

    template <Enum T>
    constexpr ReturnCode Read(T& value, TimeSpan timeout)
    {
        std::underlying_type_t<T> underlying;
        auto rc = Read(underlying, timeout);
        CHECK_RETURN_CODE(rc);

        value = static_cast<T>(underlying);
        return ReturnCode::Success;
    }

    template <Enum T>
    constexpr ReturnCode Read(T& value)
    {
        std::underlying_type_t<T> underlying;
        auto rc = Read(underlying);
        CHECK_RETURN_CODE(rc);

        value = static_cast<T>(underlying);
        return ReturnCode::Success;
    }

    constexpr ReturnCode Read(Span<uint8_t> data, TimeSpan timeout)
    {
        return _stream.Read(data, timeout);
    }

    constexpr ReturnCode Read(Span<uint8_t> data)
    {
        return _stream.Read(data);
    }

    // A string too long for `data` is still consumed through its terminator.
    ReturnCode ReadString(Span<char> data, TimeSpan timeout)
    {
        auto deadline = Clock::GetUptime() + timeout;
        uint32_t length = 0;

        while (true)
        {
            auto now = Clock::GetUptime();
            auto remaining = now < deadline ? deadline - now : TimeSpan::Zero();

            uint8_t c = 0;
            auto rc = Read(c, remaining);
            CHECK_RETURN_CODE(rc);

            if (length < data.GetLength())
            {
                data.Set(length, static_cast<char>(c));
            }

            length++;

            if (c == 0)
            {
                break;
            }
        }

        if (length > data.GetLength())
        {
            data.Set(data.GetLength() - 1, 0);
            return ReturnCode::InvalidLength;
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