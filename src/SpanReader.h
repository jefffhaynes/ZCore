#pragma once

#include "Span.h"

template<typename T>
class SpanReader
{
public:
    constexpr SpanReader(Span<T> span) : _span(span), _offset(0)
    {
    }

    constexpr Span<T> Read(uint32_t length)
    {
        auto span = _span.Skip(_offset).Take(length);
        _offset += span.GetLength();
        return span;
    }

private:
    Span<T> _span;
    uint32_t _offset;
};