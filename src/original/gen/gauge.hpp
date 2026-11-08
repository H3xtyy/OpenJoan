// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Gauge class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "float_range.hpp"
#include "particle_key.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Gauge : ParticleKey {
#include "gauge_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Gauge) == 0x23);
static_assert(offsetof(Gauge, value) == 0x1b);
#endif

}  // namespace joan::orig
