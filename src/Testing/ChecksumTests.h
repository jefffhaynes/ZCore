#pragma once

#include "Checksum.h"

namespace ChecksumTests
{
    static_assert(Checksum::Calculate(Span<const uint8_t>()) == 0, "Default checksum failed");

    static_assert(Checksum::UpdateValue(0x55, 0x0F) == static_cast<uint8_t>(0x5A), "Single-byte updates failed");

    static_assert([]{
        constexpr uint8_t data[] = { 0x01, 0x02, 0x03, 0x04 };
        return Checksum::Calculate(Span<const uint8_t>(data)) == static_cast<uint8_t>(0x01 ^ 0x02 ^ 0x03 ^ 0x04);
    }(), "Span update failed");

    static_assert([]{
        constexpr uint8_t data[] = { 0xAA };
        return Checksum::Calculate(Span<const uint8_t>(data), 0) == static_cast<uint8_t>(0xAA);
    }(), "Reset-equivalent initial state failed");

    static_assert([]{
        constexpr uint8_t data[] = { 0x55, 0x0F };
        return Checksum::Calculate(Span<const uint8_t>(data), 0xF0) == static_cast<uint8_t>(0xF0 ^ 0x55 ^ 0x0F);
    }(), "Initial value accumulation failed");
}