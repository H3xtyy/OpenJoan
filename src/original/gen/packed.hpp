// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Packed class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "order_entry.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct Packed : OrderEntry {
#include "packed_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Packed) == 0x21);
#endif

}  // namespace joan::orig
