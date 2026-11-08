// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Log class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Log {
#include "log_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Log, visible) == 0x14d);
static_assert(offsetof(Log, u8_14e) == 0x14e);
static_assert(offsetof(Log, fade_state) == 0x14f);
static_assert(offsetof(Log, fade_time) == 0x153);
static_assert(offsetof(Log, msg_len) == 0x157);
static_assert(offsetof(Log, msg_kind) == 0x1a7);
static_assert(offsetof(Log, msg_count) == 0x1f7);
static_assert(offsetof(Log, msg_time) == 0x1f9);
static_assert(offsetof(Log, msg_id) == 0x299);
#endif

}  // namespace joan::orig
