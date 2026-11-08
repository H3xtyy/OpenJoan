// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LogMessage class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LogMessage {
#include "log_message_fields.inc"
#include "log_message_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(LogMessage, u16_13) == 0x13);
static_assert(offsetof(LogMessage, u16_15) == 0x15);
static_assert(offsetof(LogMessage, text) == 0x17);
static_assert(offsetof(LogMessage, text2) == 0x1b);
#endif

}  // namespace joan::orig
