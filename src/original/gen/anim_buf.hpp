// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimBuf class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct ModelSlot;

#pragma pack(push, 1)
struct AnimBuf {
#include "anim_buf_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(AnimBuf, data) == 0x0);
static_assert(offsetof(AnimBuf, ref_count) == 0x4);
static_assert(offsetof(AnimBuf, state) == 0x6);
#endif

}  // namespace joan::orig
