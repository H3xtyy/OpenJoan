// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Ring class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "fps_timer.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Ring : FpsTimer {
#include "ring_fields.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
