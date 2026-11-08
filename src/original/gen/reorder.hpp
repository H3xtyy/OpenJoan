// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Reorder class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "batch_key.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct Bone;
struct ReorderBatch;
struct ReorderGroup;

#pragma pack(push, 1)
struct Reorder {
#include "reorder_fields.inc"
#include "reorder_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Reorder) == 0xb2);
static_assert(offsetof(Reorder, lights) == 0x0);
static_assert(offsetof(Reorder, sectors) == 0x2e);
static_assert(offsetof(Reorder, materials) == 0x5c);
static_assert(offsetof(Reorder, group_count) == 0x8a);
static_assert(offsetof(Reorder, group_capacity) == 0x8e);
static_assert(offsetof(Reorder, current_group) == 0x92);
static_assert(offsetof(Reorder, groups) == 0x96);
static_assert(offsetof(Reorder, current) == 0x9a);
static_assert(offsetof(Reorder, key) == 0x9e);
#endif

}  // namespace joan::orig
