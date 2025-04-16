#pragma once

#include "FixedAverager.h"
#include "Testing/TestSupport.h"



constexpr auto fa_delta = []{
    FixedAverager<float> averager;
    averager.Update(0);
    auto value = averager.Update(1);
    return CoreMath::Abs(value - 0.367879441f);
}();

static_assert(fa_delta < 0.00001f, "FixedAverager failed");
