# ZCore: guidance for agents

ZCore is a generic C++20 layer over Zephyr. Several apps share it as a git submodule.

## Wrap Zephyr's APIs

ZCore wraps Zephyr's APIs, as far as they go. Write a class against a Zephyr driver or
subsystem API, not a peripheral's registers or a vendor's HAL. It then works on any SoC
Zephyr has a driver for, and can be tested on native_sim.

- Where Zephyr has no driver for some hardware an app needs, write one out of tree, in
  the app's own Zephyr module, against the API the ZCore class wraps. `AesGcm` wraps
  Zephyr's crypto API, for instance, and the STM32N6's CRYP has its driver in the app
  that needed it.
- Only where Zephyr has no API at all should ZCore reach past it. Keep that part as small
  as possible, and `#error` on SoCs it doesn't support.

Some older code here reaches past Zephyr; don't take it as the pattern.

## Favor safe memory operations

Changes here should heavily favor operations that are checked, or that can't go out of
range at all, over C-style array indexing and pointer arithmetic. That holds even where
an index is plainly in range today: the next edit may not keep it so.

Prefer, roughly in this order:

- A range-for, or `CopyTo`, where every element is visited.
- `Take` and `Skip` to narrow a view. They clamp rather than fail.
- `SpanWriter` and `SpanReader` for sequential access, including multi-byte values in
  either byte order. `SpanWriter<char>` builds text.
- `Get<Index>()` and `Set<Index>()` on `Array` and `FixedSpan` where the index is a
  constant. They're checked at compile time.
- `Get`, `Set`, `TryGet`, `TrySet` and `TryCompare` where the index is computed.
- `FixedSpan<T, N>::FromArray` to wrap a C array, its length checked at compile time.
- Computing a value instead of looking it up, where that's as clear (`Base64::ToSymbol`).

Avoid:

- `operator[]`, on C arrays and on `Array` and `FixedSpan` alike.
- Pointer arithmetic, and indexing through `GetData()`. `GetData()` is only for handing
  a buffer to a C API.
- `memcpy`, `memset`, `strlen` and the other C memory and string functions.
- `reinterpret_cast` over a buffer, outside the helpers that exist for it (`AsBytes`,
  `AsConstBytes`, `MemoryMarshal`).

Where one of these is truly needed, such as filling a C API's own structure, keep it to
the smallest scope and say why in a comment. Some older code here still indexes; don't
take it as the pattern.
