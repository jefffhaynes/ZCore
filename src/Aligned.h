#pragma once

#include <stdint.h>

template<typename T, uint32_t Alignment = 4>
struct Aligned
{
    T __attribute__((aligned(Alignment))) Value;
};