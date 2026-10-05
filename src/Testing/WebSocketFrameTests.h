#pragma once

#include "WebSocketFrame.h"
#include "CoreString.h"

namespace WebSocketFrameTests
{
    constexpr uint8_t mask[] = { 0x37, 0xFA, 0x21, 0x3D };
    constexpr auto Mask = WebSocketFrame::Mask::FromArray(mask);

    constexpr bool WritesHeader(WebSocketOpcode opcode, uint64_t length, Span<const uint8_t> expected)
    {
        Array<uint8_t, WebSocketFrame::MaxHeaderLength> header;
        uint32_t written = 0;

        return WebSocketFrame::WriteHeader(header, opcode, length, Mask, written) == ReturnCode::Success
            && Span<const uint8_t>(header.Take(written)).SequenceEquals(expected);
    }

    // RFC 6455's masked "Hello".
    static_assert([]{
        constexpr uint8_t expected[] = { 0x81, 0x85, 0x37, 0xFA, 0x21, 0x3D };
        return WritesHeader(WebSocketOpcode::Text, 5, expected);
    }(), "A short header failed");

    static_assert([]{
        constexpr uint8_t hello[] = { 'H', 'e', 'l', 'l', 'o' };
        constexpr uint8_t expected[] = { 0x7F, 0x9F, 0x4D, 0x51, 0x58 };
        Array<uint8_t, 5> masked;

        return WebSocketFrame::ApplyMask(hello, masked, Mask, 0) == ReturnCode::Success && masked.SequenceEquals(expected);
    }(), "Masking failed");

    static_assert([]{
        constexpr uint8_t tail[] = { 'l', 'o' };
        constexpr uint8_t expected[] = { 0x51, 0x58 };
        Array<uint8_t, 2> masked;

        return WebSocketFrame::ApplyMask(tail, masked, Mask, 3) == ReturnCode::Success && masked.SequenceEquals(expected);
    }(), "Masking part-way into a payload failed");

    static_assert([]{
        constexpr uint8_t source[] = { 1, 2, 3 };
        Array<uint8_t, 2> masked;

        return WebSocketFrame::ApplyMask(source, masked, Mask, 0) == ReturnCode::InvalidLength;
    }(), "Masking into too short a destination wasn't refused");

    static_assert([]{
        constexpr uint8_t expected[] = { 0x82, 0xFD, 0x37, 0xFA, 0x21, 0x3D };
        return WritesHeader(WebSocketOpcode::Binary, 125, expected);
    }(), "The longest short length failed");

    static_assert([]{
        constexpr uint8_t expected[] = { 0x82, 0xFE, 0x00, 0x7E, 0x37, 0xFA, 0x21, 0x3D };
        return WritesHeader(WebSocketOpcode::Binary, 126, expected);
    }(), "The shortest 16-bit length failed");

    static_assert([]{
        constexpr uint8_t expected[] = { 0x82, 0xFE, 0xFF, 0xFF, 0x37, 0xFA, 0x21, 0x3D };
        return WritesHeader(WebSocketOpcode::Binary, 65535, expected);
    }(), "The longest 16-bit length failed");

    static_assert([]{
        constexpr uint8_t expected[] = { 0x82, 0xFF, 0, 0, 0, 0, 0, 1, 0, 0, 0x37, 0xFA, 0x21, 0x3D };
        return WritesHeader(WebSocketOpcode::Binary, 65536, expected);
    }(), "A 64-bit length failed");

    static_assert([]{
        Array<uint8_t, 5> header;
        uint32_t written = 1;

        return WebSocketFrame::WriteHeader(header, WebSocketOpcode::Binary, 5, Mask, written) == ReturnCode::InvalidLength
            && written == 0;
    }(), "A header that doesn't fit wasn't refused");

    constexpr bool Reads(Span<const uint8_t> frame, uint32_t extension, WebSocketOpcode opcode, bool isFinal, bool isMasked,
        uint64_t length)
    {
        uint32_t extensionLength = 0;
        WebSocketFrame::Header header;

        return WebSocketFrame::GetExtensionLength(frame, extensionLength) == ReturnCode::Success
            && extensionLength == extension
            && WebSocketFrame::ReadHeader(frame, header) == ReturnCode::Success
            && header.Opcode == opcode && header.IsFinal == isFinal && header.IsMasked == isMasked && header.Length == length;
    }

    constexpr bool IsRejected(Span<const uint8_t> frame, ReturnCode expected)
    {
        WebSocketFrame::Header header;
        return WebSocketFrame::ReadHeader(frame, header) == expected;
    }

    static_assert([]{
        constexpr uint8_t frame[] = { 0x82, 0x05 };
        return Reads(frame, 0, WebSocketOpcode::Binary, true, false, 5);
    }(), "Reading a short header failed");

    static_assert([]{
        constexpr uint8_t frame[] = { 0x82, 0x7E, 0x01, 0x00 };
        return Reads(frame, 2, WebSocketOpcode::Binary, true, false, 256);
    }(), "Reading a 16-bit length failed");

    static_assert([]{
        constexpr uint8_t frame[] = { 0x02, 0x7F, 0, 0, 0, 1, 0, 0, 0, 2 };
        return Reads(frame, 8, WebSocketOpcode::Binary, false, false, 0x100000002);
    }(), "Reading a 64-bit length of a frame that isn't final failed");

    static_assert([]{
        constexpr uint8_t frame[] = { 0x81, 0x85, 0x37, 0xFA, 0x21, 0x3D };
        return Reads(frame, 4, WebSocketOpcode::Text, true, true, 5);
    }(), "Reading a masked header failed");

    static_assert([]{
        constexpr uint8_t frame[] = { 0x89, 0x7D };
        return Reads(frame, 0, WebSocketOpcode::Ping, true, false, 125) && WebSocketFrame::IsControl(WebSocketOpcode::Ping)
            && WebSocketFrame::IsControl(WebSocketOpcode::Close) && !WebSocketFrame::IsControl(WebSocketOpcode::Binary);
    }(), "Reading a control frame failed");

    static_assert([]{
        constexpr uint8_t reserved[] = { 0xC2, 0x05 };
        constexpr uint8_t fragmentedPing[] = { 0x09, 0x00 };
        constexpr uint8_t longPing[] = { 0x89, 0x7E, 0x00, 0x7E };
        constexpr uint8_t cutShort[] = { 0x82, 0x7E, 0x01 };

        return IsRejected(reserved, ReturnCode::InvalidData) && IsRejected(fragmentedPing, ReturnCode::InvalidData)
            && IsRejected(longPing, ReturnCode::InvalidData) && !IsRejected(cutShort, ReturnCode::Success);
    }(), "A header the protocol forbids wasn't refused");

    // RFC 6455's handshake example.
    static_assert([]{
        constexpr uint8_t nonce[] = { 't', 'h', 'e', ' ', 's', 'a', 'm', 'p', 'l', 'e', ' ', 'n', 'o', 'n', 'c', 'e' };

        auto key = WebSocketFrame::GetKey(FixedSpan<const uint8_t, WebSocketFrame::NonceLength>::FromArray(nonce));
        auto accept = WebSocketFrame::GetAccept(key);

        return String(key.AsSpan()) == String("dGhlIHNhbXBsZSBub25jZQ==")
            && String(accept.AsSpan()) == String("s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
    }(), "The handshake key or its accept failed");
}
