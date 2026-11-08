// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's OrderEntry class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct OrderEntry {
#include "order_entry_fields.inc"
#include "order_entry_methods.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(OrderEntry) == 0x21);
static_assert(offsetof(OrderEntry, kind) == 0x0);
static_assert(offsetof(OrderEntry, priority) == 0x4);
static_assert(offsetof(OrderEntry, target) == 0x5);
static_assert(offsetof(OrderEntry, radius) == 0x9);
static_assert(offsetof(OrderEntry, spread) == 0xd);
static_assert(offsetof(OrderEntry, started) == 0x11);
static_assert(offsetof(OrderEntry, arg_15) == 0x15);
static_assert(offsetof(OrderEntry, arg_19) == 0x19);
static_assert(offsetof(OrderEntry, expire_time) == 0x1d);
#endif

}  // namespace joan::orig
