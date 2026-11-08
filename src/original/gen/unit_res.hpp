// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitRes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct UnitType;

#pragma pack(push, 1)
struct UnitRes {
#include "unit_res_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UnitRes) == 0x7);
static_assert(offsetof(UnitRes, types) == 0x0);
static_assert(offsetof(UnitRes, count) == 0x4);
static_assert(offsetof(UnitRes, loaded) == 0x6);
#endif

}  // namespace joan::orig
