// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's IDirectSound class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct IDirectSound {
#include "idirect_sound_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(IDirectSound, vtable) == 0x0);
#endif

}  // namespace joan::orig
