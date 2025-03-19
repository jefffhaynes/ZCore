#pragma once

#include <ICrc.h>

class Checksum : public ICrc
{
public:
    constexpr Checksum()
    {
    }

    constexpr ~Checksum() override = default;

    constexpr void Reset() override
    {
        _checksum = 0;
    }

    constexpr void Update(uint8_t data) override
    {
        _checksum ^= data;
    }

    constexpr void Update(Span<const uint8_t> data) override
    {
        for(auto b : data)
        {
            Update(b);
        }
    }

    constexpr uint8_t GetValue() const
    {
        return _checksum;
    }

private:
    uint8_t _checksum = 0;
};