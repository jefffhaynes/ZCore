#pragma once

#include <stdint.h>
#include <bit>
#include "Array.h"
#include "IHash.h"
#include "ReturnCode.h"
#include "Span.h"
#include "SpanReader.h"
#include "SpanWriter.h"

// SHA-1 (RFC 3174), in software and constexpr. Too weak to sign with, but the WebSocket handshake is
// built on it. An instance is a computation in parts; Compute(data) is the whole in one call.
class Sha1 : public IHash
{
public:
    static constexpr uint32_t HashLength = 20;
    static constexpr uint32_t BlockLength = 64;

    constexpr Sha1()
    {
        Reset();
    }

    // User-provided: GCC synthesizes a defaulted virtual destructor too late for Compute's constant
    // expression to use it.
    constexpr ~Sha1() override
    {
    }

    using IHash::Compute;

    // The whole of data in one call. Defined below the class: in a constant expression it destroys a
    // Sha1, which the class has to be complete for.
    static constexpr Array<uint8_t, HashLength> Compute(Span<const uint8_t> data);

    constexpr uint32_t GetDigestLength() const override
    {
        return HashLength;
    }

    constexpr uint32_t GetBlockLength() const override
    {
        return BlockLength;
    }

    constexpr ReturnCode Begin() override
    {
        Reset();
        _begun = true;

        return ReturnCode::Success;
    }

    constexpr ReturnCode Update(Span<const uint8_t> data) override
    {
        if(!_begun)
        {
            return ReturnCode::InvalidState;
        }

        _length += data.GetLength();

        // Fill the block that was left part way, if one was.
        if(_pendingLength > 0)
        {
            auto filling = data.Take(BlockLength - _pendingLength);
            filling.CopyTo(_pending.Skip(_pendingLength));
            _pendingLength += filling.GetLength();
            data = data.Skip(filling.GetLength());

            if(_pendingLength < BlockLength)
            {
                return ReturnCode::Success;
            }

            Transform(_pending);
            _pendingLength = 0;
        }

        for(; data.GetLength() >= BlockLength; data = data.Skip(BlockLength))
        {
            Transform(data.Take(BlockLength));
        }

        data.CopyTo(_pending.AsSpan());
        _pendingLength = data.GetLength();

        return ReturnCode::Success;
    }

    constexpr ReturnCode Finish(Span<uint8_t> digest) override
    {
        if(!_begun)
        {
            return ReturnCode::InvalidState;
        }

        if(digest.GetLength() != HashLength)
        {
            return ReturnCode::InvalidLength;
        }

        // What's pending, a one bit, zeros, and the message's length in bits to end a block.
        Array<uint8_t, 2 * BlockLength> ending;
        auto endingLength = _pendingLength + 1 + sizeof(uint64_t) <= BlockLength ? BlockLength : 2 * BlockLength;
        SpanWriter<uint8_t> writer(ending.Take(endingLength), Endianness::BigEndian);
        writer.Write(_pending.Take(_pendingLength));
        writer.Write(static_cast<uint8_t>(0x80));

        SpanWriter<uint8_t> lengthWriter(ending.Take(endingLength).Skip(endingLength - sizeof(uint64_t)), Endianness::BigEndian);
        lengthWriter.Write(_length * 8);

        for(Span<const uint8_t> blocks = ending.Take(endingLength); !blocks.IsEmpty(); blocks = blocks.Skip(BlockLength))
        {
            Transform(blocks.Take(BlockLength));
        }

        SpanWriter<uint8_t> hashWriter(digest, Endianness::BigEndian);

        for(auto word : _state)
        {
            hashWriter.Write(word);
        }

        Reset();
        return ReturnCode::Success;
    }

private:
    static constexpr uint32_t BlockWords = BlockLength / sizeof(uint32_t);
    static constexpr uint32_t Rounds = 80;

    using State = Array<uint32_t, HashLength / sizeof(uint32_t)>;

    State _state;
    Array<uint8_t, BlockLength> _pending;
    uint32_t _pendingLength = 0;
    uint64_t _length = 0;
    bool _begun = false;

    constexpr void Reset()
    {
        _state = State(0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u);
        _pending.Fill(0);
        _pendingLength = 0;
        _length = 0;
        _begun = false;
    }

    constexpr void Transform(Span<const uint8_t> block)
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

        auto a = _state.Get<0>();
        auto b = _state.Get<1>();
        auto c = _state.Get<2>();
        auto d = _state.Get<3>();
        auto e = _state.Get<4>();
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

        _state.Get<0>() += a;
        _state.Get<1>() += b;
        _state.Get<2>() += c;
        _state.Get<3>() += d;
        _state.Get<4>() += e;
    }
};

constexpr Array<uint8_t, Sha1::HashLength> Sha1::Compute(Span<const uint8_t> data)
{
    Sha1 sha1;
    Array<uint8_t, HashLength> hash;

    sha1.IHash::Compute(data, hash);

    return hash;
}
