// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ReorderGroup class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ReorderBatch;

#pragma pack(push, 1)
struct ReorderGroup {
#include "reorder_group_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ReorderGroup) == 0x10);
static_assert(offsetof(ReorderGroup, count) == 0x0);
static_assert(offsetof(ReorderGroup, capacity) == 0x4);
static_assert(offsetof(ReorderGroup, cursor) == 0x8);
static_assert(offsetof(ReorderGroup, batches) == 0xc);
#endif

}  // namespace joan::orig
