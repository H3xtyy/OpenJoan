// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Cam class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Cam {
#include "cam_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Cam, orient) == 0x10);
static_assert(offsetof(Cam, orient_target) == 0x20);
static_assert(offsetof(Cam, view) == 0x30);
static_assert(offsetof(Cam, proj) == 0x70);
static_assert(offsetof(Cam, matrix_b0) == 0xb0);
static_assert(offsetof(Cam, matrix_f0) == 0xf0);
static_assert(offsetof(Cam, u32_130) == 0x130);
#endif

}  // namespace joan::orig
