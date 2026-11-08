// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's MiniRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct MiniResFrame;
struct MiniResMap;

#pragma pack(push, 1)
struct MiniRes {
#include "mini_res_fields.inc"
#include "mini_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(MiniRes) == 0x31);
static_assert(offsetof(MiniRes, loaded) == 0x0);
static_assert(offsetof(MiniRes, frame_count) == 0x1);
static_assert(offsetof(MiniRes, frames) == 0x5);
static_assert(offsetof(MiniRes, map_count) == 0x9);
static_assert(offsetof(MiniRes, maps) == 0xd);
static_assert(offsetof(MiniRes, marker_count) == 0x11);
static_assert(offsetof(MiniRes, markers) == 0x15);
static_assert(offsetof(MiniRes, rect) == 0x19);
static_assert(offsetof(MiniRes, i32_21) == 0x21);
static_assert(offsetof(MiniRes, i32_25) == 0x25);
static_assert(offsetof(MiniRes, i32_29) == 0x29);
static_assert(offsetof(MiniRes, i32_2d) == 0x2d);
#endif

}  // namespace joan::orig
