// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Marker class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Billboard;

#pragma pack(push, 1)
struct Marker {
#include "marker_fields.inc"
#include "marker_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Marker, index) == 0x0);
static_assert(offsetof(Marker, kind) == 0x4);
static_assert(offsetof(Marker, pos) == 0x8);
static_assert(offsetof(Marker, pos2) == 0x14);
static_assert(offsetof(Marker, size) == 0x20);
static_assert(offsetof(Marker, u32_2a) == 0x2a);
static_assert(offsetof(Marker, u32_2e) == 0x2e);
static_assert(offsetof(Marker, lit) == 0x44);
static_assert(offsetof(Marker, alpha) == 0x45);
static_assert(offsetof(Marker, target_alpha) == 0x49);
static_assert(offsetof(Marker, fade_time) == 0x4d);
static_assert(offsetof(Marker, billboard) == 0x51);
#endif

}  // namespace joan::orig
