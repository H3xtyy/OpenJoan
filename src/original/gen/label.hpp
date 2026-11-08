// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Label class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "image2d.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Label : Image2D {
#include "label_fields.inc"
#include "label_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Label) == 0xb7);
#endif

}  // namespace joan::orig
