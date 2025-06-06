#pragma once

#include "ReturnCode.h"

namespace ReturnCodeTests
{
    constexpr ReturnCode UseMacro(ReturnCode rc)
    {
        CHECK_RETURN_CODE(rc);
        return ReturnCode::Success;
    }

    static_assert(UseMacro(ReturnCode::Success) == ReturnCode::Success,
                  "CHECK_RETURN_CODE should return Success when input is Success");

    static_assert(UseMacro(ReturnCode::Busy) == ReturnCode::Busy,
                  "CHECK_RETURN_CODE should propagate error codes");
}

