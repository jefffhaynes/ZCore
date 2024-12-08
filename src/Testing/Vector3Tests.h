#include "Vector3.h"
#include "Core/Testing/TestSupport.h"


namespace Vector3Tests
{
    // Tests for Constructors
    constexpr Vector3<int> defaultVector;
    static_assert(defaultVector.X == 0 && defaultVector.Y == 0 && defaultVector.Z == 0, "Default constructor failed");

    constexpr Vector3<float> initializedVector(1.0f, 2.0f, 3.0f);
    static_assert(initializedVector.X == 1.0f && initializedVector.Y == 2.0f && initializedVector.Z == 3.0f, "Parameterized constructor failed");

    // Tests for Operators
    static_assert((Vector3<int>(1, 2, 3) + Vector3<int>(1, 1, 1)) == Vector3<int>(2, 3, 4), "Addition operator failed");

    static_assert((Vector3<int>(5, 5, 5) - Vector3<int>(2, 2, 2)) == Vector3<int>(3, 3, 3), "Subtraction operator failed");

    static_assert((Vector3<int>(1, 2, 3) * 2) == Vector3<int>(2, 4, 6), "Multiplication operator failed");

    static_assert((Vector3<int>(6, 8, 10) / 2) == Vector3<int>(3, 4, 5), "Division operator failed");
}