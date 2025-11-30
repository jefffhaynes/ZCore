#pragma once

#include <type_traits>

#define DEBUG_HOOK()                                                     \
    do                                                                   \
    {                                                                    \
        if (!std::is_constant_evaluated())                               \
        {                                                                \
            volatile int debugBreakpoint = 0;                            \
            (void)debugBreakpoint;                                       \
        }                                                                \
    } while (false)


class DebugHook
{
public:
    static constexpr void Trigger()
    {
        DEBUG_HOOK(); // set breakpoint here
    }
};