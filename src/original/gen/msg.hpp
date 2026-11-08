// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Msg class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Msg {
#include "msg_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Msg, type) == 0x0);
static_assert(offsetof(Msg, arg) == 0x2);
static_assert(offsetof(Msg, arg2) == 0x4);
static_assert(offsetof(Msg, u16_6) == 0x6);
static_assert(offsetof(Msg, u16_8) == 0x8);
static_assert(offsetof(Msg, from) == 0xe);
static_assert(offsetof(Msg, to) == 0xf);
#endif

}  // namespace joan::orig
