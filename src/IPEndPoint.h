#pragma once

#include <stdint.h>
#include "IPAddress.h"

// The port is in host order.
struct IPEndPoint
{
    IPAddress Address;
    uint16_t Port = 0;

    constexpr IPEndPoint()
    {
    }

    constexpr IPEndPoint(IPAddress address, uint16_t port) : Address(address), Port(port)
    {
    }

    constexpr bool operator==(const IPEndPoint& other) const
    {
        return Address == other.Address && Port == other.Port;
    }
};
