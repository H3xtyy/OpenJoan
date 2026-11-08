// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Group class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Group {
#include "group_fields.inc"
#include "group_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Group) == 0x58);
static_assert(offsetof(Group, leader) == 0x0);
static_assert(offsetof(Group, count) == 0x2);
static_assert(offsetof(Group, members) == 0x3);
static_assert(offsetof(Group, class_counts) == 0x2b);
static_assert(offsetof(Group, formation) == 0x37);
static_assert(offsetof(Group, x) == 0x38);
static_assert(offsetof(Group, y) == 0x3c);
static_assert(offsetof(Group, z) == 0x40);
static_assert(offsetof(Group, angle) == 0x44);
static_assert(offsetof(Group, front_x) == 0x48);
static_assert(offsetof(Group, front_y) == 0x4c);
static_assert(offsetof(Group, u32_50) == 0x50);
static_assert(offsetof(Group, u32_54) == 0x54);
#endif

}  // namespace joan::orig
