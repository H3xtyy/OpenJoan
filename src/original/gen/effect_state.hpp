// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's EffectState class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "range.hpp"

namespace joan::orig {

struct Actor;
struct DynArray;
struct ParticlePool;
struct Plane;

#pragma pack(push, 1)
struct EffectState {
#include "effect_state_fields.inc"
#include "effect_state_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(EffectState) == 0x78);
static_assert(offsetof(EffectState, initialised) == 0x0);
static_assert(offsetof(EffectState, started_) == 0x1);
static_assert(offsetof(EffectState, visible) == 0x2);
static_assert(offsetof(EffectState, name_) == 0x3);
static_assert(offsetof(EffectState, parts) == 0x24);
static_assert(offsetof(EffectState, object_) == 0x28);
static_assert(offsetof(EffectState, range) == 0x2c);
static_assert(offsetof(EffectState, pos) == 0x44);
static_assert(offsetof(EffectState, prev_pos) == 0x50);
static_assert(offsetof(EffectState, yaw) == 0x5c);
static_assert(offsetof(EffectState, i16_60) == 0x60);
static_assert(offsetof(EffectState, i16_62) == 0x62);
static_assert(offsetof(EffectState, owns_emitters) == 0x64);
static_assert(offsetof(EffectState, share_refs) == 0x68);
static_assert(offsetof(EffectState, shared_from) == 0x6c);
static_assert(offsetof(EffectState, emitters) == 0x70);
static_assert(offsetof(EffectState, pools) == 0x74);
#endif

}  // namespace joan::orig
