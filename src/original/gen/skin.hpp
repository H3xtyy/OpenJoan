// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Skin class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct SkinVertex;

#pragma pack(push, 1)
struct Skin {
#include "skin_fields.inc"
#include "skin_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Skin) == 0x3e);
static_assert(offsetof(Skin, vertex_count) == 0x0);
static_assert(offsetof(Skin, vertex_cap) == 0x4);
static_assert(offsetof(Skin, vertices) == 0x8);
static_assert(offsetof(Skin, i32_c) == 0xc);
static_assert(offsetof(Skin, center) == 0x10);
static_assert(offsetof(Skin, radius) == 0x1c);
static_assert(offsetof(Skin, slots) == 0x20);
static_assert(offsetof(Skin, u8_30) == 0x30);
static_assert(offsetof(Skin, slot_index) == 0x31);
static_assert(offsetof(Skin, owner) == 0x32);
static_assert(offsetof(Skin, shadow) == 0x36);
static_assert(offsetof(Skin, shadow_of) == 0x3a);
#endif

}  // namespace joan::orig
