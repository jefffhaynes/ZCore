#pragma once

#include "MemoryMarshal.h"

template<typename T>
class SpanReader
{
public:
    constexpr SpanReader(Span<T> span) : _span(span), _offset(0)
    {
    }

    constexpr Span<T> ReadSpan(uint32_t length)
    {
        auto span = GetRemaining().Take(length);
        _offset += span.GetLength();
        return span;
    }

    template <typename U = T, Arithmetic TValue>
    constexpr std::enable_if_t<std::is_same_v<U, uint8_t>, ReturnCode> Read(TValue& value)
    {
        if (_offset + sizeof(TValue) > _span.GetLength())
        {
            return ReturnCode::InvalidLength;
        }

        // read using bitwise operations instead of MemoryMarshal so it can be constexpr
        value = 0;
        for (uint32_t i = 0; i < sizeof(TValue); i++)
        {
            uint8_t byte;
            auto rc = _span.Get(_offset++, byte);
            CHECK_RETURN_CODE(rc);
            
            value |= static_cast<TValue>(byte) << (i * 8);
        }

        return ReturnCode::Success;
    }

    template <typename U = T, Enum TValue>
    constexpr std::enable_if_t<std::is_same_v<U, uint8_t>, ReturnCode> Read(TValue& value)
    {
        std::underlying_type_t<TValue> underlying;
        auto rc = Read(underlying);
        CHECK_RETURN_CODE(rc);

        value = static_cast<TValue>(underlying);
        return ReturnCode::Success;
    }

    template <typename U = T, ComplexSafe TValue>
    std::enable_if_t<std::is_same_v<U, uint8_t>, ReturnCode> Read(TValue& value)
    {
        auto data = MemoryMarshal::AsBytes(value);
        return Read(data);
    }

    constexpr ReturnCode Read(Span<uint8_t> data)
    {
        auto length = data.GetLength();
        auto rc = GetRemaining().Take(length).CopyTo(data);
        CHECK_RETURN_CODE(rc);

        _offset += length;

        return ReturnCode::Success;
    }

    ReturnCode ReadString(Span<char> data)
    {
        for (uint32_t i = 0; i < data.GetLength(); i++)
        {
            uint8_t c;
            auto rc = Read(c);
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


private:
    Span<T> _span;
    uint32_t _offset;

    constexpr Span<T> GetRemaining()
    {
        return _span.Skip(_offset);
    }
};