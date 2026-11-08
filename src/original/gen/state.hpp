// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's State class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "hud.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct State : Hud {
#include "state_fields.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
