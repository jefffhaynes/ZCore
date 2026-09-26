#pragma once

#include "MemoryMarshal.h"
#include "BinaryPrimitives.h"

template<typename T>
class SpanReader
{
    template <typename U>
    static constexpr bool IsBytes = std::is_same_v<std::remove_const_t<U>, uint8_t>;

public:
    constexpr SpanReader(Span<T> span, Endianness endianness = Endianness::LittleEndian)
        : _span(span), _offset(0), _endianness(endianness)
    {
    }

    constexpr Span<T> ReadSpan(uint32_t length)
    {
        auto span = GetRemaining().Take(length);
        _offset += span.GetLength();
        return span;
    }

    template <typename U = T, Arithmetic TValue>
    constexpr std::enable_if_t<IsBytes<U>, ReturnCode> Read(TValue& value)
    {
        auto rc = BinaryPrimitives::Read(GetRemaining(), value, _endianness);
        CHECK_RETURN_CODE(rc);

        _offset += sizeof(TValue);

        return ReturnCode::Success;
    }

    template <typename U = T, Enum TValue>
    constexpr std::enable_if_t<IsBytes<U>, ReturnCode> Read(TValue& value)
    {
        std::underlying_type_t<TValue> underlying;
        auto rc = Read(underlying);
        CHECK_RETURN_CODE(rc);

        value = static_cast<TValue>(underlying);
        return ReturnCode::Success;
    }

    constexpr ReturnCode Read(Span<uint8_t> data)
    {
        auto length = data.GetLength();
        auto remaining = GetRemaining();

        if (remaining.GetLength() < length)
        {
            return ReturnCode::InvalidLength;
        }

        auto rc = remaining.Take(length).CopyTo(data);
        CHECK_RETURN_CODE(rc);

        _offset += length;

        return ReturnCode::Success;
    }

    constexpr ReturnCode ReadString(Span<char> data)
    {
        auto remaining = GetRemaining();
        auto terminator = remaining.IndexOf(0);

        if (terminator < 0 || static_cast<uint32_t>(terminator) >= data.GetLength())
        {
            return ReturnCode::InvalidLength;
        }

        auto length = static_cast<uint32_t>(terminator) + 1;

        for (uint32_t i = 0; i < length; i++)
        {
            data.Set(i, static_cast<char>(remaining.GetData()[i]));
        }

        _offset += length;

        return ReturnCode::Success;
    }


private:
    Span<T> _span;
    uint32_t _offset;
    Endianness _endianness;

    constexpr Span<T> GetRemaining()
    {
        return _span.Skip(_offset);
    }
};