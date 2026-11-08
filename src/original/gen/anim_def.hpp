// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimDef class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct AlphaTrack;
struct AnimAction;
struct ArrI32;
struct BoneTree;
struct ColourTrack;
struct EventList;
struct Track;

#pragma pack(push, 1)
struct AnimDef {
#include "anim_def_fields.inc"
#include "anim_def_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(AnimDef, loaded) == 0x0);
static_assert(offsetof(AnimDef, source_set) == 0x1);
static_assert(offsetof(AnimDef, u16_02) == 0x2);
static_assert(offsetof(AnimDef, dir) == 0x6);
static_assert(offsetof(AnimDef, name) == 0x107);
static_assert(offsetof(AnimDef, track_count) == 0x128);
static_assert(offsetof(AnimDef, action_count) == 0x12c);
static_assert(offsetof(AnimDef, start) == 0x130);
static_assert(offsetof(AnimDef, end) == 0x134);
static_assert(offsetof(AnimDef, actions) == 0x138);
static_assert(offsetof(AnimDef, sort_index) == 0x13c);
static_assert(offsetof(AnimDef, tracks) == 0x140);
static_assert(offsetof(AnimDef, bones) == 0x144);
static_assert(offsetof(AnimDef, bones_resolved) == 0x148);
static_assert(offsetof(AnimDef, child_count) == 0x14c);
static_assert(offsetof(AnimDef, quiet) == 0x14e);
static_assert(offsetof(AnimDef, extra_count) == 0x14f);
static_assert(offsetof(AnimDef, colour_tracks) == 0x153);
static_assert(offsetof(AnimDef, alpha_tracks) == 0x157);
static_assert(offsetof(AnimDef, events) == 0x15b);
static_assert(offsetof(AnimDef, children) == 0x15f);
static_assert(offsetof(AnimDef, loader_17f) == 0x17f);
static_assert(offsetof(AnimDef, loader_183) == 0x183);
static_assert(offsetof(AnimDef, loader_187) == 0x187);
#endif

}  // namespace joan::orig
