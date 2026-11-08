// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimDb class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct TexScriptRec;

#pragma pack(push, 1)
struct AnimDb {
#include "anim_db_fields.inc"
#include "anim_db_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AnimDb) == 0x7);
static_assert(offsetof(AnimDb, records) == 0x0);
static_assert(offsetof(AnimDb, count) == 0x4);
static_assert(offsetof(AnimDb, loaded) == 0x6);
#endif

}  // namespace joan::orig
