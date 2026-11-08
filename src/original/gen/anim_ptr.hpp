// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimPtr class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Anim;

#pragma pack(push, 1)
struct AnimPtr {
#include "anim_ptr_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(AnimPtr) == 0x4);
static_assert(offsetof(AnimPtr, anim) == 0x0);
#endif

}  // namespace joan::orig
