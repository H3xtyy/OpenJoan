// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Types class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Types {
#include "types_fields.inc"
#include "types_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Types, list) == 0x4);
#endif

}  // namespace joan::orig
