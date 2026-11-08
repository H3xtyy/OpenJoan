// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SpatialIndex class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "sp_index.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SpatialIndex : SpIndex {
#include "spatial_index_fields.inc"
#include "spatial_index_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
