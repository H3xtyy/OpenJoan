// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Viewport class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "camera.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Viewport : Camera {
#include "viewport_fields.inc"
#include "viewport_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
