#include "Aligned.h"

// static_assert([]{
//     constexpr Aligned<uint8_t[16]> alignedArray = {};
//     auto address = (uintptr_t) &alignedArray.Value[0];
//     return address % 16 == 0;
// }());