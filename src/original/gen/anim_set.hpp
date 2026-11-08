// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimSet class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct AnimGroupRec;

#pragma pack(push, 1)
struct AnimSet {
#include "anim_set_fields.inc"
#include "anim_set_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(AnimSet, id) == 0x0);
static_assert(offsetof(AnimSet, group_count) == 0x2c);
static_assert(offsetof(AnimSet, groups) == 0x30);
static_assert(offsetof(AnimSet, ref_count) == 0x34);
static_assert(offsetof(AnimSet, stream_state) == 0x38);
static_assert(offsetof(AnimSet, owns_groups) == 0x39);
#endif

}  // namespace joan::orig
