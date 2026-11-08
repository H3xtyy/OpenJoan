// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Emitter class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "particle_key.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Emitter : ParticleKey {
#include "emitter_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Emitter) == 0x33);
static_assert(offsetof(Emitter, lo) == 0x1b);
static_assert(offsetof(Emitter, hi) == 0x27);
#endif

}  // namespace joan::orig
