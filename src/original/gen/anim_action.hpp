// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimAction class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct AnimAction {
#include "anim_action_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AnimAction) == 0x41);
static_assert(offsetof(AnimAction, name) == 0x0);
static_assert(offsetof(AnimAction, flag) == 0x33);
static_assert(offsetof(AnimAction, value) == 0x34);
static_assert(offsetof(AnimAction, start) == 0x38);
static_assert(offsetof(AnimAction, end) == 0x3c);
static_assert(offsetof(AnimAction, keys_loaded) == 0x40);
#endif

}  // namespace joan::orig
