// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Speech class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Actor;
struct FileTxt;

#pragma pack(push, 1)
struct Speech {
#include "speech_fields.inc"
#include "speech_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Speech) == 0x138);
static_assert(offsetof(Speech, script) == 0x0);
static_assert(offsetof(Speech, target) == 0x4);
static_assert(offsetof(Speech, text_) == 0x8);
static_assert(offsetof(Speech, text_len) == 0xc);
static_assert(offsetof(Speech, text_cap) == 0x10);
static_assert(offsetof(Speech, time) == 0x14);
static_assert(offsetof(Speech, last_tick) == 0x18);
static_assert(offsetof(Speech, time_f) == 0x1c);
static_assert(offsetof(Speech, next_time) == 0x20);
static_assert(offsetof(Speech, columns) == 0x24);
static_assert(offsetof(Speech, text_column) == 0x28);
static_assert(offsetof(Speech, column_count) == 0x2c);
static_assert(offsetof(Speech, pending) == 0x30);
static_assert(offsetof(Speech, state) == 0x130);
static_assert(offsetof(Speech, speed) == 0x134);
#endif

}  // namespace joan::orig
