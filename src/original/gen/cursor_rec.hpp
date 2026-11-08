// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CursorRec class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Bitmap;

#pragma pack(push, 1)
struct CursorRec {
#include "cursor_rec_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(CursorRec) == 0x8);
static_assert(offsetof(CursorRec, hot_x) == 0x0);
static_assert(offsetof(CursorRec, hot_y) == 0x2);
static_assert(offsetof(CursorRec, bitmap) == 0x4);
#endif

}  // namespace joan::orig
