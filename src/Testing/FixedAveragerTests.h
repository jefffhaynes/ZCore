#pragma once

#include "FixedAverager.h"
#include "Testing/TestSupport.h"



constexpr auto fa_delta = []{
    FixedAverager<> averager;
    averager.Update(0);
    auto value = averager.Update(1);
    return CoreMath::Abs(value - 0.632120559f);
}();

static_assert(fa_delta < 0.00001f, "FixedAverager failed");



constexpr auto fa_delta2 = []{
    FixedAverager<> averager(TimeSpan::FromMilliseconds(1000), TimeSpan::FromSeconds(2));
    averager.Update(0);
    auto value = averager.Update(1);
    return CoreMath::Abs(value - 0.39346934f);
}();

static_assert(fa_delta2 < 0.00001f, "FixedAverager failed");
