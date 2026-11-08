// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitData class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

struct Grid;
struct HeldItem;

#pragma pack(push, 1)
struct UnitData {
#include "unit_data_fields.inc"
#include "unit_data_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(UnitData) == 0x1e);
static_assert(offsetof(UnitData, hand_mode) == 0x0);
static_assert(offsetof(UnitData, hands) == 0x2);
static_assert(offsetof(UnitData, equipped) == 0xa);
static_assert(offsetof(UnitData, extra_item) == 0xe);
static_assert(offsetof(UnitData, list_12) == 0x12);
static_assert(offsetof(UnitData, grid_16) == 0x16);
static_assert(offsetof(UnitData, grid_1a) == 0x1a);
#endif

}  // namespace joan::orig
