#pragma once

#include <stdint.h>
#include <cstring>
#include <type_traits>
#include "Iterator.h"
#include "ReturnCode.h"


template<typename T>
struct Span
{
public:
    constexpr Span() : Span(nullptr, 0)
    {
    }
    
    constexpr Span(T* data, uint32_t length) : _data(data), _length(length)
    {
    }

    template<typename TArray, uint32_t Length>
    constexpr Span(TArray(&array)[Length]) : Span(array, Length)
    {
    }

    template<typename TOther>
    constexpr Span(const Span<TOther>& other) : Span(static_cast<T*>(other.GetData()), other.GetLength())
    {
    }

    constexpr T* GetData() { return _data; }
    constexpr const T* GetData() const { return _data; }
    constexpr auto GetLength() const { return _length; }

    constexpr bool IsEmpty() const
    {
        return _length == 0;
    }

    constexpr auto begin() { return Iterator(_data, _length); }
    constexpr auto end() { return Iterator(_data + _length); }
    constexpr auto begin() const { return ConstIterator<T>(_data, _length); }
    constexpr auto end() const { return ConstIterator<T>(_data + _length); }

    constexpr ReturnCode Set(uint32_t index, T value)
    {
        if(index < _length)
        {
            _data[index] = value;
            return ReturnCode::Success;
        }

        return ReturnCode::OutOfRange;
    }

    constexpr ReturnCode Get(uint32_t index, std::remove_const_t<T>& value) const
    {
        if(index < _length)
        {
            value = _data[index];
            return ReturnCode::Success;
        }

        return ReturnCode::OutOfRange;
    }

    constexpr bool TryGet(uint32_t index, std::remove_const_t<T>& value) const
    {
        return Get(index, value) == ReturnCode::Success;
    }

    constexpr bool TrySet(uint32_t index, T value)
    {
        return Set(index, value) == ReturnCode::Success;
    }

    constexpr bool TryCompare(uint32_t index, T value) const
    {
        return index < _length && _data[index] == value;
    }

    constexpr auto& operator=(const Span& other)
    {
        _data = other._data;
        _length = other._length;

        return *this;
    }

    constexpr auto Take(uint32_t length) const
    {
        return Span(_data, length < _length ? length : _length);
    }
    
    constexpr auto Take(uint32_t length)
    {
        return Span(_data, length < _length ? length : _length);
    }

    constexpr auto Skip(uint32_t length) const
    {
        if(length < _length)
        {
            return Span(_data + length, _length - length);
        }

        return Span(_data + _length, 0);
    }
    
    constexpr auto Skip(uint32_t length)
    {
        if(length < _length)
        {
            return Span(_data + length, _length - length);
        }

        return Span(_data + _length, 0);
    }

    constexpr ReturnCode CopyTo(Span<std::remove_const_t<T>> other) const
    {
        return CopyTo(other, NoOp);
    }

    constexpr ReturnCode CopyTo(Span<std::remove_const_t<T>> other, uint32_t offset) const
    {
        return CopyTo(other.Skip(offset));
    }

    template<typename TOther>
    constexpr ReturnCode CopyTo(Span<TOther> other, TOther(*transform)(std::remove_const_t<T>)) const
    {
        if (transform == nullptr)
        {
            return ReturnCode::InvalidArgument;
        }
        
        auto length = GetLength();

        if(other.GetLength() < length)
        {
            return ReturnCode::InvalidLength;
        }

        for(uint32_t i = 0; i < length; i++)
        {
            other.Set(i, transform(_data[i]));
        }

        return ReturnCode::Success;
    }

    constexpr bool SequenceEquals(Span<T> other) const
    {
        auto length = GetLength();

        if(length != other.GetLength())
        {
            return false;
        }

        for(uint32_t i = 0; i < length; i++)
        {
            if(_data[i] != other._data[i])
            {
                return false;
            }
        }

        return true;
    }
    
