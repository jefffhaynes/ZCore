#pragma once

#include <stdint.h>

// A 16-bit pixel: red in the top 5 bits, green in the middle 6, blue in the bottom 5.
struct Rgb565
{
    uint16_t Value = 0;

    constexpr Rgb565()
    {
    }

    constexpr explicit Rgb565(uint16_t value) : Value(value)
    {
    }

    static constexpr Rgb565 FromRgb(uint8_t red, uint8_t green, uint8_t blue)
    {
        return Rgb565(static_cast<uint16_t>((red >> 3) << 11 | (green >> 2) << 5 | blue >> 3));
    }

    constexpr uint8_t GetRed() const { return Widen(Value >> 11, 5); }
    constexpr uint8_t GetGreen() const { return Widen((Value >> 5) & 0x3F, 6); }
    constexpr uint8_t GetBlue() const { return Widen(Value & 0x1F, 5); }

    constexpr bool operator==(const Rgb565& other) const
    {
        return Value == other.Value;
    }

private:
    // Repeats the top bits in the low ones, so full scale stays full scale.
    static constexpr uint8_t Widen(uint32_t channel, uint32_t bits)
    {
        return static_cast<uint8_t>(channel << (8 - bits) | channel >> (2 * bits - 8));
    }
};

static_assert(sizeof(Rgb565) == sizeof(uint16_t), "Rgb565 must be a single 16-bit pixel");
