#pragma once

#include "Clock.h"
#include "Debug.h"

// #pragma GCC push_options
// #pragma GCC optimize ("O0")

class Profiler
{
public:
    Profiler(StringLiteral name) : _name(name)
    {
        _start = Clock::GetUptime();
    }

    ~Profiler()
    {
        auto end = Clock::GetUptime();
        auto duration = end - _start;
        Debug::WriteLine("%s: %.3f ms", _name.GetData(), (float) duration.ToMilliseconds());
    }

private:
    StringLiteral _name;
    TimeSpan _start;
};

// #pragma GCC pop_options
