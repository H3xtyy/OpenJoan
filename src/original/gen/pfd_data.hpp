// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's PfdData class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "path_space.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct PfdData : PathSpace {
#include "pfd_data_fields.inc"
#include "pfd_data_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
