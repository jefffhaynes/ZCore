#pragma once

#include "Span2D.h"

namespace Span2DTests
{
    // 3 x 3 values in rows of stride 4; the last column (-1) is padding.
    constexpr int padded[12] =
    {
         1,  2,  3, -1,
         4,  5,  6, -1,
         7,  8,  9, -1,
    };

    constexpr Span2D<const int> view(padded, 3, 3, 4);

    static_assert(view.GetData() == padded, "GetData method failed");
    static_assert(view.GetWidth() == 3 && view.GetHeight() == 3 && view.GetStride() == 4, "Dimensions failed");
    static_assert(!view.IsEmpty(), "IsEmpty method failed");

    constexpr Span2D<const int> emptyView;
    static_assert(emptyView.IsEmpty(), "IsEmpty method failed for empty view");
    static_assert(Span2D<const int>(padded, 0, 3).IsEmpty(), "IsEmpty method failed for zero width");

    // packed rows: stride defaults to width
    static_assert(Span2D<const int>(padded, 4, 3).GetStride() == 4, "Packed constructor failed");

    static_assert(view.TryCompare(0, 0, 1) && view.TryCompare(2, 0, 3), "TryCompare method failed");
    static_assert(view.TryCompare(0, 1, 4) && view.TryCompare(2, 2, 9), "Stride addressing failed");
    static_assert(!view.TryCompare(3, 0, -1), "TryCompare read the padding");
    static_assert(!view.TryCompare(0, 3, 0), "TryCompare read past the last row");

    static_assert([]{
        int value = 0;
        return view.Get(1, 2, value) == ReturnCode::Success && value == 8;
    }(), "Get method failed");

    static_assert([]{
        int value = 0;
        return view.Get(3, 0, value) == ReturnCode::OutOfRange
            && view.Get(0, 3, value) == ReturnCode::OutOfRange
            && !view.TryGet(3, 3, value);
    }(), "Get method failed out of range");

    static_assert([]{
        int values[12] = {};
        Span2D<int> span(values, 3, 3, 4);

        return span.Set(2, 1, 7) == ReturnCode::Success && values[6] == 7
            && span.Set(3, 1, 7) == ReturnCode::OutOfRange && values[7] == 0
            && !span.TrySet(0, 3, 7);
    }(), "Set method failed");

    static_assert(view.GetRow(1).GetLength() == 3 && view.GetRow(1).TryCompare(0, 4)
        && view.GetRow(1).TryCompare(2, 6), "GetRow method failed");
    static_assert(view.GetRow(3).IsEmpty(), "GetRow method failed past the last row");

    // iteration visits each row once and never the padding
    static_assert([]{
        int sum = 0;
        uint32_t rows = 0;

        for(auto row : view)
        {
            for(auto value : row)
            {
                sum += value;
            }

            rows++;
        }

        return sum == 45 && rows == 3;
    }(), "Row iteration failed");

    static_assert([]{
        uint32_t rows = 0;

        for(auto row : Span2D<const int>(padded, 0, 3, 4))
        {
            (void)row;
            rows++;
        }

        return rows == 0;
    }(), "Row iteration visited empty rows");

    constexpr auto centre = view.Slice(1, 1, 2, 2);
    static_assert(centre.GetWidth() == 2 && centre.GetHeight() == 2 && centre.GetStride() == 4, "Slice dimensions failed");
    static_assert(centre.TryCompare(0, 0, 5) && centre.TryCompare(1, 1, 9), "Slice contents failed");

    constexpr auto clipped = view.Slice(2, 1, 5, 5);
    static_assert(clipped.GetWidth() == 1 && clipped.GetHeight() == 2 && clipped.TryCompare(0, 1, 9), "Slice clipping failed");

    static_assert(view.Slice(3, 0, 1, 1).IsEmpty() && view.Slice(0, 3, 1, 1).IsEmpty(), "Slice past the edge failed");

    static_assert([]{
        int values[12] = {};
        Span2D<int> span(values, 3, 3, 4);
        span.Slice(1, 1, 2, 2).Fill(5);

        // the slice is filled; the rest of the view and the padding are not
        return values[5] == 5 && values[6] == 5 && values[9] == 5 && values[10] == 5
            && values[4] == 0 && values[7] == 0 && values[11] == 0 && values[1] == 0;
    }(), "Fill method failed");

    static_assert([]{
        int destination[4] = {};
        Span2D<int> target(destination, 2, 2);

        return view.Slice(1, 1, 2, 2).CopyTo(target) == ReturnCode::Success
            && destination[0] == 5 && destination[1] == 6 && destination[2] == 8 && destination[3] == 9;
    }(), "CopyTo method failed");

    static_assert([]{
        int destination[4] = {};
        return view.CopyTo(Span2D<int>(destination, 2, 2)) == ReturnCode::InvalidLength;
    }(), "CopyTo method failed for a small destination");

    static_assert([]{
        constexpr int packed[4] = { 5, 6, 8, 9 };
        return view.Slice(1, 1, 2, 2).SequenceEquals(Span2D<const int>(packed, 2, 2))
            && !view.Slice(0, 0, 2, 2).SequenceEquals(Span2D<const int>(packed, 2, 2))
            && !view.Slice(1, 1, 2, 1).SequenceEquals(Span2D<const int>(packed, 2, 2));
    }(), "SequenceEquals method failed");

    static_assert([]{
        Span2D<const int> result;
        return Span2D<const int>::FromSpan(Span<const int>(padded), 3, 3, 4, result) == ReturnCode::Success
            && result.TryCompare(2, 2, 9);
    }(), "FromSpan method failed");

    static_assert([]{
        // the last row needs only its width, not a whole stride
        Span2D<const int> result;
        return Span2D<const int>::FromSpan(Span<const int>(padded, 11), 3, 3, 4, result) == ReturnCode::Success
            && Span2D<const int>::FromSpan(Span<const int>(padded, 10), 3, 3, 4, result) == ReturnCode::InvalidLength
            && Span2D<const int>::FromSpan(Span<const int>(padded), 3, 3, 2, result) == ReturnCode::InvalidArgument
            && Span2D<const int>::FromSpan(Span<const int>(padded), 4, 3, result) == ReturnCode::Success;
    }(), "FromSpan validation failed");

    static_assert([]{
        int values[4] = { 1, 2, 3, 4 };
        Span2D<int> span(values, 2, 2);
        Span2D<const int> readOnly = span;

        return readOnly.TryCompare(1, 1, 4);
    }(), "Const conversion failed");
}
