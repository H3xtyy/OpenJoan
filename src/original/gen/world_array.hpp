// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WorldArray class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct World;

#pragma pack(push, 1)
struct WorldArray : DynArrayB {
#include "world_array_fields.inc"
#include "world_array_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(WorldArray, active) == 0x2e);
static_assert(offsetof(WorldArray, selected) == 0x32);
static_assert(offsetof(WorldArray, u32_36) == 0x36);
#endif

}  // namespace joan::orig
