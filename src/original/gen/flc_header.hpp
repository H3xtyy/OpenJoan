// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's FlcHeader class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct FlcHeader {
#include "flc_header_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(FlcHeader) == 0x11);
static_assert(offsetof(FlcHeader, width) == 0x0);
static_assert(offsetof(FlcHeader, height) == 0x2);
static_assert(offsetof(FlcHeader, frame_count) == 0x4);
static_assert(offsetof(FlcHeader, frame) == 0x6);
static_assert(offsetof(FlcHeader, pixels) == 0x8);
static_assert(offsetof(FlcHeader, palette) == 0xc);
static_assert(offsetof(FlcHeader, loop) == 0x10);
#endif

}  // namespace joan::orig
