// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Scene class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "light_pick.hpp"

namespace joan::orig {

struct ActorPtr;
struct BillboardPtr;
struct Light;
struct LightPtr;
struct Occluder;
struct Plane;
struct ScenePose;
struct SceneSPtr;
struct SectorNode;
struct SkyBox;

#pragma pack(push, 1)
struct Scene {
#include "scene_fields.inc"
#include "scene_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Scene) == 0xd5);
static_assert(offsetof(Scene, params) == 0x0);
static_assert(offsetof(Scene, sky) == 0x18);
static_assert(offsetof(Scene, plane_1c) == 0x1c);
static_assert(offsetof(Scene, mode) == 0x20);
static_assert(offsetof(Scene, sector_count) == 0x24);
static_assert(offsetof(Scene, sector_capacity) == 0x28);
static_assert(offsetof(Scene, sectors) == 0x2c);
static_assert(offsetof(Scene, tree) == 0x30);
static_assert(offsetof(Scene, visible) == 0x34);
static_assert(offsetof(Scene, visible_count) == 0x38);
static_assert(offsetof(Scene, visible_capacity) == 0x3c);
static_assert(offsetof(Scene, list_40) == 0x40);
static_assert(offsetof(Scene, list_40_count) == 0x44);
static_assert(offsetof(Scene, list_40_capacity) == 0x48);
static_assert(offsetof(Scene, light_count) == 0x4c);
static_assert(offsetof(Scene, light_capacity) == 0x50);
static_assert(offsetof(Scene, lights) == 0x54);
static_assert(offsetof(Scene, occluders) == 0x58);
static_assert(offsetof(Scene, occluder_total) == 0x5c);
static_assert(offsetof(Scene, occluder_capacity) == 0x60);
static_assert(offsetof(Scene, occluder_count_) == 0x64);
static_assert(offsetof(Scene, u32_68) == 0x68);
static_assert(offsetof(Scene, u32_6c) == 0x6c);
static_assert(offsetof(Scene, lit_) == 0x70);
static_assert(offsetof(Scene, plane_71) == 0x71);
static_assert(offsetof(Scene, actor_count) == 0x75);
static_assert(offsetof(Scene, actor_capacity) == 0x79);
static_assert(offsetof(Scene, actors) == 0x7d);
static_assert(offsetof(Scene, plane_81) == 0x81);
static_assert(offsetof(Scene, light_pick) == 0x85);
static_assert(offsetof(Scene, camera) == 0xb5);
static_assert(offsetof(Scene, ptr_b9) == 0xb9);
static_assert(offsetof(Scene, fog_flags) == 0xbd);
static_assert(offsetof(Scene, material_count) == 0xc1);
static_assert(offsetof(Scene, materials) == 0xc5);
static_assert(offsetof(Scene, region_buffer) == 0xc9);
static_assert(offsetof(Scene, u32_cd) == 0xcd);
static_assert(offsetof(Scene, buffer_d1) == 0xd1);
#endif

}  // namespace joan::orig
