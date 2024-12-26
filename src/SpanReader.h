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
        auto span = _span.Skip(_offset).Take(length);
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
            _span.Get(_offset++, byte);
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


private:
    Span<T> _span;
    uint32_t _offset;
};