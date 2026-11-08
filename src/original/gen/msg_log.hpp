// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MsgLog class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MsgLog : DynArrayB {
#include "msg_log_fields.inc"
#include "msg_log_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MsgLog) == 0x82);
static_assert(offsetof(MsgLog, count) == 0x2e);
static_assert(offsetof(MsgLog, last) == 0x32);
static_assert(offsetof(MsgLog, cur_kind) == 0x36);
static_assert(offsetof(MsgLog, first_of_kind) == 0x3a);
static_assert(offsetof(MsgLog, count_of_kind) == 0x5e);
#endif

}  // namespace joan::orig
