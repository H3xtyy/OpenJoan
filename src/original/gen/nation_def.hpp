// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's NationDef class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct NationDef {
#include "nation_def_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(NationDef) == 0x4e);
static_assert(offsetof(NationDef, name) == 0x0);
static_assert(offsetof(NationDef, id) == 0x9);
static_assert(offsetof(NationDef, text) == 0xb);
static_assert(offsetof(NationDef, race) == 0x4c);
#endif

}  // namespace joan::orig
