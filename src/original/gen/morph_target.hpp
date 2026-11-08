// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MorphTarget class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Vec3f;

#pragma pack(push, 1)
struct MorphTarget {
#include "morph_target_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MorphTarget) == 0x44);
static_assert(offsetof(MorphTarget, name) == 0x0);
static_assert(offsetof(MorphTarget, positions) == 0x34);
static_assert(offsetof(MorphTarget, position_deltas) == 0x38);
static_assert(offsetof(MorphTarget, normals) == 0x3c);
static_assert(offsetof(MorphTarget, normal_deltas) == 0x40);
#endif

}  // namespace joan::orig
