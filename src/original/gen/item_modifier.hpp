// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ItemModifier class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ItemModifier {
#include "item_modifier_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ItemModifier) == 0xc);
static_assert(offsetof(ItemModifier, kind) == 0x0);
static_assert(offsetof(ItemModifier, base) == 0x4);
static_assert(offsetof(ItemModifier, per_level) == 0x8);
#endif

}  // namespace joan::orig
