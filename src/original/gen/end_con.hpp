// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's EndCon class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct EndCon {
#include "end_con_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(EndCon) == 0x1a);
static_assert(offsetof(EndCon, state) == 0x0);
static_assert(offsetof(EndCon, text) == 0x1);
static_assert(offsetof(EndCon, text2) == 0x5);
static_assert(offsetof(EndCon, result) == 0x9);
static_assert(offsetof(EndCon, i16_b) == 0xb);
static_assert(offsetof(EndCon, i16_d) == 0xd);
static_assert(offsetof(EndCon, used) == 0xf);
static_assert(offsetof(EndCon, reward_gold) == 0x11);
static_assert(offsetof(EndCon, reward_exp) == 0x15);
static_assert(offsetof(EndCon, rewarded) == 0x19);
#endif

}  // namespace joan::orig
