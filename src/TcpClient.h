#pragma once

#include "Socket.h"
#include "Clock.h"
#include "Streams/InputStream.h"
#include "Streams/OutputStream.h"

#if !defined(CONFIG_NET_TCP)
#error "TcpClient needs CONFIG_NET_TCP"
#endif

class TcpClient : public Socket, public InputStream, public OutputStream
{
public:
    // A write fails with Timeout once the peer has taken nothing for writeTimeout.
    constexpr explicit TcpClient(TimeSpan writeTimeout = TimeSpan::FromSeconds(10)) : _writeTimeout(writeTimeout)
    {
    }

    // Gives up after the stack's CONFIG_NET_SOCKETS_CONNECT_TIMEOUT.
    ReturnCode Connect(IPEndPoint remoteEndPoint)
    {
        auto rc = Open(SOCK_STREAM, IPPROTO_TCP);
        CHECK_RETURN_CODE(rc);

        auto address = NetworkHelper::ToNative(remoteEndPoint);

        if(zsock_connect(GetDescriptor(), reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) < 0)
        {
            rc = GetLastError();
            Close();
            return rc;
        }

        _isEndOfStream = false;

        return ReturnCode::Success;
    }

    ReturnCode GetRemoteEndPoint(IPEndPoint& endPoint) const
    {
        struct sockaddr_in address = {};
        socklen_t length = sizeof(address);

        if(zsock_getpeername(GetDescriptor(), reinterpret_cast<struct sockaddr*>(&address), &length) < 0)
        {
            return GetLastError();
        }

        endPoint = NetworkHelper::FromNative(address);

        return ReturnCode::Success;
    }

    // Timeout if nothing arrives in time. The peer closing counts as arriving.
    ReturnCode WaitForData(TimeSpan timeout) const
    {
        return Wait(ZSOCK_POLLIN, timeout);
    }

    // What has arrived, without waiting.
    ReturnCode Read(Span<uint8_t> data, uint32_t& read) override
    {
        read = 0;

        if(!IsOpen())
        {
            return ReturnCode::InvalidState;
        }

        if(data.IsEmpty())
        {
            return ReturnCode::Success;
        }

        auto length = zsock_recv(GetDescriptor(), data.GetData(), data.GetLength(), ZSOCK_MSG_DONTWAIT);

        if(length > 0)
        {
            read = static_cast<uint32_t>(length);
        }
        else if(length == 0)
        {
            _isEndOfStream = true;
        }
        else if(errno != EAGAIN)
        {
            return GetLastError();
        }

        return ReturnCode::Success;
    }

    // As InputStream's, but it sleeps in poll until data arrives rather than retrying.
    ReturnCode Read(Span<uint8_t> data, TimeSpan timeout = TimeSpan::FromSeconds(1))
    {
        auto start = Clock::GetUptime();

        while(!data.IsEmpty())
        {
            uint32_t read = 0;
            auto rc = Read(data, read);
            CHECK_RETURN_CODE(rc);

            data = data.Skip(read);

            if(data.IsEmpty())
            {
                break;
            }

            if(_isEndOfStream)
            {
                return ReturnCode::InvalidLength;
            }

            rc = WaitForData(timeout - (Clock::GetUptime() - start));
            CHECK_RETURN_CODE(rc);
        }

        return ReturnCode::Success;
    }

    // True once the peer has closed and everything it sent has been read.
    bool IsEndOfStream() override
    {
        return _isEndOfStream;
    }

    ReturnCode Write(Span<const uint8_t> data) override
    {
        if(!IsOpen())
        {
            return ReturnCode::InvalidState;
        }

        while(!data.IsEmpty())
        {
            auto sent = zsock_send(GetDescriptor(), data.GetData(), data.GetLength(), ZSOCK_MSG_DONTWAIT);

            if(sent >= 0)
            {
                data = data.Skip(static_cast<uint32_t>(sent));
            }
            else if(errno == EAGAIN)
            {
                auto rc = Wait(ZSOCK_POLLOUT, _writeTimeout);
                CHECK_RETURN_CODE(rc);
            }
            else
            {
                return GetLastError();
            }
        }

        return ReturnCode::Success;
    }

private:
    friend class TcpListener;

    TimeSpan _writeTimeout;
    bool _isEndOfStream = false;

    void Attach(int descriptor)
    {
        Socket::Attach(descriptor);
        _isEndOfStream = false;
    }
};
