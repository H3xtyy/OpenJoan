// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's NameEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct NameEntry {
#include "name_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(NameEntry) == 0x38);
static_assert(offsetof(NameEntry, index) == 0x0);
static_assert(offsetof(NameEntry, name) == 0x4);
static_assert(offsetof(NameEntry, u8_36) == 0x36);
#endif

}  // namespace joan::orig
