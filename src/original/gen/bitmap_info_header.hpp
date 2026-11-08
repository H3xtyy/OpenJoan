// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BitmapInfoHeader class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct BitmapInfoHeader {
#include "bitmap_info_header_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BitmapInfoHeader) == 0x28);
static_assert(offsetof(BitmapInfoHeader, size) == 0x0);
static_assert(offsetof(BitmapInfoHeader, width) == 0x4);
static_assert(offsetof(BitmapInfoHeader, height) == 0x8);
static_assert(offsetof(BitmapInfoHeader, planes) == 0xc);
static_assert(offsetof(BitmapInfoHeader, bit_count) == 0xe);
static_assert(offsetof(BitmapInfoHeader, compression) == 0x10);
static_assert(offsetof(BitmapInfoHeader, size_image) == 0x14);
static_assert(offsetof(BitmapInfoHeader, x_pels_per_meter) == 0x18);
static_assert(offsetof(BitmapInfoHeader, y_pels_per_meter) == 0x1c);
static_assert(offsetof(BitmapInfoHeader, clr_used) == 0x20);
static_assert(offsetof(BitmapInfoHeader, clr_important) == 0x24);
#endif

}  // namespace joan::orig
