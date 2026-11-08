// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LodQueue class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Audio;
struct LodEntry;

#pragma pack(push, 1)
struct LodQueue {
#include "lod_queue_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(LodQueue, u32_0) == 0x0);
static_assert(offsetof(LodQueue, entries) == 0x4);
static_assert(offsetof(LodQueue, flags) == 0x8);
static_assert(offsetof(LodQueue, active_count) == 0xc);
static_assert(offsetof(LodQueue, arr_10) == 0x10);
static_assert(offsetof(LodQueue, enabled) == 0x54);
static_assert(offsetof(LodQueue, entry_count) == 0x60);
static_assert(offsetof(LodQueue, audio) == 0x64);
#endif

}  // namespace joan::orig
