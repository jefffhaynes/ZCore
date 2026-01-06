#pragma once

#include "TimeSpan.h"
#include "CoreMath.h"

class AveragerHelper
{
public:
    static constexpr TimeSpan DefaultTimeConstant = TimeSpan::FromSeconds(1);
    
    static constexpr float GetAlpha(TimeSpan timeInterval, TimeSpan timeConstant)
    {
        if (timeInterval < TimeSpan::Zero())
        {
            timeInterval = TimeSpan::Zero();
        }

        if (timeConstant == TimeSpan::Zero())
        {
            // zero time constant means instant change
            return 1;
        }

        auto exponent = -timeInterval / timeConstant;
        return 1 - CoreMath::Exp(exponent);
    }
};
