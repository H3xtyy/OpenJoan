// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ItemRequirement class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ItemRequirement {
#include "item_requirement_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ItemRequirement) == 0x10);
static_assert(offsetof(ItemRequirement, kind) == 0x0);
static_assert(offsetof(ItemRequirement, base) == 0x4);
static_assert(offsetof(ItemRequirement, per_level) == 0x8);
static_assert(offsetof(ItemRequirement, is_min) == 0xc);
#endif

}  // namespace joan::orig
