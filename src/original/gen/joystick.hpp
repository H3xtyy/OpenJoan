// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Joystick class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IDirectInputDevice8;

#pragma pack(push, 1)
struct Joystick {
#include "joystick_fields.inc"
#include "joystick_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Joystick) == 0xc6);
static_assert(offsetof(Joystick, device) == 0x0);
static_assert(offsetof(Joystick, index) == 0x4);
static_assert(offsetof(Joystick, u8_8) == 0x8);
static_assert(offsetof(Joystick, u8_9) == 0x9);
static_assert(offsetof(Joystick, axis_offset) == 0xa);
static_assert(offsetof(Joystick, button_offset) == 0x32);
static_assert(offsetof(Joystick, axis_min) == 0x62);
static_assert(offsetof(Joystick, axis_max) == 0x8a);
static_assert(offsetof(Joystick, axis_map) == 0xb2);
static_assert(offsetof(Joystick, button_map) == 0xb8);
#endif

}  // namespace joan::orig
