#pragma once

#include "MemoryMarshal.h"

namespace MemoryMarshalTests
{
    enum class TestEnum : uint8_t
    {
        Value
    };

    struct TestStruct
    {
        uint16_t a;
        uint32_t b;
    };

    static_assert(Safe<int> && Safe<float> && Safe<bool> && Safe<TestEnum>, "Safe failed for numbers and enums");
    static_assert(!Safe<TestStruct> && !Safe<Span<uint8_t>> && !Safe<int*>, "Safe accepted a struct, span or pointer");
}
