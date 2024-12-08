#pragma once

#include "Span.h"
#include <stdint.h>
#include <math.h>
#include <cmath>
#include <algorithm>


class SpanExtensions
{
public:
    template<typename T>
    struct AggregateResultWithIndex
    {
        T Value;
        uint32_t Index;
    };

    template<typename T>
    struct AggregateResultWithSum
    {
        T Value;
        float Sum;
    };

    template<typename T>
    struct AggregateResultWithSumAndIndex
    {
        T Value;
        float Sum;
        uint32_t Index;
    };

    template<typename T>
    static constexpr T Sum(Span<T> span)
    {
        return span.Aggregate([](T a, T b) constexpr { return a + b; });
    }
    
    template<typename T>
    static constexpr T Min(Span<T> span)
    {
        return span.Aggregate([](T a, T b) constexpr { return a < b ? a : b; });
    }

    template<typename T>
    static constexpr T Min(Span<T> span, uint32_t& index)
    {
        T seedValue = {};
        span.Get(0, seedValue);
        auto seed = AggregateResultWithIndex<T>{seedValue, 0};

        auto result = span.template Aggregate<AggregateResultWithIndex<T>>([](AggregateResultWithIndex<T> a, T b, uint32_t i) 
            constexpr { return b < a.Value ? AggregateResultWithIndex<T>{b, i} : a; }, seed);
        index = result.Index;
        return result.Value;
    }

    template<typename T>
    static constexpr T MinAndMean(Span<T> span, float& mean)
    {
        T seedValue = {};
        span.Get(0, seedValue);
        auto seed = AggregateResultWithSum<T>{seedValue, 0};

        auto result = span.template Aggregate<AggregateResultWithSum<T>>([](AggregateResultWithSum<T> a, T b) 
            constexpr { return b < a.Value ? AggregateResultWithSum<T>{b, a.Sum + b} : AggregateResultWithSum<T>{a.Value, a.Sum + b}; }, seed);
        mean = result.Sum / span.GetLength();
        return result.Value;
    }

    template<typename T>
    static constexpr T MinAndMean(Span<T> span, float& mean, uint32_t& index)
    {
        T seedValue = {};
        span.Get(0, seedValue);
        auto seed = AggregateResultWithSumAndIndex<T>{seedValue, 0, 0};

        auto result = span.template Aggregate<AggregateResultWithSumAndIndex<T>>([](AggregateResultWithSumAndIndex<T> a, T b, uint32_t i) 
            constexpr { return b < a.Value ? AggregateResultWithSumAndIndex<T>{b, a.Sum + b, i} : AggregateResultWithSumAndIndex<T>{a.Value, a.Sum + b, a.Index}; }, seed);
        mean = result.Sum / span.GetLength();
        index = result.Index;
        return result.Value;
    }

    template<typename T>
    static constexpr T Max(Span<T> span)
    {
        return span.Aggregate([](T a, T b) constexpr { return a > b ? a : b; });
    }

    template<typename T>
    static constexpr T Max(Span<T> span, uint32_t& index)
    {
        T seedValue = {};
        span.Get(0, seedValue);
        auto seed = AggregateResultWithIndex<T>{seedValue, 0};

        auto result = span.template Aggregate<AggregateResultWithIndex<T>>([](AggregateResultWithIndex<T> a, T b, uint32_t i) 
            constexpr { return b > a.Value ? AggregateResultWithIndex<T>{b, i} : a; }, seed);
        index = result.Index;
        return result.Value;
    }
    
    template<typename T>
    static constexpr T MaxAndMean(Span<T> span, float& mean)
    {
        T seedValue = {};
        span.Get(0, seedValue);
        auto seed = AggregateResultWithSum<T>{seedValue, 0};

        auto result = span.template Aggregate<AggregateResultWithSum<T>>([](AggregateResultWithSum<T> a, T b) 
            constexpr { return b > a.Value ? AggregateResultWithSum<T>{b, a.Sum + b} : AggregateResultWithSum<T>{a.Value, a.Sum + b}; }, seed);
        mean = result.Sum / span.GetLength();
        return result.Value;
    }

    template<typename T>
    static constexpr T MaxAndMean(Span<T> span, float& mean, uint32_t& index)
    {
        T seedValue = {};
        span.Get(0, seedValue);
        auto seed = AggregateResultWithSumAndIndex<T>{seedValue, 0, 0};

        auto result = span.template Aggregate<AggregateResultWithSumAndIndex<T>>([](AggregateResultWithSumAndIndex<T> a, T b, uint32_t i) 
            constexpr { return b > a.Value ? AggregateResultWithSumAndIndex<T>{b, a.Sum + b, i} : AggregateResultWithSumAndIndex<T>{a.Value, a.Sum + b, a.Index}; }, seed);
        mean = result.Sum / span.GetLength();
        index = result.Index;
        return result.Value;
    }

    template<typename T>
    static constexpr float Mean(Span<T> span)
    {
        return Sum(span) / span.GetLength();
    }

    template<typename T>
    static constexpr float StandardDeviation(Span<T> span, float mean)
    {
        auto sum = span.template Aggregate<float, float>([](float a, T b, float state) 
            constexpr { return a + (b - state) * (b - state); }, 0, mean);
        return sqrt(sum / span.GetLength());
    }
};