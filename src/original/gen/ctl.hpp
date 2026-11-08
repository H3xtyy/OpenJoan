// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Ctl class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Texture;

#pragma pack(push, 1)
struct Ctl {
#include "ctl_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Ctl, vtable) == 0x0);
static_assert(offsetof(Ctl, i8_4) == 0x4);
static_assert(offsetof(Ctl, x) == 0x5);
static_assert(offsetof(Ctl, y) == 0x7);
static_assert(offsetof(Ctl, right) == 0x9);
static_assert(offsetof(Ctl, bottom) == 0xb);
static_assert(offsetof(Ctl, width) == 0xd);
static_assert(offsetof(Ctl, height) == 0xf);
static_assert(offsetof(Ctl, fit_to_screen) == 0x11);
static_assert(offsetof(Ctl, hit_enabled) == 0x12);
static_assert(offsetof(Ctl, activatable) == 0x13);
static_assert(offsetof(Ctl, activated) == 0x14);
static_assert(offsetof(Ctl, u32_15) == 0x15);
#endif

}  // namespace joan::orig
