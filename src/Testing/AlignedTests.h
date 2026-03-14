#include "Aligned.h"

static_assert(alignof(Aligned<int, 16>) == 16);