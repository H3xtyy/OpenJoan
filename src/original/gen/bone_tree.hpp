// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BoneTree class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct BoneNode;

#pragma pack(push, 1)
struct BoneTree {
#include "bone_tree_fields.inc"
#include "bone_tree_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BoneTree) == 0xc);
static_assert(offsetof(BoneTree, count) == 0x0);
static_assert(offsetof(BoneTree, block) == 0x4);
static_assert(offsetof(BoneTree, nodes) == 0x8);
#endif

}  // namespace joan::orig
