#pragma once

#include <stdint.h>

template<typename T>
class Iterator
{
public:
    constexpr Iterator(T* data, uint32_t length = 0) : _data(data), _end(data + length)
    {
    }

    constexpr Iterator& operator++()
    {
        if (_data != _end)
        {
            _data++;
        }

        return *this;
    }

    constexpr bool operator==(const Iterator& rhs) const
    {
        return _data == rhs._data;
    }

    constexpr bool operator!=(const Iterator& rhs) const
    {
        return _data != rhs._data;
    }

    constexpr T& operator*() const
    {
        return *_data;
    }

private:
    T* _data;
    T* _end;
};

template<typename T>
class ConstIterator
{
public:
    constexpr ConstIterator(const T* data, uint32_t length = 0) : _data(data), _end(data + length)
    {
    }

    constexpr ConstIterator& operator++()
    {
        if (_data != _end)
        {
            _data++;
        }

        return *this;
    }

    constexpr bool operator==(const ConstIterator& rhs) const
    {
        return _data == rhs._data;
    }

    constexpr bool operator!=(const ConstIterator& rhs) const
    {
        return _data != rhs._data;
    }

    constexpr const T& operator*() const
    {
        return *_data;
    }

private:
    const T* _data;
    const T* _end;
};
