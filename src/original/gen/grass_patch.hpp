// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's GrassPatch class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Scene;

#pragma pack(push, 1)
struct GrassPatch {
#include "grass_patch_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(GrassPatch) == 0x66);
static_assert(offsetof(GrassPatch, owner) == 0x0);
static_assert(offsetof(GrassPatch, box) == 0x8);
static_assert(offsetof(GrassPatch, grid_x) == 0x20);
static_assert(offsetof(GrassPatch, grid_y) == 0x22);
static_assert(offsetof(GrassPatch, scene) == 0x24);
static_assert(offsetof(GrassPatch, object) == 0x28);
#endif

}  // namespace joan::orig
