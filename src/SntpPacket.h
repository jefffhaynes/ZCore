#pragma once

#include <stdint.h>
#include "BinaryPrimitives.h"
#include "ReturnCode.h"
#include "Span.h"
#include "SpanWriter.h"

// SNTP's one packet (RFC 4330), as a client asks with it and reads a server's answer from it.
class SntpPacket
{
public:
    static constexpr uint32_t Length = 48;

    // When the question reached the server and when the answer left it, in microseconds since 1970 UTC,
    // and how far the server's own clock may be from true, in microseconds.
    struct Answer
    {
        int64_t Received = 0;
        int64_t Sent = 0;
        int64_t RootDistance = 0;
    };

    // A question. The stamp is whatever the client will know its answer by: the server sends it back.
    static constexpr ReturnCode WriteQuestion(Span<uint8_t> destination, uint64_t stamp)
    {
        SpanWriter<uint8_t> writer(destination, Endianness::BigEndian);

        auto rc = writer.Write(Question);
        CHECK_RETURN_CODE(rc);

        // Everything between is the server's to fill in.
        for(uint32_t offset = 1; offset < SentOffset; offset++)
        {
            rc = writer.Write(static_cast<uint8_t>(0));
            CHECK_RETURN_CODE(rc);
        }

        return writer.Write(stamp);
    }

    // Busy where the server says to ask less often, and InvalidData where the packet isn't a server's
    // answer to this question, or the server has no time to give.
    static constexpr ReturnCode ReadAnswer(Span<const uint8_t> packet, uint64_t stamp, Answer& answer)
    {
        uint8_t first = 0;
        uint8_t stratum = 0;
        uint32_t rootDelay = 0;
        uint32_t rootDispersion = 0;
        uint64_t asked = 0;
        uint64_t received = 0;
        uint64_t sent = 0;

        if(!packet.TryGet(0, first) || !packet.TryGet(StratumOffset, stratum)
            || !BinaryPrimitives::TryReadBigEndian(packet.Skip(RootDelayOffset), rootDelay)
            || !BinaryPrimitives::TryReadBigEndian(packet.Skip(RootDispersionOffset), rootDispersion)
            || !BinaryPrimitives::TryReadBigEndian(packet.Skip(AskedOffset), asked)
            || !BinaryPrimitives::TryReadBigEndian(packet.Skip(ReceivedOffset), received)
            || !BinaryPrimitives::TryReadBigEndian(packet.Skip(SentOffset), sent))
        {
            return ReturnCode::InvalidLength;
        }

        if((first & ModeMask) != FromServer || asked != stamp)
        {
            return ReturnCode::InvalidData;
        }

        if(stratum == KissOfDeath)
        {
            return ReturnCode::Busy;
        }

        if((first >> LeapShift) == Unsynchronised || stratum > MaxStratum || sent == 0)
        {
            return ReturnCode::InvalidData;
        }

        answer.Received = ToUnixMicroseconds(received);
        answer.Sent = ToUnixMicroseconds(sent);
        answer.RootDistance = ToMicroseconds(rootDelay) / 2 + ToMicroseconds(rootDispersion);

        return ReturnCode::Success;
    }

    // The time it was when the answer arrived, and how far off that may be, from when the question left
    // and the answer arrived by the client's own count of microseconds. The trip is taken to be as long
    // one way as the other, so the time is off by half of what it isn't.
    static constexpr void Resolve(const Answer& answer, int64_t askedAt, int64_t answeredAt, int64_t& time, int64_t& uncertainty)
    {
        auto ahead = ((answer.Received - askedAt) + (answer.Sent - answeredAt)) / 2;
        auto trip = (answeredAt - askedAt) - (answer.Sent - answer.Received);

        time = answeredAt + ahead;
        uncertainty = (trip > 0 ? trip : 0) / 2 + answer.RootDistance;
    }

private:
    // No leap second warned of, version 4, from a client.
    static constexpr uint8_t Question = 0x23;

    static constexpr uint8_t ModeMask = 0x07;
    static constexpr uint8_t FromServer = 4;
    static constexpr uint32_t LeapShift = 6;
    static constexpr uint8_t Unsynchronised = 3;
    static constexpr uint8_t KissOfDeath = 0;
    static constexpr uint8_t MaxStratum = 15;

    static constexpr uint32_t StratumOffset = 1;
    static constexpr uint32_t RootDelayOffset = 4;
    static constexpr uint32_t RootDispersionOffset = 8;
    static constexpr uint32_t AskedOffset = 24;
    static constexpr uint32_t ReceivedOffset = 32;
    static constexpr uint32_t SentOffset = 40;

    static constexpr int64_t MicrosecondsPerSecond = 1000000;

    // NTP counts seconds from 1900, and 1970 is this many on.
    static constexpr int64_t SecondsTo1970 = 2208988800;

    // Its 32 bits of seconds ran from 1968 with the top bit set, and start again from 0 in 2036.
    static constexpr uint64_t FirstEraTopBit = 0x80000000;
    static constexpr int64_t SecondsPerEra = 0x100000000;

    // Seconds and a 32-bit fraction of one.
    static constexpr int64_t ToUnixMicroseconds(uint64_t timestamp)
    {
        auto seconds = static_cast<int64_t>(timestamp >> 32);
        auto fraction = timestamp & 0xFFFFFFFF;

        if(static_cast<uint64_t>(seconds) < FirstEraTopBit)
        {
            seconds += SecondsPerEra;
        }

        return (seconds - SecondsTo1970) * MicrosecondsPerSecond + static_cast<int64_t>((fraction * MicrosecondsPerSecond) >> 32);
    }

    // Seconds and a 16-bit fraction of one.
    static constexpr int64_t ToMicroseconds(uint32_t interval)
    {
        return static_cast<int64_t>((static_cast<uint64_t>(interval) * MicrosecondsPerSecond) >> 16);
    }
};
