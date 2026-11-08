// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TextDesc class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "image2d.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TextDesc : Image2D {
#include "text_desc_fields.inc"
#include "text_desc_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TextDesc) == 0xb7);
#endif

}  // namespace joan::orig
