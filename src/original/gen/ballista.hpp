// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Ballista class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Ballista {
#include "ballista_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Ballista, ptr_8) == 0x8);
static_assert(offsetof(Ballista, i32_50) == 0x50);
static_assert(offsetof(Ballista, i32_54) == 0x54);
static_assert(offsetof(Ballista, i32_58) == 0x58);
static_assert(offsetof(Ballista, i32_5c) == 0x5c);
static_assert(offsetof(Ballista, f32_174) == 0x174);
static_assert(offsetof(Ballista, f32_178) == 0x178);
static_assert(offsetof(Ballista, f32_17c) == 0x17c);
static_assert(offsetof(Ballista, f32_184) == 0x184);
static_assert(offsetof(Ballista, f32_188) == 0x188);
static_assert(offsetof(Ballista, f32_18c) == 0x18c);
static_assert(offsetof(Ballista, f32_190) == 0x190);
static_assert(offsetof(Ballista, f32_194) == 0x194);
static_assert(offsetof(Ballista, f32_328) == 0x328);
static_assert(offsetof(Ballista, i16_36c) == 0x36c);
static_assert(offsetof(Ballista, u8_36f) == 0x36f);
static_assert(offsetof(Ballista, i32_370) == 0x370);
static_assert(offsetof(Ballista, i32_374) == 0x374);
static_assert(offsetof(Ballista, i32_378) == 0x378);
static_assert(offsetof(Ballista, i32_37c) == 0x37c);
static_assert(offsetof(Ballista, f32_494) == 0x494);
static_assert(offsetof(Ballista, f32_498) == 0x498);
static_assert(offsetof(Ballista, f32_49c) == 0x49c);
static_assert(offsetof(Ballista, f32_4a4) == 0x4a4);
static_assert(offsetof(Ballista, f32_4a8) == 0x4a8);
static_assert(offsetof(Ballista, f32_4ac) == 0x4ac);
static_assert(offsetof(Ballista, f32_4b0) == 0x4b0);
static_assert(offsetof(Ballista, f32_4b4) == 0x4b4);
static_assert(offsetof(Ballista, u32_4b8) == 0x4b8);
static_assert(offsetof(Ballista, f32_4bc) == 0x4bc);
#endif

}  // namespace joan::orig
