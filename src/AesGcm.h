#pragma once

#include <stdint.h>
#include <zephyr/crypto/crypto.h>
#include "Array.h"
#include "Device.h"
#include "ErrorConverter.h"
#include "FixedSpan.h"
#include "ReturnCode.h"
#include "Span.h"

#if !defined(CONFIG_CRYPTO)
#error "AesGcm needs CONFIG_CRYPTO"
#endif

// AES-GCM (NIST SP 800-38D) through Zephyr's crypto API, on whichever driver `device` is: the SoC's own
// hardware, or mbedTLS in software (CONFIG_CRYPTO_MBEDTLS_SHIM).
//
// It takes what TLS's AES-GCM suites use (RFC 5288, RFC 8446): 96-bit nonces and whole 128-bit tags.
// A record seals in place, with its header as the associated data and the tag following it, where the
// driver works in place (CAP_INPLACE_OPS); mbedTLS's doesn't.
//
// Each instance holds a key, and each operation is a session of its own, so instances for either
// direction of a connection can be used from any thread, as far as the driver allows. Don't change a key
// while an operation is using it.
//
// A nonce must never repeat under one key. Two messages sealed with the same one give away the XOR of
// their plaintexts, and enough to forge tags.
class AesGcm : public Device
{
public:
    static constexpr uint32_t NonceLength = 12;
    static constexpr uint32_t TagLength = 16;

    using Nonce = FixedSpan<const uint8_t, NonceLength>;

    explicit AesGcm(const struct device* device) : Device(device)
    {
    }

    // A copy would be one more place the key has to be wiped from.
    AesGcm(const AesGcm&) = delete;
    AesGcm& operator=(const AesGcm&) = delete;

    ~AesGcm()
    {
        ClearKey();
    }

    // 16, 24 or 32 bytes, for AES-128, -192 or -256, as far as the driver takes them. InvalidLength
    // otherwise, and then there's no key.
    ReturnCode SetKey(Span<const uint8_t> key)
    {
        ClearKey();

        if(key.GetLength() != 16 && key.GetLength() != 24 && key.GetLength() != 32)
        {
            return ReturnCode::InvalidLength;
        }

        key.CopyTo(_key);
        _keyLength = key.GetLength();

        return ReturnCode::Success;
    }

    void ClearKey()
    {
        // Through volatile, so the stores aren't dropped from an object that's about to die.
        for(auto& value : _key)
        {
            static_cast<volatile uint8_t&>(value) = 0;
        }

        _keyLength = 0;
    }

    // Encrypts `plaintext` into `ciphertext`, and writes the tag that authenticates both it and
    // `associatedData`. `ciphertext` may be `plaintext` itself, to seal in place.
    // - InvalidState without a key, or if the device isn't ready.
    // - InvalidLength if `ciphertext` is shorter than `plaintext`, or `tag` isn't TagLength.
    // - InvalidArgument if `ciphertext` overlaps `plaintext` without starting where it does, or overlaps `tag`.
    // - NotSupported to seal in place where the driver can't.
    // - Otherwise as the driver says: mbedTLS's takes only 16-byte keys, say.
    ReturnCode Seal(Nonce nonce, Span<const uint8_t> associatedData, Span<const uint8_t> plaintext,
        Span<uint8_t> ciphertext, Span<uint8_t> tag) const
    {
        auto rc = Check(plaintext, ciphertext, tag);
        CHECK_RETURN_CODE(rc);

        return Run(CRYPTO_CIPHER_OP_ENCRYPT, nonce, associatedData, plaintext, ciphertext.Take(plaintext.GetLength()),
            tag);
    }

    // Decrypts `ciphertext` into `plaintext`, if `tag` authenticates it and `associatedData`.
    // `plaintext` may be `ciphertext` itself, to open in place.
    // - InvalidData if the tag doesn't match: the driver says -EFAULT, as mbedTLS's does.
    // - Otherwise as Seal.
    // Once decryption has begun, a failure zeroes `plaintext`: what came out is unauthenticated.
    ReturnCode Open(Nonce nonce, Span<const uint8_t> associatedData, Span<const uint8_t> ciphertext,
        Span<const uint8_t> tag, Span<uint8_t> plaintext) const
    {
        auto rc = Check(ciphertext, plaintext, tag);
        CHECK_RETURN_CODE(rc);

        return Run(CRYPTO_CIPHER_OP_DECRYPT, nonce, associatedData, ciphertext, plaintext.Take(ciphertext.GetLength()),
            tag);
    }

private:
    static constexpr uint32_t MaxKeyLength = 32;

