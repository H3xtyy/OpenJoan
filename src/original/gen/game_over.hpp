// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's GameOver class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"

namespace joan::orig {

struct BmpRes;
struct Texture;

#pragma pack(push, 1)
struct GameOver {
#include "game_over_fields.inc"
#include "game_over_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(GameOver) == 0x9ed);
static_assert(offsetof(GameOver, retry) == 0x0);
static_assert(offsetof(GameOver, load) == 0x346);
static_assert(offsetof(GameOver, quit) == 0x68c);
static_assert(offsetof(GameOver, image) == 0x9d2);
static_assert(offsetof(GameOver, image2) == 0x9d6);
static_assert(offsetof(GameOver, u32_9da) == 0x9da);
static_assert(offsetof(GameOver, u32_9de) == 0x9de);
static_assert(offsetof(GameOver, blink) == 0x9e2);
static_assert(offsetof(GameOver, blink_on) == 0x9e3);
static_assert(offsetof(GameOver, blink_time) == 0x9e4);
static_assert(offsetof(GameOver, text) == 0x9e8);
static_assert(offsetof(GameOver, is_open) == 0x9ec);
#endif

}  // namespace joan::orig
