#pragma once

#include <zephyr/random/random.h>
#include "Array.h"
#include "Clock.h"
#include "CoreString.h"
#include "Lock.h"
#include "Mutex.h"
#include "StringParser.h"
#include "TcpClient.h"
#include "WebSocketFrame.h"

// A WebSocket client (RFC 6455) over TcpClient, for messages sent and received whole.
//
// Why not Zephyr's (CONFIG_WEBSOCKET_CLIENT): that one copies every message into heap memory to mask
// it, and pulls in mbedTLS for the handshake's SHA-1. This one masks through a segment-sized buffer on
// its way to the socket, allocates nothing, and turns the socket's send delay off.
//
// One thread may send while another receives. Call Connect and Close with neither under way.
class WebSocketClient
{
public:
    static constexpr uint16_t SwitchingProtocols = 101;

    // `timeout` bounds the handshake, and each frame once it has started to arrive or to leave.
    explicit WebSocketClient(TimeSpan timeout = TimeSpan::FromSeconds(10)) : _tcp(timeout), _timeout(timeout)
    {
    }

    // False once the connection has failed or the server has closed it, until the next Connect.
    bool IsOpen() const
    {
        return _tcp.IsOpen() && !_isBroken;
    }

    // The server's HTTP status from the last Connect, or 0 if it didn't answer.
    uint16_t GetStatus() const
    {
        return _status;
    }

    // `headers` are further request lines, each ending in CRLF. InvalidOperation if the server answers
    // with anything but 101, and InvalidData if what it answers isn't a WebSocket handshake.
    ReturnCode Connect(IPEndPoint endPoint, String host, String path, String headers = String())
    {
        Close();
        _status = 0;

        auto rc = _tcp.Connect(endPoint);
        CHECK_RETURN_CODE(rc);

        rc = Upgrade(host, path, headers);

        if(rc != ReturnCode::Success)
        {
            _tcp.Close();
        }

        return rc;
    }

    // Says goodbye and drops the connection, without waiting for the server's own goodbye.
    ReturnCode Close()
    {
        if(IsOpen())
        {
            Send(Span<const uint8_t>(), WebSocketOpcode::Close);
        }

        _isBroken = false;

        return _tcp.Close();
    }

    ReturnCode Send(Span<const uint8_t> message, WebSocketOpcode opcode = WebSocketOpcode::Binary)
    {
        return Send(message, Span<const uint8_t>(), opcode);
    }

    // One message from two parts, such as a header and what it describes.
    ReturnCode Send(Span<const uint8_t> head, Span<const uint8_t> body, WebSocketOpcode opcode = WebSocketOpcode::Binary)
    {
        Lock lock(_sendMutex);

        if(!IsOpen())
        {
            return ReturnCode::InvalidState;
        }

        auto rc = SendFrame(head, body, opcode);

        // Part of a frame may have gone, and nothing sent after that would make sense to the server.
        return rc == ReturnCode::Success ? rc : Break(rc);
    }

