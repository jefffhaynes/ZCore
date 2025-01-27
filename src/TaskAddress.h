#pragma once

#include <stdint.h>

class TaskAddress
{
public:
    constexpr TaskAddress() : _address(0)
    {
    }
    
    constexpr TaskAddress(uint32_t address) : _address(address)
    {
    }

    constexpr uint32_t GetAddress() const
    {
        return _address;
    }

private:
    uint32_t _address;
};
