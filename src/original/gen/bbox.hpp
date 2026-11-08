// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BBox class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../engine.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct BBox : Box {
#include "bbox_fields.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
