// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Textures class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct TexturePtrArray;

#pragma pack(push, 1)
struct Textures {
#include "textures_fields.inc"
#include "textures_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Textures, table) == 0x8);
static_assert(offsetof(Textures, mode) == 0x30);
#endif

}  // namespace joan::orig
