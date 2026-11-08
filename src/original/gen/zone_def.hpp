// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ZoneDef class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ZoneDef {
#include "zone_def_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(ZoneDef, name) == 0x4);
static_assert(offsetof(ZoneDef, settle) == 0x25);
static_assert(offsetof(ZoneDef, kind) == 0x26);
static_assert(offsetof(ZoneDef, shape_count) == 0x27);
static_assert(offsetof(ZoneDef, values) == 0x28);
static_assert(offsetof(ZoneDef, owner) == 0x38);
static_assert(offsetof(ZoneDef, script) == 0x3c);
static_assert(offsetof(ZoneDef, disable) == 0x45);
static_assert(offsetof(ZoneDef, flag_46) == 0x46);
static_assert(offsetof(ZoneDef, flag_47) == 0x47);
static_assert(offsetof(ZoneDef, shape_kind) == 0x48);
static_assert(offsetof(ZoneDef, coords) == 0x4c);
static_assert(offsetof(ZoneDef, time) == 0xdc);
#endif

}  // namespace joan::orig
