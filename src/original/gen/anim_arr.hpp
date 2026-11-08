// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimArr class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "alpha_track.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct AnimArr : AlphaTrack {
#include "anim_arr_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AnimArr) == 0x8);
#endif

}  // namespace joan::orig
