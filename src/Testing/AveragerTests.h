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


// assignment test
static_assert([]{
    Averager<TimeSpan> averager;
    averager = Averager<TimeSpan>(TimeSpan::FromSeconds(10));
    averager.Update(TimeSpan::Zero, TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(TimeSpan::FromSeconds(1), TimeSpan::FromMilliseconds(200));
    return CoreMath::Abs(value - TimeSpan::FromSeconds(0.019801327)) < TimeSpan::FromSeconds(0.001);
}(), "Averager failed");