    // The packet's lengths are ints.
    static constexpr uint32_t MaxLength = INT32_MAX;

    Array<uint8_t, MaxKeyLength> _key;
    uint32_t _keyLength = 0;

    // What's refused before the driver is asked.
    ReturnCode Check(Span<const uint8_t> input, Span<const uint8_t> output, Span<const uint8_t> tag) const
    {
        if(_keyLength == 0)
        {
            return ReturnCode::InvalidState;
        }

        if(output.GetLength() < input.GetLength() || tag.GetLength() != TagLength || input.GetLength() > MaxLength)
        {
            return ReturnCode::InvalidLength;
        }

        // In place is the same bytes; bytes shifted from them would read back blocks already written. Spans
        // of one byte overlap only where they start at the same place.
        output = output.Take(input.GetLength());

        if(input.Overlaps(output) && !input.Take(1).Overlaps(output.Take(1)))
        {
            return ReturnCode::InvalidArgument;
        }

        // Sealing, the tag would be written over ciphertext; opening, plaintext would be written over the
        // tag before it's compared. (An empty span overlaps anything it starts inside.)
        if(!output.IsEmpty() && tag.Overlaps(output))
        {
            return ReturnCode::InvalidArgument;
        }

        return ReturnCode::Success;
    }

    // `output` is as long as `input`. Sealing, the driver writes `tag`; opening, it checks it.
    ReturnCode Run(cipher_op operation, Nonce nonce, Span<const uint8_t> associatedData, Span<const uint8_t> input,
        Span<uint8_t> output, Span<const uint8_t> tag) const
    {
        auto rc = FailIfNotReady();
        CHECK_RETURN_CODE(rc);

        bool inPlace = !input.IsEmpty() && input.Overlaps(output);
        auto capabilities = static_cast<uint32_t>(crypto_query_hwcaps(GetDevice()));

        if(inPlace && (capabilities & CAP_INPLACE_OPS) == 0)
        {
            return ReturnCode::NotSupported;
        }

        // The session only points at the key, which outlives it.
        struct cipher_ctx context = {};
        context.key.bit_stream = _key.GetData();
        context.keylen = static_cast<uint16_t>(_keyLength);
        context.flags = CAP_RAW_KEY | CAP_SYNC_OPS | (inPlace ? CAP_INPLACE_OPS : CAP_SEPARATE_IO_BUFS);
        context.mode_params.gcm_info.nonce_len = NonceLength;
        context.mode_params.gcm_info.tag_len = TagLength;

        auto err = cipher_begin_session(GetDevice(), &context, CRYPTO_CIPHER_ALGO_AES, CRYPTO_CIPHER_MODE_GCM, operation);
        rc = Convert(err);
        CHECK_RETURN_CODE(rc);

        // Zephyr's packets aren't const-correct. The driver only reads the input, associated data and
        // nonce, and the tag when opening; it writes the tag when sealing, which Seal passes writable.
        struct cipher_pkt packet = {};
        packet.in_buf = const_cast<uint8_t*>(input.GetData());
        packet.in_len = static_cast<int>(input.GetLength());
        packet.out_buf = output.GetData();
        packet.out_buf_max = static_cast<int>(output.GetLength());

        struct cipher_aead_pkt aead = {};
        aead.pkt = &packet;
        aead.ad = const_cast<uint8_t*>(associatedData.GetData());
        aead.ad_len = associatedData.GetLength();
        aead.tag = const_cast<uint8_t*>(tag.GetData());

        err = cipher_gcm_op(&context, &aead, const_cast<uint8_t*>(nonce.GetData()));
        cipher_free_session(GetDevice(), &context);

        if(err < 0 && operation == CRYPTO_CIPHER_OP_DECRYPT)
        {
            // Not every driver clears what it decrypted before finding the tag wrong.
            output.Fill(0);
            return err == -EFAULT ? ReturnCode::InvalidData : Convert(err);
        }

        return Convert(err);
    }

    static ReturnCode Convert(int err)
    {
        return err == -ENOTSUP ? ReturnCode::NotSupported : ErrorConverter::Convert(err);
    }
};
