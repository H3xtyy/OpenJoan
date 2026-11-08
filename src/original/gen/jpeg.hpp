// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Jpeg class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct JpegCompress;
struct JpegErrorMgr;

#pragma pack(push, 1)
struct Jpeg {
#include "jpeg_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Jpeg, cinfo) == 0x0);
static_assert(offsetof(Jpeg, err) == 0x4);
static_assert(offsetof(Jpeg, u32_08) == 0x8);
#endif

}  // namespace joan::orig
