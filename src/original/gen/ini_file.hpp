// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's IniFile class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct IniPtr;

#pragma pack(push, 1)
struct IniFile {
#include "ini_file_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(IniFile, sections) == 0x0);
static_assert(offsetof(IniFile, section_count) == 0x4);
#endif

}  // namespace joan::orig
