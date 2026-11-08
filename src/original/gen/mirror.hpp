// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Mirror class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "vec3f.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Mirror {
#include "mirror_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Mirror) == 0xbc);
static_assert(offsetof(Mirror, eye) == 0x0);
static_assert(offsetof(Mirror, at) == 0xc);
static_assert(offsetof(Mirror, up) == 0x18);
static_assert(offsetof(Mirror, dir) == 0x24);
static_assert(offsetof(Mirror, right) == 0x30);
static_assert(offsetof(Mirror, view) == 0x3c);
static_assert(offsetof(Mirror, inv_view) == 0x7c);
#endif

}  // namespace joan::orig
