#pragma once

#include <stdint.h>
#include <type_traits>

#include "CoreString.h"
#include "SpanWriter.h"

class StringFormatter
{
public:
    // Returns the text, or an empty String if it doesn't fit or can't be formatted.
    // The text isn't null-terminated, so it can fill the buffer.
    template<typename... Args>
    static constexpr String Format(Span<char> buffer, String format, Args... args)
    {
        if (buffer.IsEmpty())
        {
            return String();
        }

        SpanWriter<char> writer(buffer);
        auto rc = WriteArguments(writer, format, args...);

        return rc == ReturnCode::Success ? String(writer.GetWrittenSpan()) : String();
    }

private:
    static constexpr uint32_t DefaultPrecision = 6;

    // The most fraction digits that fit 32 bits.
    static constexpr uint32_t MaxPrecision = 9;

    template<typename T, typename... Types>
    static constexpr bool IsAnyOf = (std::is_same_v<T, Types> || ...);

    static constexpr ReturnCode WriteArguments(SpanWriter<char>& writer, Span<const char> format)
    {
        auto rc = WriteText(writer, format);
        CHECK_RETURN_CODE(rc);

        // What's left is a specifier with no argument.
        return format.IsEmpty() ? ReturnCode::Success : ReturnCode::InvalidArgument;
    }

    template<typename T, typename... Args>
    static constexpr ReturnCode WriteArguments(SpanWriter<char>& writer, Span<const char> format,
        T value, Args... args)
    {
        auto rc = WriteText(writer, format);
        CHECK_RETURN_CODE(rc);

        // Arguments past the last specifier are ignored.
        if (!TrySkip(format, '%'))
        {
            return ReturnCode::Success;
        }

        rc = WriteArgument(writer, format, value);
        CHECK_RETURN_CODE(rc);

        return WriteArguments(writer, format, args...);
    }

    // Copies text up to the next specifier, leaving the format at its '%'.
    static constexpr ReturnCode WriteText(SpanWriter<char>& writer, Span<const char>& format)
    {
        while (true)
        {
            auto percent = format.IndexOf('%');
            auto text = format.Take(percent < 0 ? format.GetLength() : static_cast<uint32_t>(percent));

            auto rc = writer.Write(text);
            CHECK_RETURN_CODE(rc);

            format = format.Skip(text.GetLength());

            // "%%" is a literal '%'.
            if (!format.TryCompare(1, '%'))
            {
                return ReturnCode::Success;
            }

            rc = writer.Write('%');
            CHECK_RETURN_CODE(rc);

            format = format.Skip(2);
        }
    }

    // Writes one argument, with the format just past its specifier's '%'.
    template<typename T>
    static constexpr ReturnCode WriteArgument(SpanWriter<char>& writer, Span<const char>& format, T value)
    {
        if constexpr (std::is_enum_v<T>)
        {
            return WriteArgument(writer, format, static_cast<std::underlying_type_t<T>>(value));
        }
        else if constexpr (IsAnyOf<T, int, unsigned int, int8_t, uint8_t, int16_t, uint16_t>)
        {
            return WriteInteger(writer, format, static_cast<int>(value));
        }
        else if constexpr (IsAnyOf<T, float, double>)
        {
            return WriteFloat(writer, format, static_cast<float>(value));
        }
        else if constexpr (std::is_same_v<T, char>)
        {
            return TrySkip(format, 'c') ? writer.Write(value) : ReturnCode::NotSupported;
        }
        else if constexpr (std::is_same_v<T, const char*>)
        {
            return TrySkip(format, 's') ? writer.Write(String::FromNullTerminated(value)) : ReturnCode::NotSupported;
        }
        else
        {
            return ReturnCode::NotSupported;
        }
    }

