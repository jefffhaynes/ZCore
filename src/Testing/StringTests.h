#pragma once

#include "CoreString.h"
#include "Array.h"

namespace StringTests
{
    // Equality
    static_assert(String("Hello") == String("Hello"), "Equality operator failed");

    // Inequality
    static_assert(String("Hello") != String("World"), "Inequality operator failed");

    // StartsWith
    static_assert(String("Hello").StartsWith(String("He")), "StartsWith method failed");
    static_assert(!String("Hello").StartsWith(String("World")), "StartsWith method failed");

    // IndexOf
    static_assert(String("Hello").IndexOf('l') == 2, "IndexOf method failed");
    static_assert(String("Hello").IndexOf('!') == -1, "IndexOf method failed");

    // LastIndexOf
    static_assert(String("Hello").LastIndexOf('l') == 3, "LastIndexOf method failed");
    static_assert(String("Hello").LastIndexOf('!') == -1, "LastIndexOf method failed");

    // Substring
    static_assert(String("Hello").Substring(2) == String("llo"), "Substring method failed");

    // FromNullTerminated
    static_assert(String::FromNullTerminated("Hello") == String("Hello"), "FromNullTerminated method failed");
    static_assert([]{
        Array<char, 6> data = {'H', 'e', 'l', 'l', 'o', '\0'};
        return String::FromNullTerminated(data.GetData()) == String("Hello");
    }(), "FromNullTerminated method failed");
}

namespace StringLiteralTests
{
    // constructors
    static_assert(StringLiteral("Hello") == String("Hello"), "Equality operator failed");

    // string with termination
    static_assert([]{
        Array<char, 6> data = {'H', 'e', 'l', 'l', 'o', '\0'};
        StringLiteral literal(data);
        return literal == StringLiteral("Hello");
    }(), "Dangerous string failed");

    // dangerous string with no termination
    static_assert([]{
        Array<char, 5> data = {'H', 'e', 'l', 'l', 'o'};
        StringLiteral literal(data);
        return literal == StringLiteral("Hell");
    }(), "Dangerous string failed");
}