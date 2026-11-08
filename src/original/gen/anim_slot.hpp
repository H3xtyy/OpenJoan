// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimSlot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct AnimDef;

#pragma pack(push, 1)
struct AnimSlot {
#include "anim_slot_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AnimSlot) == 0x130);
static_assert(offsetof(AnimSlot, anim) == 0x0);
static_assert(offsetof(AnimSlot, path) == 0x4);
static_assert(offsetof(AnimSlot, name) == 0x105);
static_assert(offsetof(AnimSlot, load_state) == 0x126);
static_assert(offsetof(AnimSlot, refs) == 0x128);
static_assert(offsetof(AnimSlot, buffer) == 0x12c);
#endif

}  // namespace joan::orig
