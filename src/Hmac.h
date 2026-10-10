#pragma once

#include <stdint.h>
#include "Array.h"
#include "IHash.h"
#include "ReturnCode.h"
#include "Span.h"

// HMAC (RFC 2104) over any IHash: a keyed digest, in parts (Begin with the key, Update, Finish) or in
// one call. A key of any length; one longer than the hash's block is hashed first, as the RFC says.
// Key material is wiped when a computation ends. Constexpr over a constexpr hash, such as Sha1.
class Hmac
{
public:
    static constexpr uint32_t MaxBlockLength = 128;
    static constexpr uint32_t MaxDigestLength = 64;

    explicit constexpr Hmac(IHash& hash) : _hash(hash)
    {
    }

    Hmac(const Hmac&) = delete;
    Hmac& operator=(const Hmac&) = delete;

    constexpr ~Hmac()
    {
        _outerPad.Fill(0);
    }

    constexpr uint32_t GetDigestLength() const
    {
        return _hash.GetDigestLength();
    }

    // InvalidLength if the hash's block or digest is longer than this class holds.
    constexpr ReturnCode Begin(Span<const uint8_t> key)
    {
        auto blockLength = _hash.GetBlockLength();

        if(blockLength > MaxBlockLength || _hash.GetDigestLength() > MaxDigestLength)
        {
            return ReturnCode::InvalidLength;
        }

        Array<uint8_t, MaxBlockLength> block;
        block.Fill(0);

        auto rc = key.GetLength() > blockLength
            ? _hash.Compute(key, block.Take(_hash.GetDigestLength()))
            : key.CopyTo(block.AsSpan());
        CHECK_RETURN_CODE(rc);

        auto innerPad = block.Take(blockLength);
        auto outerPad = _outerPad.Take(blockLength);

        for(uint32_t i = 0; i < blockLength; i++)
        {
            uint8_t byte = 0;
            block.TryGet(i, byte);
            innerPad.TrySet(i, static_cast<uint8_t>(byte ^ 0x36));
            outerPad.TrySet(i, static_cast<uint8_t>(byte ^ 0x5c));
        }

        rc = _hash.Begin();

        if(rc == ReturnCode::Success)
        {
            rc = _hash.Update(innerPad);
        }

        block.Fill(0);
        return rc;
    }

    constexpr ReturnCode Update(Span<const uint8_t> data)
    {
        return _hash.Update(data);
    }

    // Writes the MAC, which must be exactly GetDigestLength() long, and ends the computation.
    constexpr ReturnCode Finish(Span<uint8_t> mac)
    {
        auto digestLength = _hash.GetDigestLength();

        if(mac.GetLength() != digestLength)
        {
            return ReturnCode::InvalidLength;
        }

        Array<uint8_t, MaxDigestLength> innerDigest;
        auto digest = innerDigest.Take(digestLength);

        auto rc = _hash.Finish(digest);

        if(rc == ReturnCode::Success)
        {
            rc = _hash.Begin();
        }

        if(rc == ReturnCode::Success)
        {
            rc = _hash.Update(_outerPad.Take(_hash.GetBlockLength()));
        }

        if(rc == ReturnCode::Success)
        {
            rc = _hash.Update(digest);
        }

        if(rc == ReturnCode::Success)
        {
            rc = _hash.Finish(mac);
        }

        innerDigest.Fill(0);
        _outerPad.Fill(0);
        return rc;
    }

    // The MAC of data under key, in one call.
    constexpr ReturnCode Compute(Span<const uint8_t> key, Span<const uint8_t> data, Span<uint8_t> mac)
    {
        auto rc = Begin(key);
        CHECK_RETURN_CODE(rc);

        rc = Update(data);
        CHECK_RETURN_CODE(rc);

        return Finish(mac);
    }

private:
    IHash& _hash;
    Array<uint8_t, MaxBlockLength> _outerPad;
};
