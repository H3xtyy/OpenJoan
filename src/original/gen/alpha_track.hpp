// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AlphaTrack class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct AlphaKey;

#pragma pack(push, 1)
struct AlphaTrack {
#include "alpha_track_fields.inc"
#include "alpha_track_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AlphaTrack) == 0x8);
static_assert(offsetof(AlphaTrack, count) == 0x0);
static_assert(offsetof(AlphaTrack, keys) == 0x4);
#endif

}  // namespace joan::orig
