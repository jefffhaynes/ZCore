#pragma once

#include "ReturnCode.h"
#include "Vector3.h"
#include "Units/Acceleration.h"

class IAccelerometer
{
public:
    virtual ReturnCode Sample(Vector3<Acceleration>& acceleration) = 0;
};
