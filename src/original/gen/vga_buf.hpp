// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's VgaBuf class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Palette16;
struct Palette24;
struct Palette32;
struct Palette8;

#pragma pack(push, 1)
struct VgaBuf {
#include "vga_buf_fields.inc"
#include "vga_buf_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(VgaBuf) == 0x10);
static_assert(offsetof(VgaBuf, rgb) == 0x0);
static_assert(offsetof(VgaBuf, map8) == 0x4);
static_assert(offsetof(VgaBuf, pal16) == 0x8);
static_assert(offsetof(VgaBuf, pal32) == 0xc);
#endif

}  // namespace joan::orig
