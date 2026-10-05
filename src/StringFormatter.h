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

    // What opens a number's specifier: %[+][0][width]
    struct Layout
    {
        bool AlwaysSign;
        bool ZeroPad;
        uint32_t Width;
    };

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
        else if constexpr (std::is_same_v<T, char>)
        {
            return TrySkip(format, 'c') ? writer.Write(value) : ReturnCode::NotSupported;
        }
        else if constexpr (std::is_integral_v<T>)
        {
            using Bits = std::conditional_t<(sizeof(T) > sizeof(uint32_t)), uint64_t, uint32_t>;
            return WriteInteger(writer, format, static_cast<Bits>(value), std::is_signed_v<T>);
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            return WriteFloat(writer, format, static_cast<float>(value));
        }
        else if constexpr (std::is_base_of_v<Span<const char>, T>)
        {
            return TrySkip(format, 's') ? writer.Write(value) : ReturnCode::NotSupported;
        }
        else if constexpr (std::is_same_v<T, const char*> || std::is_same_v<T, char*>)
        {
            if (value == nullptr)
            {
                return ReturnCode::NullArgument;
            }

            return TrySkip(format, 's') ? writer.Write(String::FromNullTerminated(value)) : ReturnCode::NotSupported;
        }
        else
        {
            return ReturnCode::NotSupported;
        }
    }

    // %[+][0][width][h, l, ll or z] then d, i, u or x. `bits` is the argument, sign-extended:
    // d and i print its value, u and x its bits, and hx its low 16.
    template<typename TBits>
    static constexpr ReturnCode WriteInteger(SpanWriter<char>& writer, Span<const char>& format,
        TBits bits, bool isSigned)
    {
        auto layout = ReadLayout(format);
        auto isShort = TrySkip(format, 'h');

        // The argument's type gives its size, so these change nothing.
        while (TrySkip(format, 'l') || TrySkip(format, 'z'))
        {
        }

        if (TrySkip(format, 'd') || TrySkip(format, 'i'))
        {
            auto isNegative = isSigned && static_cast<std::make_signed_t<TBits>>(bits) < 0;
            auto magnitude = isNegative ? static_cast<TBits>(0 - bits) : bits;

            return WriteNumber(writer, layout, GetSign(layout, isNegative), magnitude, 10);
        }

        if (TrySkip(format, 'u'))
        {
            return WriteNumber(writer, layout, '\0', bits, 10);
        }

        if (TrySkip(format, 'x'))
        {
            return WriteNumber(writer, layout, '\0', isShort ? static_cast<TBits>(bits & 0xFFFF) : bits, 16);
        }

        return ReturnCode::NotSupported;
    }

    // %[+][0][width][.precision]f
    static constexpr ReturnCode WriteFloat(SpanWriter<char>& writer, Span<const char>& format, float value)
    {
        auto layout = ReadLayout(format);
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

        auto wholeDigits = CountDigits(whole, 10);
        auto pointAndFraction = precision == 0 ? 0 : 1 + precision;

        auto rc = WriteLead(writer, layout, GetSign(layout, isNegative), wholeDigits + pointAndFraction);
        CHECK_RETURN_CODE(rc);

        rc = WriteDigits(writer, whole, 10, wholeDigits);
        CHECK_RETURN_CODE(rc);

        if (precision == 0)
        {
            return ReturnCode::Success;
        }

        rc = writer.Write('.');
        CHECK_RETURN_CODE(rc);

        return WriteDigits(writer, scaled, 10, precision);
    }

    template<typename TBits>
    static constexpr ReturnCode WriteNumber(SpanWriter<char>& writer, const Layout& layout, char sign,
        TBits value, uint32_t base)
    {
        auto digits = CountDigits(value, base);

        auto rc = WriteLead(writer, layout, sign, digits);
        CHECK_RETURN_CODE(rc);

        return WriteDigits(writer, value, base, digits);
    }

    // Writes the sign, if any, and the padding that brings it and `length` more characters up to
    // the width. Zeros go after the sign, and spaces before it.
    static constexpr ReturnCode WriteLead(SpanWriter<char>& writer, const Layout& layout, char sign, uint32_t length)
    {
        auto hasSign = sign != '\0';
        auto used = length + (hasSign ? 1 : 0);
        auto padding = used < layout.Width ? layout.Width - used : 0;

        auto rc = WriteRepeated(writer, ' ', layout.ZeroPad ? 0 : padding);
        CHECK_RETURN_CODE(rc);

        if (hasSign)
        {
            rc = writer.Write(sign);
            CHECK_RETURN_CODE(rc);
        }

        return WriteRepeated(writer, '0', layout.ZeroPad ? padding : 0);
    }

    static constexpr ReturnCode WriteRepeated(SpanWriter<char>& writer, char character, uint32_t count)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            auto rc = writer.Write(character);
            CHECK_RETURN_CODE(rc);
        }

        return ReturnCode::Success;
    }

    // Writes `value` as `digits` digits, with leading zeros if it has fewer.
    template<typename TBits>
    static constexpr ReturnCode WriteDigits(SpanWriter<char>& writer, TBits value, uint32_t base, uint32_t digits)
    {
        // The first digit's place value.
        TBits place = 1;

        for (uint32_t i = 1; i < digits; i++)
        {
            place *= base;
        }

        for (; place > 0; place /= base)
        {
            auto digit = static_cast<uint32_t>(value / place % base);
            auto rc = writer.Write(static_cast<char>(digit < 10 ? '0' + digit : 'a' + digit - 10));
            CHECK_RETURN_CODE(rc);
        }

        return ReturnCode::Success;
    }

    template<typename TBits>
    static constexpr uint32_t CountDigits(TBits value, uint32_t base)
    {
        uint32_t digits = 1;

        for (; value >= base; value /= base)
        {
            digits++;
        }

        return digits;
    }

    static constexpr char GetSign(const Layout& layout, bool isNegative)
    {
        if (isNegative)
        {
            return '-';
        }

        return layout.AlwaysSign ? '+' : '\0';
    }

    static constexpr Layout ReadLayout(Span<const char>& format)
    {
        auto alwaysSign = TrySkip(format, '+');
        auto zeroPad = TrySkip(format, '0');
        auto width = ReadNumber(format);

        return { alwaysSign, zeroPad, width };
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