    // The next message. Pings are answered and pongs skipped on the way.
    // - Timeout if none starts to arrive in time.
    // - InvalidLength, with the message skipped, if `buffer` can't hold it.
    // - A Close comes back as a message, and is the last.
    ReturnCode Receive(Span<uint8_t> buffer, uint32_t& length, WebSocketOpcode& opcode, TimeSpan timeout)
    {
        length = 0;

        while(true)
        {
            if(!IsOpen())
            {
                return ReturnCode::InvalidState;
            }

            auto rc = _tcp.WaitForData(timeout);
            CHECK_RETURN_CODE(rc);

            WebSocketFrame::Header header;
            rc = ReadHeader(header);

            if(rc != ReturnCode::Success)
            {
                return Break(rc);
            }

            opcode = header.Opcode;

            if(WebSocketFrame::IsControl(opcode))
            {
                Array<uint8_t, WebSocketFrame::MaxControlLength> control;
                auto payload = control.Take(static_cast<uint32_t>(header.Length));

                rc = _tcp.Read(payload, _timeout);

                if(rc != ReturnCode::Success)
                {
                    return Break(rc);
                }

                if(opcode == WebSocketOpcode::Close)
                {
                    // Echoing the status code completes the closing handshake.
                    Send(payload.Take(sizeof(uint16_t)), WebSocketOpcode::Close);
                    _isBroken = true;

                    return ReturnCode::Success;
                }

                if(opcode == WebSocketOpcode::Ping)
                {
                    rc = Send(payload, WebSocketOpcode::Pong);
                    CHECK_RETURN_CODE(rc);
                }

                continue;
            }

            // A server may split a message across frames, but none we talk to does.
            if(!header.IsFinal || opcode == WebSocketOpcode::Continuation)
            {
                return Break(ReturnCode::NotSupported);
            }

            if(header.Length > buffer.GetLength())
            {
                rc = Skip(header.Length);

                return rc == ReturnCode::Success ? ReturnCode::InvalidLength : Break(rc);
            }

            length = static_cast<uint32_t>(header.Length);
            rc = _tcp.Read(buffer.Take(length), _timeout);

            return rc == ReturnCode::Success ? rc : Break(rc);
        }
    }

private:
    // An Ethernet segment's worth of TCP payload, so each write to the socket leaves as one packet.
    static constexpr uint32_t SegmentLength = 1460;
    static constexpr uint32_t MaxRequestLength = 512;
    static constexpr uint32_t MaxLineLength = 128;
    static constexpr uint32_t MaxResponseLength = 4096;
    static constexpr uint32_t StatusLength = 3;

    TcpClient _tcp;
    TimeSpan _timeout;
    Mutex _sendMutex;
    Array<uint8_t, SegmentLength> _segment;
    uint16_t _status = 0;
    bool _isBroken = false;

    ReturnCode Break(ReturnCode rc)
    {
        _isBroken = true;
        return rc;
    }

    ReturnCode SendFrame(Span<const uint8_t> head, Span<const uint8_t> body, WebSocketOpcode opcode)
    {
        Array<uint8_t, WebSocketFrame::MaskLength> mask;
        sys_rand_get(mask.GetData(), mask.GetLength());

        uint32_t used = 0;
        uint64_t offset = 0;

        auto rc = WebSocketFrame::WriteHeader(_segment, opcode, static_cast<uint64_t>(head.GetLength()) + body.GetLength(),
            mask, used);
        CHECK_RETURN_CODE(rc);

        for(auto part : { head, body })
        {
            while(!part.IsEmpty())
            {
                if(used == SegmentLength)
                {
                    rc = _tcp.Write(_segment);
                    CHECK_RETURN_CODE(rc);

                    used = 0;
                }

                // Masked on its way into the segment, so the message is read once and never written.
                auto chunk = part.Take(SegmentLength - used);

                rc = WebSocketFrame::ApplyMask(chunk, _segment.Skip(used), mask, offset);
                CHECK_RETURN_CODE(rc);

                used += chunk.GetLength();
                offset += chunk.GetLength();
                part = part.Skip(chunk.GetLength());
            }
        }

        return _tcp.Write(_segment.Take(used));
    }

    ReturnCode ReadHeader(WebSocketFrame::Header& header)
    {
        Array<uint8_t, WebSocketFrame::MaxHeaderLength> bytes;
        uint32_t extensionLength = 0;

        auto rc = _tcp.Read(bytes.Take(WebSocketFrame::StartLength), _timeout);
        CHECK_RETURN_CODE(rc);

        rc = WebSocketFrame::GetExtensionLength(bytes, extensionLength);
        CHECK_RETURN_CODE(rc);

        rc = _tcp.Read(bytes.Skip(WebSocketFrame::StartLength).Take(extensionLength), _timeout);
        CHECK_RETURN_CODE(rc);

        rc = WebSocketFrame::ReadHeader(bytes.Take(WebSocketFrame::StartLength + extensionLength), header);
        CHECK_RETURN_CODE(rc);

        // Only clients mask.
        return header.IsMasked ? ReturnCode::InvalidData : ReturnCode::Success;
    }

    ReturnCode Skip(uint64_t length)
    {
        Array<uint8_t, MaxLineLength> discarded;

        while(length > 0)
        {
            auto chunk = discarded.Take(length < MaxLineLength ? static_cast<uint32_t>(length) : MaxLineLength);

            auto rc = _tcp.Read(chunk, _timeout);
            CHECK_RETURN_CODE(rc);

            length -= chunk.GetLength();
        }

        return ReturnCode::Success;
    }

