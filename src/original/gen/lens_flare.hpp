// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LensFlare class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "lens_flares.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LensFlare : LensFlares {
#include "lens_flare_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LensFlare) == 0x14);
#endif

}  // namespace joan::orig
