// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Overlapped class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Overlapped {
#include "overlapped_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Overlapped) == 0x14);
static_assert(offsetof(Overlapped, internal) == 0x0);
static_assert(offsetof(Overlapped, internal_high) == 0x4);
static_assert(offsetof(Overlapped, offset) == 0x8);
static_assert(offsetof(Overlapped, offset_high) == 0xc);
static_assert(offsetof(Overlapped, event) == 0x10);
#endif

}  // namespace joan::orig
