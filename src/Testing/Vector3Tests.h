#include <numbers>
#include "Vector3.h"
#include "Temperature.h"
#include "TestSupport.h"


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

    static_assert((Vector3<double>(1, 2, 3) * 0.1) == Vector3<double>(1 * 0.1, 2 * 0.1, 3 * 0.1),
        "Multiplication truncated a double");
    static_assert((Vector3<double>(1, 2, 3) / 10.0) == Vector3<double>(1 / 10.0, 2 / 10.0, 3 / 10.0),
        "Division truncated a double");
    static_assert((Vector3<float>(1, 2, 3) * 0.5) == Vector3<float>(0.5f, 1.0f, 1.5f), "Float vector by a double failed");

    template<typename TVector, typename TScalar>
    concept CanScale = requires(TVector vector, TScalar scalar) { vector * scalar; vector / scalar; };

    static_assert(CanScale<Vector3<int>, int> && CanScale<Vector3<float>, double> && CanScale<Vector3<double>, int>,
        "Scaling failed to compile");
    static_assert(!CanScale<Vector3<int>, float> && !CanScale<Vector3<int>, double>,
        "An integer vector scaled by a fraction");

    // Unit-typed vectors scale by plain numbers, as Averager<Vector3<Temperature>> does.
    static_assert((Vector3<Temperature>(Temperature::FromKelvin(300), Temperature::FromKelvin(200),
        Temperature::FromKelvin(100)) * 0.5f).X == Temperature::FromKelvin(150), "Unit-typed scaling failed");

    constexpr double Epsilon = 1e-12;

    constexpr bool AreAlmostEqual(const Vector3<double>& a, const Vector3<double>& b)
    {
        return ::AreAlmostEqual(a.X, b.X, Epsilon) && ::AreAlmostEqual(a.Y, b.Y, Epsilon)
            && ::AreAlmostEqual(a.Z, b.Z, Epsilon);
    }

    static_assert(Vector3<double>(2, 3, 6).LengthSquared() == 49, "LengthSquared failed");
    static_assert(Vector3<double>(2, 3, 6).Length() == 7, "Length failed");
    static_assert(Vector3<double>(0, 3, 4).Normalize() == Vector3<double>(0, 0.6, 0.8), "Normalize failed");
    static_assert(Vector3<double>().Normalize() == Vector3<double>(), "Normalize made zero NaN");

    static_assert(Vector3<double>(1, 2, 3).ProjectOntoPlane(Vector3<double>::ZAxis) == Vector3<double>(1, 2, 0),
        "ProjectOntoPlane failed");

    static_assert(AreAlmostEqual(Vector3<double>::XAxis.RotationTo(Vector3<double>::YAxis),
        Vector3<double>(0, 0, std::numbers::pi / 2)), "RotationTo failed");
    static_assert(Vector3<double>::XAxis.RotationTo(Vector3<double>::XAxis) == Vector3<double>(),
        "RotationTo failed for parallel vectors");

    static_assert(AreAlmostEqual(Vector3<double>::XAxis.Rotate(Vector3<double>(0, 0, std::numbers::pi / 2)),
        Vector3<double>::YAxis), "Rotate by a rotation vector failed");
    static_assert(Vector3<double>(1, 2, 3).Rotate(Vector3<double>()) == Vector3<double>(1, 2, 3),
        "Rotate by zero failed");

    static_assert([]
    {
        const auto from = Vector3<double>(1, 2, 2) / 3.0;
        const auto to = Vector3<double>(2, -1, 2) / 3.0;
        return AreAlmostEqual(from.Rotate(from.RotationTo(to)), to);
    }(), "Rotate didn't undo RotationTo");

    // Agrees with Rotate(Angle, axis), which is float, as Angle is.
    static_assert([]
    {
        const auto vector = Vector3<float>(1, -2, 0.5f);
        const auto rotation = Vector3<float>(0.3f, -0.2f, 0.5f);
        const auto expected = vector.Rotate(Angle::FromRadians(rotation.Length()), rotation.Normalize());
        const auto actual = vector.Rotate(rotation);

        return ::AreAlmostEqual(actual.X, expected.X, 1e-5f) && ::AreAlmostEqual(actual.Y, expected.Y, 1e-5f)
            && ::AreAlmostEqual(actual.Z, expected.Z, 1e-5f);
    }(), "Rotate by a rotation vector disagreed with Rotate(Angle, axis)");
}