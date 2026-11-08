// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Slots class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Slots {
#include "slots_fields.inc"
#include "slots_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Slots, u32_0) == 0x0);
static_assert(offsetof(Slots, arr_4) == 0x4);
static_assert(offsetof(Slots, ptr_c) == 0xc);
static_assert(offsetof(Slots, arr_44) == 0x44);
static_assert(offsetof(Slots, i32_56) == 0x56);
#endif

}  // namespace joan::orig
