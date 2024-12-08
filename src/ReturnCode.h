#pragma once

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

#define CHECK_RETURN_CODE(rc) if(rc != ReturnCode::Success) return rc;