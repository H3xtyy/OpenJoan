// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's KeyList16 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "colour_track.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct KeyList16 : ColourTrack {
#include "key_list16_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(KeyList16) == 0x8);
#endif

}  // namespace joan::orig
