#pragma once

#include "Array.h"
#include "SntpPacket.h"

namespace SntpPacketTests
{
    constexpr uint64_t Stamp = 0x0102030405060708;

    // 2026-10-10 00:00:00 UTC, which is 4000579200 s since 1900, or 0xEE73FE80.
    constexpr int64_t Midnight = 1791590400000000;

    // A server's answer: received at a quarter past the second, and sent at half past.
    constexpr Array<uint8_t, SntpPacket::Length> MakeAnswer(uint8_t first = 0x24, uint8_t stratum = 1)
    {
        const uint8_t bytes[] = {
            first, stratum, 0, 0xEC,
            0, 0, 0x08, 0,
            0, 0, 0x01, 0,
            'G', 'P', 'S', 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            1, 2, 3, 4, 5, 6, 7, 8,
            0xEE, 0x73, 0xFE, 0x80, 0x40, 0, 0, 0,
            0xEE, 0x73, 0xFE, 0x80, 0x80, 0, 0, 0
        };

        Array<uint8_t, SntpPacket::Length> packet;
        Span<const uint8_t>(bytes).CopyTo(packet);

        return packet;
    }

    static_assert([]{
        constexpr uint8_t expected[] = {
            0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8
        };
        Array<uint8_t, SntpPacket::Length> question;

        return SntpPacket::WriteQuestion(question, Stamp) == ReturnCode::Success && question.SequenceEquals(expected);
    }(), "A question isn't a version 4 client's, with its stamp where the server sends it back from");

    static_assert([]{
        Array<uint8_t, SntpPacket::Length - 1> question;
        return SntpPacket::WriteQuestion(question, Stamp) != ReturnCode::Success;
    }(), "A short buffer wasn't refused");

    static_assert([]{
        const auto packet = MakeAnswer();
        SntpPacket::Answer answer;

        // The root's delay is 1/32 s, of which half counts, and its dispersion 1/256 s.
        return SntpPacket::ReadAnswer(packet, Stamp, answer) == ReturnCode::Success
            && answer.Received == Midnight + 250000 && answer.Sent == Midnight + 500000 && answer.RootDistance == 15625 + 3906;
    }(), "An answer's times didn't read");

    static_assert([]{
        const auto packet = MakeAnswer();
        SntpPacket::Answer answer;

        return SntpPacket::ReadAnswer(packet, Stamp + 1, answer) == ReturnCode::InvalidData;
    }(), "An answer to another question wasn't refused");

    static_assert([]{
        const auto question = MakeAnswer(0x23);
        const auto unsynchronised = MakeAnswer(0xE4);
        const auto unreachable = MakeAnswer(0x24, 16);
        SntpPacket::Answer answer;

        return SntpPacket::ReadAnswer(question, Stamp, answer) == ReturnCode::InvalidData
            && SntpPacket::ReadAnswer(unsynchronised, Stamp, answer) == ReturnCode::InvalidData
            && SntpPacket::ReadAnswer(unreachable, Stamp, answer) == ReturnCode::InvalidData;
    }(), "A packet that isn't a server's with the time wasn't refused");

    static_assert([]{
        const auto packet = MakeAnswer(0x24, 0);
        SntpPacket::Answer answer;

        return SntpPacket::ReadAnswer(packet, Stamp, answer) == ReturnCode::Busy;
    }(), "A server saying to ask less often wasn't heard");

    static_assert([]{
        const auto packet = MakeAnswer();
        SntpPacket::Answer answer;

        return SntpPacket::ReadAnswer(packet.Take(SntpPacket::Length - 1), Stamp, answer) == ReturnCode::InvalidLength;
    }(), "A short answer wasn't refused");

    static_assert([]{
        // 100 s into NTP's second era, which starts in 2036.
        constexpr uint8_t packet[] = {
            0x24, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0
        };
        SntpPacket::Answer answer;

        return SntpPacket::ReadAnswer(packet, Stamp, answer) == ReturnCode::Success && answer.Sent == 2085978596000000;
    }(), "A time after 2036 didn't read");

    // The server's clock is Midnight ahead of the client's count. The question takes 10 ms to arrive, the
    // server 10 ms to answer, and the answer 10 ms to come back.
    static_assert([]{
        SntpPacket::Answer answer = { Midnight + 1010000, Midnight + 1020000, 300 };
        int64_t time = 0;
        int64_t uncertainty = 0;

        SntpPacket::Resolve(answer, 1000000, 1030000, time, uncertainty);

        return time == Midnight + 1030000 && uncertainty == 10000 + 300;
    }(), "The time isn't the server's, with the trip allowed for");

    // The same with the way back taking 30 ms: the time is taken to be 10 ms earlier than it is, which is
    // within what it says it may be off by.
    static_assert([]{
        SntpPacket::Answer answer = { Midnight + 1010000, Midnight + 1020000, 0 };
        int64_t time = 0;
        int64_t uncertainty = 0;

        SntpPacket::Resolve(answer, 1000000, 1050000, time, uncertainty);

        return time == Midnight + 1050000 - 10000 && uncertainty == 20000;
    }(), "A trip longer one way isn't off by half the difference");
}
