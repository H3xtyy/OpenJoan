// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's NameList class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct NameList {
#include "name_list_fields.inc"
#include "name_list_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(NameList, values) == 0x3c);
static_assert(offsetof(NameList, text_lens) == 0x60);
static_assert(offsetof(NameList, u32_69) == 0x69);
static_assert(offsetof(NameList, u8_8d) == 0x8d);
static_assert(offsetof(NameList, u32_96) == 0x96);
static_assert(offsetof(NameList, start_time) == 0xba);
static_assert(offsetof(NameList, count) == 0xbe);
static_assert(offsetof(NameList, dirty) == 0xc0);
#endif

}  // namespace joan::orig
