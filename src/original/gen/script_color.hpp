// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ScriptColor class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ScriptColor {
#include "script_color_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ScriptColor) == 0x8);
static_assert(offsetof(ScriptColor, lo) == 0x0);
static_assert(offsetof(ScriptColor, hi) == 0x4);
#endif

}  // namespace joan::orig
