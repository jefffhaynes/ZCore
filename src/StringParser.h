#pragma once

#include <concepts>
#include <limits>
#include <stdint.h>
#include <type_traits>
#include "CoreString.h"
#include "ReturnCode.h"

class StringParser
{
public:
    // Decimal digits and nothing else, but for a '-' ahead of a signed type's. OutOfRange if the
    // number doesn't fit T.
    template<std::integral T> requires (!std::same_as<T, bool>)
    static constexpr ReturnCode Parse(String text, T& value)
    {
        using Magnitude = std::make_unsigned_t<T>;

        auto isNegative = std::is_signed_v<T> && text.StartsWith("-");
        auto digits = isNegative ? text.Substring(1) : text;

        if(digits.IsEmpty())
        {
            return ReturnCode::InvalidData;
        }

        // A signed type's lowest value is one further from zero than its highest.
        auto highest = static_cast<Magnitude>(std::numeric_limits<T>::max());
        auto limit = static_cast<Magnitude>(highest + (isNegative ? 1 : 0));
        Magnitude magnitude = 0;

        for(auto character : digits)
        {
            if(character < '0' || character > '9')
            {
                return ReturnCode::InvalidData;
            }

            auto digit = static_cast<Magnitude>(character - '0');

            if(magnitude > (limit - digit) / 10)
            {
                return ReturnCode::OutOfRange;
            }

            magnitude = static_cast<Magnitude>(magnitude * 10 + digit);
        }

        value = static_cast<T>(isNegative ? Magnitude(0) - magnitude : magnitude);

        return ReturnCode::Success;
    }

    template<std::integral T> requires (!std::same_as<T, bool>)
    static constexpr bool TryParse(String text, T& value)
    {
        return Parse(text, value) == ReturnCode::Success;
    }
};
