// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Effects class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct EffectShaderPtrArray;

#pragma pack(push, 1)
struct Effects {
#include "effects_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Effects, shaders) == 0x8);
static_assert(offsetof(Effects, shader_count) == 0x10);
static_assert(offsetof(Effects, library) == 0x1c);
#endif

}  // namespace joan::orig
