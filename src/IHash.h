#pragma once

#include <stdint.h>
#include "ReturnCode.h"
#include "Span.h"

// A hash: a digest of data, in one call or in parts. An instance holds one computation at a time:
// Begin, Update for each part, Finish. Sha1 is one in software, constexpr; Hash is one over whatever
// device Zephyr's crypto API has. Hmac and Hkdf take any.
class IHash
{
public:
    virtual constexpr ~IHash() = default;

    virtual constexpr uint32_t GetDigestLength() const = 0;

    // The block the algorithm works in, which HMAC pads its key to.
    virtual constexpr uint32_t GetBlockLength() const = 0;

    virtual constexpr ReturnCode Begin() = 0;

    virtual constexpr ReturnCode Update(Span<const uint8_t> data) = 0;

    // Writes the digest, which must be exactly GetDigestLength() long, and ends the computation.
    // InvalidLength otherwise, with the computation still open.
    virtual constexpr ReturnCode Finish(Span<uint8_t> digest) = 0;

    // The whole of data in one call. A digest of the wrong length is refused before anything begins.
    constexpr ReturnCode Compute(Span<const uint8_t> data, Span<uint8_t> digest)
    {
        if(digest.GetLength() != GetDigestLength())
        {
            return ReturnCode::InvalidLength;
        }

        auto rc = Begin();
        CHECK_RETURN_CODE(rc);

        rc = Update(data);
        CHECK_RETURN_CODE(rc);

        return Finish(digest);
    }
};
