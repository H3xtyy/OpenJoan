// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's UnitTypes class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "unit_res.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct UnitTypes : UnitRes {
#include "unit_types_fields.inc"
#include "unit_types_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
