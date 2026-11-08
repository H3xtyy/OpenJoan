// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CutScene class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct CamZones;
struct Texture;

#pragma pack(push, 1)
struct CutScene {
#include "cut_scene_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CutScene) == 0x48);
static_assert(offsetof(CutScene, target) == 0x0);
static_assert(offsetof(CutScene, active) == 0x4);
static_assert(offsetof(CutScene, mode) == 0x5);
static_assert(offsetof(CutScene, mode_prev) == 0x6);
static_assert(offsetof(CutScene, duration) == 0x7);
static_assert(offsetof(CutScene, elapsed) == 0xb);
static_assert(offsetof(CutScene, state) == 0xf);
static_assert(offsetof(CutScene, u8_13) == 0x13);
static_assert(offsetof(CutScene, zones) == 0x14);
static_assert(offsetof(CutScene, cam_pos) == 0x18);
static_assert(offsetof(CutScene, cam_dist) == 0x24);
static_assert(offsetof(CutScene, cam_pitch) == 0x28);
static_assert(offsetof(CutScene, cam_yaw) == 0x2c);
static_assert(offsetof(CutScene, follow_target) == 0x30);
static_assert(offsetof(CutScene, bar_size) == 0x34);
static_assert(offsetof(CutScene, i32_36) == 0x36);
static_assert(offsetof(CutScene, bar_tex) == 0x3a);
static_assert(offsetof(CutScene, bar_on) == 0x3e);
static_assert(offsetof(CutScene, started) == 0x3f);
static_assert(offsetof(CutScene, u8_40) == 0x40);
static_assert(offsetof(CutScene, hud_a) == 0x41);
static_assert(offsetof(CutScene, hud_b) == 0x42);
static_assert(offsetof(CutScene, hud_c) == 0x43);
static_assert(offsetof(CutScene, scene_flag) == 0x44);
#endif

}  // namespace joan::orig
