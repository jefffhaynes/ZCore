#pragma once

#include <stdint.h>
#include "Array.h"
#include "Clock.h"
#include "CriticalSection.h"
#include "ErrorConverter.h"
#include "FixedSpan.h"
#include "Lock.h"
#include "Mutex.h"
#include "ReturnCode.h"
#include "Span.h"
#include "SpanReader.h"
#include "SpanWriter.h"

#if !defined(CONFIG_SOC_SERIES_STM32N6X)
#error "AesGcm.h drives the STM32N6's CRYP; this SoC has no AES-GCM hardware it supports"
#endif

// For the CRYP's register map. Nothing here calls ST's HAL or LL.
#include <soc.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/clock_control/stm32_clock_control.h>

#if !defined(CRYP) || !defined(CRYP_CR_NPBLB)
#error "AesGcm.h needs a CRYP with GCM and NPBLB, which this STM32N6 doesn't have"
#endif

// AES-GCM (NIST SP 800-38D) on the SoC's own hardware: the STM32N6's CRYP, through its registers.
//
// It takes what TLS's AES-GCM suites use (RFC 5288, RFC 8446): 96-bit nonces and whole 128-bit tags.
// A record seals in place, with its header as the associated data and the tag following it.
//
// Each instance holds a key. There's one CRYP: an operation has it to itself and loads its own key,
// so instances for either direction of a connection can be used from any thread. Don't change a key
// while an operation is using it.
//
// A nonce must never repeat under one key. Two messages sealed with the same one give away the XOR of
// their plaintexts, and enough to forge tags.
class AesGcm
{
public:
    static constexpr uint32_t NonceLength = 12;
    static constexpr uint32_t TagLength = 16;

    using Nonce = FixedSpan<const uint8_t, NonceLength>;

    AesGcm() = default;

    // A copy would be one more place the key has to be wiped from.
    AesGcm(const AesGcm&) = delete;
    AesGcm& operator=(const AesGcm&) = delete;

    ~AesGcm()
    {
        ClearKey();
    }

    // 16, 24 or 32 bytes, for AES-128, -192 or -256. InvalidLength otherwise, and then there's no key.
    ReturnCode SetKey(Span<const uint8_t> key)
    {
        ClearKey();

        uint32_t keySize = 0;

        switch(key.GetLength())
        {
            case 16: keySize = 0; break;
            case 24: keySize = CRYP_CR_KEYSIZE_0; break;
            case 32: keySize = CRYP_CR_KEYSIZE_1; break;
            default: return ReturnCode::InvalidLength;
        }

        // The registers take the key's words most significant byte first, its last word in K3RR.
        SpanReader<const uint8_t> reader(key, Endianness::BigEndian);

        for(auto& word : _key.Skip(_key.GetLength() - key.GetLength() / sizeof(uint32_t)))
        {
            reader.Read(word);
        }

        _keySize = keySize;
        _hasKey = true;

        return ReturnCode::Success;
    }

    void ClearKey()
    {
        // Through volatile, so the stores aren't dropped from an object that's about to die.
        for(auto& word : _key)
        {
            static_cast<volatile uint32_t&>(word) = 0;
        }

        _hasKey = false;
    }

    // Encrypts `plaintext` into `ciphertext`, and writes the tag that authenticates both it and
    // `associatedData`. `ciphertext` may be `plaintext` itself, to seal in place.
    // - InvalidState without a key.
    // - InvalidLength if `ciphertext` is shorter than `plaintext`, or `tag` isn't TagLength.
    // - InvalidArgument if `ciphertext` overlaps `plaintext` without starting where it does, or overlaps `tag`.
    // - Timeout if the hardware stops answering.
    ReturnCode Seal(Nonce nonce, Span<const uint8_t> associatedData, Span<const uint8_t> plaintext,
        Span<uint8_t> ciphertext, Span<uint8_t> tag) const
    {
        auto rc = Check(plaintext, ciphertext, tag);
        CHECK_RETURN_CODE(rc);

        return Run(Direction::Encrypt, nonce, associatedData, plaintext, ciphertext.Take(plaintext.GetLength()), tag);
    }

