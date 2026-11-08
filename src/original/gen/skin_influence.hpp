// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SkinInfluence class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SkinInfluence {
#include "skin_influence_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SkinInfluence) == 0x20);
static_assert(offsetof(SkinInfluence, bone) == 0x0);
static_assert(offsetof(SkinInfluence, pos) == 0x4);
static_assert(offsetof(SkinInfluence, normal) == 0x10);
static_assert(offsetof(SkinInfluence, weight) == 0x1c);
#endif

}  // namespace joan::orig
