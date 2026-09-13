#pragma once

#include "PidController.h"

// Two updates inside the same clock tick must not produce a non-finite output.
// (Before the dt guard, dt == 0 made the derivative inf/NaN, and inf * 0 gain
// is still NaN, which callers that accumulate the output could never recover
// from.) NaN is the only float that is not equal to itself.

namespace PidControllerTests
{
    constexpr float RunSameTickTwice(float kp, float ki, float kd)
    {
        auto controller = PidController(kp, ki, kd, 100.0f);
        auto t = TimeSpan::FromMilliseconds(1000);

        controller.Update(80.0f, t);                     // first sample: initialises, returns 0
        controller.Update(90.0f, t + TimeSpan::FromMilliseconds(10));
        return controller.Update(95.0f, t + TimeSpan::FromMilliseconds(10));   // same tick as before
    }

    constexpr float ZeroGainsSameTick = RunSameTickTwice(0.0001f, 0.0f, 0.0f);
    static_assert(ZeroGainsSameTick == ZeroGainsSameTick, "PID: dt == 0 produced NaN with zero I/D gains");

    constexpr float AllGainsSameTick = RunSameTickTwice(0.5f, 0.1f, 0.2f);
    static_assert(AllGainsSameTick == AllGainsSameTick, "PID: dt == 0 produced NaN");
    static_assert(AllGainsSameTick < 1.0e6f && AllGainsSameTick > -1.0e6f, "PID: dt == 0 produced an unbounded output");

    // The repeated sample is proportional-only (plus the integral so far), so it
    // must equal kp * error exactly when ki is zero.
    static_assert(RunSameTickTwice(2.0f, 0.0f, 0.5f) == 2.0f * (100.0f - 95.0f), "PID: dt == 0 should fall back to the proportional term");

    // Normal spacing is unaffected: a later sample still applies the derivative.
    constexpr float RunSpaced()
    {
        auto controller = PidController(0.0f, 0.0f, 1.0f, 0.0f);
        auto t = TimeSpan::FromSeconds(1);

        controller.Update(0.0f, t);
        return controller.Update(-1.0f, t + TimeSpan::FromSeconds(1));   // error 1 over 1 s
    }

    static_assert(RunSpaced() == 1.0f, "PID: derivative term broken by the dt guard");
}
