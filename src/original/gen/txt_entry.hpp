// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TxtEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TxtEntry {
#include "txt_entry_fields.inc"
#include "txt_entry_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TxtEntry) == 0x15);
static_assert(offsetof(TxtEntry, id) == 0x0);
static_assert(offsetof(TxtEntry, key) == 0x4);
static_assert(offsetof(TxtEntry, name) == 0x8);
static_assert(offsetof(TxtEntry, text) == 0x11);
#endif

}  // namespace joan::orig
