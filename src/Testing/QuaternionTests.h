#pragma once

#include <numbers>
#include "Quaternion.h"
#include "TestSupport.h"

namespace QuaternionTests
{
    constexpr double Epsilon = 1e-12;

    constexpr bool AreAlmostEqual(const Vector3<double>& a, const Vector3<double>& b)
    {
        return ::AreAlmostEqual(a.X, b.X, Epsilon) && ::AreAlmostEqual(a.Y, b.Y, Epsilon)
            && ::AreAlmostEqual(a.Z, b.Z, Epsilon);
    }

    static_assert(Quaternion<double>::Identity.Rotate(Vector3<double>(1, 2, 3)) == Vector3<double>(1, 2, 3),
        "Identity failed");

    static_assert(Quaternion<double>::FromRotationVector(Vector3<double>()) == Quaternion<double>::Identity,
        "FromRotationVector failed for zero");

    static_assert(AreAlmostEqual(
        Quaternion<double>::FromRotationVector(Vector3<double>(0, 0, std::numbers::pi / 2)).Rotate(Vector3<double>::XAxis),
        Vector3<double>::YAxis), "FromRotationVector failed");

    static_assert([]
    {
        const auto quarter = Quaternion<double>::FromRotationVector(Vector3<double>(std::numbers::pi / 2, 0, 0));
        return AreAlmostEqual((quarter * quarter).Rotate(Vector3<double>::YAxis), Vector3<double>(0, -1, 0));
    }(), "Two quarter turns didn't make a half turn");

    // Agrees with rotating the vector directly.
    static_assert([]
    {
        const auto rotation = Vector3<double>(0.3, -0.2, 0.5);
        const auto vector = Vector3<double>(1, 2, 3);
        return AreAlmostEqual(Quaternion<double>::FromRotationVector(rotation).Rotate(vector), vector.Rotate(rotation));
    }(), "FromRotationVector disagreed with Vector3::Rotate");

    // Tiny angles take the small-angle branch.
    static_assert(AreAlmostEqual(
        Quaternion<double>::FromRotationVector(Vector3<double>(0, 0, 1e-9)).Rotate(Vector3<double>::XAxis),
        Vector3<double>(1, 1e-9, 0)), "FromRotationVector failed for a tiny angle");
}
