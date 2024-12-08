#pragma once

#include <stdlib.h>

template<typename T>
class Deleter
{
public:
    Deleter(T* p) : _p(p)
    {
    }

    ~Deleter()
    {
        free(_p);
    }

private:
    T* _p;
};