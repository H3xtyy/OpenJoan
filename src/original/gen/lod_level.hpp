// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LodLevel class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LodLevel {
#include "lod_level_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LodLevel) == 0x8);
static_assert(offsetof(LodLevel, value) == 0x0);
static_assert(offsetof(LodLevel, i32_4) == 0x4);
#endif

}  // namespace joan::orig
