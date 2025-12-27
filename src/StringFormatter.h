#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <Array.h>
#include "CoreString.h"

#include <cstddef>
#include <type_traits>


class StringFormatter
{
public:
    template<typename... Args>
    static constexpr String Format(Span<char> buffer, String format, Args... args) 
    {
        if (buffer.GetLength() == 0) 
        {
            return String();
        }

        auto* data = buffer.GetData();
        auto length = buffer.GetLength();
        auto* fmt = format.GetData();
        int result = format_helper(data, length, fmt, args...);
        if (result < 0) 
        {
            return String(); 
        }
        
        auto written = (int)(buffer.GetLength() - length);

        return buffer.Take(written);
    }

private:
    // Function to convert integer to string with specified base and padding
    static constexpr int integer_to_base_string(
        char* buffer, 
        size_t size, 
        unsigned int value, 
        int base, 
        int width = 0, 
        char pad_char = ' '
    ) {
        char temp[33]; // Enough for 32-bit integer in binary
        int i = 0;

        if (base < 2 || base > 16) return -1; // Unsupported base

        // Handle zero value separately
        if (value == 0) {
            temp[i++] = '0';
        } else {
            // Convert integer to reversed string with specified base
            while (value && i < (int)sizeof(temp) - 1) {
                unsigned int digit = value % base;
                temp[i++] = digit < 10 ? '0' + digit : 'a' + digit - 10;
                value /= base;
            }
        }

        // Calculate padding
        int total_len = (i > width) ? i : width;
        if ((size_t)total_len >= size) {
            return -1; // Not enough space
        }

        int pad_len = total_len - i;

        // Add padding
        int pos = 0;
        for (int j = 0; j < pad_len; ++j) {
            buffer[pos++] = pad_char;
        }

        // Reverse the string into the buffer
        for (int j = 0; j < i; ++j) {
            buffer[pos++] = temp[i - j - 1];
        }
        buffer[pos] = '\0';
        return pos;
    }

    // Function to convert integer to decimal string with padding
    // Added: `bool always_sign` to control whether we put a '+' in front of non-negative values
    static constexpr int integer_to_string(
        char* buffer, 
        size_t size, 
        int value, 
        int width = 0, 
        char pad_char = ' ', 
        bool always_sign = false
    ) {
        char temp[12]; // Buffer to hold the integer string
        int i = 0;
        bool negative = false;

        // Check sign
        if (value < 0) {
            negative = true;
            value = -value;
        }

        // Special case 0
        if (value == 0) {
            temp[i++] = '0';
        } else {
            // Convert integer to reversed string
            while (value && i < (int)sizeof(temp) - 1) {
                temp[i++] = '0' + (value % 10);
                value /= 10;
            }
        }

        // Add sign character (negative or plus, if requested)
        if (negative) {
            temp[i++] = '-';
        } else if (always_sign) {
            temp[i++] = '+';
        }

        // Calculate padding
        int total_len = (i > width) ? i : width;
        if ((size_t)total_len >= size) {
            return -1; // Not enough space
        }

        int pad_len = total_len - i;

        // Add padding
        int pos = 0;
        for (int j = 0; j < pad_len; ++j) {
            buffer[pos++] = pad_char;
        }

        // Reverse the string into the buffer
        for (int j = 0; j < i; ++j) {
            buffer[pos++] = temp[i - j - 1];
        }
        buffer[pos] = '\0';
        return pos;
    }

