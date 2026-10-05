#pragma once

#include "Base64.h"
#include "CoreString.h"

namespace Base64Tests
{
    template<uint32_t N>
    constexpr bool Encodes(const char (&text)[N], String expected)
    {
        Array<uint8_t, N> bytes;
        Span<const char>(text).template CopyTo<uint8_t>(bytes, [](char c) { return static_cast<uint8_t>(c); });

        Array<char, 16> encoded;
        uint32_t written = 0;

        return Base64::Encode(bytes.Take(N - 1), encoded, written) == ReturnCode::Success
            && String(encoded.Take(written)) == expected
            && written == Base64::GetEncodedLength(N - 1);
    }

    // RFC 4648's examples.
    static_assert(Encodes("", ""), "An empty input failed");
    static_assert(Encodes("f", "Zg=="), "One byte failed");
    static_assert(Encodes("fo", "Zm8="), "Two bytes failed");
    static_assert(Encodes("foo", "Zm9v"), "A whole group failed");
    static_assert(Encodes("foob", "Zm9vYg=="), "A group and one byte failed");
    static_assert(Encodes("fooba", "Zm9vYmE="), "A group and two bytes failed");
    static_assert(Encodes("foobar", "Zm9vYmFy"), "Two groups failed");

    static_assert([]{
        constexpr uint8_t bytes[] = { 0xFB, 0xFF, 0xBE };
        Array<char, 4> encoded;
        uint32_t written = 0;

        return Base64::Encode(bytes, encoded, written) == ReturnCode::Success && String(encoded.Take(written)) == String("+/++");
    }(), "The last symbols of the alphabet failed");

    static_assert([]{
        constexpr uint8_t bytes[] = { 1, 2, 3, 4 };
        Array<char, 7> encoded;
        uint32_t written = 1;

        return Base64::Encode(bytes, encoded, written) == ReturnCode::InvalidLength && written == 0;
    }(), "A destination too short wasn't refused");
}
