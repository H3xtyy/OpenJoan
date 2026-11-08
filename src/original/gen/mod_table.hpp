// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ModTable class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "item_def.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct ModTable : ItemDef {
#include "mod_table_fields.inc"
#include "mod_table_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