    ReturnCode Upgrade(String host, String path, String headers)
    {
        auto rc = _tcp.SetNoDelay(true);
        CHECK_RETURN_CODE(rc);

        Array<uint8_t, WebSocketFrame::NonceLength> nonce;
        sys_rand_get(nonce.GetData(), nonce.GetLength());

        auto key = WebSocketFrame::GetKey(nonce);

        Array<char, MaxRequestLength> request;
        SpanWriter<char> writer(request);

        for(auto part : { String("GET "), path, String(" HTTP/1.1\r\nHost: "), host,
                String("\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Version: 13\r\nSec-WebSocket-Key: "),
                String(key.AsSpan()), String("\r\n"), headers, String("\r\n") })
        {
            rc = writer.Write(part);
            CHECK_RETURN_CODE(rc);
        }

        rc = _tcp.Write(writer.GetWrittenSpan().AsConstBytes());
        CHECK_RETURN_CODE(rc);

        return ReadResponse(WebSocketFrame::GetAccept(key));
    }

    ReturnCode ReadResponse(Array<char, WebSocketFrame::AcceptLength> expectedAccept)
    {
        auto deadline = Clock::GetUptime() + _timeout;
        uint32_t remaining = MaxResponseLength;
        bool isAccepted = false;

        for(bool isStatusLine = true; ; isStatusLine = false)
        {
            Array<char, MaxLineLength> buffer;
            String line;

            auto rc = ReadLine(buffer, line, deadline, remaining);
            CHECK_RETURN_CODE(rc);

            if(isStatusLine)
            {
                // "HTTP/1.1 101 Switching Protocols"
                auto space = line.IndexOf(' ');

                if(space < 0 || !StringParser::TryParse(String(line.Skip(space + 1).Take(StatusLength)), _status))
                {
                    return ReturnCode::InvalidData;
                }
            }
            else if(line.IsEmpty())
            {
                break;
            }
            else
            {
                String value;

                if(TryGetHeader(line, "sec-websocket-accept", value))
                {
                    isAccepted = value == String(expectedAccept.AsSpan());
                }
            }
        }

        if(_status != SwitchingProtocols)
        {
            return ReturnCode::InvalidOperation;
        }

        return isAccepted ? ReturnCode::Success : ReturnCode::InvalidData;
    }

    // A byte at a time, so nothing of the first frame is taken with the response. A line longer than
    // `buffer` loses its end, which no line read here has.
    ReturnCode ReadLine(Span<char> buffer, String& line, TimeSpan deadline, uint32_t& remaining)
    {
        SpanWriter<char> writer(buffer);

        while(true)
        {
            if(remaining == 0)
            {
                return ReturnCode::InvalidData;
            }

            Array<uint8_t, 1> byte;

            auto rc = _tcp.Read(byte, deadline - Clock::GetUptime());
            CHECK_RETURN_CODE(rc);

            remaining--;

            if(byte.Get<0>() == '\n')
            {
                break;
            }

            if(byte.Get<0>() != '\r')
            {
                writer.Write(static_cast<char>(byte.Get<0>()));
            }
        }

        line = String(writer.GetWrittenSpan());

        return ReturnCode::Success;
    }

    // `name` in lower case. HTTP header names match in any case.
    static bool TryGetHeader(String line, String name, String& value)
    {
        auto colon = line.IndexOf(':');

        if(colon < 0 || static_cast<uint32_t>(colon) != name.GetLength())
        {
            return false;
        }

        uint32_t index = 0;

        for(auto actual : line.Take(colon))
        {
            auto lower = actual >= 'A' && actual <= 'Z' ? static_cast<char>(actual - 'A' + 'a') : actual;

            if(!name.TryCompare(index, lower))
            {
                return false;
            }

            index++;
        }

        value = String(line.Skip(colon + 1));

        while(value.TryCompare(0, ' ') || value.TryCompare(0, '\t'))
        {
            value = value.Substring(1);
        }

        while(!value.IsEmpty() && (value.TryCompare(value.GetLength() - 1, ' ') || value.TryCompare(value.GetLength() - 1, '\t')))
        {
            value = String(value.Take(value.GetLength() - 1));
        }

        return true;
    }
};
