// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's InputEvent class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "pad.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct InputEvent {
#include "input_event_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(InputEvent) == 0x51);
static_assert(offsetof(InputEvent, type) == 0x0);
static_assert(offsetof(InputEvent, time) == 0x4);
static_assert(offsetof(InputEvent, skey_state) == 0x8);
static_assert(offsetof(InputEvent, x) == 0xa);
static_assert(offsetof(InputEvent, y) == 0xe);
static_assert(offsetof(InputEvent, scan_code) == 0x12);
static_assert(offsetof(InputEvent, joy_index) == 0x16);
static_assert(offsetof(InputEvent, joy_arg) == 0x17);
static_assert(offsetof(InputEvent, joy_state) == 0x18);
#endif

}  // namespace joan::orig
