#pragma once

#include "Span.h"
#include "CoreString.h"

#include "pfr/pfr.hpp"


template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename T>
concept Enum = std::is_enum_v<T>;

template <typename T>
constexpr bool all_fields_are_safe()
{
    bool result = true;

    pfr::for_each_field(T{}, [&](const auto& field)
    {
        using FieldType = std::decay_t<decltype(field)>;
        result = result && (Arithmetic<FieldType> 
            || Enum<FieldType> 
            || all_fields_are_safe<FieldType>());
    });

    return result;
}


template<typename T>
concept ComplexSafe = all_fields_are_safe<T>() && !Arithmetic<T> && !Enum<T>;

template<typename T>
concept Safe = Arithmetic<T> || Enum<T> || ComplexSafe<T>;

template<typename T>
concept Unsafe = !Safe<T>;

class MemoryMarshal
{
public:
    template<typename T>
    static constexpr Span<T> AsSpan(T& value)
    {
        return Span<T>(&value, 1);
    }

    template<typename T>
    static constexpr Span<const T> AsConstSpan(const T& value)
    {
        return Span<const T>(&value, 1);
    }

    template<typename T, uint32_t Length>
    static constexpr Span<T> AsSpan(T (&value)[Length])
    {
        return Span<T>(value);
    }

    template<typename T, uint32_t Length>
    static constexpr Span<const T> AsConstSpan(const T (&value)[Length])
    {
        return Span<const T>(value);
    }

    template<typename T>
    static constexpr Span<T> AsSpan(T* data, uint32_t length)
    {
        return Span<T>(data, length);
    }

    template<typename T>
    static constexpr Span<const T> AsConstSpan(const T* data, uint32_t length)
    {
        return Span<const T>(data, length);
    }

    template<Safe T>
    static Span<uint8_t> AsBytes(T& value)
    {
        return AsBytesUnsafe(value);
    }

    template<Safe T>
    static Span<const uint8_t> AsConstBytes(T& value)
    {
        return AsConstBytesUnsafe(value);
    }
    
    template<typename T>
    static Span<uint8_t> AsBytesUnsafe(T& value)
    {
        auto* p = reinterpret_cast<uint8_t*>(&value);
        return Span<uint8_t>(p, sizeof(value));
    }
    
    template<typename T>
    static Span<const uint8_t> AsConstBytesUnsafe(T& value)
    {
        auto* p = reinterpret_cast<const uint8_t*>(&value);
        return Span<const uint8_t>(p, sizeof(value));
    }
};