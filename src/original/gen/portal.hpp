// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Portal class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Portal {
#include "portal_fields.inc"
#include "portal_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Portal, corners) == 0x0);
static_assert(offsetof(Portal, plane) == 0x30);
static_assert(offsetof(Portal, box) == 0x40);
static_assert(offsetof(Portal, side_a) == 0x58);
static_assert(offsetof(Portal, side_b) == 0x5c);
static_assert(offsetof(Portal, open_b) == 0x60);
static_assert(offsetof(Portal, open_a) == 0x61);
static_assert(offsetof(Portal, clipped) == 0x62);
static_assert(offsetof(Portal, open) == 0x63);
static_assert(offsetof(Portal, from_b) == 0x64);
#endif

}  // namespace joan::orig
