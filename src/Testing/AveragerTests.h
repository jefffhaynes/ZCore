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
    return abs(value - 0.329679954f) < 0.001f;
}(), "Averager failed");

static_assert([]{
    Averager<float> averager;
    averager.Update(0, TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(1, TimeSpan::FromMilliseconds(200));
    return abs(value - 0.550671036f) < 0.001f;
}(), "Averager failed");

static_assert([]{
    Averager<float> averager(TimeSpan::FromSeconds(10));
    averager.Update(0, TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(1, TimeSpan::FromMilliseconds(200));
    return abs(value - 0.019801327f) < 0.001;
}(), "Averager failed");

static_assert([]{
    Averager<TimeSpan> averager(TimeSpan::FromSeconds(10));
    averager.Update(TimeSpan::Zero(), TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(TimeSpan::FromSeconds(1), TimeSpan::FromMilliseconds(200));
    return CoreMath::Abs(value - TimeSpan::FromSeconds(0.019801327)) < TimeSpan::FromSeconds(0.001);
}(), "Averager failed");

constexpr auto delta = []{
    Averager<float> averager;
    averager.Update(0, TimeSpan::FromMilliseconds(0));
    averager.Update(1, TimeSpan::FromMilliseconds(100));
    auto value = averager.Update(2, TimeSpan::FromMilliseconds(200));
    return value - 0.276431829f;
}();

static_assert(delta < 0.00001f, "Averager failed");

constexpr auto delta2 = []{
    Averager<float> averager(TimeSpan::FromMilliseconds(10));
    averager.Update(0, TimeSpan::Zero());
    return averager.Update(1, TimeSpan::FromMilliseconds(400));
}();

static_assert(delta2 == 1.0f, "Averager failed");

// no time lapsed
constexpr auto delta3 = []{
    Averager<float> averager(TimeSpan::FromMilliseconds(10));
    averager.Update(0, TimeSpan::Zero());
    return averager.Update(1, TimeSpan::Zero());
}();

static_assert(delta3 == 0.0f, "Averager failed");


// tau zero
constexpr auto delta4 = []{
    Averager<float> averager(TimeSpan::Zero());
    averager.Update(0, TimeSpan::Zero());
    return averager.Update(1, TimeSpan::FromMilliseconds(100));
}();

static_assert(delta4 == 1.0f, "Averager failed");

// assignment test
static_assert([]{
    Averager<TimeSpan> averager;
    averager = Averager<TimeSpan>(TimeSpan::FromSeconds(10));
    averager.Update(TimeSpan::Zero(), TimeSpan::FromMilliseconds(0));
    auto value = averager.Update(TimeSpan::FromSeconds(1), TimeSpan::FromMilliseconds(200));
    return CoreMath::Abs(value - TimeSpan::FromSeconds(0.019801327)) < TimeSpan::FromSeconds(0.001);
}(), "Averager failed");