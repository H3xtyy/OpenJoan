// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Bitmap class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Bitmap {
#include "bitmap_fields.inc"
#include "bitmap_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Bitmap, width_) == 0x0);
static_assert(offsetof(Bitmap, height_) == 0x2);
#endif

}  // namespace joan::orig
