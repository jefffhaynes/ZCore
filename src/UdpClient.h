#pragma once

#include "Socket.h"

#if !defined(CONFIG_NET_UDP)
#error "UdpClient needs CONFIG_NET_UDP"
#endif

class UdpClient : public Socket
{
public:
    // Receives on this port and sends from it. Port 0 leaves the choice to the stack.
    constexpr explicit UdpClient(uint16_t port = 0) : _port(port)
    {
    }

    ReturnCode Open()
    {
        auto rc = Socket::Open(SOCK_DGRAM, IPPROTO_UDP);
        CHECK_RETURN_CODE(rc);

        rc = Bind(IPEndPoint(IPAddress::Any(), _port));

        if(rc != ReturnCode::Success)
        {
            Close();
        }

        return rc;
    }

    // One datagram, which must fit the interface's MTU: 1472 bytes on Ethernet.
    ReturnCode Send(Span<const uint8_t> datagram, IPEndPoint remoteEndPoint) const
    {
        if(!IsOpen())
        {
            return ReturnCode::InvalidState;
        }

        auto address = NetworkHelper::ToNative(remoteEndPoint);
        auto sent = zsock_sendto(GetDescriptor(), datagram.GetData(), datagram.GetLength(), 0,
            reinterpret_cast<struct sockaddr*>(&address), sizeof(address));

        if(sent < 0)
        {
            return GetLastError();
        }

        return static_cast<uint32_t>(sent) == datagram.GetLength() ? ReturnCode::Success : ReturnCode::InvalidLength;
    }

    // One datagram. InvalidLength if it was longer than the buffer, which then holds its start.
    ReturnCode Receive(Span<uint8_t> buffer, uint32_t& received, IPEndPoint& remoteEndPoint, TimeSpan timeout) const
    {
        received = 0;

        auto rc = Wait(ZSOCK_POLLIN, timeout);
        CHECK_RETURN_CODE(rc);

        struct sockaddr_in address = {};
        socklen_t addressLength = sizeof(address);

        // With ZSOCK_MSG_TRUNC the result is the datagram's length, not what fitted.
        auto length = zsock_recvfrom(GetDescriptor(), buffer.GetData(), buffer.GetLength(),
            ZSOCK_MSG_DONTWAIT | ZSOCK_MSG_TRUNC, reinterpret_cast<struct sockaddr*>(&address), &addressLength);

        if(length < 0)
        {
            return errno == EAGAIN ? ReturnCode::Timeout : GetLastError();
        }

        remoteEndPoint = NetworkHelper::FromNative(address);

        if(static_cast<uint32_t>(length) > buffer.GetLength())
        {
            received = buffer.GetLength();
            return ReturnCode::InvalidLength;
        }

        received = static_cast<uint32_t>(length);

        return ReturnCode::Success;
    }

    ReturnCode Receive(Span<uint8_t> buffer, uint32_t& received, TimeSpan timeout) const
    {
        IPEndPoint remoteEndPoint;
        return Receive(buffer, received, remoteEndPoint, timeout);
    }

private:
    uint16_t _port;
};
