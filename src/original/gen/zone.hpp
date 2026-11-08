// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Zone class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "input.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Zone : Input {
#include "zone_fields.inc"
#include "zone_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
