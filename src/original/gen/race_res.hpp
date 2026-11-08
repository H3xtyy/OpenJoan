// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's RaceRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct RaceRes {
#include "race_res_fields.inc"
#include "race_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(RaceRes, loaded) == 0x0);
static_assert(offsetof(RaceRes, count) == 0x4);
static_assert(offsetof(RaceRes, races) == 0x8);
#endif

}  // namespace joan::orig
