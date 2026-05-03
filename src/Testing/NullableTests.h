#pragma once

#include "Nullable.h"

namespace NullableTests
{
    constexpr Nullable<int> empty;
    static_assert(!empty.HasValue(), "Default construction failed");
    static_assert(empty.GetValueOrDefault() == 0, "Default value failed");
    static_assert(empty != 1, "Empty inequality failed");

    constexpr Nullable<int> initialized(7);
    static_assert(initialized.HasValue(), "Value construction failed");
    static_assert(initialized.GetValue() == 7, "GetValue failed");
    static_assert(initialized.GetValueOrDefault() == 7, "GetValueOrDefault failed");
    static_assert(initialized == 7, "Equality failed");
    static_assert(initialized != 8, "Inequality failed");

    static_assert([]{
        Nullable<int> value;
        value = 3;
        return value.HasValue() && value.GetValue() == 3;
    }(), "Assignment failed");

    static_assert([]{
        const Nullable<int> value(5);
        int result = 0;
        return value.TryGetValue(result) && result == 5;
    }(), "TryGetValue failed");

    static_assert([]{
        Nullable<int> value(5);
        value.Clear();
        return !value.HasValue() && value.GetValueOrDefault() == 0 && value != 5;
    }(), "Clear failed");
}