// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LensFlares class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct FlareEntry;

#pragma pack(push, 1)
struct LensFlares {
#include "lens_flares_fields.inc"
#include "lens_flares_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LensFlares) == 0x14);
static_assert(offsetof(LensFlares, entries) == 0x0);
static_assert(offsetof(LensFlares, count) == 0x4);
static_assert(offsetof(LensFlares, steps) == 0x6);
static_assert(offsetof(LensFlares, fade_min) == 0x8);
static_assert(offsetof(LensFlares, fade_max) == 0xc);
static_assert(offsetof(LensFlares, centre_x) == 0x10);
static_assert(offsetof(LensFlares, centre_y) == 0x12);
#endif

}  // namespace joan::orig
