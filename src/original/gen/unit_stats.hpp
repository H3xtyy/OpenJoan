// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitStats class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitStats {
#include "unit_stats_fields.inc"
#include "unit_stats_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UnitStats) == 0x3b);
static_assert(offsetof(UnitStats, values) == 0x0);
static_assert(offsetof(UnitStats, u16_38) == 0x38);
#endif

}  // namespace joan::orig
