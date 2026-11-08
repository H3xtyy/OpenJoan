// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's BuyScreen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "shop_stage.hpp"
#include "unit_data.hpp"

namespace joan::orig {

struct Button;
struct I32Array;

#pragma pack(push, 1)
struct BuyScreen {
#include "buy_screen_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(BuyScreen) == 0x259);
static_assert(offsetof(BuyScreen, tex) == 0x0);
static_assert(offsetof(BuyScreen, sheet) == 0x4);
static_assert(offsetof(BuyScreen, panel_a) == 0x8);
static_assert(offsetof(BuyScreen, panel_b) == 0xc);
static_assert(offsetof(BuyScreen, open) == 0x10);
static_assert(offsetof(BuyScreen, dragging) == 0x11);
static_assert(offsetof(BuyScreen, place) == 0x12);
static_assert(offsetof(BuyScreen, unit) == 0x16);
static_assert(offsetof(BuyScreen, items_b) == 0x1a);
static_assert(offsetof(BuyScreen, items) == 0x38);
static_assert(offsetof(BuyScreen, items_loaded) == 0x56);
static_assert(offsetof(BuyScreen, gold) == 0x57);
static_assert(offsetof(BuyScreen, prices) == 0x5b);
static_assert(offsetof(BuyScreen, stage_player) == 0x5f);
static_assert(offsetof(BuyScreen, stage_shop) == 0x148);
static_assert(offsetof(BuyScreen, drag_state) == 0x231);
static_assert(offsetof(BuyScreen, drag_start_ms) == 0x235);
static_assert(offsetof(BuyScreen, drag_item) == 0x239);
static_assert(offsetof(BuyScreen, drag_index) == 0x23d);
static_assert(offsetof(BuyScreen, drag_from_shop) == 0x241);
static_assert(offsetof(BuyScreen, drag_type) == 0x245);
static_assert(offsetof(BuyScreen, drag_level) == 0x249);
static_assert(offsetof(BuyScreen, drag_count) == 0x24b);
static_assert(offsetof(BuyScreen, u32_24d) == 0x24d);
static_assert(offsetof(BuyScreen, u32_251) == 0x251);
static_assert(offsetof(BuyScreen, u32_255) == 0x255);
#endif

}  // namespace joan::orig
