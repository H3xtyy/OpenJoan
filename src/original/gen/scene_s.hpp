// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SceneS class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct LightPtr;
struct Plane;
struct PtrSlot;
struct Scene;
struct SceneNode;
struct SceneNodePtr;
struct SceneObject;
struct SectorNode;

#pragma pack(push, 1)
struct SceneS {
#include "scene_s_fields.inc"
#include "scene_s_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SceneS) == 0xe4);
static_assert(offsetof(SceneS, scene) == 0x0);
static_assert(offsetof(SceneS, node) == 0x4);
static_assert(offsetof(SceneS, desc) == 0x8);
static_assert(offsetof(SceneS, inner_box) == 0x20);
static_assert(offsetof(SceneS, plane) == 0x38);
static_assert(offsetof(SceneS, object) == 0x3c);
static_assert(offsetof(SceneS, plane2) == 0x40);
static_assert(offsetof(SceneS, items) == 0x44);
static_assert(offsetof(SceneS, item_count) == 0x48);
static_assert(offsetof(SceneS, item_cap) == 0x4c);
static_assert(offsetof(SceneS, ptr_50) == 0x50);
static_assert(offsetof(SceneS, portal_refs) == 0x54);
static_assert(offsetof(SceneS, u8_58) == 0x58);
static_assert(offsetof(SceneS, planes_ready) == 0x59);
static_assert(offsetof(SceneS, clip_planes) == 0x5a);
static_assert(offsetof(SceneS, u32_ba) == 0xba);
static_assert(offsetof(SceneS, node_cap) == 0xbe);
static_assert(offsetof(SceneS, node_count) == 0xc2);
static_assert(offsetof(SceneS, nodes) == 0xc6);
static_assert(offsetof(SceneS, nodes_tail) == 0xca);
static_assert(offsetof(SceneS, nodes_head) == 0xce);
static_assert(offsetof(SceneS, light_count) == 0xd2);
static_assert(offsetof(SceneS, light_cap) == 0xd6);
static_assert(offsetof(SceneS, lights) == 0xda);
static_assert(offsetof(SceneS, lit_) == 0xde);
static_assert(offsetof(SceneS, casts_shadows_) == 0xdf);
static_assert(offsetof(SceneS, u32_e0) == 0xe0);
#endif

}  // namespace joan::orig
