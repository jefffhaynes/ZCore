#pragma once

#include "TimeSpan.h"

namespace TimeSpanTests
{
    static_assert(TimeSpan::FromNanoseconds(1).ToNanoseconds() == 1, "FromNanoseconds failed");
    static_assert(TimeSpan::FromMicroseconds(1).ToMicroseconds() == 1, "FromMicroseconds failed");
    static_assert(TimeSpan::FromMilliseconds(1).ToSeconds() == 0.001, "ToSeconds failed");
    static_assert(TimeSpan::FromMilliseconds(1000).ToMilliseconds() == 1000, "FromMilliseconds or ToMilliseconds failed");

    static_assert(TimeSpan::FromSeconds(1).ToMilliseconds() == 1000, "FromSeconds failed");

    static_assert(TimeSpan::FromSeconds(1).ToSeconds() == 1, "ToSeconds failed");

    static_assert((TimeSpan::FromMilliseconds(500) + TimeSpan::FromMilliseconds(500)).ToMilliseconds() == 1000, "Addition operator failed");

    static_assert((TimeSpan::FromMilliseconds(1000) - TimeSpan::FromMilliseconds(500)).ToMilliseconds() == 500, "Subtraction operator failed");

    static_assert(TimeSpan::FromMilliseconds(1000) / TimeSpan::FromMilliseconds(500) == 2, "Division operator (TimeSpan / TimeSpan) failed");

    static_assert((TimeSpan::FromMilliseconds(1000) / 2).ToMilliseconds() == 500, "Division operator (TimeSpan / scalar) failed");

    static_assert(TimeSpan::FromMilliseconds(500) < TimeSpan::FromMilliseconds(1000), "Less than operator failed");

    static_assert(TimeSpan::FromMilliseconds(1000) > TimeSpan::FromMilliseconds(500), "Greater than operator failed");

    static_assert([]{
        TimeSpan ts = TimeSpan::FromMilliseconds(1000);
        ts += TimeSpan::FromMilliseconds(500);
        return ts.ToMilliseconds() == 1500;
    }(), "Addition assignment operator failed");

    static_assert(TimeSpan::FromMilliseconds(1000) == TimeSpan::FromMilliseconds(1000), "Equality operator failed");

    static_assert(TimeSpan::FromMilliseconds(1000) != TimeSpan::FromMilliseconds(500), "Inequality operator failed");

    static_assert((TimeSpan::FromMilliseconds(500) * 2).ToMilliseconds() == 1000, "Multiplication operator failed");

    static_assert(TimeSpan::Zero.ToMilliseconds() == 0, "Zero static member failed");
}