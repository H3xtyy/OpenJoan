// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's JpegCompress class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct JpegErrorMgr;

#pragma pack(push, 1)
struct JpegCompress {
#include "jpeg_compress_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(JpegCompress) == 0x15b);
static_assert(offsetof(JpegCompress, err) == 0x0);
static_assert(offsetof(JpegCompress, mem) == 0x4);
static_assert(offsetof(JpegCompress, progress) == 0x8);
static_assert(offsetof(JpegCompress, client_data) == 0xc);
static_assert(offsetof(JpegCompress, is_decompressor) == 0x10);
static_assert(offsetof(JpegCompress, global_state) == 0x11);
static_assert(offsetof(JpegCompress, dest) == 0x15);
static_assert(offsetof(JpegCompress, image_width) == 0x19);
static_assert(offsetof(JpegCompress, image_height) == 0x1d);
static_assert(offsetof(JpegCompress, input_components) == 0x21);
static_assert(offsetof(JpegCompress, in_color_space) == 0x25);
static_assert(offsetof(JpegCompress, next_scanline) == 0xc6);
#endif

}  // namespace joan::orig
