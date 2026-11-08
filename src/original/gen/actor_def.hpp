// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ActorDef class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Actor;
struct Bone;
struct InstanceList;
struct Model;
struct Plane;

#pragma pack(push, 1)
struct ActorDef {
#include "actor_def_fields.inc"
#include "actor_def_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ActorDef) == 0x2d8);
static_assert(offsetof(ActorDef, state) == 0x0);
static_assert(offsetof(ActorDef, loaded_) == 0x3);
static_assert(offsetof(ActorDef, anim_file) == 0x4);
static_assert(offsetof(ActorDef, user) == 0x8);
static_assert(offsetof(ActorDef, model) == 0xc);
static_assert(offsetof(ActorDef, bone_count) == 0x10);
static_assert(offsetof(ActorDef, bone_map) == 0x14);
static_assert(offsetof(ActorDef, mesh_list_18) == 0x18);
static_assert(offsetof(ActorDef, mesh_list_1c) == 0x1c);
static_assert(offsetof(ActorDef, lods_created) == 0x20);
static_assert(offsetof(ActorDef, lod_limit) == 0x24);
static_assert(offsetof(ActorDef, lod_merged) == 0x29);
static_assert(offsetof(ActorDef, lod_count) == 0x2e);
static_assert(offsetof(ActorDef, merged_mask) == 0x33);
static_assert(offsetof(ActorDef, matrices) == 0x34);
static_assert(offsetof(ActorDef, instances) == 0x38);
static_assert(offsetof(ActorDef, batch) == 0x178);
static_assert(offsetof(ActorDef, batch_planes) == 0x18c);
static_assert(offsetof(ActorDef, instance_list) == 0x2cc);
static_assert(offsetof(ActorDef, prev) == 0x2d0);
static_assert(offsetof(ActorDef, next) == 0x2d4);
#endif

}  // namespace joan::orig
