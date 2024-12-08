#pragma once

#include "Span.h"
#include "Array.h"

class StringLiteral;

class String : public Span<const char>
{
public:
    constexpr String() : Span("", 0)
    {
    }

    template<uint32_t N>
    constexpr String(const char (&string)[N]) : Span(string, N - 1)
    {
    }

    constexpr String(const String& other) : Span(other)
    {
    }

    constexpr String(const Span<const char>& other) : Span(other)
    {
    }

    constexpr String(const Span<char>& other) : Span(other)
    {
    }

    // Delete constructors that accept pointer types
    template <typename T, typename = typename std::enable_if<std::is_pointer<T>::value>::type>
    String(T) = delete;

    // assignment from const char (&)[N]
    template<uint32_t N>
    String& operator=(const char (&string)[N])
    {
        Span::operator=(string);
        return *this;
    }

    constexpr bool operator==(const String& other) const
    {
        return other.SequenceEquals(*this);
    }

    constexpr bool operator==(const StringLiteral& other) const;

    static constexpr String FromNullTerminated(const char* string)
    {
        return String(string, GetStringLength(string));
    }

    operator Span<const uint8_t>() const
    {
        return Span<const uint8_t>(reinterpret_cast<const uint8_t*>(GetData()), GetLength());
    }

    constexpr bool StartsWith(const String& other) const
    {
        auto substring = Take(other.GetLength());
        return substring.SequenceEquals(other);
    }
    
    constexpr String Substring(uint32_t start) const
    {
        return Span::Skip(start);
    }

protected:
    constexpr String(const char* string, uint32_t length) : Span(string, length)
    {
    }

    static uint32_t constexpr GetStringLength(Span<char> string)
    {
        auto index = string.IndexOf('\0');
        return index == -1 ? string.GetLength() - 1 : index;
    }

    static uint32_t constexpr GetStringLength(const char* string)
    {
        uint32_t length = 0;
        while(string[length] != '\0')
        {
            length++;
        }

        return length;
    }
};

class StringLiteral : public String
{
public:
    constexpr StringLiteral() : String("", 0)
    {
    }

    template<uint32_t Length>
    constexpr StringLiteral(const char (&string)[Length]) : String(string, Length - 1)
    {
    }

    template<uint32_t Length>
    constexpr StringLiteral(Array<char, Length>& data) : String(data.GetData(), GetStringLength(data))
    {
        if(data.AsSpan().IndexOf('\0') == -1)
        {
            data.Set(Length - 1, 0);
        }
    }
};

inline constexpr bool String::operator==(const StringLiteral& other) const
{
    return other.SequenceEquals(*this);
}