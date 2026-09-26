#pragma once

#include <stdint.h>
#include <type_traits>
#include "Span.h"
#include "ReturnCode.h"

// Rows are counted, not pointed at, so no pointer is formed past the last row.
template<typename T>
class RowIterator
{
public:
    constexpr RowIterator(T* data, uint32_t width, uint32_t stride, uint32_t row)
        : _data(data), _width(width), _stride(stride), _row(row)
    {
    }

    constexpr RowIterator& operator++()
    {
        _row++;
        return *this;
    }

    constexpr bool operator==(const RowIterator& rhs) const
    {
        return _row == rhs._row;
    }

    constexpr bool operator!=(const RowIterator& rhs) const
    {
        return _row != rhs._row;
    }

    constexpr Span<T> operator*() const
    {
        return Span<T>(_data + _row * _stride, _width);
    }

private:
    T* _data;
    uint32_t _width;
    uint32_t _stride;
    uint32_t _row;
};

// Stride is in elements, not bytes.
template<typename T>
struct Span2D
{
public:
    constexpr Span2D() : Span2D(nullptr, 0, 0, 0)
    {
    }

    constexpr Span2D(T* data, uint32_t width, uint32_t height) : Span2D(data, width, height, width)
    {
    }

    constexpr Span2D(T* data, uint32_t width, uint32_t height, uint32_t stride)
        : _data(data), _width(width), _height(height), _stride(stride)
    {
    }

    template<typename TOther>
    constexpr Span2D(const Span2D<TOther>& other)
        : Span2D(static_cast<T*>(other.GetData()), other.GetWidth(), other.GetHeight(), other.GetStride())
    {
    }

    static constexpr ReturnCode FromSpan(Span<T> span, uint32_t width, uint32_t height,
        uint32_t stride, Span2D& result)
    {
        if(stride < width)
        {
            return ReturnCode::InvalidArgument;
        }

        if(GetExtent(width, height, stride) > span.GetLength())
        {
            return ReturnCode::InvalidLength;
        }

        result = Span2D(span.GetData(), width, height, stride);

        return ReturnCode::Success;
    }

    static constexpr ReturnCode FromSpan(Span<T> span, uint32_t width, uint32_t height, Span2D& result)
    {
        return FromSpan(span, width, height, width, result);
    }

    constexpr T* GetData() { return _data; }
    constexpr const T* GetData() const { return _data; }
    constexpr auto GetWidth() const { return _width; }
    constexpr auto GetHeight() const { return _height; }
    constexpr auto GetStride() const { return _stride; }

    constexpr bool IsEmpty() const
    {
        return _width == 0 || _height == 0;
    }

    constexpr auto begin() { return RowIterator<T>(_data, _width, _stride, 0); }
    constexpr auto end() { return RowIterator<T>(_data, _width, _stride, GetRowCount()); }
    constexpr auto begin() const { return RowIterator<const T>(_data, _width, _stride, 0); }
    constexpr auto end() const { return RowIterator<const T>(_data, _width, _stride, GetRowCount()); }

    constexpr Span<T> GetRow(uint32_t y)
    {
        return y < _height ? Span<T>(_data + y * _stride, _width) : Span<T>();
    }

    constexpr Span<const T> GetRow(uint32_t y) const
    {
        return y < _height ? Span<const T>(_data + y * _stride, _width) : Span<const T>();
    }

    constexpr ReturnCode Set(uint32_t x, uint32_t y, T value)
    {
        if(x < _width && y < _height)
        {
            _data[y * _stride + x] = value;
            return ReturnCode::Success;
        }

        return ReturnCode::OutOfRange;
    }

    constexpr ReturnCode Get(uint32_t x, uint32_t y, std::remove_const_t<T>& value) const
    {
        if(x < _width && y < _height)
        {
            value = _data[y * _stride + x];
            return ReturnCode::Success;
        }

        return ReturnCode::OutOfRange;
    }

    constexpr bool TryGet(uint32_t x, uint32_t y, std::remove_const_t<T>& value) const
    {
        return Get(x, y, value) == ReturnCode::Success;
    }

    constexpr bool TrySet(uint32_t x, uint32_t y, T value)
    {
        return Set(x, y, value) == ReturnCode::Success;
    }

    constexpr bool TryCompare(uint32_t x, uint32_t y, T value) const
    {
        return x < _width && y < _height && _data[y * _stride + x] == value;
    }

    constexpr auto Slice(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
    {
        x = x < _width ? x : _width;
        y = y < _height ? y : _height;
        width = width < _width - x ? width : _width - x;
        height = height < _height - y ? height : _height - y;

        if(width == 0 || height == 0)
        {
            return Span2D(_data, 0, 0, _stride);
        }

        return Span2D(_data + y * _stride + x, width, height, _stride);
    }

    constexpr ReturnCode CopyTo(Span2D<std::remove_const_t<T>> other) const
    {
        if(other.GetWidth() < _width || other.GetHeight() < _height)
        {
            return ReturnCode::InvalidLength;
        }

        for(uint32_t y = 0; y < GetRowCount(); y++)
        {
            auto rc = GetRow(y).CopyTo(other.GetRow(y));
            CHECK_RETURN_CODE(rc);
        }

        return ReturnCode::Success;
    }

    constexpr bool SequenceEquals(Span2D<T> other) const
    {
        if(_width != other.GetWidth() || _height != other.GetHeight())
        {
            return false;
        }

        for(uint32_t y = 0; y < GetRowCount(); y++)
        {
            if(!GetRow(y).SequenceEquals(other.GetRow(y)))
            {
                return false;
            }
        }

        return true;
    }

    constexpr void Fill(T value)
    {
        for(auto row : *this)
        {
            row.Fill(value);
        }
    }

private:
    T* _data;
    uint32_t _width;
    uint32_t _height;
    uint32_t _stride;

    constexpr uint32_t GetRowCount() const
    {
        return IsEmpty() ? 0 : _height;
    }

    static constexpr uint64_t GetExtent(uint32_t width, uint32_t height, uint32_t stride)
    {
        if(width == 0 || height == 0)
        {
            return 0;
        }

        return static_cast<uint64_t>(height - 1) * stride + width;
    }
};
