#pragma once

#include "Array.h"

template<typename T>
class SpanWriter
{
public:
    constexpr SpanWriter(Span<T> span) : _span(span), _offset(0)
    {
    }

    template<uint32_t N>
    constexpr SpanWriter(Array<T, N>& array) : SpanWriter(array.AsSpan())
    {
    }

    constexpr ReturnCode Write(Span<std::add_const_t<T>> span)
    {
        auto rc = span.CopyTo(_span.Skip(_offset));
        CHECK_RETURN_CODE(rc);

        _offset += span.GetLength();

        return ReturnCode::Success;
    }

    constexpr Span<T> GetWrittenSpan()
    {
        return _span.Take(_offset);
    }

    constexpr uint32_t GetWritten()
    {
        return _offset;
    }

private:
    Span<T> _span;
    uint32_t _offset;
};