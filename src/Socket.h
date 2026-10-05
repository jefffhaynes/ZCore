#pragma once

#include <errno.h>
#include <limits.h>
#include <zephyr/net/socket.h>
#include "NetworkHelper.h"
#include "ErrorConverter.h"
#include "ReturnCode.h"
#include "Span.h"
#include "TimeSpan.h"

#if !defined(CONFIG_NET_SOCKETS) || !defined(CONFIG_NET_IPV4)
#error "Socket needs CONFIG_NET_SOCKETS and CONFIG_NET_IPV4"
#endif

// What UdpClient, TcpClient and TcpListener share.
class Socket
{
public:
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    ~Socket()
    {
        Close();
    }

    constexpr bool IsOpen() const
    {
        return _descriptor >= 0;
    }

    ReturnCode Close()
    {
        if(!IsOpen())
        {
            return ReturnCode::Success;
        }

        auto err = zsock_close(_descriptor);
        _descriptor = Closed;

        return err < 0 ? GetLastError() : ReturnCode::Success;
    }

    ReturnCode GetLocalEndPoint(IPEndPoint& endPoint) const
    {
        struct sockaddr_in address = {};
        socklen_t length = sizeof(address);

        if(zsock_getsockname(_descriptor, reinterpret_cast<struct sockaddr*>(&address), &length) < 0)
        {
            return GetLastError();
        }

        endPoint = NetworkHelper::FromNative(address);

        return ReturnCode::Success;
    }

protected:
    constexpr Socket()
    {
    }

    constexpr int GetDescriptor() const
    {
        return _descriptor;
    }

    ReturnCode Open(int type, int protocol)
    {
        if(IsOpen())
        {
            return ReturnCode::InvalidState;
        }

        _descriptor = zsock_socket(AF_INET, type, protocol);

        return IsOpen() ? ReturnCode::Success : GetLastError();
    }

    // Takes over a socket that's already open, closing its own.
    void Attach(int descriptor)
    {
        Close();
        _descriptor = descriptor;
    }

    ReturnCode Bind(IPEndPoint localEndPoint) const
    {
        auto address = NetworkHelper::ToNative(localEndPoint);
        auto err = zsock_bind(_descriptor, reinterpret_cast<struct sockaddr*>(&address), sizeof(address));

        return err < 0 ? GetLastError() : ReturnCode::Success;
    }

    // Timeout if the socket isn't readable (ZSOCK_POLLIN) or writable (ZSOCK_POLLOUT) in time.
    ReturnCode Wait(short events, TimeSpan timeout) const
    {
        if(!IsOpen())
        {
            return ReturnCode::InvalidState;
        }

        struct zsock_pollfd descriptor = {};
        descriptor.fd = _descriptor;
        descriptor.events = events;

        auto ready = zsock_poll(&descriptor, 1, ToPollTimeout(timeout));

        if(ready < 0)
        {
            return GetLastError();
        }

        return ready == 0 ? ReturnCode::Timeout : ReturnCode::Success;
    }

    static ReturnCode GetLastError()
    {
        return errno == ETIMEDOUT ? ReturnCode::Timeout : ErrorConverter::Convert(-errno);
    }

private:
    static constexpr int Closed = -1;

    int _descriptor = Closed;

    // Rounded up, so a wait shorter than poll's millisecond still waits.
    static constexpr int ToPollTimeout(TimeSpan timeout)
    {
        auto milliseconds = timeout.ToMilliseconds();

        if(milliseconds <= 0)
        {
            return 0;
        }

        if(milliseconds >= INT_MAX)
        {
            return INT_MAX;
        }

        auto whole = static_cast<int>(milliseconds);

        return whole < milliseconds ? whole + 1 : whole;
    }
};
