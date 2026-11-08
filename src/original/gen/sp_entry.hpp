// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SpEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct I32Array;

#pragma pack(push, 1)
struct SpEntry {
#include "sp_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SpEntry) == 0x58);
static_assert(offsetof(SpEntry, key) == 0x0);
static_assert(offsetof(SpEntry, key_flag) == 0x4);
static_assert(offsetof(SpEntry, active) == 0x8);
static_assert(offsetof(SpEntry, a) == 0xc);
static_assert(offsetof(SpEntry, b) == 0x10);
static_assert(offsetof(SpEntry, c) == 0x14);
static_assert(offsetof(SpEntry, d) == 0x18);
static_assert(offsetof(SpEntry, x0) == 0x1c);
static_assert(offsetof(SpEntry, y0) == 0x20);
static_assert(offsetof(SpEntry, x1) == 0x24);
static_assert(offsetof(SpEntry, y1) == 0x28);
static_assert(offsetof(SpEntry, grid_a) == 0x2c);
static_assert(offsetof(SpEntry, inline_a) == 0x30);
static_assert(offsetof(SpEntry, grid_b) == 0x40);
static_assert(offsetof(SpEntry, inline_b) == 0x44);
static_assert(offsetof(SpEntry, capacity) == 0x54);
#endif

}  // namespace joan::orig
