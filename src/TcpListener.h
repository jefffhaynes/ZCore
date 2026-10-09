#pragma once

#include "TcpClient.h"

class TcpListener : public Socket
{
public:
    // backlog is how many connections can wait to be accepted.
    constexpr explicit TcpListener(uint16_t port, uint32_t backlog = 1) : _port(port), _backlog(backlog)
    {
    }

#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
    // Over TLS 1.2, where `tls` names the certificate and key the server presents. Accept then makes
    // each connection's handshake, and fails if the client turns the server down.
    constexpr TcpListener(uint16_t port, TlsOptions tls, uint32_t backlog = 1) : _port(port), _backlog(backlog), _tls(tls)
    {
    }
#endif

    ReturnCode Start()
    {
        auto rc = Open(SOCK_STREAM, GetProtocol());
        CHECK_RETURN_CODE(rc);

        rc = Secure();

        if(rc != ReturnCode::Success)
        {
            Close();
            return rc;
        }

        // Lets it restart on its port while connections it accepted are still open or closing.
        int reuse = 1;
        zsock_setsockopt(GetDescriptor(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        rc = Bind(IPEndPoint(IPAddress::Any(), _port));

        if(rc == ReturnCode::Success && zsock_listen(GetDescriptor(), static_cast<int>(_backlog)) < 0)
        {
            rc = GetLastError();
        }

        if(rc != ReturnCode::Success)
        {
            Close();
        }

        return rc;
    }

    // Timeout if nobody connects in time. Closes whatever the client had open.
    ReturnCode Accept(TcpClient& client, TimeSpan timeout) const
    {
        auto rc = Wait(ZSOCK_POLLIN, timeout);
        CHECK_RETURN_CODE(rc);

        auto descriptor = zsock_accept(GetDescriptor(), nullptr, nullptr);

        if(descriptor < 0)
        {
            return GetLastError();
        }

        client.Attach(descriptor);

        return ReturnCode::Success;
    }

private:
    uint16_t _port;
    uint32_t _backlog;

#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
    TlsOptions _tls;

    constexpr int GetProtocol() const
    {
        return _tls.IsEnabled() ? static_cast<int>(IPPROTO_TLS_1_2) : static_cast<int>(IPPROTO_TCP);
    }

    ReturnCode Secure() const
    {
        return _tls.IsEnabled() ? _tls.Apply(GetDescriptor()) : ReturnCode::Success;
    }
#else
    static constexpr int GetProtocol()
    {
        return IPPROTO_TCP;
    }

    static constexpr ReturnCode Secure()
    {
        return ReturnCode::Success;
    }
#endif
};
