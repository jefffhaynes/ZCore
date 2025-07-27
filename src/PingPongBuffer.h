#pragma once

#include <Array.h>

template<typename T, uint32_t Length>
class PingPongBuffer
{
public:
    constexpr PingPongBuffer() = default;

    constexpr FixedSpan<T, Length> GetActive()
    {
        return _state ? _buffer1.AsFixedSpan() : _buffer2.AsFixedSpan();
    }

    constexpr FixedSpan<T, Length> GetInactive()
    {
        return _state ? _buffer2.AsFixedSpan() : _buffer1.AsFixedSpan();
    }

    constexpr void Swap()
    {
        _state = !_state;
    }

private:
    Array<T, Length> _buffer1;
    Array<T, Length> _buffer2;
    bool _state = false;
};