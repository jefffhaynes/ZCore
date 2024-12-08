#pragma once

#include "Averager.h"

static_assert([]{
    Averager<float> averager;
    auto value = averager.Update(5, TimeSpan::FromMilliseconds(100));
    return value == 5;
}(), "Averager failed");

static_assert([]{
    Averager<float> averager;
    averager.Update(5, TimeSpan::FromMilliseconds(100));
    auto value = averager.Update(10, TimeSpan::FromMilliseconds(200));
    return value - 5.3846 < 0.0001;
}(), "Averager failed");

// test smoothing value of 0.5
static_assert([]{
    Averager<float> averager(0.5);
    averager.Update(5, TimeSpan::FromMilliseconds(100));
    auto value = averager.Update(10, TimeSpan::FromMilliseconds(200));
    return value - 5.1961 < 0.0001;
}(), "Averager failed");