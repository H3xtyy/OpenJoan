// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Top4 class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "top4_entry.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Top4 {
#include "top4_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Top4) == 0x28);
static_assert(offsetof(Top4, sample) == 0x0);
static_assert(offsetof(Top4, rec_no) == 0x4);
static_assert(offsetof(Top4, count) == 0x6);
static_assert(offsetof(Top4, e0) == 0x8);
static_assert(offsetof(Top4, e1) == 0x10);
static_assert(offsetof(Top4, e2) == 0x18);
static_assert(offsetof(Top4, e3) == 0x20);
#endif

}  // namespace joan::orig
