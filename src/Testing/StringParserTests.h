#pragma once

#include "StringParser.h"

namespace StringParserTests
{
    template<typename T>
    constexpr bool Parses(String text, T expected)
    {
        T value = 0;
        return StringParser::Parse(text, value) == ReturnCode::Success && value == expected;
    }

    // A failure leaves the value as it was.
    template<typename T>
    constexpr bool Fails(String text, ReturnCode expected)
    {
        T value = 42;
        return StringParser::Parse(text, value) == expected && !StringParser::TryParse(text, value) && value == 42;
    }

    static_assert(Parses<uint32_t>("0", 0) && Parses<uint32_t>("7", 7) && Parses<uint32_t>("12345", 12345),
        "Parse method failed");
    static_assert(Parses<uint32_t>("007", 7) && Parses<uint8_t>("0000000000000000000000255", 255),
        "Parse method failed for leading zeros");

    static_assert([]{
        int32_t value = 0;
        return StringParser::TryParse("-17", value) && value == -17;
    }(), "TryParse method failed");

    static_assert(Fails<uint32_t>("", ReturnCode::InvalidData), "Parse method took an empty string");
    static_assert(Fails<uint32_t>("12a", ReturnCode::InvalidData) && Fails<uint32_t>("a12", ReturnCode::InvalidData)
        && Fails<uint32_t>("1.5", ReturnCode::InvalidData), "Parse method took a character that isn't a digit");
    static_assert(Fails<uint32_t>(" 1", ReturnCode::InvalidData) && Fails<uint32_t>("1 ", ReturnCode::InvalidData),
        "Parse method took a space");
    static_assert(Fails<uint32_t>("+1", ReturnCode::InvalidData) && Fails<int32_t>("+1", ReturnCode::InvalidData),
        "Parse method took a plus sign");
    static_assert(Fails<uint32_t>("-1", ReturnCode::InvalidData) && Fails<uint32_t>("-0", ReturnCode::InvalidData),
        "Parse method took a minus sign for an unsigned type");
    static_assert(Fails<int32_t>("-", ReturnCode::InvalidData) && Fails<int32_t>("--1", ReturnCode::InvalidData)
        && Fails<int32_t>("1-", ReturnCode::InvalidData), "Parse method took a misplaced minus sign");

    static_assert(Parses<int32_t>("-0", 0) && Parses<int32_t>("-1", -1) && Parses<int32_t>("-007", -7),
        "Parse method failed for negative numbers");

    static_assert(Parses<uint8_t>("255", 255) && Fails<uint8_t>("256", ReturnCode::OutOfRange)
        && Fails<uint8_t>("1000", ReturnCode::OutOfRange), "Parse method failed at uint8_t's range");
    static_assert(Parses<int8_t>("127", 127) && Parses<int8_t>("-128", -128)
        && Fails<int8_t>("128", ReturnCode::OutOfRange) && Fails<int8_t>("-129", ReturnCode::OutOfRange),
        "Parse method failed at int8_t's range");

    static_assert(Parses<uint16_t>("65535", 65535) && Fails<uint16_t>("65536", ReturnCode::OutOfRange),
        "Parse method failed at uint16_t's range");
    static_assert(Parses<int16_t>("32767", 32767) && Parses<int16_t>("-32768", -32768)
        && Fails<int16_t>("32768", ReturnCode::OutOfRange) && Fails<int16_t>("-32769", ReturnCode::OutOfRange),
        "Parse method failed at int16_t's range");

    static_assert(Parses<uint32_t>("4294967295", UINT32_MAX) && Fails<uint32_t>("4294967296", ReturnCode::OutOfRange),
        "Parse method failed at uint32_t's range");
    static_assert(Parses<int32_t>("2147483647", INT32_MAX) && Parses<int32_t>("-2147483648", INT32_MIN)
        && Fails<int32_t>("2147483648", ReturnCode::OutOfRange)
        && Fails<int32_t>("-2147483649", ReturnCode::OutOfRange), "Parse method failed at int32_t's range");

    static_assert(Parses<uint64_t>("18446744073709551615", UINT64_MAX)
        && Fails<uint64_t>("18446744073709551616", ReturnCode::OutOfRange),
        "Parse method failed at uint64_t's range");
    static_assert(Parses<int64_t>("9223372036854775807", INT64_MAX)
        && Parses<int64_t>("-9223372036854775808", INT64_MIN)
        && Fails<int64_t>("9223372036854775808", ReturnCode::OutOfRange)
        && Fails<int64_t>("-9223372036854775809", ReturnCode::OutOfRange), "Parse method failed at int64_t's range");

    static_assert(Fails<uint64_t>("99999999999999999999999999999999", ReturnCode::OutOfRange)
        && Fails<int8_t>("-99999999999999999999999999999999", ReturnCode::OutOfRange),
        "Parse method wrapped around on a long number");

    static_assert(Parses<uint32_t>(String("12345").Substring(2), 345), "Parse method read outside the string");
}
