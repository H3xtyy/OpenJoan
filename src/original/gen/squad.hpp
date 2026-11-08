// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Squad class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../unit.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Squad : Unit {
#include "squad_fields.inc"
#include "squad_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
