#pragma once

#include "Crc.h"

namespace CrcTests
{
    constexpr uint8_t testData[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };

    static_assert(Crc8<>::Calculate(Span<const uint8_t>()) == 0x00, "CRC8 default initial value failed");
    static_assert(Crc16<>::Calculate(Span<const uint8_t>()) == 0xFFFF, "CRC16 default initial value failed");

    static_assert(Crc8<>::Calculate(Span<const uint8_t>(testData)) == 0xF4, "CRC8 known vector failed");
    static_assert(Crc16<>::Calculate(Span<const uint8_t>(testData)) == 0x29B1, "CRC16 known vector failed");

    static_assert([]{
        constexpr uint8_t bytes[] = { 0x01, 0x02, 0x03 };
        auto crc = static_cast<uint8_t>(0);

        for(auto byte : bytes)
        {
            crc = Crc8<>::UpdateValue(crc, byte);
        }

        return crc == Crc8<>::Calculate(Span<const uint8_t>(bytes));
    }(), "CRC8 incremental update failed");
}