    // %[+][0][width][h] then d, i, u or x
    static constexpr ReturnCode WriteInteger(SpanWriter<char>& writer, Span<const char>& format, int value)
    {
        auto alwaysSign = TrySkip(format, '+');
        auto padding = TrySkip(format, '0') ? '0' : ' ';
        auto width = ReadNumber(format);
        auto isShort = TrySkip(format, 'h');
        auto bits = static_cast<uint32_t>(value);

        if (TrySkip(format, 'd') || TrySkip(format, 'i') || TrySkip(format, 'u'))
        {
            if (value < 0)
            {
                return WriteNumber(writer, 0 - bits, 10, width, padding, '-');
            }

            return WriteNumber(writer, bits, 10, width, padding, alwaysSign ? '+' : '\0');
        }

        if (TrySkip(format, 'x'))
        {
            return WriteNumber(writer, isShort ? bits & 0xFFFF : bits, 16, width, padding);
        }

        return ReturnCode::NotSupported;
    }

    // %[width][.precision]f, where the width is ignored
    static constexpr ReturnCode WriteFloat(SpanWriter<char>& writer, Span<const char>& format, float value)
    {
        ReadNumber(format);
        auto precision = TrySkip(format, '.') ? ReadNumber(format) : DefaultPrecision;

        if (!TrySkip(format, 'f') || precision > MaxPrecision)
        {
            return ReturnCode::NotSupported;
        }

        auto isNegative = value < 0;
        auto magnitude = isNegative ? -value : value;

        // The whole part must fit 32 bits. NaN fails this too.
        if (!(magnitude < 4294967296.0f))
        {
            return ReturnCode::OutOfRange;
        }

        auto whole = static_cast<uint32_t>(magnitude);
        auto fraction = magnitude - static_cast<float>(whole);

        uint32_t scale = 1;

        for (uint32_t i = 0; i < precision; i++)
        {
            scale *= 10;
        }

        auto scaled = static_cast<uint32_t>(fraction * static_cast<float>(scale) + 0.5f);

        // The fraction rounded up to one.
        if (scaled == scale)
        {
            whole++;
            scaled = 0;
        }

        auto rc = WriteNumber(writer, whole, 10, 0, ' ', isNegative ? '-' : '\0');
        CHECK_RETURN_CODE(rc);

        rc = writer.Write('.');
        CHECK_RETURN_CODE(rc);

        return precision == 0 ? ReturnCode::Success : WriteNumber(writer, scaled, 10, precision, '0');
    }

    // Pads on the left to `width`, then writes the sign, if any, and the digits.
    static constexpr ReturnCode WriteNumber(SpanWriter<char>& writer, uint32_t value, uint32_t base,
        uint32_t width = 0, char padding = ' ', char sign = '\0')
    {
        // The leading digit's place value, and how many characters the number takes.
        uint32_t place = 1;
        uint32_t length = sign == '\0' ? 1 : 2;

        while (value / place >= base)
        {
            place *= base;
            length++;
        }

        for (; length < width; length++)
        {
            auto rc = writer.Write(padding);
            CHECK_RETURN_CODE(rc);
        }

        if (sign != '\0')
        {
            auto rc = writer.Write(sign);
            CHECK_RETURN_CODE(rc);
        }

        for (; place > 0; place /= base)
        {
            auto digit = value / place % base;
            auto rc = writer.Write(static_cast<char>(digit < 10 ? '0' + digit : 'a' + digit - 10));
            CHECK_RETURN_CODE(rc);
        }

        return ReturnCode::Success;
    }

    // Consumes `character` if the format starts with it.
    static constexpr bool TrySkip(Span<const char>& format, char character)
    {
        if (!format.TryCompare(0, character))
        {
            return false;
        }

        format = format.Skip(1);
        return true;
    }

    // Consumes leading digits, giving 0 if there are none.
    static constexpr uint32_t ReadNumber(Span<const char>& format)
    {
        uint32_t number = 0;
        char digit = '\0';

        while (format.TryGet(0, digit) && digit >= '0' && digit <= '9')
        {
            number = number * 10 + static_cast<uint32_t>(digit - '0');
            format = format.Skip(1);
        }

        return number;
    }
};
