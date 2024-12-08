#pragma once

#include <stdint.h>

class UserRegisters
{
public:
    template<uint32_t Index>
    static constexpr void Set(uint32_t value)
    {
        if(Get<Index>() != value)
        {
		    NRF_UICR_S->OTP[Index] = value;
        }
    }
    
    template<uint32_t Index>
    static constexpr uint32_t Get()
    {
        static_assert(Index < RegisterCount);

        return NRF_UICR_S->OTP[Index];
    }

private:
    static const uint32_t RegisterCount = 192;
};