    // Decrypts `ciphertext` into `plaintext`, if `tag` authenticates it and `associatedData`.
    // `plaintext` may be `ciphertext` itself, to open in place.
    // - InvalidData if the tag doesn't match.
    // - Otherwise as Seal.
    // Once decryption has begun, a failure zeroes `plaintext`: what came out is unauthenticated.
    ReturnCode Open(Nonce nonce, Span<const uint8_t> associatedData, Span<const uint8_t> ciphertext,
        Span<const uint8_t> tag, Span<uint8_t> plaintext) const
    {
        auto rc = Check(ciphertext, plaintext, tag);
        CHECK_RETURN_CODE(rc);

        plaintext = plaintext.Take(ciphertext.GetLength());

        Array<uint8_t, TagLength> computed;
        rc = Run(Direction::Decrypt, nonce, associatedData, ciphertext, plaintext, computed);

        if(rc == ReturnCode::Success && !Matches(computed, tag))
        {
            rc = ReturnCode::InvalidData;
        }

        if(rc != ReturnCode::Success)
        {
            plaintext.Fill(0);
        }

        return rc;
    }

private:
    static constexpr uint32_t BlockLength = 16;
    static constexpr uint32_t MaxKeyLength = 32;
    static constexpr uint32_t BitsPerByte = 8;

    // The CRYP counts blocks from 2, keeping 1 for the tag (SP 800-38D's J0).
    static constexpr uint32_t FirstCounter = 2;

    // Each wait is for one block, a few hundred cycles at most; this much longer means it's stuck.
    static constexpr TimeSpan Timeout = TimeSpan::FromMilliseconds(10);

    enum class Direction : uint32_t
    {
        Encrypt = 0,
        Decrypt = CRYP_CR_ALGODIR
    };

    struct Phase
    {
        static constexpr uint32_t Init = 0;
        static constexpr uint32_t Header = CRYP_CR_GCM_CCMPH_0;
        static constexpr uint32_t Payload = CRYP_CR_GCM_CCMPH_1;
        static constexpr uint32_t Final = CRYP_CR_GCM_CCMPH;
    };

    Array<uint32_t, MaxKeyLength / sizeof(uint32_t)> _key;
    uint32_t _keySize = 0;
    bool _hasKey = false;

    static inline Mutex _crypMutex;

