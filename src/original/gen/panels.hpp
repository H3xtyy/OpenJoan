// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Panels class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "stream_slot.hpp"

namespace joan::orig {

struct Sound3D;

#pragma pack(push, 1)
struct Panels {
#include "panels_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Panels) == 0x134);
static_assert(offsetof(Panels, volume) == 0x0);
static_assert(offsetof(Panels, source_a) == 0x4);
static_assert(offsetof(Panels, source_c) == 0x8);
static_assert(offsetof(Panels, source_d) == 0xc);
static_assert(offsetof(Panels, source_b) == 0x10);
static_assert(offsetof(Panels, slots) == 0x14);
#endif

}  // namespace joan::orig
