// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TFFrameInfo class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TFFrameInfo {
#include "tfframe_info_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TFFrameInfo) == 0x11);
static_assert(offsetof(TFFrameInfo, width) == 0x0);
static_assert(offsetof(TFFrameInfo, height) == 0x2);
static_assert(offsetof(TFFrameInfo, frame_count) == 0x4);
static_assert(offsetof(TFFrameInfo, frame) == 0x6);
static_assert(offsetof(TFFrameInfo, pixels) == 0x8);
static_assert(offsetof(TFFrameInfo, palette) == 0xc);
static_assert(offsetof(TFFrameInfo, loop) == 0x10);
#endif

}  // namespace joan::orig
