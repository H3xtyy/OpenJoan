// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ParticleKey class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "float_range.hpp"

namespace joan::orig {

struct EffectKey;

#pragma pack(push, 1)
struct ParticleKey {
#include "particle_key_fields.inc"
#include "particle_key_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ParticleKey) == 0x1b);
static_assert(offsetof(ParticleKey, vtable) == 0x0);
static_assert(offsetof(ParticleKey, kind_) == 0x4);
static_assert(offsetof(ParticleKey, fade) == 0x8);
static_assert(offsetof(ParticleKey, time_range) == 0x9);
static_assert(offsetof(ParticleKey, time_) == 0x11);
static_assert(offsetof(ParticleKey, owner) == 0x15);
static_assert(offsetof(ParticleKey, next) == 0x19);
#endif

}  // namespace joan::orig
