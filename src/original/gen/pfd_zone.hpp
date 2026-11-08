// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PfdZone class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct PathSpace;
struct PfdPointPtrArray;
struct PfdZonePtrArray;

#pragma pack(push, 1)
struct PfdZone {
#include "pfd_zone_fields.inc"
#include "pfd_zone_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(PfdZone, id) == 0x0);
static_assert(offsetof(PfdZone, kind) == 0x4);
static_assert(offsetof(PfdZone, bbox_min) == 0x8);
static_assert(offsetof(PfdZone, bbox_max) == 0x14);
static_assert(offsetof(PfdZone, blocked) == 0x20);
static_assert(offsetof(PfdZone, name_id) == 0x24);
static_assert(offsetof(PfdZone, point_count) == 0x28);
static_assert(offsetof(PfdZone, visit_stamp) == 0x2c);
static_assert(offsetof(PfdZone, points) == 0x30);
static_assert(offsetof(PfdZone, links) == 0x34);
static_assert(offsetof(PfdZone, search_stamp) == 0x38);
static_assert(offsetof(PfdZone, came_from) == 0x3c);
static_assert(offsetof(PfdZone, children) == 0x40);
static_assert(offsetof(PfdZone, open_next) == 0x44);
static_assert(offsetof(PfdZone, centre_x) == 0x48);
static_assert(offsetof(PfdZone, centre_y) == 0x4c);
static_assert(offsetof(PfdZone, centre_z) == 0x50);
static_assert(offsetof(PfdZone, child_count) == 0x52);
static_assert(offsetof(PfdZone, closed) == 0x57);
static_assert(offsetof(PfdZone, cost_g) == 0x58);
static_assert(offsetof(PfdZone, cost_f) == 0x5c);
static_assert(offsetof(PfdZone, cost_h) == 0x60);
static_assert(offsetof(PfdZone, flags) == 0x64);
static_assert(offsetof(PfdZone, max_flags) == 0x66);
static_assert(offsetof(PfdZone, flags_sum) == 0x68);
static_assert(offsetof(PfdZone, path_space) == 0x6c);
static_assert(offsetof(PfdZone, group) == 0x70);
#endif

}  // namespace joan::orig