    constexpr void Fill(T value)
    {
        for(uint32_t i = 0; i < _length; i++)
        {
            _data[i] = value;
        }
    }

    constexpr auto Aggregate(T(*func)(T, T)) const
    {
        if (func == nullptr)
        {
            return T();
        }

        auto length = GetLength();

        if(length == 0)
        {
            return T();
        }

        auto result = _data[0];
        for(uint32_t i = 1; i < length; i++)
        {
            result = func(result, _data[i]);
        }

        return result;
    }
    
    constexpr auto Aggregate(T(*func)(T, T, uint32_t)) const
    {
        if (func == nullptr)
        {
            return T();
        }

        auto length = GetLength();

        if(length == 0)
        {
            return T();
        }

        auto result = _data[0];
        for(uint32_t i = 1; i < length; i++)
        {
            result = func(result, _data[i], i);
        }

        return result;
    }

    template<typename TAggregate>
    constexpr auto Aggregate(TAggregate(*func)(TAggregate, T), TAggregate seed) const
    {
        if (func == nullptr)
        {
            return TAggregate();
        }

        auto length = GetLength();

        if(length == 0)
        {
            return TAggregate();
        }

        auto result = func(seed, _data[0]);
        for(uint32_t i = 1; i < length; i++)
        {
            result = func(result, _data[i]);
        }

        return result;
    }

    template<typename TAggregate>
    constexpr auto Aggregate(TAggregate(*func)(TAggregate, T, uint32_t), TAggregate seed) const
    {
        if (func == nullptr)
        {
            return TAggregate();
        }

        auto length = GetLength();

        if(length == 0)
        {
            return TAggregate();
        }

        auto result = func(seed, _data[0], 0);
        for(uint32_t i = 1; i < length; i++)
        {
            result = func(result, _data[i], i);
        }

        return result;
    }
    
    template<typename TAggregate, typename TState>
    constexpr auto Aggregate(TAggregate(*func)(TAggregate, T, TState), TAggregate seed, TState state) const
    {
        if (func == nullptr)
        {
            return TAggregate();
        }
        
        auto length = GetLength();

        if(length == 0)
        {
            return TAggregate();
        }

        auto result = func(seed, _data[0], state);
        for(uint32_t i = 1; i < length; i++)
        {
            result = func(result, _data[i], state);
        }

        return result;
    }

    template<typename TAggregate, typename TState>
    constexpr auto Aggregate(TAggregate(*func)(TAggregate, T, uint32_t, TState), TAggregate seed, TState state) const
    {
        if (func == nullptr)
        {
            return TAggregate();
        }
        
        auto length = GetLength();

        if(length == 0)
        {
            return TAggregate();
        }

        auto result = func(seed, _data[0], 0, state);
        for(uint32_t i = 1; i < length; i++)
        {
            result = func(result, _data[i], i, state);
        }

        return result;
    }

    constexpr int IndexOf(T value) const
    {
        for (uint32_t i = 0; i < GetLength(); i++)
        {
            if (_data[i] == value)
            {
                return i;
            }
        }

        return -1;
    }

    constexpr int LastIndexOf(T value) const
    {
        for (uint32_t i = GetLength(); i > 0; i--)
        {
            if (_data[i - 1] == value)
            {
                return i - 1;
            }
        }

        return -1;
    }

    auto AsBytes()
    {
        return Span<uint8_t>(reinterpret_cast<uint8_t*>(_data), _length * sizeof(T));
    }

    auto AsConstBytes() const
    {
        return Span<const uint8_t>(reinterpret_cast<const uint8_t*>(_data), _length * sizeof(T));
    }

private:
    T* _data;
    uint32_t _length;

    template<typename TValue>
    static constexpr TValue NoOp(TValue value)
    {
        return value;
    }
};

// template deduction guides

template<typename TArray, uint32_t Length>
Span(TArray(&)[Length]) -> Span<TArray>;
