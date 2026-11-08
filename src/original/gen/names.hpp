// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Names class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Names : DynArray {
#include "names_fields.inc"
#include "names_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
