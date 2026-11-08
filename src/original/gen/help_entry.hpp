// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's HelpEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct HelpEntry {
#include "help_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(HelpEntry) == 0x70);
static_assert(offsetof(HelpEntry, key) == 0x0);
static_assert(offsetof(HelpEntry, nums) == 0x9);
static_assert(offsetof(HelpEntry, title) == 0x2d);
static_assert(offsetof(HelpEntry, text) == 0x6a);
static_assert(offsetof(HelpEntry, text_len) == 0x6e);
#endif

}  // namespace joan::orig
