#pragma once

#include "AveragerHelper.h"
#include "TestSupport.h"

namespace AveragerHelperTests
{
    static_assert(AveragerHelper::DefaultTimeConstant == TimeSpan::FromSeconds(1), "DefaultTimeConstant failed");

    static_assert(AveragerHelper::GetAlpha(TimeSpan::Zero(), TimeSpan::FromSeconds(1)) == 0.0f, "Zero interval failed");

    static_assert(AveragerHelper::GetAlpha(TimeSpan::FromMilliseconds(-10), TimeSpan::FromSeconds(1)) == 0.0f, "Negative interval clamp failed");

    static_assert(AveragerHelper::GetAlpha(TimeSpan::FromMilliseconds(100), TimeSpan::Zero()) == 1.0f, "Zero time constant failed");

    constexpr auto alpha100Ms = AveragerHelper::GetAlpha(TimeSpan::FromMilliseconds(100), TimeSpan::FromSeconds(1));
    static_assert(AreAlmostEqual(alpha100Ms, 0.09516258f), "100 ms alpha failed");

    constexpr auto alpha500Ms = AveragerHelper::GetAlpha(TimeSpan::FromMilliseconds(500), TimeSpan::FromSeconds(1));
    static_assert(AreAlmostEqual(alpha500Ms, 0.39346934f), "500 ms alpha failed");
}