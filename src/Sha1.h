#pragma once

#include <stdint.h>
#include <bit>
#include "Array.h"
#include "Span.h"
#include "SpanReader.h"
#include "SpanWriter.h"

// SHA-1 (RFC 3174). Too weak to sign with, but the WebSocket handshake is built on it.
class Sha1
{
public:
    static constexpr uint32_t HashLength = 20;

    static constexpr Array<uint8_t, HashLength> Compute(Span<const uint8_t> data)
    {
        State state(0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u);
        auto remaining = data;

        for(; remaining.GetLength() >= BlockLength; remaining = remaining.Skip(BlockLength))
        {
            Update(state, remaining.Take(BlockLength));
        }

        // What's left, a one bit, zeros, and the message's length in bits to end a block.
        Array<uint8_t, 2 * BlockLength> ending;
        auto endingLength = remaining.GetLength() + 1 + sizeof(uint64_t) <= BlockLength ? BlockLength : 2 * BlockLength;

        SpanWriter<uint8_t> writer(ending.Take(endingLength), Endianness::BigEndian);
        writer.Write(remaining);
        writer.Write(static_cast<uint8_t>(0x80));

        SpanWriter<uint8_t> lengthWriter(ending.Take(endingLength).Skip(endingLength - sizeof(uint64_t)), Endianness::BigEndian);
        lengthWriter.Write(static_cast<uint64_t>(data.GetLength()) * 8);

        for(Span<const uint8_t> blocks = ending.Take(endingLength); !blocks.IsEmpty(); blocks = blocks.Skip(BlockLength))
        {
            Update(state, blocks.Take(BlockLength));
        }

        Array<uint8_t, HashLength> hash;
        SpanWriter<uint8_t> hashWriter(hash, Endianness::BigEndian);

        for(auto word : state)
        {
            hashWriter.Write(word);
        }

        return hash;
    }

private:
    static constexpr uint32_t BlockLength = 64;
    static constexpr uint32_t BlockWords = BlockLength / sizeof(uint32_t);
    static constexpr uint32_t Rounds = 80;

    using State = Array<uint32_t, HashLength / sizeof(uint32_t)>;

    static constexpr void Update(State& state, Span<const uint8_t> block)
    {
        Array<uint32_t, Rounds> schedule;
        SpanReader<const uint8_t> reader(block, Endianness::BigEndian);
        SpanWriter<uint32_t> expanded(schedule);

        auto earlier = [&schedule](uint32_t index)
        {
            uint32_t word = 0;
            schedule.TryGet(index, word);

            return word;
        };

        for(uint32_t i = 0; i < Rounds; i++)
        {
            uint32_t word = 0;

            if(i < BlockWords)
            {
                reader.Read(word);
            }
            else
            {
                word = std::rotl(earlier(i - 3) ^ earlier(i - 8) ^ earlier(i - 14) ^ earlier(i - 16), 1);
            }

            expanded.Write(word);
        }

        auto a = state.Get<0>();
        auto b = state.Get<1>();
        auto c = state.Get<2>();
        auto d = state.Get<3>();
        auto e = state.Get<4>();

        uint32_t round = 0;

        for(auto word : schedule)
        {
            uint32_t mix = 0;
            uint32_t constant = 0;

            if(round < 20)
            {
                mix = (b & c) | (~b & d);
                constant = 0x5A827999u;
            }
            else if(round < 40)
            {
                mix = b ^ c ^ d;
                constant = 0x6ED9EBA1u;
            }
            else if(round < 60)
            {
                mix = (b & c) | (b & d) | (c & d);
                constant = 0x8F1BBCDCu;
            }
            else
            {
                mix = b ^ c ^ d;
                constant = 0xCA62C1D6u;
            }

            auto next = std::rotl(a, 5) + mix + e + constant + word;

            e = d;
            d = c;
            c = std::rotl(b, 30);
            b = a;
            a = next;
            round++;
        }

        state.Get<0>() += a;
        state.Get<1>() += b;
        state.Get<2>() += c;
        state.Get<3>() += d;
        state.Get<4>() += e;
    }
};
