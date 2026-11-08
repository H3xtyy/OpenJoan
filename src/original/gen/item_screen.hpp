// Generated (re/facades/gen2.py: methods; re/layout/gen_types.py: data members from re/layout/layouts.json):
// the game's ItemScreen class.
#pragma once
#include <cstddef>
#include <cstdint>

#include "../../call.hpp"
#include "../../fn_names.hpp"
#include "../ui.hpp"
#include "label.hpp"

namespace joan::orig {

struct Actor;
struct Camera;
struct Scene;
struct SceneS;
struct Texture;

#pragma pack(push, 1)
struct ItemScreen {
#include "item_screen_fields.inc"
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu  // the original (x86) layout
static_assert(sizeof(ItemScreen) == 0xe27);
static_assert(offsetof(ItemScreen, open) == 0x0);
static_assert(offsetof(ItemScreen, shown) == 0x1);
static_assert(offsetof(ItemScreen, item_id) == 0x2);
static_assert(offsetof(ItemScreen, u32_4) == 0x4);
static_assert(offsetof(ItemScreen, u32_8) == 0x8);
static_assert(offsetof(ItemScreen, image_a) == 0xc);
static_assert(offsetof(ItemScreen, image_b) == 0x10);
static_assert(offsetof(ItemScreen, image_c) == 0x14);
static_assert(offsetof(ItemScreen, camera) == 0x18);
static_assert(offsetof(ItemScreen, scene) == 0x1c);
static_assert(offsetof(ItemScreen, shadow_scene) == 0x20);
static_assert(offsetof(ItemScreen, actor) == 0x24);
static_assert(offsetof(ItemScreen, label0) == 0x28);
static_assert(offsetof(ItemScreen, label1) == 0xdf);
static_assert(offsetof(ItemScreen, label2) == 0x196);
static_assert(offsetof(ItemScreen, label3) == 0x24d);
static_assert(offsetof(ItemScreen, label4) == 0x304);
static_assert(offsetof(ItemScreen, label5) == 0x3bb);
static_assert(offsetof(ItemScreen, label6) == 0x472);
static_assert(offsetof(ItemScreen, label7) == 0x529);
static_assert(offsetof(ItemScreen, label8) == 0x5e0);
static_assert(offsetof(ItemScreen, button) == 0x697);
static_assert(offsetof(ItemScreen, label9) == 0x9dd);
static_assert(offsetof(ItemScreen, label10) == 0xa94);
static_assert(offsetof(ItemScreen, label11) == 0xb4b);
static_assert(offsetof(ItemScreen, label12) == 0xc02);
static_assert(offsetof(ItemScreen, label13) == 0xcb9);
static_assert(offsetof(ItemScreen, label14) == 0xd70);
#endif

}  // namespace joan::orig
