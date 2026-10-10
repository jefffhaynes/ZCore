#pragma once

#include "Array.h"
#include "Hkdf.h"
#include "Hmac.h"
#include "Sha1.h"

// HMAC-SHA1 against RFC 2202, and Sha1 in parts, at compile time.
namespace HmacTests
{
    template<size_t N>
    constexpr Array<uint8_t, N - 1> Ascii(const char (&text)[N])
    {
        Array<uint8_t, N - 1> bytes;

        for(uint32_t i = 0; i < N - 1; i++)
        {
            bytes.TrySet(i, static_cast<uint8_t>(text[i]));
        }

        return bytes;
    }

    static_assert([]{
        constexpr uint8_t expected[] = { 0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a, 0xba, 0x3e,
            0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0, 0xd8, 0x9d };
        Sha1 sha1;
        Array<uint8_t, Sha1::HashLength> digest;
        auto message = Ascii("abc");

        // In parts of 1 and 2 bytes, and whole.
        return sha1.Begin() == ReturnCode::Success
            && sha1.Update(message.Take(1)) == ReturnCode::Success
            && sha1.Update(message.Skip(1)) == ReturnCode::Success
            && sha1.Finish(digest) == ReturnCode::Success
            && digest.SequenceEquals(expected)
            && sha1.Compute(message, digest) == ReturnCode::Success
            && digest.SequenceEquals(expected)
            && Sha1::Compute(message).SequenceEquals(expected);
    }(), "SHA-1 in parts isn't SHA-1");

    static_assert([]{
        // 130 bytes: two blocks and a bit, in parts that end mid-word and mid-block.
        Array<uint8_t, 130> message;
        for(uint32_t i = 0; i < message.GetLength(); i++)
        {
            message.TrySet(i, static_cast<uint8_t>(i * 3));
        }

        Sha1 sha1;
        Array<uint8_t, Sha1::HashLength> inParts;

        return sha1.Begin() == ReturnCode::Success
            && sha1.Update(message.Take(63)) == ReturnCode::Success
            && sha1.Update(message.Skip(63).Take(2)) == ReturnCode::Success
            && sha1.Update(message.Skip(65)) == ReturnCode::Success
            && sha1.Finish(inParts) == ReturnCode::Success
            && Sha1::Compute(message).SequenceEquals(inParts);
    }(), "SHA-1 in parts across blocks isn't SHA-1");

    static_assert([]{
        Sha1 sha1;
        Array<uint8_t, Sha1::HashLength> digest;
        Array<uint8_t, Sha1::HashLength - 1> shortDigest;

        return sha1.Update(Ascii("abc")) == ReturnCode::InvalidState
            && sha1.Finish(digest) == ReturnCode::InvalidState
            && sha1.Begin() == ReturnCode::Success
            && sha1.Finish(shortDigest) == ReturnCode::InvalidLength
            && sha1.Finish(digest) == ReturnCode::Success
            && sha1.Finish(digest) == ReturnCode::InvalidState;
    }(), "SHA-1 took a computation out of order");

    static_assert([]{
        // RFC 2202, case 1.
        constexpr uint8_t expected[] = { 0xb6, 0x17, 0x31, 0x86, 0x55, 0x05, 0x72, 0x64, 0xe2, 0x8b,
            0xc0, 0xb6, 0xfb, 0x37, 0x8c, 0x8e, 0xf1, 0x46, 0xbe, 0x00 };
        Array<uint8_t, 20> key;
        key.Fill(0x0b);
        Sha1 sha1;
        Hmac hmac(sha1);
        Array<uint8_t, Sha1::HashLength> mac;

        return hmac.Compute(key, Ascii("Hi There"), mac) == ReturnCode::Success && mac.SequenceEquals(expected);
    }(), "HMAC-SHA1 case 1 is wrong");

    static_assert([]{
        // RFC 2202, case 2, in parts.
        constexpr uint8_t expected[] = { 0xef, 0xfc, 0xdf, 0x6a, 0xe5, 0xeb, 0x2f, 0xa2, 0xd2, 0x74,
            0x16, 0xd5, 0xf1, 0x84, 0xdf, 0x9c, 0x25, 0x9a, 0x7c, 0x79 };
        Sha1 sha1;
        Hmac hmac(sha1);
        Array<uint8_t, Sha1::HashLength> mac;
        auto message = Ascii("what do ya want for nothing?");

        return hmac.Begin(Ascii("Jefe")) == ReturnCode::Success
            && hmac.Update(message.Take(5)) == ReturnCode::Success
            && hmac.Update(message.Skip(5)) == ReturnCode::Success
            && hmac.Finish(mac) == ReturnCode::Success
            && mac.SequenceEquals(expected);
    }(), "HMAC-SHA1 case 2 is wrong");

    static_assert([]{
        // RFC 2202, case 6: a key longer than the block, hashed first.
        constexpr uint8_t expected[] = { 0xaa, 0x4a, 0xe5, 0xe1, 0x52, 0x72, 0xd0, 0x0e, 0x95, 0x70,
            0x56, 0x37, 0xce, 0x8a, 0x3b, 0x55, 0xed, 0x40, 0x21, 0x12 };
        Array<uint8_t, 80> key;
        key.Fill(0xaa);
        Sha1 sha1;
        Hmac hmac(sha1);
        Array<uint8_t, Sha1::HashLength> mac;

        return hmac.Compute(key, Ascii("Test Using Larger Than Block-Size Key - Hash Key First"), mac) == ReturnCode::Success
            && mac.SequenceEquals(expected);
    }(), "HMAC-SHA1 case 6 is wrong");

    static_assert([]{
        // HKDF over SHA-1 runs; its answers are checked against RFC 5869 on a device.
        Array<uint8_t, 22> ikm;
        ikm.Fill(0x0b);
        Sha1 sha1;
        Hkdf hkdf(sha1);
        Array<uint8_t, 42> okm;
        Array<uint8_t, Sha1::HashLength> prk;
        Array<uint8_t, Sha1::HashLength - 1> shortPrk;

        return hkdf.DeriveKey(ikm, Span<const uint8_t>(), Span<const uint8_t>(), okm) == ReturnCode::Success
            && hkdf.Extract(Span<const uint8_t>(), ikm, prk) == ReturnCode::Success
            && hkdf.Expand(shortPrk, Span<const uint8_t>(), okm) == ReturnCode::InvalidLength;
    }(), "HKDF over SHA-1 didn't run");
}
