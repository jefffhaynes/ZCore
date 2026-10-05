#pragma once

#include <zephyr/net/net_ip.h>
#include "IPEndPoint.h"

class NetworkHelper
{
public:
    static struct in_addr ToNative(IPAddress address)
    {
        auto bytes = address.GetBytes();

        return { .s4_addr = { bytes.Get<0>(), bytes.Get<1>(), bytes.Get<2>(), bytes.Get<3>() } };
    }

    static struct sockaddr_in ToNative(IPEndPoint endPoint)
    {
        struct sockaddr_in native = {};
        native.sin_family = AF_INET;
        native.sin_port = htons(endPoint.Port);
        native.sin_addr = ToNative(endPoint.Address);

        return native;
    }

    static IPAddress FromNative(const struct in_addr& native)
    {
        return IPAddress(FixedSpan<const uint8_t, IPAddress::Length>::FromArray(native.s4_addr));
    }

    static IPEndPoint FromNative(const struct sockaddr_in& native)
    {
        return IPEndPoint(FromNative(native.sin_addr), ntohs(native.sin_port));
    }
};
