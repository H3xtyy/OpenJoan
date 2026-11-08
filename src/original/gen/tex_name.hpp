// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexName class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TexName {
#include "tex_name_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TexName) == 0x28);
static_assert(offsetof(TexName, name) == 0x0);
static_assert(offsetof(TexName, ext_mask) == 0x22);
static_assert(offsetof(TexName, dir) == 0x24);
#endif

}  // namespace joan::orig
