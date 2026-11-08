// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MorphAnim class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct MorphTrack;

#pragma pack(push, 1)
struct MorphAnim {
#include "morph_anim_fields.inc"
#include "morph_anim_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MorphAnim) == 0x10);
static_assert(offsetof(MorphAnim, loaded) == 0x0);
static_assert(offsetof(MorphAnim, track_count) == 0x4);
static_assert(offsetof(MorphAnim, tracks) == 0x8);
static_assert(offsetof(MorphAnim, length) == 0xc);
#endif

}  // namespace joan::orig
