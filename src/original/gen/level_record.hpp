// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's LevelRecord class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "level_rec.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct LevelRecord : LevelRec {
#include "level_record_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(LevelRecord) == 0x7);
#endif

}  // namespace joan::orig
