// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BmpEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct BmpEntry {
#include "bmp_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BmpEntry) == 0x38);
static_assert(offsetof(BmpEntry, name) == 0x0);
static_assert(offsetof(BmpEntry, file) == 0x9);
static_assert(offsetof(BmpEntry, x0) == 0x2a);
static_assert(offsetof(BmpEntry, y0) == 0x2c);
static_assert(offsetof(BmpEntry, x1) == 0x2e);
static_assert(offsetof(BmpEntry, y1) == 0x30);
static_assert(offsetof(BmpEntry, u16_32) == 0x32);
static_assert(offsetof(BmpEntry, width) == 0x34);
static_assert(offsetof(BmpEntry, height) == 0x36);
#endif

}  // namespace joan::orig
