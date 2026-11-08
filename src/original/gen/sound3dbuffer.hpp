// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Sound3DBuffer class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Sound3DBuffer {
#include "sound3dbuffer_fields.inc"
#include "sound3dbuffer_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Sound3DBuffer) == 0x11a);
static_assert(offsetof(Sound3DBuffer, al_buffer) == 0x0);
static_assert(offsetof(Sound3DBuffer, refs) == 0x4);
static_assert(offsetof(Sound3DBuffer, loaded) == 0x6);
static_assert(offsetof(Sound3DBuffer, u8_7) == 0x7);
static_assert(offsetof(Sound3DBuffer, format) == 0x10c);
static_assert(offsetof(Sound3DBuffer, size) == 0x110);
static_assert(offsetof(Sound3DBuffer, frequency) == 0x114);
static_assert(offsetof(Sound3DBuffer, loop) == 0x118);
static_assert(offsetof(Sound3DBuffer, release_unused) == 0x119);
#endif

}  // namespace joan::orig
