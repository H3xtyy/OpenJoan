// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SoundSlots class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SoundSlots {
#include "sound_slots_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SoundSlots) == 0xb8);
static_assert(offsetof(SoundSlots, u32_0) == 0x0);
static_assert(offsetof(SoundSlots, u32_4) == 0x4);
static_assert(offsetof(SoundSlots, u32_48) == 0x48);
static_assert(offsetof(SoundSlots, slots) == 0x50);
#endif

}  // namespace joan::orig
