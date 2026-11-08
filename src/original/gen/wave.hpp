// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Wave class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Audio;

#pragma pack(push, 1)
struct Wave {
#include "wave_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Wave, not_loaded) == 0x50);
static_assert(offsetof(Wave, wave_count) == 0x54);
static_assert(offsetof(Wave, audio) == 0x64);
#endif

}  // namespace joan::orig
