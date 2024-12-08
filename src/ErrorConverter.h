#pragma once

#include "ReturnCode.h"
#include "errno.h"
#include <nrfx.h>

class ErrorConverter
{
public:
    static ReturnCode Convert(int err)
    {
        if(err >= 0)
        {
            return ReturnCode::Success;
        }

        switch(err)
        {
            case -EPERM: return ReturnCode::InvalidOperation;
            case -ENOENT: return ReturnCode::NotFound;
            case -ESRCH: return ReturnCode::NotFound; 
            case -EALREADY:
            case -EAGAIN: return ReturnCode::Busy;
            case -EINVAL: return ReturnCode::InvalidOperation;
        }

        return ReturnCode::InvalidOperation;
    }

    static ReturnCode Convert(nrfx_err_t err)
    {
        switch (err)
        {
            case NRFX_SUCCESS: return ReturnCode::Success;
            case NRFX_ERROR_NO_MEM: return ReturnCode::OutOfMemory;
            case NRFX_ERROR_NOT_SUPPORTED: return ReturnCode::NotSupported;
            case NRFX_ERROR_INVALID_PARAM: return ReturnCode::InvalidArgument;
            case NRFX_ERROR_INVALID_STATE: return ReturnCode::InvalidState;
            case NRFX_ERROR_INVALID_LENGTH: return ReturnCode::InvalidLength;
            case NRFX_ERROR_TIMEOUT: return ReturnCode::Timeout;
            case NRFX_ERROR_NULL: return ReturnCode::NullArgument;
            case NRFX_ERROR_BUSY: return ReturnCode::Busy;
            default: return ReturnCode::InvalidOperation;
        }
    }
};