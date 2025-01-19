#pragma once

#include "Clock.h"
#include "Debug.h"

class Profiler
{
public:
    Profiler(StringLiteral name) : _name(name)
    {
        _start = Clock::GetUptime();
    }

    ~Profiler()
    {
        Complete();
    }

    void Complete()
    {
        if (_completed)
        {
            return;
        }

        auto end = Clock::GetUptime();
        auto duration = end - _start;
        Debug::WriteLine("%s: %.3f ms", _name.GetData(), (float) duration.ToMilliseconds());

        _completed = true;
    }

private:
    bool _completed = false;
    StringLiteral _name;
    TimeSpan _start;
};
