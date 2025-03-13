#pragma once

#include "InputStream.h"
#include "EventHandler.h"

class InputStreamWithDataReceived : public InputStream
{
public:
    EventHandler<> DataReceived;

    virtual constexpr uint32_t GetAvailable() = 0;
};