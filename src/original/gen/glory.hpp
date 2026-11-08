// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Glory class.
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
struct Glory {
#include "glory_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Glory, button0) == 0x0);
static_assert(offsetof(Glory, button1) == 0x346);
static_assert(offsetof(Glory, button2) == 0x68c);
static_assert(offsetof(Glory, button3) == 0x9d2);
static_assert(offsetof(Glory, u32_d18) == 0xd18);
static_assert(offsetof(Glory, u32_d1c) == 0xd1c);
static_assert(offsetof(Glory, sprites) == 0xd20);
static_assert(offsetof(Glory, button_sprites) == 0xd24);
static_assert(offsetof(Glory, blink_on) == 0xd28);
static_assert(offsetof(Glory, blink_state) == 0xd29);
static_assert(offsetof(Glory, blink_time) == 0xd2a);
static_assert(offsetof(Glory, text) == 0xd2e);
static_assert(offsetof(Glory, done) == 0xd32);
#endif

}  // namespace joan::orig
