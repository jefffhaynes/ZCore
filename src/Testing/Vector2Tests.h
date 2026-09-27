#pragma once

#include "Vector2.h"

namespace Vector2Tests
{
    constexpr Vector2<int> defaultVector;
    static_assert(defaultVector == Vector2<int>(0, 0), "Default constructor failed");

    constexpr Vector2<float> initializedVector(1.0f, 2.0f);
    static_assert(initializedVector.X == 1.0f && initializedVector.Y == 2.0f, "Parameterized constructor failed");

    static_assert((Vector2<int>(1, 2) + Vector2<int>(3, 4)) == Vector2<int>(4, 6), "Addition operator failed");
    static_assert((Vector2<int>(5, 7) - Vector2<int>(2, 3)) == Vector2<int>(3, 4), "Subtraction operator failed");
    static_assert((Vector2<float>(2.0f, 3.0f) * 2.0f) == Vector2<float>(4.0f, 6.0f), "Multiplication operator failed");
    static_assert((Vector2<float>(8.0f, 6.0f) / 2.0f) == Vector2<float>(4.0f, 3.0f), "Division operator failed");
    static_assert(Vector2<int>::Zero == Vector2<int>(0, 0), "Zero constant failed");

    static_assert((Vector2<double>(1, 2) * 0.1) == Vector2<double>(1 * 0.1, 2 * 0.1), "Multiplication truncated a double");
    static_assert((Vector2<double>(1, 2) / 10.0) == Vector2<double>(1 / 10.0, 2 / 10.0), "Division truncated a double");
    static_assert((Vector2<float>(1, 2) * 0.5) == Vector2<float>(0.5f, 1.0f), "Float vector by a double failed");
    static_assert((Vector2<int>(1, 2) * 2) == Vector2<int>(2, 4), "Integer multiplication failed");
    static_assert((Vector2<int>(6, 8) / 2) == Vector2<int>(3, 4), "Integer division failed");

    template<typename TVector, typename TScalar>
    concept CanScale = requires(TVector vector, TScalar scalar) { vector * scalar; vector / scalar; };

    static_assert(CanScale<Vector2<int>, int> && CanScale<Vector2<float>, double> && CanScale<Vector2<double>, int>,
        "Scaling failed to compile");
    static_assert(!CanScale<Vector2<int>, float> && !CanScale<Vector2<int>, double>,
        "An integer vector scaled by a fraction");
}