// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's List class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

struct Font2d;

#pragma pack(push, 1)
struct List : DynArray {
#include "list_fields.inc"
#include "list_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
