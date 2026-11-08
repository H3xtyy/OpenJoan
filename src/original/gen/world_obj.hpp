// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WorldObj class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Group;

#pragma pack(push, 1)
struct WorldObj {
#include "world_obj_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(WorldObj, id) == 0x0);
static_assert(offsetof(WorldObj, nation) == 0x2);
static_assert(offsetof(WorldObj, group) == 0x33);
#endif

}  // namespace joan::orig
