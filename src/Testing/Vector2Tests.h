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
}