#pragma once

#include "TcpClient.h"

class TcpListener : public Socket
{
public:
    // backlog is how many connections can wait to be accepted.
    constexpr explicit TcpListener(uint16_t port, uint32_t backlog = 1) : _port(port), _backlog(backlog)
    {
    }

    ReturnCode Start()
    {
        auto rc = Open(SOCK_STREAM, IPPROTO_TCP);
        CHECK_RETURN_CODE(rc);

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
};
