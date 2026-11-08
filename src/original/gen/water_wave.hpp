// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's WaterWave class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct ActorDef;
struct AnimDef;

#pragma pack(push, 1)
struct WaterWave : DynArrayB {
#include "water_wave_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(WaterWave, loaded) == 0x2e);
static_assert(offsetof(WaterWave, life_ms) == 0x2f);
static_assert(offsetof(WaterWave, model) == 0x33);
static_assert(offsetof(WaterWave, anim) == 0x37);
#endif

}  // namespace joan::orig
