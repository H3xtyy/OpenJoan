// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ArcBall class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "trackball.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ArcBall {
#include "arc_ball_fields.inc"
#include "arc_ball_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ArcBall) == 0x134);
static_assert(offsetof(ArcBall, trackball) == 0x0);
static_assert(offsetof(ArcBall, u32_c) == 0xc);
static_assert(offsetof(ArcBall, q_down) == 0x10);
static_assert(offsetof(ArcBall, q_now) == 0x20);
static_assert(offsetof(ArcBall, rotation) == 0x30);
static_assert(offsetof(ArcBall, tmp_matrix) == 0x70);
static_assert(offsetof(ArcBall, matrix_b0) == 0xb0);
static_assert(offsetof(ArcBall, matrix_f0) == 0xf0);
static_assert(offsetof(ArcBall, dragging) == 0x130);
#endif

}  // namespace joan::orig
