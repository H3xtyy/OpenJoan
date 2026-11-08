// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's KeyFrame class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct KeyFrame {
#include "key_frame_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(KeyFrame) == 0x20);
static_assert(offsetof(KeyFrame, time) == 0x0);
static_assert(offsetof(KeyFrame, rotation) == 0x4);
static_assert(offsetof(KeyFrame, position) == 0x14);
#endif

}  // namespace joan::orig
