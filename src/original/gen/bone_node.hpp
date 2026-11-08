// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BoneNode class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct BoneNode {
#include "bone_node_fields.inc"
#include "bone_node_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BoneNode) == 0x1c);
static_assert(offsetof(BoneNode, index) == 0x0);
static_assert(offsetof(BoneNode, name_id) == 0x4);
static_assert(offsetof(BoneNode, parent) == 0x8);
static_assert(offsetof(BoneNode, prev_sibling) == 0xc);
static_assert(offsetof(BoneNode, next_sibling) == 0x10);
static_assert(offsetof(BoneNode, last_child) == 0x14);
static_assert(offsetof(BoneNode, first_child) == 0x18);
#endif

}  // namespace joan::orig
