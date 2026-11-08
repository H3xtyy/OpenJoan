// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Cmd class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Cmd {
#include "cmd_fields.inc"
#include "cmd_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Cmd, unit_id) == 0x0);
static_assert(offsetof(Cmd, u32_18) == 0x18);
static_assert(offsetof(Cmd, u32_1c) == 0x1c);
static_assert(offsetof(Cmd, i8_96) == 0x96);
static_assert(offsetof(Cmd, target) == 0x97);
static_assert(offsetof(Cmd, i32_a2) == 0xa2);
static_assert(offsetof(Cmd, i8_ac) == 0xac);
static_assert(offsetof(Cmd, i32_bb) == 0xbb);
static_assert(offsetof(Cmd, i8_c4) == 0xc4);
static_assert(offsetof(Cmd, target_unit) == 0xd3);
static_assert(offsetof(Cmd, i32_d7) == 0xd7);
static_assert(offsetof(Cmd, u16_db) == 0xdb);
static_assert(offsetof(Cmd, i32_dd) == 0xdd);
#endif

}  // namespace joan::orig
