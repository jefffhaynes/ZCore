#pragma once

#include <ICrc.h>
#include <Array.h>

template<uint8_t Polynomial, uint8_t InitialValue = 0>
class Crc8 : public ICrc
{
public:
    constexpr Crc8()
    {
        Reset();
    }

    constexpr ~Crc8() override = default;

    constexpr void Reset() override
    {
        _crc = InitialValue;
    }

    constexpr void Update(uint8_t data) override
    {
        _crc = _table[_crc ^ data];
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
        return _crc;
    }

private:
    static constexpr Array<uint8_t, 256> _table = []()
    {
        Array<uint8_t, 256> table;

        for (uint16_t i = 0; i < 256; i++)
        {
            uint8_t crc = i;

            for (uint8_t j = 0; j < 8; j++)
            {
                crc = (crc & 0x80) ? (crc << 1) ^ Polynomial : crc << 1;
            }

            table[i] = crc;
        }

        return table;
    }();

    uint8_t _crc;
};