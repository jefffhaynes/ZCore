#pragma once

#include "DebugHook.h"

enum class ReturnCode
{
    Success,
    InvalidOperation,
    NotFound,
    NotSupported,
    NotImplemented,
    InvalidArgument,
    OutOfRange,
    NullArgument,
    InvalidLength,
    InvalidData,
    InvalidState,
    OutOfMemory,
    Timeout,
    Busy
};


#define CHECK_RETURN_CODE(rc)               \
    do                                      \
    {                                       \
        if ((rc) != ReturnCode::Success)    \
        {                                   \
            DebugHook::Trigger();           \
            return (rc);                    \
        }                                   \
    } while (0)
