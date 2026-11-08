// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Surface class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IDirect3DSurface8;

#pragma pack(push, 1)
struct Surface {
#include "surface_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Surface) == 0x34);
static_assert(offsetof(Surface, surface) == 0x0);
static_assert(offsetof(Surface, owned) == 0x4);
static_assert(offsetof(Surface, locked) == 0x8);
static_assert(offsetof(Surface, bits) == 0xc);
static_assert(offsetof(Surface, pitch) == 0x10);
static_assert(offsetof(Surface, lock_bits) == 0x14);
static_assert(offsetof(Surface, lock_pitch) == 0x18);
static_assert(offsetof(Surface, width) == 0x1c);
static_assert(offsetof(Surface, height) == 0x20);
static_assert(offsetof(Surface, format_bits) == 0x24);
static_assert(offsetof(Surface, bytes_per_pixel) == 0x28);
static_assert(offsetof(Surface, blend_mode) == 0x2c);
static_assert(offsetof(Surface, lock_depth) == 0x2d);
static_assert(offsetof(Surface, lock_stack) == 0x2e);
static_assert(offsetof(Surface, u32_30) == 0x30);
#endif

}  // namespace joan::orig
