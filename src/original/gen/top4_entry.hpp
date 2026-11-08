// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Top4Entry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Top4Entry {
#include "top4_entry_fields.inc"
#include "top4_entry_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Top4Entry) == 0x8);
static_assert(offsetof(Top4Entry, key) == 0x0);
static_assert(offsetof(Top4Entry, value) == 0x4);
#endif

}  // namespace joan::orig
