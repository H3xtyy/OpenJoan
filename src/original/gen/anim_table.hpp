// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's AnimTable class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "anim_set.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct AnimTable : AnimSet {
#include "anim_table_fields.inc"
#include "anim_table_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
