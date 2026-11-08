// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Ini class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IniEntry;
struct IniEntryPtr;

#pragma pack(push, 1)
struct Ini {
#include "ini_fields.inc"
#include "ini_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Ini) == 0xf);
static_assert(offsetof(Ini, name) == 0x0);
static_assert(offsetof(Ini, entries) == 0x9);
static_assert(offsetof(Ini, entry_count) == 0xd);
#endif

}  // namespace joan::orig
