// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Blitter class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "surface.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Blitter : Surface {
#include "blitter_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Blitter) == 0x34);
#endif

}  // namespace joan::orig
