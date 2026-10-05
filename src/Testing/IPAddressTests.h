#pragma once

#include "IPAddress.h"
#include "IPEndPoint.h"

namespace IPAddressTests
{
    constexpr IPAddress address(192, 168, 1, 10);
    constexpr uint8_t zeros[] = { 0, 0, 0, 0 };
    constexpr uint8_t bytes[] = { 192, 168, 1, 10 };

    static_assert(IPAddress().GetBytes().SequenceEquals(zeros), "Default constructor failed");
    static_assert(address.GetBytes().SequenceEquals(bytes), "Bytes aren't in network order");
    static_assert(address.GetBytes().GetLength() == IPAddress::Length && address.GetBytes().Get<3>() == 10,
        "GetBytes method failed");

    static_assert(IPAddress(FixedSpan<const uint8_t, 4>::FromArray(bytes)) == address,
        "FixedSpan constructor failed");
    static_assert(IPAddress(address.GetBytes()) == address, "GetBytes didn't construct the same address");
    static_assert(address.GetBytes().SequenceEquals(IPAddress(192, 168, 1, 10).GetBytes())
        && !address.GetBytes().SequenceEquals(IPAddress::Loopback().GetBytes()), "GetBytes didn't compare");

    static_assert(IPAddress::Any() == IPAddress(0, 0, 0, 0), "Any failed");
    static_assert(IPAddress::Loopback() == IPAddress(127, 0, 0, 1), "Loopback failed");
    static_assert(IPAddress::Broadcast() == IPAddress(255, 255, 255, 255), "Broadcast failed");

    static_assert(address == IPAddress(192, 168, 1, 10) && !(address == IPAddress(192, 168, 1, 11))
        && !(address == IPAddress(10, 168, 1, 10)), "Equality operator failed");

    constexpr bool Parses(String text, IPAddress expected)
    {
        IPAddress parsed;
        return IPAddress::Parse(text, parsed) == ReturnCode::Success && parsed == expected;
    }

    constexpr bool IsRejected(String text)
    {
        IPAddress parsed(1, 2, 3, 4);

        return IPAddress::Parse(text, parsed) == ReturnCode::InvalidData && !IPAddress::TryParse(text, parsed)
            && parsed == IPAddress(1, 2, 3, 4);
    }

    static_assert(Parses("192.168.1.10", address), "Parse method failed");
    static_assert(Parses("0.0.0.0", IPAddress::Any()), "Parse method failed for zeros");
    static_assert(Parses("255.255.255.255", IPAddress::Broadcast()), "Parse method failed at the top of the range");
    static_assert(Parses("1.20.3.100", IPAddress(1, 20, 3, 100)), "Parse method failed for mixed lengths");

    static_assert(IsRejected(""), "Parse method took an empty string");
    static_assert(IsRejected("192.168.1"), "Parse method took three parts");
    static_assert(IsRejected("192.168.1.10.5"), "Parse method took five parts");
    static_assert(IsRejected("192.168.1."), "Parse method took an empty last part");
    static_assert(IsRejected("192.168.1.10."), "Parse method took a trailing dot");
    static_assert(IsRejected(".192.168.1.10"), "Parse method took a leading dot");
    static_assert(IsRejected("192..1.10"), "Parse method took an empty part");
    static_assert(IsRejected("192.168.1.256"), "Parse method took a part over 255");
    static_assert(IsRejected("192.168.1.1000"), "Parse method took a four-digit part");
    static_assert(IsRejected("192.168.01.10"), "Parse method took a leading zero");
    static_assert(IsRejected("192.168.1.00"), "Parse method took a doubled zero");
    static_assert(IsRejected("192.168.1.10 "), "Parse method took a trailing space");
    static_assert(IsRejected("192.168.1.-1"), "Parse method took a sign");
    static_assert(IsRejected("192.168.1.a"), "Parse method took a letter");
    static_assert(IsRejected("192.168.1.+1"), "Parse method took a plus sign");
    static_assert(IsRejected("1.2.3.4.5.6"), "Parse method took six parts");
    static_assert(IsRejected("...") && IsRejected("."), "Parse method took dots alone");
    static_assert(IsRejected("192.168.1.99999999999999999999"), "Parse method took a part that overflows");

