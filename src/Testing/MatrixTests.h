#pragma once

#include "Matrix.h"

namespace MatrixTests
{
    constexpr Matrix<int, 2, 3> defaultMatrix;

    static_assert(defaultMatrix.GetRowCount() == 2, "GetRowCount failed");
    static_assert(defaultMatrix.GetColumnCount() == 3, "GetColumnCount failed");

    static_assert([]{
        int value = 1;
        return defaultMatrix.Get(0, 0, value) == ReturnCode::Success && value == 0;
    }(), "Default initialization failed");

    static_assert([]{
        constexpr Matrix<int, 2, 2> matrix = { { { 1, 2 }, { 3, 4 } } };
        int value = 0;
        return matrix.Get(1, 0, value) == ReturnCode::Success && value == 3;
    }(), "Initializer-list construction failed");

    static_assert([]{
        constexpr Matrix<int, 2, 2> matrix = { { { 1, 2, 9 }, { 3, 4 }, { 5, 6 } } };
        int value = 0;
        return matrix.Get(1, 1, value) == ReturnCode::Success && value == 4;
    }(), "Initializer-list bounds handling failed");

    static_assert([]{
        Matrix<int, 2, 2> matrix;
        if(matrix.Set(1, 1, 42) != ReturnCode::Success)
        {
            return false;
        }

        int value = 0;
        return matrix.Get(1, 1, value) == ReturnCode::Success && value == 42;
    }(), "Set/Get failed");

    static_assert([]{
        Matrix<int, 2, 2> matrix;
        int value = 0;
        return matrix.Set(2, 0, 1) == ReturnCode::OutOfRange && matrix.Get(0, 2, value) == ReturnCode::OutOfRange;
    }(), "Out-of-range handling failed");
}