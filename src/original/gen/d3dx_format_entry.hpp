// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's D3dxFormatEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct D3dxFormatEntry {
#include "d3dx_format_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(D3dxFormatEntry) == 0x28);
static_assert(offsetof(D3dxFormatEntry, format) == 0x0);
static_assert(offsetof(D3dxFormatEntry, kind) == 0x4);
static_assert(offsetof(D3dxFormatEntry, bits_per_pixel) == 0x8);
static_assert(offsetof(D3dxFormatEntry, channel_bits) == 0xc);
static_assert(offsetof(D3dxFormatEntry, convertible) == 0x20);
static_assert(offsetof(D3dxFormatEntry, listed) == 0x24);
#endif

}  // namespace joan::orig