    static_assert([]{
        IPAddress parsed;
        return IPAddress::TryParse("10.0.0.1", parsed) && parsed == IPAddress(10, 0, 0, 1);
    }(), "TryParse method failed");

    constexpr bool Formats(IPAddress value, String expected)
    {
        Array<char, IPAddress::MaxStringLength> buffer;
        return value.ToString(buffer) == expected;
    }

    static_assert(Formats(address, "192.168.1.10"), "ToString method failed");
    static_assert(Formats(IPAddress::Any(), "0.0.0.0"), "ToString method failed for zeros");
    static_assert(Formats(IPAddress::Broadcast(), "255.255.255.255"), "ToString method failed at the longest text");
    static_assert(Formats(IPAddress(1, 20, 100, 9), "1.20.100.9"), "ToString method failed for mixed lengths");

    static_assert([]{
        Array<char, 12> exact;
        Array<char, 11> small;
        return address.ToString(exact) == String("192.168.1.10") && address.ToString(small).IsEmpty()
            && address.ToString(Span<char>()).IsEmpty();
    }(), "ToString method didn't respect the buffer's length");

    static_assert([]{
        Array<char, IPAddress::MaxStringLength> buffer;
        IPAddress parsed;

        for(uint32_t value = 0; value <= 255; value += 5)
        {
            IPAddress original(static_cast<uint8_t>(value), static_cast<uint8_t>(255 - value),
                static_cast<uint8_t>(value / 2), static_cast<uint8_t>(value * 7));

            if(!IPAddress::TryParse(original.ToString(buffer), parsed) || !(parsed == original))
            {
                return false;
            }
        }

        return true;
    }(), "Text from ToString didn't parse back to the same address");

    static_assert([]{
        IPAddress fromBytes;
        Span<const uint8_t> source(bytes);

        return IPAddress::FromBytes(source, fromBytes) == ReturnCode::Success && fromBytes == address
            && IPAddress::FromBytes(source.Take(3), fromBytes) == ReturnCode::InvalidLength
            && IPAddress::FromBytes(Span<const uint8_t>(), fromBytes) == ReturnCode::InvalidLength;
    }(), "FromBytes method failed");

    static_assert(IPAddress::Any().IsAny() && !address.IsAny(), "IsAny method failed");
    static_assert(IPAddress::Loopback().IsLoopback() && IPAddress(127, 255, 0, 3).IsLoopback() && !address.IsLoopback()
        && !IPAddress(128, 0, 0, 1).IsLoopback(), "IsLoopback method failed");
    static_assert(IPAddress(224, 0, 0, 1).IsMulticast() && IPAddress(239, 255, 255, 255).IsMulticast()
        && !IPAddress(223, 255, 255, 255).IsMulticast() && !IPAddress(240, 0, 0, 0).IsMulticast()
        && !address.IsMulticast(), "IsMulticast method failed");

    constexpr IPAddress mask(255, 255, 255, 0);
    static_assert(address.IsInSubnet(IPAddress(192, 168, 1, 0), mask)
        && address.IsInSubnet(IPAddress(192, 168, 1, 77), mask)
        && !address.IsInSubnet(IPAddress(192, 168, 2, 0), mask)
        && address.IsInSubnet(IPAddress(192, 168, 2, 0), IPAddress(255, 255, 0, 0))
        && address.IsInSubnet(IPAddress(10, 0, 0, 0), IPAddress::Any()), "IsInSubnet method failed");

    static_assert((address & mask) == IPAddress(192, 168, 1, 0)
        && (address & IPAddress(255, 240, 0, 255)) == IPAddress(192, 160, 0, 10)
        && (address & IPAddress::Broadcast()) == address && (address & IPAddress::Any()).IsAny(),
        "And operator failed");

    static_assert(IPEndPoint().Address == IPAddress::Any() && IPEndPoint().Port == 0,
        "IPEndPoint default constructor failed");
    static_assert(IPEndPoint(address, 5000).Address == address && IPEndPoint(address, 5000).Port == 5000,
        "IPEndPoint constructor failed");
    static_assert(IPEndPoint(address, 5000) == IPEndPoint(address, 5000)
        && !(IPEndPoint(address, 5000) == IPEndPoint(address, 5001))
        && !(IPEndPoint(address, 5000) == IPEndPoint(IPAddress::Loopback(), 5000)),
        "IPEndPoint equality operator failed");
}
