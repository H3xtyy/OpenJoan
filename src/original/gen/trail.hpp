// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Trail class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "trail_nodes.hpp"

namespace joan::orig {

struct Actor;
struct ActorDef;
struct Scene;

#pragma pack(push, 1)
struct Trail {
#include "trail_fields.inc"
#include "trail_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Trail) == 0xaf);
static_assert(offsetof(Trail, actor) == 0x0);
static_assert(offsetof(Trail, actor_def) == 0x4);
static_assert(offsetof(Trail, scene) == 0x8);
static_assert(offsetof(Trail, unit_id) == 0xc);
static_assert(offsetof(Trail, vtx_base) == 0x10);
static_assert(offsetof(Trail, attached) == 0x14);
static_assert(offsetof(Trail, enabled) == 0x15);
static_assert(offsetof(Trail, points) == 0x16);
static_assert(offsetof(Trail, point_count) == 0x8e);
static_assert(offsetof(Trail, node_count) == 0x90);
static_assert(offsetof(Trail, rate_hz) == 0x92);
static_assert(offsetof(Trail, max_nodes) == 0x94);
static_assert(offsetof(Trail, dirty) == 0x96);
static_assert(offsetof(Trail, hold_start) == 0x97);
static_assert(offsetof(Trail, hold_ms) == 0x9b);
static_assert(offsetof(Trail, last_add) == 0x9f);
static_assert(offsetof(Trail, nodes) == 0xa3);
#endif

}  // namespace joan::orig
