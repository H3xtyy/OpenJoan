// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MiniResFrame class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct MiniResFrame {
#include "mini_res_frame_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MiniResFrame) == 0x20);
static_assert(offsetof(MiniResFrame, kind) == 0x9);
static_assert(offsetof(MiniResFrame, sub_a) == 0xb);
static_assert(offsetof(MiniResFrame, sub_b) == 0xd);
static_assert(offsetof(MiniResFrame, x0) == 0xf);
static_assert(offsetof(MiniResFrame, y0) == 0x13);
static_assert(offsetof(MiniResFrame, x1) == 0x17);
static_assert(offsetof(MiniResFrame, y1) == 0x1b);
static_assert(offsetof(MiniResFrame, is_default) == 0x1f);
#endif

}  // namespace joan::orig
