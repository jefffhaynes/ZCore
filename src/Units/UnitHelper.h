#pragma once

#include "Unit.h"


// friend class that can get underlying values
class UnitHelper
{
public:
    template<typename Derived, bool Signed, typename TValue>
    static constexpr typename Unit<Derived, Signed, TValue>::ValueType ToRawValue(const Unit<Derived, Signed, TValue> unit)
    {
        return unit._value;
    }

    template<typename Derived, bool Signed, typename TValue>
    static void FromRawValue(typename Unit<Derived, Signed, TValue>::ValueType value, Unit<Derived, Signed, TValue>& unit)
    {
        unit = Unit<Derived, Signed, TValue>(value);
    }
};