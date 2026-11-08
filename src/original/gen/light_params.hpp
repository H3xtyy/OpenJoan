// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightParams class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LightParams {
#include "light_params_fields.inc"
#include "light_params_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LightParams) == 0x60);
static_assert(offsetof(LightParams, vec_0) == 0x0);
static_assert(offsetof(LightParams, vec_c) == 0xc);
static_assert(offsetof(LightParams, vec_18) == 0x18);
static_assert(offsetof(LightParams, vec_24) == 0x24);
static_assert(offsetof(LightParams, sun_dir) == 0x30);
static_assert(offsetof(LightParams, scale_b) == 0x3c);
static_assert(offsetof(LightParams, f32_48) == 0x48);
static_assert(offsetof(LightParams, scale_a) == 0x4c);
#endif

}  // namespace joan::orig
