// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Message class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Message {
#include "message_fields.inc"
#include "message_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Message, list_index) == 0x0);
static_assert(offsetof(Message, state) == 0x4);
static_assert(offsetof(Message, has_pic) == 0x8);
static_assert(offsetof(Message, x) == 0x9);
static_assert(offsetof(Message, y) == 0xd);
static_assert(offsetof(Message, u8_11) == 0x11);
static_assert(offsetof(Message, shown_len) == 0x15);
static_assert(offsetof(Message, len) == 0x17);
static_assert(offsetof(Message, sound_id) == 0x19);
static_assert(offsetof(Message, sound_started) == 0x1d);
static_assert(offsetof(Message, text) == 0x21);
static_assert(offsetof(Message, u8_25) == 0x25);
static_assert(offsetof(Message, pic) == 0x2e);
static_assert(offsetof(Message, revealed_chars) == 0x3b);
static_assert(offsetof(Message, priority_) == 0x3f);
static_assert(offsetof(Message, start_time) == 0x43);
static_assert(offsetof(Message, colour) == 0x47);
#endif

}  // namespace joan::orig
