#pragma once

#include "Array.h"
#include "Clock.h"
#include "IPEndPoint.h"
#include "SntpPacket.h"
#include "UdpClient.h"

// What a time server answered: the time it was, in microseconds since 1970 UTC, at a moment on Clock's
// uptime, and how far off that may be, which is half the trip there and back and the server's own doubt.
struct NetworkTime
{
    int64_t Time = 0;
    TimeSpan Uptime = TimeSpan::Zero();
    TimeSpan Uncertainty = TimeSpan::Zero();
};

// Asks an NTP server the time. Zephyr 4.3 has an SNTP client of its own, which allows for the trip only
// with an option that stamps the question by one clock and the answer by another, and so refuses every
// answer. This one is SntpPacket over a UdpClient.
class SntpClient
{
public:
    static constexpr uint16_t Port = 123;

    constexpr explicit SntpClient(IPEndPoint server) : _server(server)
    {
    }

    // One question and its answer. Timeout if none comes in time, Busy if the server says to ask less
    // often, and InvalidData if what comes isn't an answer to the question or has no time to give.
    ReturnCode Query(NetworkTime& time, TimeSpan timeout) const
    {
        UdpClient socket;
        Array<uint8_t, MaxAnswerLength> packet;

        auto rc = socket.Open();
        CHECK_RETURN_CODE(rc);

        // The question is stamped with when it left, which no other question shares.
        auto askedAt = Clock::GetUptime();
        auto stamp = static_cast<uint64_t>(askedAt.ToMicroseconds());

        rc = SntpPacket::WriteQuestion(packet.Take(SntpPacket::Length), stamp);
        CHECK_RETURN_CODE(rc);

        rc = socket.Send(packet.Take(SntpPacket::Length), _server);
        CHECK_RETURN_CODE(rc);

        while(true)
        {
            auto left = timeout - (Clock::GetUptime() - askedAt);

            if(left <= TimeSpan::Zero())
            {
                return ReturnCode::Timeout;
            }

            uint32_t received = 0;
            IPEndPoint from;

            rc = socket.Receive(packet, received, from, left);
            auto answeredAt = Clock::GetUptime();

            CHECK_RETURN_CODE(rc);

            // A packet from anywhere else is no answer. There may be one yet.
            if(from != _server)
            {
                continue;
            }

            SntpPacket::Answer answer;
            rc = SntpPacket::ReadAnswer(packet.Take(received), stamp, answer);
            CHECK_RETURN_CODE(rc);

            int64_t uncertainty = 0;
            SntpPacket::Resolve(answer, static_cast<int64_t>(stamp), static_cast<int64_t>(answeredAt.ToMicroseconds()), time.Time,
                uncertainty);

            time.Uptime = answeredAt;
            time.Uncertainty = TimeSpan::FromMicroseconds(uncertainty);

            return ReturnCode::Success;
        }
    }

private:
    // An answer may carry more after its 48 bytes, such as a signature.
    static constexpr uint32_t MaxAnswerLength = 128;

    IPEndPoint _server;
};
