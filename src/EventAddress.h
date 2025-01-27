#pragma once

#include <stdint.h>

class EventAddress
{
public:
    constexpr EventAddress() : _address(0)
    {
    }
    
    constexpr EventAddress(uint32_t address) : _address(address)
    {
    }

    constexpr uint32_t GetAddress() const
    {
        return _address;
    }

private:
    uint32_t _address;
};
