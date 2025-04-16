#pragma once

#include "CoreMath.h"
#include "Testing/TestSupport.h"

#include <numbers>

constexpr auto cm_delta = []{
    auto value = CoreMath::Exp(1.0);
    return value - std::numbers::e;
}();

static_assert(cm_delta < 0.00001f, "CoreMath failed");


constexpr auto cm_delta2 = []{
    auto value = CoreMath::Exp(0.0);
    return value - 1.0f;
}();

static_assert(cm_delta2 < 0.00001f, "CoreMath failed");


constexpr auto cm_delta3 = []{
    auto value = CoreMath::Exp(2.0);
    return value - std::numbers::e * std::numbers::e;
}();

static_assert(cm_delta3 < 0.00001f, "CoreMath failed");


// e^-5
constexpr auto cm_delta4 = []{
    auto value = CoreMath::Exp(-5.0);
    return CoreMath::Abs(value);
}();

static_assert(cm_delta4 < 0.00001f, "CoreMath failed");


constexpr auto cm_delta5 = []{
    auto value = CoreMath::Exp(-1.0);
    return CoreMath::Abs(value - 0.36787944117f);
}();

static_assert(cm_delta5 < 0.00001f, "CoreMath failed");
