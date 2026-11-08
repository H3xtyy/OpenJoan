// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexTag class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TexTag {
#include "tex_tag_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TexTag) == 0x8);
static_assert(offsetof(TexTag, name) == 0x0);
static_assert(offsetof(TexTag, bit) == 0x6);
#endif

}  // namespace joan::orig
