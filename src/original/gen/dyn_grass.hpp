// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's DynGrass class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct GrassPatch;

#pragma pack(push, 1)
struct DynGrass {
#include "dyn_grass_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(DynGrass, density) == 0x0);
static_assert(offsetof(DynGrass, enabled) == 0x4);
static_assert(offsetof(DynGrass, patches) == 0x5);
static_assert(offsetof(DynGrass, patch_count) == 0x9);
static_assert(offsetof(DynGrass, patches2) == 0xd);
static_assert(offsetof(DynGrass, patch2_count) == 0x11);
static_assert(offsetof(DynGrass, res_ids) == 0x15);
static_assert(offsetof(DynGrass, res_count) == 0x1a5);
static_assert(offsetof(DynGrass, wind) == 0x1a9);
static_assert(offsetof(DynGrass, pos) == 0x1b5);
static_assert(offsetof(DynGrass, last_pos) == 0x1c1);
static_assert(offsetof(DynGrass, f32_1cd) == 0x1cd);
#endif

}  // namespace joan::orig
