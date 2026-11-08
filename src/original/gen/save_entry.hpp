// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SaveEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct SceneS;

#pragma pack(push, 1)
struct SaveEntry {
#include "save_entry_fields.inc"
#include "save_entry_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SaveEntry) == 0x43);
static_assert(offsetof(SaveEntry, type) == 0x0);
static_assert(offsetof(SaveEntry, target_kind) == 0x4);
static_assert(offsetof(SaveEntry, target_id) == 0x8);
static_assert(offsetof(SaveEntry, ref_kind) == 0xa);
static_assert(offsetof(SaveEntry, ref_id) == 0xe);
static_assert(offsetof(SaveEntry, sub) == 0x10);
static_assert(offsetof(SaveEntry, u8_12) == 0x12);
static_assert(offsetof(SaveEntry, values) == 0x13);
static_assert(offsetof(SaveEntry, range) == 0x1f);
static_assert(offsetof(SaveEntry, follow_ref) == 0x23);
static_assert(offsetof(SaveEntry, next_time) == 0x26);
static_assert(offsetof(SaveEntry, expire_time) == 0x2a);
static_assert(offsetof(SaveEntry, active) == 0x2e);
static_assert(offsetof(SaveEntry, scene) == 0x3f);
#endif

}  // namespace joan::orig
