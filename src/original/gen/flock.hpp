// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Flock class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct DynArray;
struct Scene;
struct SceneS;

#pragma pack(push, 1)
struct Flock {
#include "flock_fields.inc"
#include "flock_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Flock) == 0x5b);
static_assert(offsetof(Flock, id) == 0x0);
static_assert(offsetof(Flock, kind) == 0x2);
static_assert(offsetof(Flock, scene) == 0x6);
static_assert(offsetof(Flock, group_node) == 0xa);
static_assert(offsetof(Flock, active) == 0xe);
static_assert(offsetof(Flock, max_speed) == 0xf);
static_assert(offsetof(Flock, max_urgency) == 0x13);
static_assert(offsetof(Flock, max_urgency2) == 0x17);
static_assert(offsetof(Flock, speed_urgency) == 0x1b);
static_assert(offsetof(Flock, half_speed) == 0x1f);
static_assert(offsetof(Flock, keep_away) == 0x23);
static_assert(offsetof(Flock, separate) == 0x27);
static_assert(offsetof(Flock, def_range) == 0x2b);
static_assert(offsetof(Flock, use_truth) == 0x2f);
static_assert(offsetof(Flock, react_enemy) == 0x30);
static_assert(offsetof(Flock, width) == 0x31);
static_assert(offsetof(Flock, height) == 0x33);
static_assert(offsetof(Flock, num_flock) == 0x35);
static_assert(offsetof(Flock, num_entity) == 0x37);
static_assert(offsetof(Flock, spawned_groups) == 0x39);
static_assert(offsetof(Flock, box) == 0x3b);
static_assert(offsetof(Flock, members) == 0x53);
static_assert(offsetof(Flock, groups) == 0x57);
#endif

}  // namespace joan::orig
