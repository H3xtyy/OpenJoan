// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ParticlePool class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ParticlePool : DynArrayB {
#include "particle_pool_fields.inc"
#include "particle_pool_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ParticlePool) == 0x3e);
static_assert(offsetof(ParticlePool, alive_) == 0x2e);
static_assert(offsetof(ParticlePool, emitted) == 0x32);
static_assert(offsetof(ParticlePool, last_emit) == 0x36);
static_assert(offsetof(ParticlePool, carry) == 0x3a);
#endif

}  // namespace joan::orig
