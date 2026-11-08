// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ColourKey class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ColourKey {
#include "colour_key_fields.inc"
#include "colour_key_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ColourKey) == 0x10);
static_assert(offsetof(ColourKey, time) == 0x0);
static_assert(offsetof(ColourKey, rgb) == 0x4);
#endif

}  // namespace joan::orig
