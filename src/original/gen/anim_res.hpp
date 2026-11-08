// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"
#include "../res_idx.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct AnimRes {
#include "anim_res_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(AnimRes, count) == 0x0);
static_assert(offsetof(AnimRes, entries) == 0x4);
static_assert(offsetof(AnimRes, loaded) == 0x8);
static_assert(offsetof(AnimRes, index_a) == 0xc);
static_assert(offsetof(AnimRes, index_b) == 0x141);
static_assert(offsetof(AnimRes, list) == 0x276);
#endif

}  // namespace joan::orig
