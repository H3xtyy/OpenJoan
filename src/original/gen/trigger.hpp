// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Trigger class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct SceneS;

#pragma pack(push, 1)
struct Trigger {
#include "trigger_fields.inc"
#include "trigger_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Trigger, flags) == 0x0);
static_assert(offsetof(Trigger, target_kind) == 0x4);
static_assert(offsetof(Trigger, target_id) == 0x8);
static_assert(offsetof(Trigger, ref_kind) == 0xa);
static_assert(offsetof(Trigger, ref_id) == 0xe);
static_assert(offsetof(Trigger, sub) == 0x10);
static_assert(offsetof(Trigger, center) == 0x13);
static_assert(offsetof(Trigger, range) == 0x1f);
static_assert(offsetof(Trigger, value24) == 0x24);
static_assert(offsetof(Trigger, next_time) == 0x26);
static_assert(offsetof(Trigger, expire_time) == 0x2a);
static_assert(offsetof(Trigger, active) == 0x2e);
static_assert(offsetof(Trigger, radius) == 0x2f);
static_assert(offsetof(Trigger, scene) == 0x3f);
#endif

}  // namespace joan::orig
