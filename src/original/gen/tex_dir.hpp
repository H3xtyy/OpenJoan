// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexDir class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct TexDirEntry;
struct TexTag;

#pragma pack(push, 1)
struct TexDir {
#include "tex_dir_fields.inc"
#include "tex_dir_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(TexDir, ready) == 0x0);
static_assert(offsetof(TexDir, dir_count) == 0x4);
static_assert(offsetof(TexDir, dir_cap) == 0x8);
static_assert(offsetof(TexDir, dirs) == 0xc);
static_assert(offsetof(TexDir, tag_count) == 0x10);
static_assert(offsetof(TexDir, tag_cap) == 0x14);
static_assert(offsetof(TexDir, tags) == 0x18);
#endif

}  // namespace joan::orig
