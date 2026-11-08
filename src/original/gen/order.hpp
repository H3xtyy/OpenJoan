// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Order class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../dynarray.hpp"
#include "order_entry.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Order : DynArray {
#include "order_fields.inc"
#include "order_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Order) == 0x3e);
static_assert(offsetof(Order, current) == 0x1d);
#endif

}  // namespace joan::orig
