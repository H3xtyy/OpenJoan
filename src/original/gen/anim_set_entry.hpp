// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimSetEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "anim_group_rec.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct AnimSetEntry : AnimGroupRec {
#include "anim_set_entry_fields.inc"
#include "anim_set_entry_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AnimSetEntry) == 0x99);
#endif

}  // namespace joan::orig
