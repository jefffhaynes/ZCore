#pragma once

#include <zephyr/kernel.h>
#include "Span.h"
#include "CoreString.h"
#include "SafeTuple.h"

#include "pfr/pfr.hpp"

template <typename T>
constexpr bool all_fields_are_safe()
{
    bool result = true;

    pfr::for_each_field(T{}, [&](const auto& field)
    {
        using FieldType = std::decay_t<decltype(field)>;
        result = result && (std::is_arithmetic_v<FieldType> 
            || std::is_enum_v<FieldType> 
            || all_fields_are_safe<FieldType>());
    });

    return result;
}

template<typename T>
concept Safe = std::is_arithmetic_v<T> || std::is_enum_v<T> || all_fields_are_safe<T>();

template<typename T>
concept Unsafe = !Safe<T>;

class MemoryMarshal
{
public:
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