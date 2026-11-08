// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimKey class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct AnimKey {
#include "anim_key_fields.inc"
#include "anim_key_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AnimKey) == 0x20);
static_assert(offsetof(AnimKey, time) == 0x0);
static_assert(offsetof(AnimKey, rot) == 0x4);
static_assert(offsetof(AnimKey, pos) == 0x14);
#endif

}  // namespace joan::orig
