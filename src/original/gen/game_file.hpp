// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's GameFile class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct GameFile {
#include "game_file_fields.inc"
#include "game_file_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(GameFile) == 0x1a7);
static_assert(offsetof(GameFile, index) == 0x0);
static_assert(offsetof(GameFile, flags) == 0x2);
static_assert(offsetof(GameFile, file_time) == 0x108);
static_assert(offsetof(GameFile, local_time) == 0x110);
static_assert(offsetof(GameFile, mode) == 0x118);
static_assert(offsetof(GameFile, u8_119) == 0x119);
static_assert(offsetof(GameFile, name) == 0x11a);
static_assert(offsetof(GameFile, u32_19a) == 0x19a);
static_assert(offsetof(GameFile, u8_19e) == 0x19e);
static_assert(offsetof(GameFile, game_time) == 0x19f);
static_assert(offsetof(GameFile, u32_1a3) == 0x1a3);
#endif

}  // namespace joan::orig
