#pragma once

#include "ReturnCode.h"

class IGpio
{
public:

    virtual ReturnCode Set(bool value) = 0;
};