    // Function to convert float to string with specified precision
    static constexpr int float_to_string(char* buffer, size_t size, float value, int precision) {
        char temp[32]; // Adjust size as needed
        int pos = 0;
        bool negative = false;

        if (precision < 0) precision = 0;

        if (value < 0) {
            negative = true;
            value = -value;
        }

        int int_part = static_cast<int>(value);
        float fractional_part = value - int_part;

        // Multiply fractional part to get desired precision
        unsigned long frac_multiplier = 1;
        for (int i = 0; i < precision; ++i) {
            frac_multiplier *= 10;
        }

        // Round the fractional part properly
        unsigned long frac = static_cast<unsigned long>(fractional_part * frac_multiplier + 0.5f);
        if (frac == frac_multiplier) {   // e.g. 0.99 with precision 1 ⇒ 10
            ++int_part;                  // bump the integral part
            frac = 0;                    // fractional digits are now all zeros
        }
        
        // Convert integer part
        int len_int = integer_to_string(temp, sizeof(temp), int_part);
        if (len_int < 0) return -1;

        int total_len = len_int + (negative ? 1 : 0) + 1 + precision; // incl. '-', '.', decimal digits
        if (size < static_cast<size_t>(total_len + 1)) return -1;

        if (negative) {
            buffer[pos++] = '-';
        }

        // Copy integer part
        for (int i = 0; i < len_int; ++i) {
            buffer[pos++] = temp[i];
        }

        buffer[pos++] = '.';

        // Convert fractional part
        if (precision > 0) {
            char frac_str[32]; // Should be enough
            for (int i = precision - 1; i >= 0; --i) {
                frac_str[i] = '0' + (frac % 10);
                frac /= 10;
            }

            // Copy fractional part
            for (int i = 0; i < precision; ++i) {
                buffer[pos++] = frac_str[i];
            }
        }

        buffer[pos] = '\0';
        return pos;
    }

