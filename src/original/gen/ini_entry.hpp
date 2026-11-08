// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's IniEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct IniEntry {
#include "ini_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(IniEntry) == 0x109);
static_assert(offsetof(IniEntry, key) == 0x0);
static_assert(offsetof(IniEntry, value) == 0x9);
#endif

}  // namespace joan::orig
