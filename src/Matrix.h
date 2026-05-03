#pragma once

#include <initializer_list>

#include <Span.h>

template<typename T, uint32_t Rows, uint32_t Columns>
class Matrix
{
public:
    constexpr Matrix() : _data{}
    {
    }

    constexpr Matrix(std::initializer_list<std::initializer_list<T>> init) 
    {
        std::size_t row = 0;
        for (auto& subList : init) 
        {
            std::size_t col = 0;
            for (auto& elem : subList) 
            {
                if (row < Rows && col < Columns) 
                {
                    _data[row][col] = elem;
                }
                ++col;
            }
            ++row;
        }
    }

    constexpr uint32_t GetRowCount() const
    {
        return Rows;
    }

    constexpr uint32_t GetColumnCount() const
    {
        return Columns;
    }

    constexpr ReturnCode Set(uint32_t row, uint32_t column, T value)
    {
        if(row < Rows && column < Columns)
        {
            _data[row][column] = value;
            return ReturnCode::Success;
        }

        return ReturnCode::OutOfRange;
    }

    constexpr ReturnCode Get(uint32_t row, uint32_t column, T& value) const
    {
        if(row < Rows && column < Columns)
        {
            value = _data[row][column];
            return ReturnCode::Success;
        }

        return ReturnCode::OutOfRange;
    }


private:
    T _data[Rows][Columns];
};

