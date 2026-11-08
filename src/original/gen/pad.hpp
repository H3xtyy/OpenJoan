// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Pad class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Joystick;

#pragma pack(push, 1)
struct Pad {
#include "pad_fields.inc"
#include "pad_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Pad) == 0x39);
static_assert(offsetof(Pad, joystick) == 0x0);
static_assert(offsetof(Pad, axis_) == 0x4);
static_assert(offsetof(Pad, button) == 0x2c);
#endif

}  // namespace joan::orig
