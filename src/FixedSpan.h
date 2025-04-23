#pragma once

#include <stdint.h>

template<typename T, uint32_t Length>
struct FixedSpan
{
public:
    template<typename U, uint32_t L>
    friend class FixedSpan;

    constexpr FixedSpan() : _data(nullptr)
    {
    }
    
    template<uint32_t ArrayLength>
    static constexpr FixedSpan<T, Length> FromArray(T (&data)[ArrayLength])
    {
        static_assert(Length <= ArrayLength);
        return FixedSpan<T, Length>(data);
    }

    template<uint32_t Offset, uint32_t ArrayLength>
    static constexpr FixedSpan<T, Length> FromArray(T (&data)[ArrayLength])
    {
        static_assert(Offset + Length <= ArrayLength);
        return FixedSpan<T, Length>(data + Offset);
    }

    static constexpr FixedSpan<T, Length> FromPointerUnsafe(T* data)
    {
        return FixedSpan<T, Length>(data);
    }

    constexpr const Span<const T> AsSpan() const { return Span<const T>(_data, Length); }
    constexpr Span<T> AsSpan() { return Span<T>(_data, Length); }

    constexpr T* GetData() { return _data; }
    constexpr const T* GetData() const { return _data; }
    constexpr uint32_t GetLength() const { return Length; }

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
    
    template<uint32_t Index>
    constexpr void Set(T value)
    {
        static_assert(Index < Length);
        _data[Index] = value;
    }

    template<uint32_t Index>
    constexpr const T& Get() const
    {
        static_assert(Index < Length);
        return _data[Index];
    }
    
    template<uint32_t Index>
    constexpr T& Get()
    {
        static_assert(Index < Length);
        return _data[Index];
    }

    constexpr ReturnCode Set(uint32_t index, T value)
    {
        if(index < Length)
        {
            _data[index] = value;
            return ReturnCode::Success;
        }

        return ReturnCode::OutOfRange;
    }

    constexpr ReturnCode Get(uint32_t index, std::remove_const_t<T>& value) const
    {
        if(index < Length)
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

    constexpr FixedSpan& operator=(const FixedSpan& rhs)
    {
        _data = rhs._data;
        return *this;
    }
    
    template<uint32_t OtherLength>
    constexpr const FixedSpan<const T, OtherLength < Length ? OtherLength : Length> Take() const
    {
        return FixedSpan<const T, OtherLength < Length ? OtherLength : Length>(_data);
    }
    
    template<uint32_t OtherLength>
    constexpr const FixedSpan<T, OtherLength < Length ? OtherLength : Length> Take()
    {
        return FixedSpan<T, OtherLength < Length ? OtherLength : Length>(_data);
    }

    constexpr ReturnCode CopyTo(FixedSpan<T, Length> destination) const
    {
        return AsSpan().CopyTo(destination.AsSpan());
    }
    
private:
    T* _data;

    constexpr FixedSpan(T* data) : _data(data)
    {
    }
};
