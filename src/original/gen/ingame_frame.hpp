// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's IngameFrame class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct IngameFrame {
#include "ingame_frame_fields.inc"
#include "ingame_frame_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(IngameFrame, open) == 0x0);
static_assert(offsetof(IngameFrame, sliding) == 0x1);
static_assert(offsetof(IngameFrame, igamebg1) == 0x20);
static_assert(offsetof(IngameFrame, igamebg2) == 0x24);
static_assert(offsetof(IngameFrame, igamebg3) == 0x28);
static_assert(offsetof(IngameFrame, igamebg4) == 0x2c);
static_assert(offsetof(IngameFrame, f32_31d) == 0x31d);
static_assert(offsetof(IngameFrame, f32_3d4) == 0x3d4);
#endif

}  // namespace joan::orig
