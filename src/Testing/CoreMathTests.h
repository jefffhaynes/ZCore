#pragma once

#include "CoreMath.h"
#include "Testing/TestSupport.h"

#include <numbers>

static_assert(CoreMath::Abs(-5) == 5, "CoreMath::Abs failed for negative integer");
static_assert(CoreMath::Abs(5) == 5, "CoreMath::Abs failed for positive integer");
static_assert(CoreMath::Abs(0) == 0, "CoreMath::Abs failed for zero");
static_assert(AreAlmostEqual(CoreMath::Abs(-1.25f), 1.25f), "CoreMath::Abs failed for float");

static_assert(CoreMath::Clamp(-1, 0, 10) == 0, "CoreMath::Clamp failed for lower bound");
static_assert(CoreMath::Clamp(5, 0, 10) == 5, "CoreMath::Clamp failed for in-range value");
static_assert(CoreMath::Clamp(11, 0, 10) == 10, "CoreMath::Clamp failed for upper bound");

static_assert(!CoreMath::IsPowerOfTwo(0), "CoreMath::IsPowerOfTwo failed for zero");
static_assert(CoreMath::IsPowerOfTwo(1), "CoreMath::IsPowerOfTwo failed for one");
static_assert(CoreMath::IsPowerOfTwo(2), "CoreMath::IsPowerOfTwo failed for two");
static_assert(!CoreMath::IsPowerOfTwo(3), "CoreMath::IsPowerOfTwo failed for three");
static_assert(CoreMath::IsPowerOfTwo(1024), "CoreMath::IsPowerOfTwo failed for large power of two");

constexpr auto cm_delta = []{
    auto value = CoreMath::Exp(1.0f);
    return CoreMath::Abs(value - std::numbers::e_v<float>);
}();

static_assert(cm_delta < 0.00001f, "CoreMath failed");


constexpr auto cm_delta2 = []{
    auto value = CoreMath::Exp(0.0f);
    return CoreMath::Abs(value - 1.0f);
}();

static_assert(cm_delta2 < 0.00001f, "CoreMath failed");


constexpr auto cm_delta3 = []{
    auto value = CoreMath::Exp(2.0f);
    return CoreMath::Abs(value - std::numbers::e_v<float> * std::numbers::e_v<float>);
}();

static_assert(cm_delta3 < 0.00001f, "CoreMath failed");


// e^-5
constexpr auto cm_delta4 = []{
    auto value = CoreMath::Exp(-5.0f);
    return CoreMath::Abs(value);
}();

static_assert(cm_delta4 < 0.00001f, "CoreMath failed");


constexpr auto cm_delta5 = []{
    auto value = CoreMath::Exp(-1.0f);
    return CoreMath::Abs(value - 0.36787944117f);
}();

static_assert(cm_delta5 < 0.00001f, "CoreMath failed");

static_assert(CoreMath::Exp(-3.1f) == 0.0f, "CoreMath::Exp threshold clamp failed");

static_assert([]{
    constexpr auto expNeg2 = CoreMath::Exp(-2.0f);
    constexpr auto expNeg1 = CoreMath::Exp(-1.0f);
    constexpr auto exp0 = CoreMath::Exp(0.0f);
    constexpr auto exp1 = CoreMath::Exp(1.0f);
    return expNeg2 < expNeg1 && expNeg1 < exp0 && exp0 < exp1;
}(), "CoreMath::Exp monotonicity failed");
