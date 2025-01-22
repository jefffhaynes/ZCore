#pragma once

#include "Averager.h"
#include "Testing/TestSupport.h"

static_assert([]{
    Averager<float> averager;
    auto value = averager.Update(5, TimeSpan::FromMilliseconds(100));
    return value == 5;
}(), "Averager failed");

static_assert([]{
    Averager<float> averager;
    averager.Update(0, TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(1, TimeSpan::FromMilliseconds(100));
    return abs(value - 0.329679954) < 0.001;
}(), "Averager failed");

static_assert([]{
    Averager<float> averager;
    averager.Update(0, TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(1, TimeSpan::FromMilliseconds(200));
    return abs(value - 0.550671036) < 0.001;
}(), "Averager failed");

static_assert([]{
    Averager<float> averager(TimeSpan::FromSeconds(10));
    averager.Update(0, TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(1, TimeSpan::FromMilliseconds(200));
    return abs(value - 0.019801327) < 0.001;
}(), "Averager failed");

static_assert([]{
    Averager<TimeSpan> averager(TimeSpan::FromSeconds(10));
    averager.Update(TimeSpan::Zero, TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(TimeSpan::FromSeconds(1), TimeSpan::FromMilliseconds(200));
    return CoreMath::Abs(value - TimeSpan::FromSeconds(0.019801327)) < TimeSpan::FromSeconds(0.001);
}(), "Averager failed");

// TODO large time lapse compared to tau

constexpr auto delta = []{
    Averager<float> averager;
    averager.Update(0, TimeSpan::FromMilliseconds(0));
    averager.Update(1, TimeSpan::FromMilliseconds(100));
    auto value = averager.Update(2, TimeSpan::FromMilliseconds(200));
    return value - 0.276431829;
}();

static_assert(delta < 0.00001f, "Averager failed");

// constexpr auto delta2 = []{
//     Averager<float> averager(TimeSpan::FromMilliseconds(10));
//     averager.Update(0.2, TimeSpan::FromMilliseconds(339.96));
//     averager.Update(0.12, TimeSpan::FromMilliseconds(390.08));
//     averager.Update(-0.04, TimeSpan::FromMilliseconds(440.12));
//     return averager.Update(0.29, TimeSpan::FromMilliseconds(490.20));
// }();

// static_assert(delta2 == 0.287801336, "Averager failed");


// assignment test
static_assert([]{
    Averager<TimeSpan> averager;
    averager = Averager<TimeSpan>(TimeSpan::FromSeconds(10));
    averager.Update(TimeSpan::Zero, TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(TimeSpan::FromSeconds(1), TimeSpan::FromMilliseconds(200));
    return CoreMath::Abs(value - TimeSpan::FromSeconds(0.019801327)) < TimeSpan::FromSeconds(0.001);
}(), "Averager failed");