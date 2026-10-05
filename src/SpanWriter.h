#pragma once

#include "Array.h"
#include "MemoryMarshal.h"
#include "BinaryPrimitives.h"

template<typename T>
class SpanWriter
{
public:
    constexpr SpanWriter(Span<T> span, Endianness endianness = Endianness::LittleEndian)
        : _span(span), _offset(0), _endianness(endianness)
    {
    }

    template<uint32_t N>
    constexpr SpanWriter(Array<T, N>& array, Endianness endianness = Endianness::LittleEndian)
        : SpanWriter(array.AsSpan(), endianness)
    {
    }

    constexpr ReturnCode Write(Span<std::add_const_t<T>> span)
    {
        auto rc = span.CopyTo(_span.Skip(_offset));
        CHECK_RETURN_CODE(rc);

        _offset += span.GetLength();

        return ReturnCode::Success;
    }

    template <typename U = T, Arithmetic TValue>
    constexpr std::enable_if_t<std::is_same_v<U, uint8_t>, ReturnCode> Write(TValue value)
    {
        auto rc = BinaryPrimitives::Write(_span.Skip(_offset), value, _endianness);
        CHECK_RETURN_CODE(rc);

        _offset += sizeof(TValue);

        return ReturnCode::Success;
    }

    template <typename U = T, Enum TValue>
    constexpr std::enable_if_t<std::is_same_v<U, uint8_t>, ReturnCode> Write(TValue value)
    {
        auto underlying = static_cast<std::underlying_type_t<TValue>>(value);
        return Write(underlying);
    }

    template <typename U = T>
    std::enable_if_t<std::is_same_v<U, uint8_t>, ReturnCode> Write(String s)
    {
        auto span = s.AsConstBytes();
        return Write(span);
    }

    constexpr ReturnCode Write(T value)
    {
        if (_span.TrySet(_offset, value))
        {
            _offset++;
            return ReturnCode::Success;
        }

        return ReturnCode::InvalidLength;
    }

    template<uint32_t N>
    constexpr ReturnCode Write(T(&array)[N])
    {
        auto span = Span<T>(array);
        return Write(span);
    }

    constexpr Span<T> GetWrittenSpan()
    {
        return _span.Take(_offset);
    }

    constexpr uint32_t GetWritten()
    {
        return _offset;
    }

    constexpr bool IsFull()
    {
        return _offset == _span.GetLength();
    }

    constexpr void Reset()
    {
        _offset = 0;
    }

private:
    Span<T> _span;
    uint32_t _offset;
    Endianness _endianness;
};
