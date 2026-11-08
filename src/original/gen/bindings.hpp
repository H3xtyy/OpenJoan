// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Bindings class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "remap_keys.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Bindings : RemapKeys {
#include "bindings_fields.inc"
#include "bindings_methods.inc"
};
#pragma pack(pop)

}  // namespace joan::orig
