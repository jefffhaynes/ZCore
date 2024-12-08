#pragma once

#include <zephyr/kernel.h>
#include "Span.h"
#include "CoreString.h"
#include "SafeTuple.h"


template <class T, template <class...> class Template>
struct is_specialization : std::false_type {};

template <template <class...> class Template, class... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};


class MemoryMarshal
{
public:
    template<typename T>
    using EnableIfSafe = typename std::enable_if_t<std::is_arithmetic_v<T> || std::is_enum_v<T> || is_specialization<T, SafeTuple>{}>; 


    template<typename T, typename = EnableIfSafe<T>>
    static constexpr Span<uint8_t> AsBytes(T& value)
    {
        return Span<uint8_t>(reinterpret_cast<uint8_t*>(&value), sizeof(value));
    }

    template<typename T>
    static constexpr Span<uint8_t> AsBytesUnsafe(T& value)
    {
        return Span<uint8_t>(reinterpret_cast<uint8_t*>(&value), sizeof(value));
    }

    template<typename T, typename = EnableIfSafe<T>>
    static constexpr Span<const uint8_t> AsConstBytes(T& value)
    {
        return Span<const uint8_t>(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
    }
    
    template<typename T>
    static constexpr Span<const uint8_t> AsConstBytesUnsafe(T& value)
    {
        return Span<const uint8_t>(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
    }

    template<typename T, typename = EnableIfSafe<T>>
    static constexpr Span<const uint8_t> AsConstBytes(Span<T> value)
    {
        return Cast<T, const uint8_t>(value);
    }


    template<typename T>
    using TBytes = typename std::conditional<std::is_const_v<T>, const uint8_t, uint8_t>::type; 

    template<typename T>
    static constexpr Span<TBytes<T>> AsBytes(Span<T> span)
    {
        auto length = span.GetLength() * sizeof(T);
        return Span<TBytes<T>>(reinterpret_cast<TBytes<T>*>(span.GetData()), length);
    }

private:
    // not safe for general use due to alignment issues    
    template<typename TFrom, typename TTo>
    static constexpr Span<TTo> Cast(Span<TFrom> span)
    {
        // TODO deal with alignment
        auto* to = reinterpret_cast<TTo*>(span.GetData());
        auto length = span.GetLength() * sizeof(TFrom) / sizeof(TTo);
        return { to, length };
    }
};