    // Base case for format_helper when no arguments are left
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format) {
        while (*format && size > 1) {
            if (*format == '%') {
                if (*(format + 1) == '%') {
                    *buffer++ = '%';
                    --size;
                    format += 2;
                } else {
                    // If we see a lone '%', that’s an error in this simplistic implementation
                    return -1;
                }
            } else {
                *buffer++ = *format++;
                --size;
            }
        }
        if (size > 0) {
            *buffer = '\0';
        } else {
            *(buffer - 1) = '\0';
        }
        return 0;
    }

    // Overload for integer arguments
    template<typename... Args>
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format, int value, Args... args) {
        while (*format && size > 1) {
            if (*format == '%') {
                ++format;
                if (*format == '%') {
                    *buffer++ = '%';
                    --size;
                    ++format;
                } else {
                    // Handle flags
                    char pad_char = ' ';
                    bool always_sign = false;

                    // Check for '+' flag
                    if (*format == '+') {
                        always_sign = true;
                        ++format;
                    }
                    // Check for '0' flag (for padding)
                    if (*format == '0') {
                        pad_char = '0';
                        ++format;
                    }

                    // Parse width
                    int width = 0;
                    while (*format >= '0' && *format <= '9') {
                        width = width * 10 + (*format - '0');
                        ++format;
                    }

                    // Handle length modifier
                    bool is_short = false;
                    if (*format == 'h') {
                        is_short = true;
                        ++format;
                    }

                    // Now handle specifier
                    if (*format == 'd' || *format == 'i' || *format == 'u') {
                        ++format;
                        int len = integer_to_string(buffer, size, value, width, pad_char, always_sign);
                        if (len < 0) return -1;
                        buffer += len;
                        size -= len;
                        return format_helper(buffer, size, format, args...);

                    } else if (*format == 'x') {
                        ++format;
                        unsigned int val = is_short 
                            ? static_cast<unsigned short>(value) 
                            : static_cast<unsigned int>(value);
                        int len = integer_to_base_string(buffer, size, val, 16, width, pad_char);
                        if (len < 0) return -1;
                        buffer += len;
                        size -= len;
                        return format_helper(buffer, size, format, args...);
                    } else {
                        return -1; // Unsupported format specifier
                    }
                }
            } else {
                *buffer++ = *format++;
                --size;
            }
        }
        return format_helper(buffer, size, format);
    }

    // Overload for unsigned integer arguments
    template<typename... Args>
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format, unsigned int value, Args... args) {
        // Just reuse the int overload
        return format_helper(buffer, size, format, static_cast<int>(value), args...);
    }

    // Overload for int8_t
    template<typename... Args>
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format, int8_t value, Args... args) {
        // Just reuse the int overload
        return format_helper(buffer, size, format, static_cast<int>(value), args...);
    }

    // Overload for uint8_t
    template<typename... Args>
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format, uint8_t value, Args... args) {
        // Just reuse the unsigned int overload
        return format_helper(buffer, size, format, static_cast<unsigned int>(value), args...);
    }

    // Overload for int16_t
    template<typename... Args>
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format, int16_t value, Args... args) {
        // Just reuse the int overload
        return format_helper(buffer, size, format, static_cast<int>(value), args...);
    }

    // Overload for uint16_t
    template<typename... Args>
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format, uint16_t value, Args... args) {
        // Just reuse the unsigned int overload
        return format_helper(buffer, size, format, static_cast<unsigned int>(value), args...);
    }

    // Overload for float arguments with precision handling
    template<typename... Args>
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format, float value, Args... args) {
        while (*format && size > 1) {
            if (*format == '%') {
                ++format;
                if (*format == '%') {
                    *buffer++ = '%';
                    --size;
                    ++format;
                } else {
                    // In this implementation, we ignore width for floats
                    while (*format >= '0' && *format <= '9') {
                        ++format;
                    }
                    // Handle precision
                    int precision = 6; // Default
                    if (*format == '.') {
                        ++format;
                        precision = 0;
                        while (*format >= '0' && *format <= '9') {
                            precision = precision * 10 + (*format - '0');
                            ++format;
                        }
                    }
                    if (*format == 'f') {
                        ++format;
                        int len = float_to_string(buffer, size, value, precision);
                        if (len < 0) return -1;
                        buffer += len;
                        size -= len;
                        return format_helper(buffer, size, format, args...);
                    } else {
                        return -1; // Unsupported specifier
                    }
                }
            } else {
                *buffer++ = *format++;
                --size;
            }
        }
        return format_helper(buffer, size, format);
    }

    // Overload for double arguments
    template<typename... Args>
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format, double value, Args... args) {
        // Just reuse the float overload
        return format_helper(buffer, size, format, static_cast<float>(value), args...);
    }

    // Overload for char arguments
    template<typename... Args>
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format, char value, Args... args) {
        while (*format && size > 1) {
            if (*format == '%') {
                ++format;
                if (*format == '%') {
                    *buffer++ = '%';
                    --size;
                    ++format;
                } else {
                    if (*format == 'c') {
                        *buffer++ = value;
                        --size;
                        ++format;
                        return format_helper(buffer, size, format, args...);
                    } else {
                        return -1; // Unsupported format specifier
                    }
                }
            } else {
                *buffer++ = *format++;
                --size;
            }
        }
        return format_helper(buffer, size, format);
    }

    // Overload for string arguments
    template<typename... Args>
    static constexpr int format_helper(char*& buffer, size_t& size, const char*& format, const char* value, Args... args) {
        while (*format && size > 1) {
            if (*format == '%') {
                ++format;
                if (*format == '%') {
                    *buffer++ = '%';
                    --size;
                    ++format;
                } else {
                    if (*format == 's') {
                        while (*value && size > 1) {
                            *buffer++ = *value++;
                            --size;
                        }
                        ++format;
                        return format_helper(buffer, size, format, args...);
                    } else {
                        return -1; // Unsupported format specifier
                    }
                }
            } else {
                *buffer++ = *format++;
                --size;
            }
        }
        return format_helper(buffer, size, format);
    }

    /* Accept enum arguments by forwarding them as ints */
    template<typename Enum, typename... Args>
    static constexpr typename std::enable_if<std::is_enum<Enum>::value, int>::type
    format_helper(char*& buffer, size_t& size, const char*& format,
                Enum value, Args... args)
    {
        return format_helper(buffer, size, format,
                            static_cast<typename std::underlying_type<Enum>::type>(value),
                            args...);
    }

    /* Catch-all for truly unsupported types (now *excluding* enums) */
    template<typename T, typename... Args>
    static constexpr typename std::enable_if<!std::is_enum<T>::value, int>::type
    format_helper(char*& /*buffer*/, size_t& /*size*/, const char*& /*format*/,
                T /*unused*/, Args... /*rest*/)
    {
        return -1;   // Unsupported type
    }
};
