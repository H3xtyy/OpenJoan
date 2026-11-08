// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimEvent class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct AnimEvent {
#include "anim_event_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AnimEvent) == 0x3c);
static_assert(offsetof(AnimEvent, time) == 0x0);
static_assert(offsetof(AnimEvent, bone) == 0x4);
static_assert(offsetof(AnimEvent, name) == 0x8);
static_assert(offsetof(AnimEvent, flag) == 0x3b);
#endif

}  // namespace joan::orig
