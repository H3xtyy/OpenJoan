// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitScript class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitScript {
#include "unit_script_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UnitScript) == 0x28);
static_assert(offsetof(UnitScript, id) == 0x0);
static_assert(offsetof(UnitScript, name) == 0x2);
static_assert(offsetof(UnitScript, object) == 0x22);
static_assert(offsetof(UnitScript, kind) == 0x26);
#endif

}  // namespace joan::orig
