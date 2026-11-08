// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Display class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IDirect3DDevice8;

#pragma pack(push, 1)
struct Display {
#include "display_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Display) == 0x40c);
static_assert(offsetof(Display, state) == 0x0);
static_assert(offsetof(Display, flag) == 0x1);
static_assert(offsetof(Display, u8_2) == 0x2);
static_assert(offsetof(Display, device) == 0x4);
static_assert(offsetof(Display, colours) == 0x8);
static_assert(offsetof(Display, pixel_format) == 0x408);
#endif

}  // namespace joan::orig
