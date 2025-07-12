#pragma once

#include "ReturnCode.h"
#include "Range.h"
#include "Voltage.h"

class IAdc
{ 
public:
    virtual ReturnCode GetRange(Range<Voltage>& range) = 0;
    virtual ReturnCode Sample(int16_t& sample) = 0;
    virtual ReturnCode Sample(float& sample) = 0;
    virtual ReturnCode Sample(Voltage& sample) = 0;
};