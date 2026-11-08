// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BoxTree class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct BoxTree {
#include "box_tree_fields.inc"
#include "box_tree_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BoxTree) == 0x38);
static_assert(offsetof(BoxTree, u32_0) == 0x0);
static_assert(offsetof(BoxTree, left) == 0x4);
static_assert(offsetof(BoxTree, right) == 0x8);
static_assert(offsetof(BoxTree, u32_c) == 0xc);
static_assert(offsetof(BoxTree, box) == 0x10);
static_assert(offsetof(BoxTree, mid) == 0x28);
static_assert(offsetof(BoxTree, item) == 0x34);
#endif

}  // namespace joan::orig
