// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's EmitterSlot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct ActorDef;

#pragma pack(push, 1)
struct EmitterSlot {
#include "emitter_slot_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(EmitterSlot, active) == 0x0);
static_assert(offsetof(EmitterSlot, def) == 0x141);
static_assert(offsetof(EmitterSlot, children) == 0x145);
#endif

}  // namespace joan::orig
