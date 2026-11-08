// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Text class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "font2d.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Text : Font2d {
#include "text_fields.inc"
#include "text_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
