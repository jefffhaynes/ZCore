#pragma once

#include <stdint.h>
#include <zephyr/crypto/crypto.h>
#include "Device.h"
#include "ErrorConverter.h"
#include "IHash.h"
#include "ReturnCode.h"
#include "Span.h"

// A hash over Zephyr's crypto API, on whatever device implements it: mbedTLS behind Zephyr's shim on
// native_sim, or the STM32N6's HASH behind the driver in the app that needed it. An instance holds one
// computation at a time, as a session of the driver's; give each thread its own.
class Hash : public Device, public IHash
{
public:
    enum class Algorithm : uint8_t
    {
        Sha224,
        Sha256,
        Sha384,
        Sha512
    };

    static constexpr uint32_t MaxDigestLength = 64;
    static constexpr uint32_t MaxBlockLength = 128;

    constexpr Hash(const struct device* device, Algorithm algorithm) : Device(device), _algorithm(algorithm)
    {
    }

    Hash(const Hash&) = delete;
    Hash& operator=(const Hash&) = delete;

    ~Hash() override
    {
        End();
    }

    constexpr Algorithm GetAlgorithm() const
    {
        return _algorithm;
    }

    constexpr uint32_t GetDigestLength() const override
    {
        return GetDigestLength(_algorithm);
    }

    constexpr uint32_t GetBlockLength() const override
    {
        return GetBlockLength(_algorithm);
    }

    static constexpr uint32_t GetDigestLength(Algorithm algorithm)
    {
        switch(algorithm)
        {
            case Algorithm::Sha224: return 28;
            case Algorithm::Sha256: return 32;
            case Algorithm::Sha384: return 48;
            default: return 64;
        }
    }

    static constexpr uint32_t GetBlockLength(Algorithm algorithm)
    {
        return algorithm == Algorithm::Sha224 || algorithm == Algorithm::Sha256 ? 64 : 128;
    }

    // InvalidState if the device isn't ready or a computation is open; otherwise as the driver says.
    ReturnCode Begin() override
    {
        if(_begun)
        {
            return ReturnCode::InvalidState;
        }

        auto rc = FailIfNotReady();
        CHECK_RETURN_CODE(rc);

        _context = {};
        _context.flags = CAP_SEPARATE_IO_BUFS | CAP_SYNC_OPS;

        auto err = hash_begin_session(GetDevice(), &_context, ToZephyr(_algorithm));
        rc = ErrorConverter::Convert(err);
        CHECK_RETURN_CODE(rc);

        _begun = true;
        return ReturnCode::Success;
    }

    ReturnCode Update(Span<const uint8_t> data) override
    {
        return Run(data, nullptr, false);
    }

    ReturnCode Finish(Span<uint8_t> digest) override
    {
        if(digest.GetLength() != GetDigestLength())
        {
            return ReturnCode::InvalidLength;
        }

        auto rc = Run(Span<const uint8_t>(), digest.GetData(), true);
        End();

        return rc;
    }

private:
    Algorithm _algorithm;
    struct hash_ctx _context = {};
    bool _begun = false;

    static constexpr hash_algo ToZephyr(Algorithm algorithm)
    {
        switch(algorithm)
        {
            case Algorithm::Sha224: return CRYPTO_HASH_ALGO_SHA224;
            case Algorithm::Sha256: return CRYPTO_HASH_ALGO_SHA256;
            case Algorithm::Sha384: return CRYPTO_HASH_ALGO_SHA384;
            default: return CRYPTO_HASH_ALGO_SHA512;
        }
    }

    ReturnCode Run(Span<const uint8_t> data, uint8_t* digest, bool finish)
    {
        if(!_begun)
        {
            return ReturnCode::InvalidState;
        }

        // Zephyr's packet is read, not written, apart from the digest.
        struct hash_pkt packet = {};
        packet.in_buf = data.GetData();
        packet.in_len = data.GetLength();
        packet.out_buf = digest;

        auto err = finish ? hash_compute(&_context, &packet) : hash_update(&_context, &packet);

        if(err < 0)
        {
            End();
        }

        return ErrorConverter::Convert(err);
    }

    void End()
    {
        if(_begun)
        {
            hash_free_session(GetDevice(), &_context);
            _begun = false;
        }
    }
};
