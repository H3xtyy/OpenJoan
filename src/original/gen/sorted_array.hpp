// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SortedArray class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SortedArray {
#include "sorted_array_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SortedArray) == 0xc);
static_assert(offsetof(SortedArray, count) == 0x0);
static_assert(offsetof(SortedArray, capacity) == 0x4);
static_assert(offsetof(SortedArray, items) == 0x8);
#endif

}  // namespace joan::orig
