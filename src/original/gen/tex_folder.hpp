// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexFolder class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct TexFolder {
#include "tex_folder_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TexFolder) == 0x10c);
static_assert(offsetof(TexFolder, path) == 0x0);
static_assert(offsetof(TexFolder, recurse) == 0x105);
static_assert(offsetof(TexFolder, tex_state) == 0x106);
static_assert(offsetof(TexFolder, sub_state) == 0x107);
static_assert(offsetof(TexFolder, kind) == 0x108);
#endif

}  // namespace joan::orig
