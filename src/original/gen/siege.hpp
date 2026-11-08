// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's Siege class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "facility.hpp"
#include "siege_order.hpp"

namespace joan::orig {

struct Billboard;

#pragma pack(push, 1)
struct Siege : Facility {
#include "siege_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(Siege) == 0x2f14);
static_assert(offsetof(Siege, i32_79) == 0x79);
static_assert(offsetof(Siege, u32_7d) == 0x7d);
static_assert(offsetof(Siege, order_arg) == 0x81);
static_assert(offsetof(Siege, order_b) == 0x85);
static_assert(offsetof(Siege, order_c) == 0x86);
static_assert(offsetof(Siege, order_d) == 0x8a);
static_assert(offsetof(Siege, order_e) == 0x8e);
static_assert(offsetof(Siege, order_f) == 0x92);
static_assert(offsetof(Siege, delay) == 0x9e);
static_assert(offsetof(Siege, n_types) == 0xa2);
static_assert(offsetof(Siege, type_kind) == 0xa4);
static_assert(offsetof(Siege, type_cost) == 0xac);
static_assert(offsetof(Siege, type_need) == 0xb4);
static_assert(offsetof(Siege, type_rate) == 0xbc);
static_assert(offsetof(Siege, type_stock) == 0xcc);
static_assert(offsetof(Siege, type_made) == 0xdc);
static_assert(offsetof(Siege, group_id) == 0xec);
static_assert(offsetof(Siege, n_slots) == 0xf0);
static_assert(offsetof(Siege, slots) == 0xf2);
static_assert(offsetof(Siege, n_queue) == 0x112);
static_assert(offsetof(Siege, queue) == 0x114);
static_assert(offsetof(Siege, current) == 0x2f04);
static_assert(offsetof(Siege, time) == 0x2f06);
static_assert(offsetof(Siege, i32_2f0a) == 0x2f0a);
static_assert(offsetof(Siege, u8_2f0e) == 0x2f0e);
static_assert(offsetof(Siege, u8_2f0f) == 0x2f0f);
static_assert(offsetof(Siege, life_bar) == 0x2f10);
#endif

}  // namespace joan::orig
