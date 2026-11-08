// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's FlareEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../gameset.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct FlareEntry {
#include "flare_entry_fields.inc"
#include "flare_entry_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(FlareEntry) == 0x19b);
static_assert(offsetof(FlareEntry, colour) == 0x0);
static_assert(offsetof(FlareEntry, size) == 0x4);
static_assert(offsetof(FlareEntry, scale) == 0x6);
static_assert(offsetof(FlareEntry, texture) == 0xa);
#endif

}  // namespace joan::orig
