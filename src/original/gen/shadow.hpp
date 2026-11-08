// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Shadow class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "skin.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Shadow : Skin {
#include "shadow_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Shadow) == 0x3e);
#endif

}  // namespace joan::orig
