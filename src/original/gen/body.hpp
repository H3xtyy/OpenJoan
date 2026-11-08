// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Body class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Actor;
struct PathSpace;

#pragma pack(push, 1)
struct Body {
#include "body_fields.inc"
#include "body_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Body, actor) == 0x8);
static_assert(offsetof(Body, zone) == 0x20e);
static_assert(offsetof(Body, path_space) == 0x212);
static_assert(offsetof(Body, f32_218) == 0x218);
static_assert(offsetof(Body, anim_slot) == 0x27a);
#endif

}  // namespace joan::orig
