// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LevelRec class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LevelRec {
#include "level_rec_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LevelRec) == 0x7);
static_assert(offsetof(LevelRec, level) == 0x0);
static_assert(offsetof(LevelRec, experience) == 0x1);
static_assert(offsetof(LevelRec, gain_12c) == 0x5);
static_assert(offsetof(LevelRec, stat_points) == 0x6);
#endif

}  // namespace joan::orig
