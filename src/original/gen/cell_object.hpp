// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's CellObject class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../unit.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct CellObject : Unit {
#include "cell_object_fields.inc"
#include "cell_object_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
