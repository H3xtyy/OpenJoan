// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Tree class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DynArray;

#pragma pack(push, 1)
struct Tree {
#include "tree_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Tree, vtable) == 0x0);
static_assert(offsetof(Tree, parent) == 0x4);
static_assert(offsetof(Tree, id) == 0x14);
static_assert(offsetof(Tree, children) == 0x1c);
#endif

}  // namespace joan::orig
