// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MaterialSlot class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../en3d.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MaterialSlot {
#include "material_slot_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MaterialSlot) == 0x5c);
static_assert(offsetof(MaterialSlot, slot) == 0x0);
static_assert(offsetof(MaterialSlot, frame) == 0x4);
static_assert(offsetof(MaterialSlot, material) == 0x8);
#endif

}  // namespace joan::orig
