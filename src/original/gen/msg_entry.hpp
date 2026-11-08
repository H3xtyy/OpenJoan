// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MsgEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MsgEntry {
#include "msg_entry_fields.inc"
#include "msg_entry_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MsgEntry) == 0x1f);
static_assert(offsetof(MsgEntry, kind) == 0x0);
static_assert(offsetof(MsgEntry, type) == 0x2);
static_assert(offsetof(MsgEntry, rec_no) == 0x3);
static_assert(offsetof(MsgEntry, prev) == 0x7);
static_assert(offsetof(MsgEntry, next) == 0xb);
static_assert(offsetof(MsgEntry, value) == 0xf);
static_assert(offsetof(MsgEntry, text_len) == 0x13);
static_assert(offsetof(MsgEntry, extra_len) == 0x15);
static_assert(offsetof(MsgEntry, text) == 0x17);
static_assert(offsetof(MsgEntry, extra) == 0x1b);
#endif

}  // namespace joan::orig
