#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

template <typename Derived, bool Signed = true, typename TValue = float>
    requires(std::is_floating_point_v<TValue>)
class Unit
{
public:
    using ValueType = TValue;
    constexpr Unit() = default;

    /* same‑unit arithmetic */
    constexpr Derived operator+(const Derived& rhs) const { return Derived{ _value + rhs._value }; }
    constexpr Derived operator-(const Derived& rhs) const { return Derived{ _value - rhs._value }; }
    constexpr Derived operator-() const requires(Signed) { return Derived(-_value); }
    constexpr ValueType operator/(const Derived& rhs) const { return _value / rhs._value; }
    constexpr Derived operator%(const Derived& rhs) const requires(Signed)
    {
        return Derived{ std::fmod(_value, rhs._value) };
    }

    /* scaling by scalar */
    constexpr Derived operator*(ValueType k) const { return Derived{ _value * k }; }
    constexpr Derived operator/(ValueType k) const { return Derived{ _value / k }; }

    /* compound assignment */
    constexpr Derived& operator+=(const Derived& rhs) { _value += rhs._value; return self(); }
    constexpr Derived& operator-=(const Derived& rhs) { _value -= rhs._value; return self(); }
    constexpr Derived& operator*=(ValueType k) { _value *= k; return self(); }
    constexpr Derived& operator/=(ValueType k) { _value /= k; return self(); }

    /* comparisons */
    constexpr bool operator==(const Derived& rhs) const { return _value == rhs._value; }
    constexpr auto operator<=>(const Derived& rhs) const { return _value <=> rhs._value; }

    /* scalar on the left */
    friend constexpr Derived operator*(ValueType k, const Derived& u)
    {
        return Derived{ k * u._value };
    }

    friend constexpr Derived operator/(ValueType k, const Derived& u)
    {
        return Derived{ k / u._value };
    }

    static constexpr Derived Zero()
    {
        return Derived(static_cast<ValueType>(0));
    }

    static constexpr Derived PositiveInfinity()
    {
        return Derived(std::numeric_limits<ValueType>::infinity());
    }

    static constexpr Derived NegativeInfinity() requires(Signed)
    {
        return Derived(-std::numeric_limits<ValueType>::infinity());
    }

    constexpr Derived Magnitude() const requires(Signed)
    {
        return Derived(_value < static_cast<ValueType>(0) ? -_value : _value);
    }

    constexpr Derived EuclideanModulo(const Derived& mod) const requires(Signed)
    {
        return Derived{ std::fmod(_value + mod._value, mod._value) };
    }


protected:
    constexpr explicit Unit(ValueType value) : _value{ value } {}
    ~Unit() = default;

    /* factory */
    static constexpr Derived FromMicrounits(ValueType microunits) { return Derived(microunits * u); }
    static constexpr Derived FromMilliunits(ValueType milliunits) { return Derived(milliunits * m); }
    static constexpr Derived FromCentiunits(ValueType centiunits) { return Derived(centiunits * c); }
    static constexpr Derived FromUnits(ValueType units) { return Derived(units); }
    static constexpr Derived FromKilounits(ValueType kilounits) { return Derived(kilounits * k); }
    static constexpr Derived FromMegaunits(ValueType megaunits) { return Derived(megaunits * M); }

    /* conversion */
    constexpr ValueType ToMicrounits() const { return _value * M; }
    constexpr ValueType ToMilliunits() const { return _value * k; }
    constexpr ValueType ToCentiunits() const { return _value * c; }
    constexpr ValueType ToUnits() const { return _value; }
    constexpr ValueType ToKilounits() const { return _value * m; }
    constexpr ValueType ToMegaunits() const { return _value * u; }

private:
    static constexpr ValueType p = 1e-12;
    static constexpr ValueType n = 1e-9;
    static constexpr ValueType u = 1e-6; 
    static constexpr ValueType m = 1e-3;
    static constexpr ValueType c = 1e-2;
    static constexpr ValueType k = 1e3;
    static constexpr ValueType M = 1e6;

    ValueType _value{ static_cast<ValueType>(0) };

    constexpr Derived& self() { return static_cast<Derived&>(*this); }
};