    // What's refused before the CRYP is touched.
    ReturnCode Check(Span<const uint8_t> input, Span<const uint8_t> output, Span<const uint8_t> tag) const
    {
        if(!_hasKey)
        {
            return ReturnCode::InvalidState;
        }

        if(output.GetLength() < input.GetLength() || tag.GetLength() != TagLength)
        {
            return ReturnCode::InvalidLength;
        }

        // Each block is read whole before its result is written, so the same bytes are fine, but bytes
        // shifted from them would read back blocks already written. Spans of one byte overlap only where
        // they start at the same place.
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

    // Every byte, whatever the first difference, so the time taken doesn't say where it was.
    static bool Matches(Span<const uint8_t> computed, Span<const uint8_t> tag)
    {
        uint8_t difference = 0;
        uint32_t index = 0;

        for(auto value : computed)
        {
            uint8_t expected = 0;
            tag.Get(index++, expected);
            difference |= value ^ expected;
        }

        return difference == 0;
    }

    // `output` is as long as `input`.
    ReturnCode Run(Direction direction, Nonce nonce, Span<const uint8_t> associatedData, Span<const uint8_t> input,
        Span<uint8_t> output, Span<uint8_t> tag) const
    {
        Lock lock(_crypMutex);

        auto rc = TurnOn();
        CHECK_RETURN_CODE(rc);

        // One CRYP, named by the device header for the security state we're built for.
        auto& cryp = *CRYP;

        // From reset each time, whatever the last operation left, and back to it after, so no key stays.
        Reset(cryp);
        rc = Process(cryp, direction, nonce, associatedData, input, output, tag);
        Reset(cryp);

        return rc;
    }

    // GCM's init, header, payload and final phases.
    ReturnCode Process(CRYP_TypeDef& cryp, Direction direction, Nonce nonce, Span<const uint8_t> associatedData,
        Span<const uint8_t> input, Span<uint8_t> output, Span<uint8_t> tag) const
    {
        // Bytes go in and come out as big-endian words, so the CRYP doesn't swap them.
        uint32_t control = CRYP_CR_ALGOMODE_AES_GCM | _keySize | static_cast<uint32_t>(direction);

        cryp.CR = control | Phase::Init;

        LoadKey(cryp);
        LoadCounter(cryp, nonce);

        // The CRYP derives the hash key, then stops itself.
        cryp.CR = control | Phase::Init | CRYP_CR_CRYPEN;

        auto rc = WaitFor([&cryp] { return (cryp.CR & CRYP_CR_CRYPEN) == 0; });
        CHECK_RETURN_CODE(rc);

        rc = Begin(cryp, control | Phase::Header);
        CHECK_RETURN_CODE(rc);

        for(auto remaining = associatedData; !remaining.IsEmpty(); remaining = remaining.Skip(BlockLength))
        {
            WriteBlock(cryp, remaining.Take(BlockLength));

            rc = WaitFor([&cryp] { return (cryp.SR & CRYP_SR_IFEM) != 0; });
            CHECK_RETURN_CODE(rc);
        }

        rc = Begin(cryp, control | Phase::Payload);
        CHECK_RETURN_CODE(rc);

        auto destination = output;

        for(auto remaining = input; !remaining.IsEmpty(); remaining = remaining.Skip(BlockLength))
        {
            auto block = remaining.Take(BlockLength);

            // Padding that's encrypted would be hashed as ciphertext unless the CRYP is told it's there.
            // Decrypting, the padding is the ciphertext, and zeros hash as they should.
            if(block.GetLength() < BlockLength && direction == Direction::Encrypt)
            {
                control |= (BlockLength - block.GetLength()) << CRYP_CR_NPBLB_Pos;

                rc = Begin(cryp, control | Phase::Payload);
                CHECK_RETURN_CODE(rc);
            }

            WriteBlock(cryp, block);

            rc = WaitFor([&cryp] { return (cryp.SR & CRYP_SR_OFNE) != 0; });
            CHECK_RETURN_CODE(rc);

            ReadBlock(cryp, destination.Take(block.GetLength()));
            destination = destination.Skip(block.GetLength());
        }

        // The final phase only encrypts, whichever way the payload went.
        rc = Begin(cryp, (control & ~static_cast<uint32_t>(CRYP_CR_ALGODIR)) | Phase::Final);
        CHECK_RETURN_CODE(rc);

        Array<uint8_t, BlockLength> lengths;
        SpanWriter<uint8_t> writer(lengths, Endianness::BigEndian);
        writer.Write(static_cast<uint64_t>(associatedData.GetLength()) * BitsPerByte);
        writer.Write(static_cast<uint64_t>(input.GetLength()) * BitsPerByte);

        WriteBlock(cryp, lengths.AsSpan());

        rc = WaitFor([&cryp] { return (cryp.SR & CRYP_SR_OFNE) != 0; });
        CHECK_RETURN_CODE(rc);

        ReadBlock(cryp, tag);

        return WaitFor([&cryp] { return (cryp.SR & CRYP_SR_BUSY) == 0; });
    }

    static void Reset(CRYP_TypeDef& cryp)
    {
        cryp.CR = CRYP_CR_IPRST;
        cryp.CR = 0;
    }

    // Once the CRYP has finished what it was given: stopped, then the new phase, then started again,
    // each its own write.
    static ReturnCode Begin(CRYP_TypeDef& cryp, uint32_t control)
    {
        auto rc = WaitFor([&cryp] { return (cryp.SR & CRYP_SR_BUSY) == 0; });
        CHECK_RETURN_CODE(rc);

        uint32_t running = cryp.CR;
        cryp.CR = running & ~static_cast<uint32_t>(CRYP_CR_CRYPEN);
        cryp.CR = control;
        cryp.CR = control | CRYP_CR_CRYPEN;

        return ReturnCode::Success;
    }

    void LoadKey(CRYP_TypeDef& cryp) const
    {
        cryp.K0LR = _key.Get<0>();
        cryp.K0RR = _key.Get<1>();
        cryp.K1LR = _key.Get<2>();
        cryp.K1RR = _key.Get<3>();
        cryp.K2LR = _key.Get<4>();
        cryp.K2RR = _key.Get<5>();
        cryp.K3LR = _key.Get<6>();
        cryp.K3RR = _key.Get<7>();
    }

    // The nonce, then the counter for the first block.
    static void LoadCounter(CRYP_TypeDef& cryp, Nonce nonce)
    {
        SpanReader<const uint8_t> reader(nonce.AsSpan(), Endianness::BigEndian);
        uint32_t word = 0;

        reader.Read(word);
        cryp.IV0LR = word;

        reader.Read(word);
        cryp.IV0RR = word;

        reader.Read(word);
        cryp.IV1LR = word;

        cryp.IV1RR = FirstCounter;
    }

    // Up to a block, zero-padded.
    static void WriteBlock(CRYP_TypeDef& cryp, Span<const uint8_t> data)
    {
        Array<uint8_t, BlockLength> block;
        data.Take(BlockLength).CopyTo(block);

        SpanReader<const uint8_t> reader(block.AsSpan(), Endianness::BigEndian);

        for(uint32_t i = 0; i < BlockLength / sizeof(uint32_t); i++)
        {
            uint32_t word = 0;
            reader.Read(word);
            cryp.DIN = word;
        }
    }

    // A whole block comes out; `destination` gets as much of it as it holds.
    static void ReadBlock(CRYP_TypeDef& cryp, Span<uint8_t> destination)
    {
        Array<uint8_t, BlockLength> block;
        SpanWriter<uint8_t> writer(block, Endianness::BigEndian);

        for(uint32_t i = 0; i < BlockLength / sizeof(uint32_t); i++)
        {
            uint32_t word = cryp.DOUT;
            writer.Write(word);
        }

        block.Take(destination.GetLength()).CopyTo(destination);
    }

    template<typename TCondition>
    static ReturnCode WaitFor(TCondition condition)
    {
        // Usually it's done before we look.
        if(condition())
        {
            return ReturnCode::Success;
        }

        auto deadline = Clock::GetUptime() + Timeout;

        while(!condition())
        {
            if(Clock::GetUptime() > deadline)
            {
                return condition() ? ReturnCode::Success : ReturnCode::Timeout;
            }
        }

        return ReturnCode::Success;
    }

    static ReturnCode TurnOn()
    {
        // Zephyr's devicetree has no CRYP node for the N6, so its clock is named here.
        stm32_pclken clock = {};
        clock.bus = STM32_CLOCK_BUS_AHB3;
        clock.enr = RCC_AHB3ENR_CRYPEN;

        // Zephyr sets the bit with a read-modify-write of AHB3ENR, which the RNG's clock shares, and the
        // RNG driver turns that on and off as it runs. Nothing may come between our read and our write.
        CriticalSection criticalSection;

        auto err = clock_control_on(DEVICE_DT_GET(STM32_CLOCK_CONTROL_NODE), static_cast<clock_control_subsys_t>(&clock));
        return ErrorConverter::Convert(err);
    }
};
