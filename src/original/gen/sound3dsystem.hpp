// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Sound3DSystem class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Listener;
struct Sound3DBufferPtrArray;
struct Sound3DPtrArray;

#pragma pack(push, 1)
struct Sound3DSystem {
#include "sound3dsystem_fields.inc"
#include "sound3dsystem_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Sound3DSystem, distance_model) == 0x0);
static_assert(offsetof(Sound3DSystem, doppler_factor) == 0xa);
static_assert(offsetof(Sound3DSystem, doppler_velocity) == 0xe);
static_assert(offsetof(Sound3DSystem, device) == 0x12);
static_assert(offsetof(Sound3DSystem, u16_16) == 0x16);
static_assert(offsetof(Sound3DSystem, buffers) == 0x18);
static_assert(offsetof(Sound3DSystem, buffer_count) == 0x1c);
static_assert(offsetof(Sound3DSystem, buffer_capacity) == 0x1e);
static_assert(offsetof(Sound3DSystem, sources) == 0x20);
static_assert(offsetof(Sound3DSystem, source_count) == 0x24);
static_assert(offsetof(Sound3DSystem, source_capacity) == 0x26);
static_assert(offsetof(Sound3DSystem, listener) == 0x28);
#endif

}  // namespace joan::orig
