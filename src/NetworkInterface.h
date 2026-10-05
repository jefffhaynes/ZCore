#pragma once

#include <zephyr/net/net_if.h>
#include "NetworkHelper.h"
#include "Clock.h"
#include "ReturnCode.h"
#include "Span.h"

#if defined(CONFIG_NET_DHCPV4)
#include <zephyr/net/dhcpv4.h>
#endif

#if !defined(CONFIG_NET_IPV4)
#error "NetworkInterface needs CONFIG_NETWORKING and CONFIG_NET_IPV4"
#endif

class NetworkInterface
{
public:
    constexpr explicit NetworkInterface(struct net_if* interface) : _interface(interface)
    {
    }

    static NetworkInterface GetDefault()
    {
        return NetworkInterface(net_if_get_default());
    }

    // The interface a network device provides, e.g. an Ethernet MAC.
    static NetworkInterface FromDevice(const struct device* device)
    {
        return NetworkInterface(net_if_lookup_by_dev(device));
    }

    constexpr struct net_if* GetInterface() const
    {
        return _interface;
    }

    constexpr bool Exists() const
    {
        return _interface != nullptr;
    }

    // Able to carry traffic: enabled, and with a link.
    bool IsUp() const
    {
        return net_if_is_up(_interface);
    }

    // On Ethernet, whether the cable is connected.
    bool HasCarrier() const
    {
        return net_if_is_carrier_ok(_interface);
    }

    // The MAC address, on Ethernet.
    Span<const uint8_t> GetPhysicalAddress() const
    {
        if(!Exists())
        {
            return Span<const uint8_t>();
        }

        auto* address = net_if_get_link_addr(_interface);

        // Take keeps it inside the array, whatever length the stack reports.
        return Span<const uint8_t>(address->addr).Take(address->len);
    }

    // Replaces any address set here before, but not one DHCP assigned.
    ReturnCode SetAddress(IPAddress address, IPAddress subnetMask) const
    {
        if(!Exists())
        {
            return ReturnCode::InvalidState;
        }

        net_if_ipv4_addr_foreach(_interface, RemoveIfManual, nullptr);

        auto native = NetworkHelper::ToNative(address);
        auto nativeMask = NetworkHelper::ToNative(subnetMask);

        if(net_if_ipv4_addr_add(_interface, &native, NET_ADDR_MANUAL, 0) == nullptr)
        {
            return ReturnCode::OutOfMemory;
        }

        return net_if_ipv4_set_netmask_by_addr(_interface, &native, &nativeMask) ? ReturnCode::Success
            : ReturnCode::InvalidOperation;
    }

    ReturnCode SetGateway(IPAddress gateway) const
    {
        if(!Exists())
        {
            return ReturnCode::InvalidState;
        }

        // The stack drops the gateway silently when CONFIG_NET_IF_MAX_IPV4_COUNT is too few interfaces.
        if(net_if_config_ipv4_get(_interface, nullptr) < 0)
        {
            return ReturnCode::OutOfMemory;
        }

        auto native = NetworkHelper::ToNative(gateway);
        net_if_ipv4_set_gw(_interface, &native);

        return ReturnCode::Success;
    }

#if defined(CONFIG_NET_DHCPV4)
    // The address arrives later: see WaitForAddress.
    ReturnCode StartDhcp() const
    {
        if(!Exists())
        {
            return ReturnCode::InvalidState;
        }

        net_dhcpv4_start(_interface);

        return ReturnCode::Success;
    }

    ReturnCode StopDhcp() const
    {
        if(!Exists())
        {
            return ReturnCode::InvalidState;
        }

        net_dhcpv4_stop(_interface);

        return ReturnCode::Success;
    }
#endif

    // The first usable address that isn't link-local, whether set here or by DHCP.
    bool TryGetAddress(IPAddress& address) const
    {
        auto* native = net_if_ipv4_get_global_addr(_interface, NET_ADDR_PREFERRED);

        if(native == nullptr)
        {
            return false;
        }

        address = NetworkHelper::FromNative(*native);

        return true;
    }

    ReturnCode GetAddress(IPAddress& address) const
    {
        return TryGetAddress(address) ? ReturnCode::Success : ReturnCode::NotFound;
    }

    // Until the interface is up with an address, e.g. once DHCP has finished.
    ReturnCode WaitForAddress(IPAddress& address, TimeSpan timeout) const
    {
        auto start = Clock::GetUptime();

        while(!IsUp() || !TryGetAddress(address))
        {
            if(Clock::GetUptime() - start > timeout)
            {
                return ReturnCode::Timeout;
            }

            Clock::Sleep(PollInterval);
        }

        return ReturnCode::Success;
    }

private:
    static constexpr TimeSpan PollInterval = TimeSpan::FromMilliseconds(20);

    struct net_if* _interface;

    static void RemoveIfManual(struct net_if* interface, struct net_if_addr* address, void*)
    {
        if(address->addr_type == NET_ADDR_MANUAL)
        {
            // A copy: removing the entry clears the address it holds.
            auto native = address->address.in_addr;
            net_if_ipv4_addr_rm(interface, &native);
        }
    }
};
