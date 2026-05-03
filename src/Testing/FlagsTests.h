#pragma once

#include "Flags.h"

#include <cstdint>

namespace FlagsTests
{
    enum class TestFlags : uint8_t
    {
        None = 0,
        A = 1 << 0,
        B = 1 << 1,
        C = 1 << 2,
        All = 0x07,
    };

    constexpr auto combinedAB = Flags::Combine(TestFlags::A, TestFlags::B);

    static_assert(!Flags::HasFlag(TestFlags::None, TestFlags::A), "HasFlag failed for empty flags");
    static_assert(Flags::HasFlag(combinedAB, TestFlags::A), "HasFlag failed for first flag");
    static_assert(Flags::HasFlag(combinedAB, TestFlags::B), "HasFlag failed for second flag");
    static_assert(!Flags::HasFlag(combinedAB, TestFlags::C), "HasFlag failed for missing flag");

    static_assert(Flags::Combine(TestFlags::A, TestFlags::A) == TestFlags::A, "Combine failed for duplicate flags");
    static_assert(Flags::Combine(combinedAB, TestFlags::C) == TestFlags::All, "Combine failed for multiple flags");
}