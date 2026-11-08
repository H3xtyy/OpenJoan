// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TalkPrompt class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TalkPrompt {
#include "talk_prompt_fields.inc"
#include "talk_prompt_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(TalkPrompt, tex) == 0x0);
static_assert(offsetof(TalkPrompt, sheet) == 0x4);
static_assert(offsetof(TalkPrompt, visible) == 0x8);
static_assert(offsetof(TalkPrompt, u32_9) == 0x9);
static_assert(offsetof(TalkPrompt, object) == 0xd);
static_assert(offsetof(TalkPrompt, near_a) == 0x11);
static_assert(offsetof(TalkPrompt, near_b) == 0x15);
static_assert(offsetof(TalkPrompt, near_c) == 0x19);
static_assert(offsetof(TalkPrompt, leader) == 0x1d);
static_assert(offsetof(TalkPrompt, enemy_of) == 0x21);
static_assert(offsetof(TalkPrompt, enemy) == 0x25);
static_assert(offsetof(TalkPrompt, zone) == 0x29);
static_assert(offsetof(TalkPrompt, u32_2d) == 0x2d);
#endif

}  // namespace joan::orig
