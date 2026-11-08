// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LevelRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct LevelRec;

#pragma pack(push, 1)
struct LevelRes {
#include "level_res_fields.inc"
#include "level_res_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LevelRes) == 0x9);
static_assert(offsetof(LevelRes, count) == 0x0);
static_assert(offsetof(LevelRes, levels) == 0x4);
static_assert(offsetof(LevelRes, loaded) == 0x8);
#endif

}  // namespace joan::orig
