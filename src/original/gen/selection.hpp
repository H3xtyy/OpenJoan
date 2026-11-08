// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Selection class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Selection {
#include "selection_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Selection, flags_a) == 0x2e);
static_assert(offsetof(Selection, flags_b) == 0x38);
static_assert(offsetof(Selection, u16_40) == 0x40);
static_assert(offsetof(Selection, u16_42) == 0x42);
static_assert(offsetof(Selection, u16_46) == 0x46);
static_assert(offsetof(Selection, u16_48) == 0x48);
static_assert(offsetof(Selection, u16_4a) == 0x4a);
static_assert(offsetof(Selection, u16_4c) == 0x4c);
static_assert(offsetof(Selection, u16_50) == 0x50);
static_assert(offsetof(Selection, u16_52) == 0x52);
static_assert(offsetof(Selection, u16_54) == 0x54);
static_assert(offsetof(Selection, u16_56) == 0x56);
static_assert(offsetof(Selection, u16_58) == 0x58);
static_assert(offsetof(Selection, u16_5a) == 0x5a);
static_assert(offsetof(Selection, u16_5e) == 0x5e);
static_assert(offsetof(Selection, u16_60) == 0x60);
static_assert(offsetof(Selection, u16_62) == 0x62);
static_assert(offsetof(Selection, u16_64) == 0x64);
static_assert(offsetof(Selection, u16_68) == 0x68);
static_assert(offsetof(Selection, u16_6a) == 0x6a);
static_assert(offsetof(Selection, u16_6c) == 0x6c);
static_assert(offsetof(Selection, u16_6e) == 0x6e);
static_assert(offsetof(Selection, markers) == 0x70);
#endif

}  // namespace joan::orig
