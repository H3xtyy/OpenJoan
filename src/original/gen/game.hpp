// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Game class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "end_con.hpp"
#include "scenario.hpp"

namespace joan::orig {

struct ArrI16;

#pragma pack(push, 1)
struct Game {
#include "game_fields.inc"
#include "game_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Game, active) == 0x0);
static_assert(offsetof(Game, init_done) == 0x1);
static_assert(offsetof(Game, resources_loaded) == 0x2);
static_assert(offsetof(Game, interface_loaded) == 0x3);
static_assert(offsetof(Game, arrays_loaded) == 0x4);
static_assert(offsetof(Game, u8_5) == 0x5);
static_assert(offsetof(Game, u8_6) == 0x6);
static_assert(offsetof(Game, u32_7) == 0x7);
static_assert(offsetof(Game, u32_b) == 0xb);
static_assert(offsetof(Game, u32_f) == 0xf);
static_assert(offsetof(Game, u32_13) == 0x13);
static_assert(offsetof(Game, scenario) == 0x17);
static_assert(offsetof(Game, mode_timers) == 0x2b4);
static_assert(offsetof(Game, mode2_timer) == 0x304);
static_assert(offsetof(Game, end_state) == 0x308);
static_assert(offsetof(Game, end_condition_count) == 0x309);
static_assert(offsetof(Game, end_conditions) == 0x30b);
static_assert(offsetof(Game, u8_71b) == 0x71b);
static_assert(offsetof(Game, word_set) == 0x71c);
static_assert(offsetof(Game, word_set_count) == 0x720);
static_assert(offsetof(Game, word_set_cap) == 0x724);
static_assert(offsetof(Game, word_stack) == 0x728);
static_assert(offsetof(Game, word_stack_count) == 0x72c);
static_assert(offsetof(Game, word_stack_cap) == 0x730);
static_assert(offsetof(Game, u32_734) == 0x734);
static_assert(offsetof(Game, end_delay) == 0x738);
static_assert(offsetof(Game, end_since) == 0x73c);
static_assert(offsetof(Game, u32_740) == 0x740);
static_assert(offsetof(Game, cutscene_flag) == 0x744);
#endif

}  // namespace joan::orig
