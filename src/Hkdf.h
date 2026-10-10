#pragma once

#include <stdint.h>
#include "Array.h"
#include "Hmac.h"
#include "IHash.h"
#include "ReturnCode.h"
#include "Span.h"

// HKDF (RFC 5869) over any IHash: keys from a secret, a salt and a context. Key material is wiped on
// the way out. Constexpr over a constexpr hash.
class Hkdf
{
public:
    explicit constexpr Hkdf(IHash& hash) : _hash(hash)
    {
    }

    // The pseudorandom key from the input key material, into prk, which must be exactly the hash's
    // digest length. An empty salt is what the RFC says: a digest's length of zeros, which is what
    // HMAC makes of an empty key anyway.
    constexpr ReturnCode Extract(Span<const uint8_t> salt, Span<const uint8_t> ikm, Span<uint8_t> prk)
    {
        return Hmac(_hash).Compute(salt, ikm, prk);
    }

    // The output key material from the pseudorandom key and the context, into okm, up to 255 digests
    // long. InvalidLength past that, or if prk isn't a digest long.
    constexpr ReturnCode Expand(Span<const uint8_t> prk, Span<const uint8_t> info, Span<uint8_t> okm)
    {
        auto digestLength = _hash.GetDigestLength();

        if(prk.GetLength() != digestLength || okm.GetLength() > 255 * digestLength)
        {
            return ReturnCode::InvalidLength;
        }

        Array<uint8_t, Hmac::MaxDigestLength> block;
        auto previous = block.Take(0);
        auto rc = ReturnCode::Success;

        for(uint8_t counter = 1; !okm.IsEmpty() && rc == ReturnCode::Success; counter++)
        {
            Hmac hmac(_hash);
            Array<uint8_t, 1> count(counter);

            rc = hmac.Begin(prk);

            if(rc == ReturnCode::Success)
            {
                rc = hmac.Update(previous);
            }

            if(rc == ReturnCode::Success)
            {
                rc = hmac.Update(info);
            }

            if(rc == ReturnCode::Success)
            {
                rc = hmac.Update(count);
            }

            if(rc == ReturnCode::Success)
            {
                rc = hmac.Finish(block.Take(digestLength));
            }

            if(rc == ReturnCode::Success)
            {
                auto part = okm.Take(digestLength);
                rc = block.Take(part.GetLength()).CopyTo(part);
                okm = okm.Skip(part.GetLength());
                previous = block.Take(digestLength);
            }
        }

        block.Fill(0);
        return rc;
    }

    // Extract then Expand.
    constexpr ReturnCode DeriveKey(Span<const uint8_t> ikm, Span<const uint8_t> salt, Span<const uint8_t> info,
        Span<uint8_t> okm)
    {
        Array<uint8_t, Hmac::MaxDigestLength> prk;
        auto key = prk.Take(_hash.GetDigestLength());

        auto rc = Extract(salt, ikm, key);

        if(rc == ReturnCode::Success)
        {
            rc = Expand(key, info, okm);
        }

        prk.Fill(0);
        return rc;
    }

private:
    IHash& _hash;
};
