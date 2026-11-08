// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's TexDirEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct TexFolder;
struct TexName;

#pragma pack(push, 1)
struct TexDirEntry {
#include "tex_dir_entry_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(TexDirEntry) == 0x3d);
static_assert(offsetof(TexDirEntry, index) == 0x0);
static_assert(offsetof(TexDirEntry, name) == 0x4);
static_assert(offsetof(TexDirEntry, name_count) == 0x25);
static_assert(offsetof(TexDirEntry, name_cap) == 0x29);
static_assert(offsetof(TexDirEntry, buf_2d) == 0x2d);
static_assert(offsetof(TexDirEntry, file_count) == 0x31);
static_assert(offsetof(TexDirEntry, file_cap) == 0x35);
static_assert(offsetof(TexDirEntry, files) == 0x39);
#endif

}  // namespace joan::orig
