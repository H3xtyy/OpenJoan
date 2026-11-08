// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LightPreset class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "light_def.hpp"
#include "light_params.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LightPreset {
#include "light_preset_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LightPreset) == 0x18c);
static_assert(offsetof(LightPreset, name) == 0x0);
static_assert(offsetof(LightPreset, light_count) == 0x40);
static_assert(offsetof(LightPreset, lights) == 0x42);
static_assert(offsetof(LightPreset, params) == 0x12c);
#endif

}  // namespace joan::orig
