#pragma once

#include "InputStream.h"
#include "EventHandler.h"

class InputStreamWithDataReceived : public InputStream
{
public:
    EventHandler<> DataReceived;
};