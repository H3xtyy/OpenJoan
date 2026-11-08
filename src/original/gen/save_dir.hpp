// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SaveDir class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SaveDir : DynArray {
#include "save_dir_fields.inc"
#include "save_dir_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(SaveDir, path) == 0x1d);
static_assert(offsetof(SaveDir, u8_121) == 0x121);
static_assert(offsetof(SaveDir, char_122) == 0x122);
#endif

}  // namespace joan::orig
