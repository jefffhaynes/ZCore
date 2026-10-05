#pragma once

#include <stdint.h>
#include "Array.h"
#include "Base64.h"
#include "CoreString.h"
#include "FixedSpan.h"
#include "Sha1.h"
#include "Span.h"
#include "SpanReader.h"
#include "SpanWriter.h"
#include "ReturnCode.h"

enum class WebSocketOpcode : uint8_t
{
    Continuation = 0x0,
    Text = 0x1,
    Binary = 0x2,
    Close = 0x8,
    Ping = 0x9,
    Pong = 0xA
};

// RFC 6455's framing and handshake key, apart from any connection.
class WebSocketFrame
{
public:
    static constexpr uint32_t StartLength = 2;
    static constexpr uint32_t MaskLength = 4;
    static constexpr uint32_t MaxHeaderLength = StartLength + sizeof(uint64_t) + MaskLength;
    static constexpr uint32_t MaxControlLength = 125;
    static constexpr uint32_t NonceLength = 16;
    static constexpr uint32_t KeyLength = 24;
    static constexpr uint32_t AcceptLength = 28;

    using Mask = FixedSpan<const uint8_t, MaskLength>;

    struct Header
    {
        WebSocketOpcode Opcode = WebSocketOpcode::Continuation;
        bool IsFinal = false;
        bool IsMasked = false;
        uint64_t Length = 0;
    };

    static constexpr bool IsControl(WebSocketOpcode opcode)
    {
        return (static_cast<uint8_t>(opcode) & ControlBit) != 0;
    }

    // The header of a whole message as a client sends it: final and masked.
    static constexpr ReturnCode WriteHeader(Span<uint8_t> destination, WebSocketOpcode opcode, uint64_t length, Mask mask,
        uint32_t& written)
    {
        SpanWriter<uint8_t> writer(destination, Endianness::BigEndian);

        written = 0;

        auto rc = writer.Write(static_cast<uint8_t>(FinalBit | static_cast<uint8_t>(opcode)));
        CHECK_RETURN_CODE(rc);

        if(length < Length16)
        {
            rc = writer.Write(static_cast<uint8_t>(MaskBit | length));
            CHECK_RETURN_CODE(rc);
        }
        else if(length <= UINT16_MAX)
        {
            rc = writer.Write(static_cast<uint8_t>(MaskBit | Length16));
            CHECK_RETURN_CODE(rc);

            rc = writer.Write(static_cast<uint16_t>(length));
            CHECK_RETURN_CODE(rc);
        }
        else
        {
            rc = writer.Write(static_cast<uint8_t>(MaskBit | Length64));
            CHECK_RETURN_CODE(rc);

            rc = writer.Write(length);
            CHECK_RETURN_CODE(rc);
        }

        rc = writer.Write(mask.AsSpan());
        CHECK_RETURN_CODE(rc);

        written = writer.GetWritten();

        return ReturnCode::Success;
    }

    // How much header follows a frame's first two bytes.
    static constexpr ReturnCode GetExtensionLength(Span<const uint8_t> start, uint32_t& length)
    {
        uint8_t second = 0;
        auto rc = start.Get(1, second);
        CHECK_RETURN_CODE(rc);

        auto short7 = second & LengthMask;

        length = short7 == Length16 ? sizeof(uint16_t) : short7 == Length64 ? sizeof(uint64_t) : 0;
        length += (second & MaskBit) != 0 ? MaskLength : 0;

        return ReturnCode::Success;
    }

    // `header` is a frame's first two bytes and their extension. InvalidData for one this protocol forbids.
    static constexpr ReturnCode ReadHeader(Span<const uint8_t> header, Header& result)
    {
        SpanReader<const uint8_t> reader(header, Endianness::BigEndian);
        uint8_t first = 0;
        uint8_t second = 0;

        auto rc = reader.Read(first);
        CHECK_RETURN_CODE(rc);

        rc = reader.Read(second);
        CHECK_RETURN_CODE(rc);

        result.Opcode = static_cast<WebSocketOpcode>(first & OpcodeMask);
        result.IsFinal = (first & FinalBit) != 0;
        result.IsMasked = (second & MaskBit) != 0;
        result.Length = second & LengthMask;

        if(result.Length == Length16)
        {
            uint16_t length = 0;
            rc = reader.Read(length);
            CHECK_RETURN_CODE(rc);

            result.Length = length;
        }
        else if(result.Length == Length64)
        {
            rc = reader.Read(result.Length);
            CHECK_RETURN_CODE(rc);
        }

        auto isValid = (first & ReservedBits) == 0
            && (!IsControl(result.Opcode) || (result.IsFinal && result.Length <= MaxControlLength));

        return isValid ? ReturnCode::Success : ReturnCode::InvalidData;
    }

    // Masks or unmasks, which are the same. `offset` is how far into the frame's payload `source` starts.
    static constexpr ReturnCode ApplyMask(Span<const uint8_t> source, Span<uint8_t> destination, Mask mask, uint64_t offset)
    {
        SpanWriter<uint8_t> writer(destination);

        for(auto value : source)
        {
            uint8_t key = 0;

            auto rc = mask.Get(offset % MaskLength, key);
            CHECK_RETURN_CODE(rc);

            rc = writer.Write(static_cast<uint8_t>(value ^ key));
            CHECK_RETURN_CODE(rc);

            offset++;
        }

        return ReturnCode::Success;
    }

    // The Sec-WebSocket-Key a client sends, from 16 random bytes.
    static constexpr Array<char, KeyLength> GetKey(FixedSpan<const uint8_t, NonceLength> nonce)
    {
        Array<char, KeyLength> key;
        uint32_t written = 0;

        Base64::Encode(nonce.AsSpan(), key, written);

        return key;
    }

    // The Sec-WebSocket-Accept a server proves itself with, in answer to `key`.
    static constexpr Array<char, AcceptLength> GetAccept(FixedSpan<const char, KeyLength> key)
    {
        constexpr String Guid("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

        Array<uint8_t, KeyLength + GuidLength> challenge;

        key.AsSpan().template CopyTo<uint8_t>(challenge.Take(KeyLength), ToByte);
        Guid.template CopyTo<uint8_t>(challenge.Skip(KeyLength), ToByte);

        auto hash = Sha1::Compute(challenge);

        Array<char, AcceptLength> accept;
        uint32_t written = 0;

        Base64::Encode(hash, accept, written);

        return accept;
    }

private:
    static constexpr uint8_t FinalBit = 0x80;
    static constexpr uint8_t ReservedBits = 0x70;
    static constexpr uint8_t OpcodeMask = 0x0F;
    static constexpr uint8_t ControlBit = 0x08;
    static constexpr uint8_t MaskBit = 0x80;
    static constexpr uint8_t LengthMask = 0x7F;
    static constexpr uint8_t Length16 = 126;
    static constexpr uint8_t Length64 = 127;
    static constexpr uint32_t GuidLength = 36;

    static constexpr uint8_t ToByte(char value)
    {
        return static_cast<uint8_t>(value);
    }
};
