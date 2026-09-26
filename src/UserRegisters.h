#pragma once

#include <stdint.h>

#if defined(CONFIG_HAS_NRFX)
#include <nrfx.h>
#endif

#if !defined(NRF_UICR_S)
#error "UserRegisters.h needs the nRF UICR OTP words (nRF53/nRF91); this SoC has no equivalent"
#endif

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