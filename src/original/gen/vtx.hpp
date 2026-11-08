// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Vtx class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Vtx {
#include "vtx_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Vtx, pos_x) == 0x0);
static_assert(offsetof(Vtx, pos_y) == 0x4);
static_assert(offsetof(Vtx, pos_z) == 0x8);
static_assert(offsetof(Vtx, u32_c) == 0xc);
static_assert(offsetof(Vtx, u32_10) == 0x10);
static_assert(offsetof(Vtx, u32_14) == 0x14);
static_assert(offsetof(Vtx, u32_18) == 0x18);
static_assert(offsetof(Vtx, arr_1c) == 0x1c);
static_assert(offsetof(Vtx, arr_20) == 0x20);
static_assert(offsetof(Vtx, u32_24) == 0x24);
#endif

}  // namespace joan::orig
