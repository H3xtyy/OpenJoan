// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Terrain class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "cell.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Terrain : Cell {
#include "terrain_fields.inc"
#include "terrain_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
