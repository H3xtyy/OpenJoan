// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Lists class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../en3d.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Lists : TextureManager {
#include "lists_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Lists) == 0x3c);
#endif

}  // namespace joan::orig
