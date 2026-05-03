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
        _checksum = UpdateValue(_checksum, data);
    }

    constexpr void Update(Span<const uint8_t> data) override
    {
        _checksum = Calculate(data, _checksum);
    }

    constexpr uint8_t GetValue() const
    {
        return _checksum;
    }

    static constexpr uint8_t UpdateValue(uint8_t checksum, uint8_t data)
    {
        return checksum ^ data;
    }

    static constexpr uint8_t Calculate(Span<const uint8_t> data, uint8_t initialValue = 0)
    {
        auto checksum = initialValue;

        for(auto b : data)
        {
            checksum = UpdateValue(checksum, b);
        }

        return checksum;
    }

private:
    uint8_t _checksum = 0;
};