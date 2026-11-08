// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Elem class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Elem : DynArrayB {
#include "elem_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(Elem, alive) == 0x2e);
static_assert(offsetof(Elem, emitted) == 0x32);
static_assert(offsetof(Elem, last_emit) == 0x36);
static_assert(offsetof(Elem, carry) == 0x3a);
#endif

}  // namespace joan::orig
