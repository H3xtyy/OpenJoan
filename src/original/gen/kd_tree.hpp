// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's KdTree class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PfdZone;

#pragma pack(push, 1)
struct KdTree {
#include "kd_tree_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(KdTree, left) == 0x4);
static_assert(offsetof(KdTree, right) == 0x8);
static_assert(offsetof(KdTree, min) == 0x10);
static_assert(offsetof(KdTree, max) == 0x1c);
static_assert(offsetof(KdTree, zone) == 0x34);
#endif

}  // namespace joan::orig
