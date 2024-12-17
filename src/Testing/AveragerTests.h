#pragma once

#include "Averager.h"

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
    Averager<float> averager(10);
    averager.Update(0, TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(1, TimeSpan::FromMilliseconds(200));
    return abs(value - 0.019801327) < 0.001;
}(), "Averager failed");