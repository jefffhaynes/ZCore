#pragma once

#include <stdint.h>
#include <cmath>

template<typename T = float>
class Range
{
public:
    constexpr Range()
    {
    }

    constexpr Range(T min, T max) : _min(min), _max(max)
    {
    }

    constexpr T Min() const
    {
        return _min;
    }

    constexpr T Max() const
    {
        return _max;
    }

    constexpr T Center() const
    {
        return (_min + _max) / 2;
    }

    constexpr T Size() const
    {
        return _max - _min;
    }
    
    constexpr Range Reverse() const
    {
        return Range(_max, _min);
    }

    constexpr T Clamp(T value) const
    {
        if(value > _max)
        {
            return _max;
        }

        if(value < _min)
        {
            return _min;
        }

        return value;
    }

    constexpr bool Contains(T value) const
    {
        return !(value < _min || value > _max);
    }

    template<typename TRange>
    constexpr TRange ScaleTo(const Range<TRange>& range, T value) const
    {
        auto normalized = (value - Min()) * range.Size();
        return static_cast<TRange>(normalized / Size() + range.Min());
    }

    template<typename TRange>
    constexpr TRange ScaleToFloor(const Range<TRange>& range, T value) const
    {
        auto normalized = (value + std::numeric_limits<T>::epsilon() - Min()) * range.Size();
        auto floor = std::floor(normalized / Size() + range.Min());
        return static_cast<TRange>(floor);
    }

    template<typename TRange>
    constexpr TRange ScaleToCeiling(const Range<TRange>& range, T value) const
    {
        auto normalized = (value - std::numeric_limits<T>::epsilon() - Min()) * range.Size();
        auto ceil = std::ceil(normalized / Size() + range.Min());
        return static_cast<TRange>(ceil);
    }

    static constexpr Range<T> UnitRange = { 0, 1 };

private:
    T _min;
    T _max;
};
