#pragma once

#include <ICrc.h>
#include <Array.h>
#include <limits>

template<typename T, T Polynomial, T InitialValue = 0>
class Crc : public ICrc
{
public:
    constexpr Crc()
    {
        Reset();
    }

    constexpr ~Crc() override = default;

    constexpr void Reset() override
    {
        _crc = InitialValue;
    }

    constexpr void Update(uint8_t data) override
    {
        constexpr int Width = std::numeric_limits<T>::digits;
        constexpr int Shift = Width - 8;

        const uint8_t index = static_cast<uint8_t>((_crc >> Shift) ^ data);
        _crc = static_cast<T>((_crc << 8) ^ _table[index]);
    }

    constexpr void Update(Span<const uint8_t> data) override
    {
        for (auto b : data)
        {
            Update(b);
        }
    }

    constexpr T GetValue() const
    {
        return _crc;
    }

private:
    static constexpr Array<T, 256> _table = []()
    {
        constexpr int Width = std::numeric_limits<T>::digits;
        constexpr T TopBit = static_cast<T>(T{1} << (Width - 1));

        Array<T, 256> table;

        for (uint16_t i = 0; i < 256; i++)
        {
            T entry = static_cast<T>(static_cast<T>(i) << (Width - 8));

            for (uint8_t j = 0; j < 8; j++)
            {
                entry = (entry & TopBit) ? static_cast<T>((entry << 1) ^ Polynomial)
                                         : static_cast<T>(entry << 1);
            }

            table[i] = entry;
        }

        return table;
    }();

    T _crc;
};


template<uint8_t Polynomial = 0x07, uint8_t InitialValue = 0>
class Crc8 : public Crc<uint8_t, Polynomial, InitialValue>
{
public:
    constexpr Crc8() : Crc<uint8_t, Polynomial, InitialValue>()
    {
    }

    constexpr ~Crc8() override = default;
};

template<uint16_t Polynomial = 0x1021, uint16_t InitialValue = 0xFFFF>
class Crc16 : public Crc<uint16_t, Polynomial, InitialValue>
{
public:
    constexpr Crc16() : Crc<uint16_t, Polynomial, InitialValue>()
    {
    }

    constexpr ~Crc16() override = default;
};