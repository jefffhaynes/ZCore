#pragma once

#include <stdint.h>
#include "Array.h"
#include "CoreString.h"
#include "ReturnCode.h"
#include "SpanWriter.h"
#include "StringParser.h"

// An IPv4 address. Its bytes are in network order.
struct IPAddress
{
public:
    static constexpr uint32_t Length = 4;
    static constexpr uint32_t MaxStringLength = 15;

    constexpr IPAddress()
    {
    }

    constexpr IPAddress(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
        : _bytes(first, second, third, fourth)
    {
    }

    constexpr explicit IPAddress(FixedSpan<const uint8_t, Length> bytes)
        : _bytes(bytes.Get<0>(), bytes.Get<1>(), bytes.Get<2>(), bytes.Get<3>())
    {
    }

    static constexpr IPAddress Any() { return IPAddress(); }
    static constexpr IPAddress Loopback() { return IPAddress(127, 0, 0, 1); }
    static constexpr IPAddress Broadcast() { return IPAddress(255, 255, 255, 255); }

    // For bytes whose count is only known at run time; the FixedSpan constructor can't fail.
    static constexpr ReturnCode FromBytes(Span<const uint8_t> bytes, IPAddress& address)
    {
        if(bytes.GetLength() != Length)
        {
            return ReturnCode::InvalidLength;
        }

        return bytes.CopyTo(address._bytes);
    }

    // Four decimal parts. A part with a leading zero is rejected: elsewhere it reads as octal.
    static constexpr ReturnCode Parse(String text, IPAddress& address)
    {
        IPAddress parsed;
        uint32_t offset = 0;
        uint32_t count = 0;

        for(auto& byte : parsed._bytes)
        {
            auto remaining = text.Substring(offset);
            auto isLast = ++count == Length;
            auto dot = remaining.IndexOf('.');

            if(!isLast && dot < 0)
            {
                return ReturnCode::InvalidData;
            }

            // The last part takes all that's left, so anything after a fourth fails to parse.
            String part = isLast ? remaining : String(remaining.Take(dot));

            if((part.GetLength() > 1 && part.StartsWith("0")) || !StringParser::TryParse(part, byte))
            {
                return ReturnCode::InvalidData;
            }

            offset += part.GetLength() + 1;
        }

        address = parsed;

        return ReturnCode::Success;
    }

    static constexpr bool TryParse(String text, IPAddress& address)
    {
        return Parse(text, address) == ReturnCode::Success;
    }

    // Empty if the buffer is shorter than the text; MaxStringLength always fits.
    constexpr String ToString(Span<char> buffer) const
    {
        SpanWriter<char> writer(buffer);

        for(auto part : _bytes)
        {
            if((writer.GetWritten() > 0 && writer.Write('.') != ReturnCode::Success)
                || WriteDecimal(writer, part) != ReturnCode::Success)
            {
                return String();
            }
        }

        return writer.GetWrittenSpan();
    }

    constexpr FixedSpan<const uint8_t, Length> GetBytes() const { return _bytes; }

    constexpr bool IsAny() const { return *this == Any(); }
    constexpr bool IsLoopback() const { return _bytes.Get<0>() == 127; }
    constexpr bool IsMulticast() const { return (_bytes.Get<0>() & 0xF0) == 0xE0; }

    constexpr bool IsInSubnet(IPAddress network, IPAddress subnetMask) const
    {
        return (*this & subnetMask) == (network & subnetMask);
    }

    // With a subnet mask, the network's address.
    constexpr IPAddress operator&(const IPAddress& other) const
    {
        return IPAddress(
            static_cast<uint8_t>(_bytes.Get<0>() & other._bytes.Get<0>()),
            static_cast<uint8_t>(_bytes.Get<1>() & other._bytes.Get<1>()),
            static_cast<uint8_t>(_bytes.Get<2>() & other._bytes.Get<2>()),
            static_cast<uint8_t>(_bytes.Get<3>() & other._bytes.Get<3>()));
    }

    constexpr bool operator==(const IPAddress& other) const
    {
        return _bytes.SequenceEquals(other._bytes);
    }

private:
    Array<uint8_t, Length> _bytes;

    static constexpr ReturnCode WriteDecimal(SpanWriter<char>& writer, uint8_t value)
    {
        if(value >= 100)
        {
            auto rc = writer.Write(ToDigit(value / 100));
            CHECK_RETURN_CODE(rc);
        }

        if(value >= 10)
        {
            auto rc = writer.Write(ToDigit(value / 10));
            CHECK_RETURN_CODE(rc);
        }

        return writer.Write(ToDigit(value));
    }

    static constexpr char ToDigit(uint32_t value)
    {
        return static_cast<char>('0' + value % 10);
    }
};

static_assert(sizeof(IPAddress) == IPAddress::Length, "IPAddress must be exactly its four bytes");
