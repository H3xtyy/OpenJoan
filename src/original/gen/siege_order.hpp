// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's SiegeOrder class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct SiegeOrder {
#include "siege_order_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(SiegeOrder) == 0x93);
static_assert(offsetof(SiegeOrder, kind) == 0x0);
static_assert(offsetof(SiegeOrder, started) == 0x2);
static_assert(offsetof(SiegeOrder, queued_time) == 0x3);
static_assert(offsetof(SiegeOrder, start_time) == 0x7);
static_assert(offsetof(SiegeOrder, duration) == 0xb);
static_assert(offsetof(SiegeOrder, group) == 0x91);
#endif

}  // namespace joan::orig
