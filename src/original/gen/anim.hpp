// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Anim class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct AnimPtr;

#pragma pack(push, 1)
struct Anim {
#include "anim_fields.inc"
#include "anim_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Anim, ptr_0) == 0x0);
static_assert(offsetof(Anim, ptr_4) == 0x4);
static_assert(offsetof(Anim, f32_8) == 0x8);
static_assert(offsetof(Anim, f32_c) == 0xc);
static_assert(offsetof(Anim, bytes_10) == 0x10);
static_assert(offsetof(Anim, u32_18) == 0x18);
static_assert(offsetof(Anim, u32_1c) == 0x1c);
static_assert(offsetof(Anim, group) == 0x20);
static_assert(offsetof(Anim, members) == 0x24);
static_assert(offsetof(Anim, u32_28) == 0x28);
static_assert(offsetof(Anim, grid_x) == 0x2c);
static_assert(offsetof(Anim, grid_y) == 0x30);
static_assert(offsetof(Anim, u16_34) == 0x34);
static_assert(offsetof(Anim, member_count) == 0x36);
static_assert(offsetof(Anim, bytes_38) == 0x38);
static_assert(offsetof(Anim, cost_so_far) == 0x3c);
static_assert(offsetof(Anim, cost_total) == 0x40);
static_assert(offsetof(Anim, cost_to_goal) == 0x44);
static_assert(offsetof(Anim, weight) == 0x48);
static_assert(offsetof(Anim, weight_max) == 0x4a);
static_assert(offsetof(Anim, weight_sum) == 0x4c);
static_assert(offsetof(Anim, ptr_50) == 0x50);
static_assert(offsetof(Anim, u32_58) == 0x58);
static_assert(offsetof(Anim, u32_78) == 0x78);
static_assert(offsetof(Anim, u32_7c) == 0x7c);
static_assert(offsetof(Anim, u32_80) == 0x80);
static_assert(offsetof(Anim, u32_8c) == 0x8c);
static_assert(offsetof(Anim, ptr_90) == 0x90);
static_assert(offsetof(Anim, u32_94) == 0x94);
#endif

}  // namespace joan::orig
