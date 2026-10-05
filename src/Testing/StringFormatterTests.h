#pragma once

#include "StringFormatter.h"
#include "CoreString.h"

namespace StringFormatterTests
{
     static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, World!");
        return formatted == String("Hello, World!");
    }(), "Format method failed");

    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %d!", 42);
        return formatted == String("Hello, 42!");
    }(), "Format method failed");

    // multiple arguments
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %d %d %d!", 1, 2, 3);
        return formatted == String("Hello, 1 2 3!");
    }(), "Format method failed");

    // floats
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %.2f!", 42.0f);
        return formatted == String("Hello, 42.00!");
    }(), "Format method failed");
    
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "%.1f", 0.99f);
        return formatted == String("1.0");
    }(), "Format method failed");

    // enum
    enum class Color { Red, Green, Blue };
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Color: %d", Color::Green);
        return formatted == String("Color: 1");
    }(), "Format method failed");

    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %x!", 42);
        return formatted == String("Hello, 2a!");
    }(), "Format method failed");

    // multiple arguments
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %.1f %.2f %.3f!", 1.0f, 2.0f, 3.0f);
        return formatted == String("Hello, 1.0 2.00 3.000!");
    }(), "Format method failed");

    // lots of arguments
    static_assert([]{
        Array<char, 100> buffer;
        auto debug = StringFormatter::Format(buffer, "%6d %c %s \t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.3f\t%.3f\t%.3f\r\n", 
            123456, 'A', "BC", 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
        return debug == String("123456 A BC \t1.00\t2.00\t3.00\t4.00\t5.00\t6.000\t7.000\t8.000\r\n");
    }(), "Format method failed");

    // multiple arguments
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %.1f %.2f %d!", 1.0f, 2.0f, 3u);
        return formatted == String("Hello, 1.0 2.00 3!");
    }(), "Format method failed");

    // not enough arguments
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %d %d %d!", 1, 2);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // empty buffer
    static_assert([]{
        Span<char> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello!");
        return formatted.IsEmpty();
    }(), "Format method failed");

    // empty format string
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "");
        return formatted.IsEmpty();
    }(), "Format method failed");

    // unsupported format specifier
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %q!", 42);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // escaped percent
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %%!");
        return formatted == String("Hello, %!");
    }(), "Format method failed");

    // huge precision
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %.100f!", 42.0f);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // not enough space
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %d!", 123456789);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // not enough space
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %.2f!", 123456789.0f);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // not enough space
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, %x!", 12345678901);
        return formatted.IsEmpty();
    }(), "Format method failed");

        // Negative integers
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "Negative: %d", -42);
        return formatted == String("Negative: -42");
    }(), "Format method failed");

    // Negative floats
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "Negative float: %.2f", -42.0f);
        return formatted == String("Negative float: -42.00");
    }(), "Format method failed");

    // Minimum field width
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "Padded: %5d", 42);
        return formatted == String("Padded:    42");
    }(), "Format method failed");

    // Zero padding
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "Zero padded: %05d", 42);
        return formatted == String("Zero padded: 00042");
    }(), "Format method failed");

    // // String arguments
    // static_assert([]{
    //     Array<char, 24> buffer;
    //     auto formatted = StringFormatter::Format(buffer, "String: %s", "test");
    //     return formatted == String("String: test");
    // }(), "Format method failed");

    // Character formatting
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "Char: %c", 'A');
        return formatted == String("Char: A");
    }(), "Format method failed");

    // // Octal formatting
    // static_assert([]{
    //     Array<char, 24> buffer;
    //     auto formatted = StringFormatter::Format(buffer, "Octal: %o", 42);
    //     return formatted == String("Octal: 52");
    // }(), "Format method failed");

    // Unsigned integers
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "Unsigned: %u", 42u);
        return formatted == String("Unsigned: 42");
    }(), "Format method failed");

    // Combining multiple format specifiers
    static_assert([]{
        Array<char, 48> buffer;
        auto formatted = StringFormatter::Format(buffer, "Int: %d, Float: %.2f, Hex: %x", 42, 3.14f, 255);
        return formatted == String("Int: 42, Float: 3.14, Hex: ff");
    }(), "Format method failed");

    // // Exponential notation for floats
    // static_assert([]{
    //     Array<char, 32> buffer;
    //     auto formatted = StringFormatter::Format(buffer, "Scientific: %e", 12345.6789);
    //     return formatted == String("Scientific: 1.234568e+04");
    // }(), "Format method failed");

    // Short integers
    static_assert([]{
        Array<char, 24> buffer;
        short int value = 42;
        auto formatted = StringFormatter::Format(buffer, "Short: %hd", value);
        return formatted == String("Short: 42");
    }(), "Format method failed");

    // Long integers
    static_assert([]{
        Array<char, 32> buffer;
        long int value = 1234567890;
        auto formatted = StringFormatter::Format(buffer, "Long: %ld", value);
        return formatted == String("Long: 1234567890");
    }(), "Format method failed");

    // Adjacent format specifiers
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "%d%d%d", 1, 2, 3);
        return formatted == String("123");
    }(), "Format method failed");

    // Invalid format string (unmatched %)
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Invalid % format");
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Null buffer
    static_assert([]{
        char* buffer = nullptr;
        auto formatted = StringFormatter::Format(Span<char>(buffer, 0), "Hello");
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Buffer size exactly equal to required size
    static_assert([]{
        Array<char, 13> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, World!");
        return formatted == String("Hello, World!");
    }(), "Format method failed");

    // Text one character too long
    static_assert([]{
        Array<char, 12> buffer;
        auto formatted = StringFormatter::Format(buffer, "Hello, World!");
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Numbers filling the buffer exactly
    static_assert([]{
        Array<char, 12> buffer;
        auto formatted = StringFormatter::Format(buffer, "%d.%d.%d.%d", 192, 168, 1, 10);
        return formatted == String("192.168.1.10");
    }(), "Format method failed");

    // Last number one character too long
    static_assert([]{
        Array<char, 11> buffer;
        auto formatted = StringFormatter::Format(buffer, "%d.%d.%d.%d", 192, 168, 1, 10);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Buffer full with text and arguments left
    static_assert([]{
        Array<char, 8> buffer;
        auto formatted = StringFormatter::Format(buffer, "%d.%d.%d.%d", 192, 168, 1, 10);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Padding that doesn't fit
    static_assert([]{
        Array<char, 4> buffer;
        auto formatted = StringFormatter::Format(buffer, "%5d", 42);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Float filling the buffer exactly
    static_assert([]{
        Array<char, 5> buffer;
        auto formatted = StringFormatter::Format(buffer, "%.2f", -1.5f);
        return formatted == String("-1.50");
    }(), "Format method failed");

    // Float one character too long
    static_assert([]{
        Array<char, 4> buffer;
        auto formatted = StringFormatter::Format(buffer, "%.2f", -1.5f);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // String argument that doesn't fit
    static_assert([]{
        Array<char, 8> buffer;
        auto formatted = StringFormatter::Format(buffer, "Name: %s", "test");
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Character argument that doesn't fit
    static_assert([]{
        Array<char, 6> buffer;
        auto formatted = StringFormatter::Format(buffer, "Char: %c", 'A');
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Escaped percent that doesn't fit
    static_assert([]{
        Array<char, 3> buffer;
        auto formatted = StringFormatter::Format(buffer, "100%%");
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Nothing written past the text
    static_assert([]{
        Array<char, 8> buffer;
        buffer.Fill('#');
        auto formatted = StringFormatter::Format(buffer, "%d", 42);
        return formatted == String("42") && String(buffer.Skip(2)) == String("######");
    }(), "Format method failed");

    // Format that isn't null-terminated
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, String("%d apples").Take(2), 3);
        return formatted == String("3");
    }(), "Format method failed");

    // Most negative integer
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "%d", -2147483647 - 1);
        return formatted == String("-2147483648");
    }(), "Format method failed");

    // Zero
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "%d %x", 0, 0);
        return formatted == String("0 0");
    }(), "Format method failed");

    // Widest hex, and the short modifier
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "%x %hx", -1, -1);
        return formatted == String("ffffffff ffff");
    }(), "Format method failed");

    // Float too large for its whole part
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "%.1f", 1e10f);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Non-ASCII characters
    static_assert([]{
        Array<char, 32> buffer;
        auto formatted = StringFormatter::Format(buffer, "こんにちは: %d", 42);
        return formatted == String("こんにちは: 42");
    }(), "Format method failed");

    // Escaped characters
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Tab\\ttest");
        return formatted == String("Tab\\ttest");
    }(), "Format method failed");

    // Empty string argument
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Empty: '%s'", "");
        return formatted == String("Empty: ''");
    }(), "Format method failed");

    // String argument
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "String: %s", "test");
        return formatted == String("String: test");
    }(), "Format method failed");

    // // Width and precision specified dynamically
    // static_assert([]{
    //     Array<char, 24> buffer;
    //     int width = 5;
    //     auto formatted = StringFormatter::Format(buffer, "Dynamic: %*d", width, 42);
    //     return formatted == String("Dynamic:    42");
    // }(), "Format method failed");

    // // Pointer formatting
    // static_assert([]{
    //     Array<char, 32> buffer;
    //     int value = 42;
    //     auto formatted = StringFormatter::Format(buffer, "Pointer: %p", &value);
    //     return !formatted.IsEmpty(); // Platform-dependent output
    // }(), "Format method failed");

    // // Null pointer formatting
    // static_assert([]{
    //     Array<char, 32> buffer;
    //     int* ptr = nullptr;
    //     auto formatted = StringFormatter::Format(buffer, "Null ptr: %p", ptr);
    //     return formatted == String("Null ptr: (nil)");
    // }(), "Format method failed");

    // Exponential notation with small buffer
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Scientific: %e", 123456789.0);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Unmatched percent signs
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Unmatched % sign");
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Percent signs at end of format string
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Ends with %");
        return formatted.IsEmpty();
    }(), "Format method failed");

    // Positive sign for positive numbers
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "Positive: %+d", 42);
        return formatted == String("Positive: +42");
    }(), "Format method failed");

    // // Special floating-point values (NaN, infinity)
    // static_assert([]{
    //     Array<char, 32> buffer;
    //     float nan_value = NAN;
    //     auto formatted = StringFormatter::Format(buffer, "NaN: %f", nan_value);
    //     return formatted == String("NaN: nan");
    // }(), "Format method failed");

    // static_assert([]{
    //     Array<char, 32> buffer;
    //     float inf_value = INFINITY;
    //     auto formatted = StringFormatter::Format(buffer, "Inf: %f", inf_value);
    //     return formatted == String("Inf: inf");
    // }(), "Format method failed");

    // Zero padding goes after the sign, and spaces before it
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "%05d %+05d %5d", -42, 42, -42);
        return formatted == String("-0042 +0042   -42");
    }(), "Format method failed");

    // Unsigned values past the signed range
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "%u %d", 4000000000u, 4000000000u);
        return formatted == String("4000000000 4000000000");
    }(), "Format method failed");

    // Negative integer as unsigned
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "%u", -1);
        return formatted == String("4294967295");
    }(), "Format method failed");

    // 64-bit integers
    static_assert([]{
        Array<char, 64> buffer;
        auto formatted = StringFormatter::Format(buffer, "%lld %llu %llx",
            -9223372036854775807LL - 1, 18446744073709551615ULL, 0x123456789abcdefLL);
        return formatted == String("-9223372036854775808 18446744073709551615 123456789abcdef");
    }(), "Format method failed");

    // Enum with a 64-bit underlying type
    enum class Wide : uint64_t { Value = 5000000000 };
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "%u", Wide::Value);
        return formatted == String("5000000000");
    }(), "Format method failed");

    // size_t
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "%zu bytes", sizeof(uint32_t));
        return formatted == String("4 bytes");
    }(), "Format method failed");

    // bool
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "%d %d", true, false);
        return formatted == String("1 0");
    }(), "Format method failed");

    // No decimal point without a fraction
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "%.0f %.0f", 42.0f, 1.6f);
        return formatted == String("42 2");
    }(), "Format method failed");

    // Float width, zero padding and sign
    static_assert([]{
        Array<char, 24> buffer;
        auto formatted = StringFormatter::Format(buffer, "%8.3f|%08.3f|%+.1f", 3.14159f, -3.14159f, 2.5f);
        return formatted == String("   3.142|-003.142|+2.5");
    }(), "Format method failed");

    // Padded float that doesn't fit
    static_assert([]{
        Array<char, 7> buffer;
        auto formatted = StringFormatter::Format(buffer, "%8.3f", 3.14159f);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // String and StringLiteral arguments
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "%s, %s!", String("Hello"), StringLiteral("World"));
        return formatted == String("Hello, World!");
    }(), "Format method failed");

    // String argument that isn't null-terminated
    static_assert([]{
        Array<char, 16> buffer;
        auto formatted = StringFormatter::Format(buffer, "%s", String("Hello, World!").Take(5));
        return formatted == String("Hello");
    }(), "Format method failed");

    // Mutable C string
    static_assert([]{
        Array<char, 16> buffer;
        char name[] = "test";
        auto formatted = StringFormatter::Format(buffer, "String: %s", name);
        return formatted == String("String: test");
    }(), "Format method failed");

    // Null C string
    static_assert([]{
        Array<char, 16> buffer;
        const char* missing = nullptr;
        auto formatted = StringFormatter::Format(buffer, "%s", missing);
        return formatted.IsEmpty();
    }(), "Format method failed");

    // string literal
    static_assert([]{
        Array<char, 64> buffer;
        StringLiteral literal("World");
        auto formatted = StringFormatter::Format(buffer, "Hello, %s, here's a float: %.3f!", literal.GetData(), 1.234f);
        return formatted == String("Hello, World, here's a float: 1.234!");
    }(), "Format method failed");
}
