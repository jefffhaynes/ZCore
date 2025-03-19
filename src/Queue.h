#pragma once

#include "Span.h"
#include <type_traits>

// #pragma GCC push_options
// #pragma GCC optimize ("O0")

template <typename T>
class Queue
{
public:
    constexpr Queue(Span<T> buffer) : 
        _buffer(buffer), _read(0), _write(0), _count(0)
    {
    }

    constexpr ReturnCode Enqueue(Span<std::add_const_t<T>> items)
    {
        if (items.GetLength() == 0)
        {
            return ReturnCode::Success;
        }

        if (items.GetLength() + GetCount() > _buffer.GetLength())
        {
            return ReturnCode::OutOfMemory;
        }

        auto bufferTail = _buffer.Skip(_write);

        if (items.GetLength() > bufferTail.GetLength())
        {
            auto itemsHead = items.Take(bufferTail.GetLength());
            auto rc = itemsHead.CopyTo(bufferTail);
            CHECK_RETURN_CODE(rc);

            auto itemsTail = items.Skip(bufferTail.GetLength());
            rc = itemsTail.CopyTo(_buffer);
            CHECK_RETURN_CODE(rc);

            _write = itemsTail.GetLength();
        }
        else
        {
            auto rc = items.CopyTo(bufferTail);
            CHECK_RETURN_CODE(rc);

            _write += items.GetLength();
        }

        _count += items.GetLength();

        return ReturnCode::Success;
    }

   constexpr ReturnCode Dequeue(Span<T> items)
    {
        if (items.GetLength() > GetCount())
        {
            return ReturnCode::InvalidLength;
        }
        
        auto bufferTail = _buffer.Skip(_read);

        if (items.GetLength() > bufferTail.GetLength())
        {
            auto rc = bufferTail.CopyTo(items);
            CHECK_RETURN_CODE(rc);

            auto itemsTail = items.Skip(bufferTail.GetLength());
            auto bufferHead = _buffer.Take(itemsTail.GetLength());
            rc = bufferHead.CopyTo(itemsTail);
            CHECK_RETURN_CODE(rc);

            _read = itemsTail.GetLength();
        }
        else
        {
            auto rc = bufferTail.Take(items.GetLength()).CopyTo(items);
            CHECK_RETURN_CODE(rc);

            _read += items.GetLength();
        }

        _count -= items.GetLength();

        return ReturnCode::Success;
    }

    constexpr ReturnCode Dequeue(Span<T> items, uint32_t& count)
    {
        count = items.GetLength() > GetCount() ? GetCount() : items.GetLength();
        return Dequeue(items.Take(count));
    }

    constexpr bool TryDequeue(T& item)
    {
        if (GetCount() == 0)
        {
            return false;
        }

        Span<T> span(&item, 1);
        auto rc = Dequeue(span);
        return rc == ReturnCode::Success;
    }

    constexpr ReturnCode Enqueue(T item)
    {
        return Enqueue(Span<T>(&item, 1));
    }

    constexpr uint32_t GetCount() const
    {
        return _count;
    }

    constexpr bool IsEmpty() const
    {
        return GetCount() == 0;
    }

    constexpr bool IsFull() const
    {
        return GetCount() == _buffer.GetLength();
    }
    

private:
    Span<T> _buffer;
    uint32_t _read;
    uint32_t _write;
    uint32_t _count;
};

// #pragma GCC pop_options