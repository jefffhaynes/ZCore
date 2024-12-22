#pragma once

#include "Span.h"
#include "FixedSpan.h"
#include <functional>

template<typename T, uint32_t Length>
struct Array final
{
public:    
    template<typename U>
    using EnableIfPointer = typename std::enable_if_t<std::is_pointer_v<U>, int>;

    constexpr Array() : _data{}
    {
    }

    template <typename... TArgs> 
    constexpr Array(TArgs... values) : _data{values...}
    {
    }

    constexpr const Span<const T> AsSpan() const { return Span<const T>(_data); }
    constexpr Span<T> AsSpan() { return Span<T>(_data); }
    constexpr const FixedSpan<const T, Length> AsFixedSpan() const { return FixedSpan<const T, Length>::FromArray(_data); }
    constexpr FixedSpan<T, Length> AsFixedSpan() { return FixedSpan<T, Length>::FromArray(_data); }
    constexpr operator Span<T>() { return AsSpan(); }
    constexpr operator FixedSpan<T, Length>() { return AsFixedSpan(); }

    constexpr Iterator<T> begin() { return AsSpan().begin(); }
    constexpr Iterator<T> end() { return AsSpan().end(); }
    constexpr ConstIterator<const T> begin() const { return AsSpan().begin(); }
    constexpr ConstIterator<const T> end() const { return AsSpan().end(); }

    constexpr bool TryCompare(uint32_t index, T value) const
    {
        return AsSpan().TryCompare(index, value);
    }
    
    constexpr const T& operator[](uint32_t index) const 
    { 
        assert(index < GetLength());
        return _data[index]; 
    }

    constexpr T& operator[](uint32_t index)
    { 
        assert(index < GetLength());
        return _data[index]; 
    }

    constexpr T* GetData() { return _data; }
    constexpr const T* GetData() const { return _data; }
    constexpr uint32_t GetLength() const { return Length; }

    template<uint32_t Index>
    constexpr void Set(T value)
    {
        AsFixedSpan().template Set<Index>(value);
    }

    template<uint32_t Index>
    constexpr const T& Get() const
    {
        return AsFixedSpan().template Get<Index>();
    }
    
    template<uint32_t Index>
    constexpr T& Get()
    {
        return AsFixedSpan().template Get<Index>();
    }

    constexpr ReturnCode Set(uint32_t index, T value)
    {
        return AsSpan().Set(index, value);
    }

    constexpr ReturnCode Get(uint32_t index, std::remove_const_t<T>& value) const
    {
        return AsSpan().Get(index, value);
    }

    constexpr bool TryGet(uint32_t index, std::remove_const_t<T>& value) const
    {
        return AsSpan().TryGet(index, value);
    }

    constexpr const Span<const T> Take(uint32_t length) const
    {
        return AsSpan().Take(length);
    }

    constexpr Span<T> Take(uint32_t length)
    {
        return AsSpan().Take(length);
    }

    constexpr const Span<const T> Skip(uint32_t length) const
    {
        return AsSpan().Skip(length);
    }
        
    constexpr Span<T> Skip(uint32_t length)
    {
        return AsSpan().Skip(length);
    }

    template<typename U = T, EnableIfPointer<U> = 0>
    constexpr void Remove(T item)
    {
        for(auto& entry : *this)
        {
            if(entry == item)
            {
                entry = nullptr;
            }
        }
    }

    template<typename U = T, EnableIfPointer<U> = 0>
    constexpr ReturnCode Add(T item)
    {
        for(auto& entry : *this)
        {
            if(entry == nullptr)
            {
                entry = item;
                return ReturnCode::Success;
            }
        }

        return ReturnCode::OutOfMemory;
    }

    template<typename U = T, EnableIfPointer<U> = 0>
    constexpr uint32_t GetCount() const
    {
        uint32_t count = 0;
        for(auto& item : *this)
        {
            if(item != nullptr)
            {
                count++;
            }
        }

        return count;
    }

    
    template<typename U = T, EnableIfPointer<U> = 0>
    constexpr bool Any() const
    {
        return GetCount() > 0;
    }

    constexpr bool Contains(T item) const
    {
        for(auto& entry : *this)
        {
            if(entry == item)
            {
                return true;
            }
        }

        return false;
    }

    constexpr bool SequenceEquals(const Span<const T> other) const
    {
        return AsSpan().SequenceEquals(other);
    }

    Span<uint8_t> AsBytes()
    {
        return AsSpan().AsBytes();
    }

    Span<const uint8_t> AsConstBytes() const
    {
        return AsSpan().AsConstBytes();
    }

private:
    T _data[Length];
};


template<typename T, uint32_t Length>
Array(T (&)[Length]) -> Array<T, Length>;