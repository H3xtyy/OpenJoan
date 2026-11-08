// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LodEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "lod_level.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LodEntry {
#include "lod_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LodEntry) == 0x28);
static_assert(offsetof(LodEntry, sound) == 0x0);
static_assert(offsetof(LodEntry, resource_id) == 0x4);
static_assert(offsetof(LodEntry, level_count) == 0x6);
static_assert(offsetof(LodEntry, levels) == 0x8);
#endif

}  // namespace joan::orig
