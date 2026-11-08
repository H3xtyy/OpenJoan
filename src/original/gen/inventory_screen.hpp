// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's InventoryScreen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"

namespace joan::orig {

#pragma pack(push, 1)
struct InventoryScreen {
#include "inventory_screen_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(offsetof(InventoryScreen, tex) == 0x0);
static_assert(offsetof(InventoryScreen, sheet) == 0x4);
static_assert(offsetof(InventoryScreen, open) == 0x8);
static_assert(offsetof(InventoryScreen, press_duration_ms) == 0x9);
static_assert(offsetof(InventoryScreen, press_time_ms) == 0xd);
static_assert(offsetof(InventoryScreen, carried_source) == 0x11);
static_assert(offsetof(InventoryScreen, carried_item) == 0x12);
static_assert(offsetof(InventoryScreen, carried_grid_x) == 0x14);
static_assert(offsetof(InventoryScreen, carried_grid_y) == 0x15);
static_assert(offsetof(InventoryScreen, carried_count) == 0x16);
static_assert(offsetof(InventoryScreen, i32_17) == 0x17);
static_assert(offsetof(InventoryScreen, carried_uses) == 0x1b);
static_assert(offsetof(InventoryScreen, carried_max_uses) == 0x1f);
static_assert(offsetof(InventoryScreen, i32_23) == 0x23);
static_assert(offsetof(InventoryScreen, hover_item) == 0x27);
static_assert(offsetof(InventoryScreen, u32_29) == 0x29);
static_assert(offsetof(InventoryScreen, hover_source) == 0x2d);
static_assert(offsetof(InventoryScreen, hover_grid_key) == 0x2f);
static_assert(offsetof(InventoryScreen, last_mouse_x) == 0x31);
static_assert(offsetof(InventoryScreen, last_mouse_y) == 0x33);
static_assert(offsetof(InventoryScreen, mouse_moved_ms) == 0x35);
#endif

}  // namespace joan::orig
