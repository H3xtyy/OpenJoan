// interfac/objmenu.cpp: functions of the original d:\ww\interfac\objmenu.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include <cmath>
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../game/mission_count.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using C = jo::CampScreen;

// The menu buttons +0x54..+0x7c: CampScreen::menu_buttons[11] (indexed as a run, as the original loops do).
inline jo::Widget*& menu_button(void* self, std::int32_t k) { return jo::self<C>(self)->menu_buttons[k]; }
using joan::img::obj;
using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b109_02.cpp (game/b109_02)
//
//    - ObjMenu::build_71d2d0 0x0071d2d0 (ret 0, interfac/objmenu.cpp): 3 bars (new 0xb7,
//      0x00629400), slot list 0x0058e7b0 + 10 slot buttons, 12 buttons (new 0x346, 0x00620ee0,
//      0x00621540, 0x00623ce0, 0x0061f1b0), then the 18 sub-builders. Returns 0x00722950's eax.
//      SEH frame (new + ctor unwind): normal path only (operator new 0x7b1d35 does not throw).
namespace {
namespace game_b109_02 {

using u8 = std::uint8_t;

using joan::call::cdecl_call;

void* crt_new(u32 size) { return joan::crt::operator_new(joan::orig::arg::u32(size)); }

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), ret 0x10; returns this.
u32 make_color(i32 r, i32 g, i32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*jo::self<jo::Colour>(p));
}

JOAN_COUNTER(c_objmenu, "ObjMenu::build_71d2d0");

struct H {
    // ---- 0x0071d2d0 helpers
    // 3-state bar (new 0xb7 + 0x00629400), 0x00629660 setup, 0x0061e020 slide-in.
    void bar(jo::Widget*& off, i32 a1, i32 a2, img::Addr a3, img::Addr name, i32 slide) {
        void* m = crt_new(static_cast<std::uint32_t>(sizeof(jo::Image2D)));  // 0xb7 on x86
        off = static_cast<jo::Widget*>(m != nullptr ? joan::orig::as_image2d(m)->construct() : nullptr);
        const u32 c = make_color(0, 0, 0, 0xff);
        joan::orig::as_label(off)->create(joan::orig::arg::i32(a1), joan::orig::arg::i32(a2), joan::orig::arg::mptr(img::addr(a3)), joan::orig::arg::str(str(name)), joan::orig::arg::i32(c), i32{3}, i32{0}, i32{2}, i32{0}, i32{-1}, i32{-1}, i32{-1}, i32{-1}, i32{0}, i32{-1}, i32{-1}, i32{0}, i32{1}, i32{1});  // 0x00629660
        joan::orig::as_widget(off)->move_to(joan::orig::arg::i16(slide), joan::orig::arg::i16(a2), joan::orig::arg::f32(u32{0x3f800000}), 0.0f, 1, 0u);  // 0x0061e020
    }
    static void* new_button() {
        void* m = crt_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86
        return m != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(m)->ctor_620ee0()) : nullptr;  // 0x00620ee0
    }
    static void button_setup(void* b, i32 x0, i32 y0, i32 x1, i32 y1, std::uintptr_t cb) {
        joan::orig::as_button(b)->create(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), cb, i32{1}, i32{0}, i32{0}, i32{1}, i32{1}, i32{1});  // 0x00621540
    }
    static void button_text(void* b, std::uintptr_t text, u32 color) {
        joan::orig::as_button(b)->set_text(joan::orig::arg::str(text), img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(color), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
    }
    static void anim_reset(void* b) {
        joan::orig::as_widget(b)->scale_to(0, joan::orig::arg::f32(u32{0x3f800000}), 0.0f, 1, 0u);  // 0x0061f1b0
    }
    void button(jo::Widget*& off, i32 x0, i32 y0, i32 x1, i32 y1, std::uintptr_t cb, std::uintptr_t text_fn) {
        off = static_cast<jo::Widget*>(new_button());
        button_setup(off, x0, y0, x1, y1, cb);
        const u32 c = make_color(0xff, 0xff, 0xff, 0xff);
        const std::uintptr_t text = thiscall<std::uintptr_t>(text_fn, obj(d::g_strings));  // the text getter returns a char*
        button_text(off, text, c);
        anim_reset(off);
    }

    // 0x0071d2d0
    u32 objmenu_build() {
        c_objmenu.hit();
        jo::self<jo::CampScreen>(this)->slot_masks[0] = nullptr;
        jo::self<jo::CampScreen>(this)->slot_masks[1] = nullptr;
        jo::self<jo::CampScreen>(this)->slot_masks[2] = nullptr;
        jo::self<jo::CampScreen>(this)->slot_masks[3] = nullptr;
        jo::self<jo::CampScreen>(this)->slot_masks[4] = nullptr;
        jo::self<jo::CampScreen>(this)->slot_masks[6] = nullptr;
        jo::self<jo::CampScreen>(this)->slot_masks[7] = nullptr;
        bar(jo::self<C>(this)->top_bar, 0, 0x84, d::g_bmp_commele2, d::s_LONGBAR, -0x400);  // LONGBAR
        bar(jo::self<C>(this)->bottom_bar, 0, 0x298, d::g_bmp_commele2, d::s_LONGBAR, 0x400);
        bar(jo::self<C>(this)->side_bar, 0x19, 0xb8, d::g_bmp_objmenu, d::s_TXT_FV, -0x164);  // TXT_FV
        anim_reset(jo::self<C>(this)->side_bar);
        jo::self<jo::CampScreen>(this)->slot_buttons = joan::orig::arg::as<jo::ButtonArray10*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<std::uint32_t>(sizeof(jo::ButtonArray10))) /* 0x28 on x86 */, str(d::s_d_ww_interfac_objmenu_cpp), joan::orig::arg::i32(global<i16>(d::g_a28e40) + 0x18)));  // 0x0058e7b0
        for (i32 i = 0; i < 10; ++i) {
            void* const b = new_button();
            jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i] = static_cast<jo::Button*>(b);
            button_setup(jo::self<C>(this)->slot_buttons->buttons[i], 0x17, i * 0x28 + 0xdc, 0x17f, i * 0x28 + 0x103,
                         fn::code_value(fn::button_draw_cb_723870));  // 0x00723870
            const u32 c = make_color(0xff, 0xff, 0xff, 0xff);
            button_text(jo::self<C>(this)->slot_buttons->buttons[i], img::addr(d::kEmpty), c);
        }
        button(jo::self<C>(this)->menu_buttons[0], 0x100, 0x3e, 0x300, 0x6e, fn::code_value(fn::draw_button_722990), fn::code_value(fn::ObjMenu_open_4));   // 0x00722990, text 0x004dbb20
        button(jo::self<C>(this)->menu_buttons[1], 0x100, 0x3e, 0x300, 0x6e, fn::code_value(fn::draw_button_722990), fn::code_value(fn::ObjMenu_open_05));   // 0x004dbb50
        button(jo::self<C>(this)->menu_buttons[2], 0x100, 0x3e, 0x300, 0x6e, fn::code_value(fn::draw_button_722990), fn::code_value(fn::ObjMenu_open_06));   // 0x004dbb80
        button(jo::self<C>(this)->menu_buttons[3], 0x100, 0x3e, 0x300, 0x6e, fn::code_value(fn::draw_button_722990), fn::code_value(fn::ObjMenu_open_07));   // 0x004dbbb0
        button(jo::self<C>(this)->menu_buttons[4], 0x100, 0x3e, 0x300, 0x6e, fn::code_value(fn::draw_button_722990), fn::code_value(fn::ObjMenu_open_08));   // 0x004dbbe0
        button(jo::self<C>(this)->menu_buttons[5], 0x100, 0x3e, 0x300, 0x6e, fn::code_value(fn::draw_button_722990), fn::code_value(fn::ObjMenu_open_2c));   // 0x004dc2c0
        button(jo::self<C>(this)->menu_buttons[6], 0x100, 0x3e, 0x300, 0x6e, fn::code_value(fn::draw_button_722990), fn::code_value(fn::ObjMenu_open_3f));   // 0x004dc330
        button(jo::self<C>(this)->menu_buttons[7], 0x1ce, 0x298, 0x232, 0x2b6, fn::code_value(fn::mark_draw_723020), fn::code_value(fn::Menu_objmenu_09));  // 0x00723020, 0x004dbc10
        button(jo::self<C>(this)->menu_buttons[8], 0xa2, 0x298, 0x16a, 0x2b6, fn::code_value(fn::mark_draw_723020), fn::code_value(fn::ObjMenu_open_0a));   // 0x004dbc40
        button(jo::self<C>(this)->menu_buttons[9], 0x19c, 0x298, 0x264, 0x2b6, fn::code_value(fn::mark_draw_723020), fn::code_value(fn::Menu_objmenu_0b));  // 0x004dbc70
        button(jo::self<C>(this)->menu_buttons[10], 0x296, 0x298, 0x35e, 0x2b6, fn::code_value(fn::mark_draw_723020), fn::code_value(fn::TxtRes_objmenu_0c));  // 0x004dbca0
        button(jo::self<C>(this)->view_toggle_button, 0x183, 0x298, 0x1e7, 0x2b6, fn::code_value(fn::mark_draw_723020), fn::code_value(fn::ObjMenu_open_1a)); // 0x004dbf40
        button(jo::self<C>(this)->p_171, 0x219, 0x298, 0x27d, 0x2b6, fn::code_value(fn::mark_draw_723020), fn::code_value(fn::Menu_objmenu_4b)); // 0x004dc580
        jo::self<jo::CampScreen>(this)->visible = 0;
        joan::orig::as_obj_menu(this)->build_map();  // 0x0072d380
        joan::orig::as_obj_menu(this)->build_briefing();  // 0x00723c80
        joan::orig::as_map(this)->create_widgets_732580();  // 0x00732580
        joan::orig::as_obj_menu(this)->build_shop();  // 0x007369c0
        joan::orig::as_obj_menu(this)->build_army();  // 0x00724e00
        joan::orig::as_panel(this)->build_72a180();  // 0x0072a180
        joan::orig::as_panel(this)->build_735680();  // 0x00735680
        joan::orig::as_obj_menu(this)->build_siege();  // 0x00738e30
        joan::orig::as_obj_menu(this)->reset_briefing();  // 0x00724bf0
        joan::orig::as_screen(this)->reset_735550();  // 0x00735550
        joan::orig::as_screen(this)->reset_738d00();  // 0x00738d00
        joan::orig::as_obj_menu(this)->leave_army();  // 0x00728d20
        joan::orig::as_screen(this)->purge_items();  // 0x00731930
        joan::orig::as_obj_menu(this)->leave_debrief();  // 0x0072ca90
        joan::orig::as_screen(this)->reset_736770();  // 0x00736770
        joan::orig::as_obj_menu(this)->leave_siege();  // 0x0073b1c0
        joan::orig::as_dialog(this)->close_if_open();  // 0x00722950
        return 0;  // 0x00722950 returns nothing; the only caller (game.cpp run) discards the result
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_02", name, addr, fn)
B("ObjMenu::build_71d2d0", 0x0071d2d0, &H::objmenu_build);
#undef B

}  // namespace game_b109_02
}  // namespace

// ---------------------------------------- from modules/game_b106_01.cpp (game/b106_01)
//
//    - ObjMenu::release_71e800 0x0071e800 (ret 0, objmenu.cpp): 0x0071f360, deletes the
//      three windows +0x44..+0x4c (0x00629830 + scalar delete 0x006bd440(1)), the 10 buttons
//      of the array +0x50 (0x006217e0 + 0x006b33a0(1)) and the array (0x0058ed70 on 0xa48838,
//      line [0xa28e44] + 0x1d), buttons +0x54..+0x7c, +0x20d, +0x171; then 0x0072d550,
//      0x00723f90, 0x00732940, 0x00736d80, 0x00725440, 0x0072a470, 0x00735850, 0x00739290 (its
//      eax is returned: 0x0056e6b0 returns it).
namespace {
namespace game_b106_01 {

JOAN_COUNTER(c_objmenu_release, "ObjMenu::release_71e800");

struct H {
    // ------------------------------------------------------------------ 0x0071e800
    void release_window(jo::Widget*& off) {
        jo::Widget*& slot = off;
        if (slot == nullptr) return;
        joan::orig::as_image2d(slot)->release();  // 0x00629830
        void* const w = slot;
        if (w != nullptr) joan::orig::as_image2d(w)->destroy(joan::orig::arg::u32(i32{1}));  // 0x006bd440 scalar delete
        slot = nullptr;
    }
    template <class W>
    static void release_button(W*& slot) {
        joan::orig::as_menu_button(slot)->clear();  // 0x006217e0
        void* const b = slot;
        if (b != nullptr) joan::orig::as_combo_box(b)->scalar_dtor_6b33a0(joan::orig::arg::u32(i32{1}));  // 0x006b33a0 scalar delete
        slot = nullptr;
    }
    u32 release_71e800() {
        c_objmenu_release.hit();
        joan::orig::as_screen(this)->free_slots_and_parts();  // 0x0071f360
        release_window(jo::self<C>(this)->top_bar);
        release_window(jo::self<C>(this)->bottom_bar);
        release_window(jo::self<C>(this)->side_bar);
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons) != nullptr) {
            for (i32 i = 0; i < 10; ++i) {
                joan::orig::as_menu_button(jo::self<C>(this)->slot_buttons->buttons[i])->clear();  // 0x006217e0
                void* const b = jo::self<C>(this)->slot_buttons->buttons[i];
                if (b != nullptr) joan::orig::as_combo_box(b)->scalar_dtor_6b33a0(joan::orig::arg::u32(i32{1}));  // 0x006b33a0
                jo::self<C>(this)->slot_buttons->buttons[i] = nullptr;
            }
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons)), str(d::s_d_ww_interfac_objmenu_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a28e44)) + 0x1d));  // 0x0058ed70
            jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons) = nullptr;
        }
        for (i32 k = 0; k < 11; ++k)
            if (menu_button(this, k) != nullptr) release_button(menu_button(this, k));
        if (jo::self<C>(this)->view_toggle_button != nullptr) release_button(jo::self<C>(this)->view_toggle_button);
        if (jo::self<C>(this)->p_171 != nullptr) release_button(jo::self<C>(this)->p_171);
        joan::orig::as_obj_menu(this)->free_map();             // 0x0072d550
        joan::orig::as_obj_menu(this)->free_briefing();             // 0x00723f90
        joan::orig::as_obj_menu(this)->free_reward();             // 0x00732940
        joan::orig::as_obj_menu(this)->free_shop();             // 0x00736d80
        joan::orig::as_screen(this)->free_725440();             // 0x00725440
        joan::orig::as_obj_menu(this)->free_debrief();             // 0x0072a470
        joan::orig::as_obj_menu(this)->free_strategy();             // 0x00735850
        joan::orig::as_obj_menu(this)->free_siege();  // 0x00739290
        return 0;  // 0x00739290 returns nothing; callers (game.cpp table, atexit via 0x0056e6b0) discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_01", name, addr, fn)
B("ObjMenu::release_71e800", 0x0071e800, &H::release_71e800);
#undef B

}  // namespace game_b106_01
}  // namespace

// ---------------------------------------- from modules/game_b104_15.cpp (game/b104_15)
//
//      0x0071f360 Screen::free_slots_and_parts    (thiscall, ret 0) - frees +0x4..+0x40 through
namespace {
namespace game_b104_15 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

JOAN_COUNTER(c_free_slots, "Screen::free_slots_and_parts");

struct H {
    // ------------------------------------------------------------------------ 0x0071f360
    u32 free_slots() {
        c_free_slots.hit();
        for (u32 i = 0; i < 16; ++i) {  // +0x4..+0x40
            void*& mask = jo::self<C>(this)->slot_masks[i];
            if (mask != nullptr) {
                joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(mask));  // 0x0060a550
                mask = nullptr;
            }
        }
        joan::orig::as_obj_menu(this)->clear_map_models();         // 0x0072e370
        joan::orig::as_screen(this)->nop_732bd0();         // 0x00732bd0
        joan::orig::as_screen(this)->nop_737010();         // 0x00737010
        joan::orig::as_obj_menu(this)->free_debrief_images();         // 0x0072d060
        joan::orig::as_obj_menu(this)->free_strategy_image();         // 0x00736900
        joan::orig::as_obj_menu(this)->free_siege_images();  // 0x0073b6e0
        return 0;  // 0x0073b6e0 returns nothing; callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_15", name, addr, fn)
B("Screen::free_slots_and_parts", 0x0071f360, &H::free_slots);
#undef B

}  // namespace game_b104_15
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
//
//      0x00722950 Dialog::close_if_open         (thiscall, ret 0)
namespace {
namespace game_b108_13 {

using i8 = std::int8_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00722950
    void close_if_open() {
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->state[0]) != 0) joan::orig::as_camp_screen(this)->set_state_721a60(6u);  // 0x00721a60
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Dialog::close_if_open", 0x00722950, &H::close_if_open);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
//
//    - Scroller::init_723c80 0x00723c80 (ret 0): new TextDesc +0x81 (0x00629530, 0x006299b0,
//      0x0061f1b0), up / down arrow buttons +0x85 / +0x89.
//
//    0x00723c80 ignore eax (checked at every call site) -> void.
namespace {
namespace game_b102_15 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::str;
using joan::orig::helpers::addr;

using joan::orig::helpers::game_new;                       // operator new

// new Widget (0x346 bytes) / new TextDesc (0xb7 bytes), null when operator new fails.
void* new_widget() {
    void* const m = game_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86
    return m != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(m)->ctor_620ee0()) : nullptr;  // 0x00620ee0
}
void* new_text() {
    void* const m = game_new(static_cast<std::uint32_t>(sizeof(jo::Image2d)));  // 0xb7 on x86
    return m != nullptr ? joan::orig::as_image2d(m)->construct() : nullptr;  // 0x00629400
}

// Colour record 0x0042a260(r, g, b, a) on a 4-byte local; returns its dword.
u32 colour(i32 r, i32 g, i32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));  // 0x0042a260
    return jo::view<u32>(*jo::self<jo::Colour>(p));
}

JOAN_COUNTER(c_scroller, "Scroller::init_723c80");

struct H {
    // ------------------------------------------------------------------ 0x00723c80
    void scroller_723c80() {
        c_scroller.hit();
        jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_text) = new_text();
        const u32 white = colour(0xff, 0xff, 0xff, 0xff);
        joan::orig::as_image2d(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_text))->create(0x106, 0xd2, static_cast<jo::Font2d*>(img::obj(d::g_font_goudy14_22_white)), str(d::kEmpty), 3, joan::orig::arg::i32(white), joan::orig::arg::i32(// 0x00629530
                      0), 0, 2, 0, 0x3f800000, 0x3f800000, 0, 1, 1);
        jo::view<u16>(jo::self<jo::CampScreen>(this)->brief_line) = 0xfffc;
        const u32 edx = (addr(this) & 0xffff0000u) | static_cast<u32>(jo::view<u16>(jo::self<jo::CampScreen>(this)->brief_line));
        joan::orig::as_label(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_text))->set_layout(0x314u, 0x262u, 2u, joan::orig::arg::u32(edx), 1u, 0u, joan::orig::arg::u32(-1));  // 0x006299b0
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_text))->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);        // 0x0061f1b0
        jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_up) = new_widget();
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_up))->setup(joan::orig::arg::i16(0x1de), joan::orig::arg::i16(0xc0), joan::orig::arg::mptr(img::addr(d::g_bmp_commele2)), str(d::s_aw_u_ot), str(d::s_aw_u_dw), joan::orig::arg::str(// 0x006211d0
                      str(d::s_aw_u_ov)), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));                                                // aw_u_*
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_up))->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);  // 0x0061f1b0
        jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_down) = new_widget();
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_down))->setup(joan::orig::arg::i16(0x1de), joan::orig::arg::i16(0x261), joan::orig::arg::mptr(img::addr(d::g_bmp_commele2)), str(d::s_aw_d_ot), str(d::s_aw_d_dw), joan::orig::arg::str(// 0x006211d0
                      str(d::s_aw_d_ov)), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));                                                 // aw_d_*
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_down))->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);  // 0x0061f1b0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Scroller::init_723c80", 0x00723c80, &H::scroller_723c80);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
//
//    - Obj::release_723f90 0x00723f90 / release_732940 0x00732940 / release_736d80 0x00736d80
//      (ret 0): pre-call (0x629830 / 0x6217e0) + scalar delete (0x6bd440 / 0x6b33a0 / 0x562910).
//
//  - Return values: callers of 0x6e0900, 0x6e4c00, 0x6e4cc0, 0x6ed3e0, 0x6f1ff0, 0x6f8050,
//    0x707b20, 0x707ce0, 0x7180f0, 0x723f90, 0x732940, 0x732af0, 0x736d80, 0x7388a0, 0x742f90
//    ignore eax (checked, none is referenced as data / vtable); the replacements return void or
//    a simple value.
namespace {
namespace game_b100_16 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_release_723f90, "Obj::release_723f90");

struct H {
    // Field release: optional pre-call, scalar deleting dtor(1) when non-null, field = 0.
    template <class W>
    void release(W*& off, std::uintptr_t pre, std::uintptr_t del) {
        W*& slot = off;
        if (slot == nullptr) return;
        if (pre != 0) thiscall<u32>(pre, static_cast<void*>(slot));
        void* const p = slot;
        if (p != nullptr) thiscall<u32>(del, p, 1u);
        slot = nullptr;
    }

    // ------------------------------------------------------------------ 0x00723f90
    void release_723f90() {
        c_release_723f90.hit();
        release(jo::self<C>(this)->brief_text, fn::code_value(fn::Image2d_release), fn::code_value(fn::Sprite_scalar_dtor));  // 0x00629830, 0x006bd440
        release(jo::self<C>(this)->brief_up, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));  // 0x006217e0, 0x006b33a0
        release(jo::self<C>(this)->brief_down, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Obj::release_723f90", 0x00723f90, &H::release_723f90);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0071d270
    u32 one_71d270() { return 1; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Obj::one_71d270", 0x0071d270, &H::one_71d270);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using i32 = std::int32_t;

using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    // this = the CampScreen 0x00a53cb0: 0x278 bytes = the layout (0x270 on x86) + the 8 bytes after it (the data
    // image keeps them as b_a53cb0_tail; a CampScreen tail member needs the data image regenerated)
    void* clear_278() { memset_(this, 0, static_cast<u32>(sizeof(jo::CampScreen)) + 8u); return this; }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj71d290::Obj", 0x0071d290, &H::clear_278);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
namespace {
namespace game_b108_05 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;


JOAN_COUNTER(c_reload_masks, "Mgr::reload_masks_71efc0");

struct H {
    // ------------------------------------------------------------------ 0x0071efc0
    u32 reload_masks() {
        c_reload_masks.hit();
        void* const res = obj(d::g_texture_array);
        const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(res)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(res)->set_mode(joan::orig::arg::u32(i32{1}));            // 0x0060a740
        auto load = [&](i32 slot, img::Addr name) {
            if (jo::self<C>(this)->slot_masks[slot] == nullptr) {
                jo::self<C>(this)->slot_masks[slot] = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(res)->load(joan::orig::arg::str(str(name)), i32{1}, joan::orig::arg::u32(i32{0})));  // 0x006099c0
            }
        };
        load(0, d::s_OBJMASK);  // "OBJMASK"
        load(1, d::s_MB_OBJ);  // "MB_OBJ"
        load(2, d::s_MB_COM);  // "MB_COM"
        load(3, d::s_MB_TOWN);  // "MB_TOWN"
        load(4, d::s_MB_FRAME);  // "MB_FRAME"
        load(5, d::s_MB_FOCUS);  // "MB_FOCUS"
        load(6, d::s_COMMELE2);  // "COMMELE2"
        load(7, d::s_COMMELE3);  // "COMMELE3"
        switch (global<u32>(d::g_mission) - 1u) {
            case 0: load(8, d::s_S1_SCR); break;  // "S1_SCR"
            case 1: load(9, d::s_S3_SCR); break;  // "S3_SCR"
            case 2: load(10, d::s_S2_SCR); break;  // "S2_SCR"
            case 3: load(11, d::s_S4_SCR); break;  // "S4_SCR"
            case 4: load(12, d::s_S5_SCR); break;  // "S5_SCR"
            case 5: load(13, d::s_S6_SCR); break;  // "S6_SCR"
            case 6: load(14, d::s_S7_SCR); break;  // "S7_SCR"
            case 7: load(15, d::s_S8_SCR); break;  // "S8_SCR"
            default: break;
        }
        joan::orig::as_texture_manager(res)->set_mode(joan::orig::arg::u32(saved));  // 0x0060a740
        joan::orig::as_obj_menu(this)->init_map();        // 0x0072d6f0
        joan::orig::as_obj_menu(this)->init_reward_camera();        // 0x00732af0
        joan::orig::as_obj_menu(this)->init_shop_camera();        // 0x00736f30
        joan::orig::as_obj_menu(this)->load_strategy_image();        // 0x00736880
        joan::orig::as_obj_menu(this)->init_siege();  // 0x0073b2e0
        return 0;  // 0x0073b2e0 returns nothing; the only caller discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Mgr::reload_masks_71efc0", 0x0071efc0, &H::reload_masks);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- from modules/game_b116_06.cpp (game/b116_06)
namespace {
namespace game_b116_06 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::call::cdecl_call;

JOAN_COUNTER(c_draw, "ObjectiveMenu::draw");

constexpr u32 kOne = 0x3f800000u;  // 1.0f
void* const kMouse = obj(d::g_mouse);
void* const kStrings = obj(d::g_strings);
void* const kHud = obj(d::g_hud);
void* const kSprites = obj(d::g_sprites_a46128);

// ------------------------------------------------------------------------- 0x0071f690 helpers
u32 rgba(i32 b0, i32 b1, i32 b2, i32 b3) {  // 0x0042a260 on a 4-byte local
    u32 col = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&col)->construct(b0, b1, b2, b3));
}
// ftol(255.0f * fade): fld [0x9cfb68]; fmul [[this+0x54]+0x11]; _ftol.
i32 fade_alpha(const u8* self) {
    const Fpu fpu{true};
    return ftol(fpu.mul(global<float>(d::flt_255), jo::self<jo::CampScreen>(self)->menu_buttons[0]->scale));
}
// 0x00633500 (17 args): the time box sprite.
void time_box(i32 x, u32 col) {
    joan::orig::as_sprite_queue(kSprites)->draw_named(joan::orig::arg::i16(x), 0x18, 2u, str(d::s_tc), img::obj(d::g_bmp_objmenu), joan::orig::arg::u32(col), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);
}
// 0x00626130 (13 args).
void text3(img::Addr font, i32 x1, i32 y1, i32 x2, i32 y2, const char* s, u32 col, i32 a8) {
    joan::orig::as_text(obj(font))->draw_3(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(i32{2}), s, joan::orig::arg::u32(col), joan::orig::arg::i32(a8), i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
}

// The original frame from [ebp-0x158] up to `this` at [ebp-4].
struct Frame71 {
    char save[0x108];  // [ebp-0x158]
    i32 h;             // [ebp-0x50]
    i32 m;             // [ebp-0x4c]
    i32 s;             // [ebp-0x48]
    char time[0x40];   // [ebp-0x44]
    u8* self;          // [ebp-0x4]
};
#if UINTPTR_MAX == 0xffffffffu  // the original (x86) frame
static_assert(sizeof(Frame71) == 0x158 - 4 + 4, "Frame71");
#endif

void format_time(Frame71& f, u32 t) {
    f.h = static_cast<i32>(t / 3600000u);
    f.m = static_cast<i32>((t % 3600000u) / 60000u);
    f.s = static_cast<i32>(((t % 3600000u) % 60000u) / 1000u);
    std::uintptr_t fmt;
    if (f.s < 10 && f.m >= 10) {
        fmt = img::addr(d::s_d_d_0_d);  // "%d:%d:0%d"
    } else if (f.m < 10 && f.s >= 10) {
        fmt = img::addr(d::s_d_0_d_d);  // "%d:0%d:%d"
    } else if (f.m < 10 && f.s < 10) {
        fmt = img::addr(d::s_d_0_d_0_d);  // "%d:0%d:0%d"
    } else {
        fmt = img::addr(d::s_d_d_d);  // "%d:%d:%d"
    }
    joan::orig::libc::sprintf(joan::orig::arg::str(f.time), joan::orig::arg::str(str(fmt)), f.h, f.m, f.s);  // sprintf
}

// One time box: sprite, caption, time text (x1 / x2 = 8 / 0x94 or 0x36c / 0x3f8).
void time_panel(Frame71& f, i32 x1, i32 x2, std::uintptr_t caption_thunk, img::Addr time_global) {
    {
        const i32 a = fade_alpha(f.self);
        const u32 col = rgba(0xff, 0xff, 0xff, a);
        time_box(x1, col);
    }
    {
        const i32 a = fade_alpha(f.self);
        const u32 col = rgba(0xff, 0xff, 0xff, a);
        const char* const t = thiscall<const char*>(caption_thunk, kStrings);
        text3(d::g_font_goudy12_bold_black, x1, 0x18, x2, 0x32, t, col, 0);
    }
    format_time(f, global<u32>(time_global));
    {
        const i32 a = fade_alpha(f.self);
        const u32 col = rgba(0xff, 0xff, 0xff, a);
        text3(d::g_font_goudy14_22_white, x1, 0x2c, x2, 0x50, f.time, col, 0);
    }
}

// [[this+0x4c]+0x11] > 0 (fcomp 0.0, test ah,0x41 / jne): 0x00629bd0(2, 7) on the panel.
void fade_panel(const u8* self) {
    if (jo::self<jo::CampScreen>(self)->side_bar->scale > global<float>(d::k_9cf5c4)) {
        joan::orig::as_label(jo::self<C>(self)->side_bar)->draw(joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{7}));  // 0x00629bd0
    }
}
void hud_update(i32 arg) { joan::orig::as_menu(kHud)->reset_fades(static_cast<std::int8_t>(joan::orig::arg::i32(arg))); }  // 0x006f21c0
bool hud_mode9() { return global<i8>(d::g_game_mode) == 9; }

// Mode title at the frame +0x44 corner (font 0xa48d48).
void title(const u8* self, std::uintptr_t text_thunk) {
    const u32 col = rgba(0xff, 0xff, 0xff, 0xff);
    const char* const t = thiscall<const char*>(text_thunk, kStrings);
    const i32 y2 = static_cast<i32>(jo::self<jo::CampScreen>(self)->top_bar->y) + 0x1e;
    const i32 x2 = static_cast<i32>(jo::self<jo::CampScreen>(self)->top_bar->x) + 0x400;
    const i32 y1 = jo::self<jo::CampScreen>(self)->top_bar->y;
    const i32 x1 = jo::self<jo::CampScreen>(self)->top_bar->x;
    text3(d::g_font_goudy_bold_18, x1, y1, x2, y2, t, col, 1);
}
void hint(std::uintptr_t text_thunk) {  // 0x006f2ac0(text)
    const char* const t = thiscall<const char*>(text_thunk, kStrings);
    joan::orig::as_ingame_frame(kHud)->set_title(joan::orig::arg::str(t));
}
// +1 set: 0x004dbd00 (thunk 0x004085cb), else 0x004dbd30 (thunk 0x00408a76).
std::uintptr_t flag_text(const u8* self) { return jo::view<i8>(jo::self<jo::CampScreen>(self)->state[1]) != 0 ? fn::code_value(fn::ObjMenu_open_0e) : fn::code_value(fn::ObjMenu_open_0f); }

bool sub_screen(i8 m) { return m == 5 || m == 2 || m == 3 || m == 7 || m == 8 || m == 9; }

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ----------------------------------------------------------------------- 0x0071f690
    void draw(i32 pass_arg, i32 arg) {
        joan::canvas::ScreenScope canvas_scope(0x0071f690);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_draw.hit();
        Frame71 f;
        f.self = self();
        const i8 pass = static_cast<i8>(pass_arg);
        const i32 sub_arg = static_cast<i32>(static_cast<u8>(arg));    // pushed as `mov al, [ebp+0xc]`
        const i32 pass_byte = static_cast<i32>(static_cast<u8>(pass_arg));  // `mov al, [ebp+0x8]`
        if (jo::view<i8>(jo::self<jo::CampScreen>(f.self)->state[0]) == 0) return;
        joan::orig::as_cursor(kMouse)->reset_pos();  // 0x00592c30
        auto mode = [&f]() { return jo::view<i8>(jo::self<jo::CampScreen>(f.self)->visible); };

        if (!sub_screen(mode())) {
            if (pass != 1) {
                time_panel(f, 8, 0x94, fn::code_value(fn::ObjMenu_open_76), d::g_mission_ms);       // 0x004dc5b0
                time_panel(f, 0x36c, 0x3f8, fn::code_value(fn::ObjMenu_open_4d), d::g_play_ms);  // 0x004dc5e0
                return;
            }
            if (hud_mode9()) hud_update(0);
            fade_panel(f.self);
        } else if (mode() == 2) {
            if (pass == 1) {
                joan::orig::as_screen(f.self)->draw_black_732bf0(joan::orig::arg::u32(sub_arg));  // 0x00732bf0
            } else {
                fade_panel(f.self);
                joan::orig::as_obj_menu(f.self)->draw_reward();  // 0x00732c70
            }
            if (hud_mode9()) hud_update(pass_byte);
        } else if (mode() == 3) {
            if (pass == 1) {
                joan::orig::as_obj_menu(f.self)->draw_shop_bg(joan::orig::arg::u32(sub_arg));  // 0x00737030
            } else {
                fade_panel(f.self);
                joan::orig::as_obj_menu(f.self)->draw_shop();  // 0x007370b0
            }
            if (hud_mode9()) hud_update(pass_byte);
        } else if (mode() == 5) {
            if (hud_mode9()) hud_update(pass_byte);
        } else if (mode() == 9) {
            if (pass != 1) joan::orig::as_obj_menu(f.self)->draw_siege(static_cast<std::uint8_t>(joan::orig::arg::u32(sub_arg)));  // 0x00739540
            if (hud_mode9()) hud_update(pass_byte);
        } else if (mode() == 7 || mode() == 8) {
            hud_update(pass_byte);
        }

        {
            const i8 m = mode();
            if ((m == 5 || m == 2 || m == 3 || m == 7 || m == 8) && pass != 2) return;
        }

        for (i32 k = 0; k < 7; ++k) {  // +0x54..+0x6c
            joan::orig::as_button(menu_button(f.self, k))->draw(2u, 7);  // 0x00622cc0
        }
        joan::orig::as_slider(f.self)->draw_mask(joan::orig::arg::u32(sub_arg));  // 0x007240f0
        joan::orig::as_obj_menu(f.self)->draw_debrief(joan::orig::arg::u32(sub_arg));  // 0x0072a620
        joan::orig::as_obj_menu(f.self)->draw_army(static_cast<std::uint8_t>(joan::orig::arg::u32(sub_arg)));  // 0x00725870
        joan::orig::as_obj_menu(f.self)->draw_map(joan::orig::arg::u32(sub_arg));  // 0x0072e480
        joan::orig::as_obj_menu(f.self)->draw_strategy(joan::orig::arg::u32(sub_arg));  // 0x00735930
        joan::orig::as_label(jo::self<C>(f.self)->top_bar)->draw(joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{7}));  // 0x00629bd0

        switch (static_cast<u32>(static_cast<i32>(mode()))) {  // jump table 0x00720755
        case 0:
            title(f.self, flag_text(f.self));
            hint(fn::code_value(fn::ObjMenu_open_0d));  // 0x004dbcd0
            break;
        case 1:
            title(f.self, fn::code_value(fn::ObjMenu_open_4));  // 0x004dbb20
            hint(fn::code_value(fn::ObjMenu_open_0f));           // 0x004dbd30
            break;
        case 2:
            title(f.self, fn::code_value(fn::ObjMenu_open_05));  // 0x004dbb50
            hint(flag_text(f.self));
            break;
        case 3:
            title(f.self, fn::code_value(fn::ObjMenu_open_06));  // 0x004dbb80
            hint(flag_text(f.self));
            break;
        case 4:
            title(f.self, fn::code_value(fn::ObjMenu_open_07));  // 0x004dbbb0
            hint(flag_text(f.self));
            break;
        case 5:
            title(f.self, fn::code_value(fn::ObjMenu_open_08));  // 0x004dbbe0
            hint(flag_text(f.self));
            break;
        case 7:
            title(f.self, fn::code_value(fn::ObjMenu_open_10));  // 0x004dbd60
            hint(fn::code_value(fn::ObjMenu_open_0e));           // 0x004dbd00
            break;
        case 8:
            title(f.self, fn::code_value(fn::ObjMenu_open_2c));  // 0x004dc2c0
            hint(flag_text(f.self));
            break;
        case 9:
            title(f.self, fn::code_value(fn::ObjMenu_open_3f));  // 0x004dc330
            hint(flag_text(f.self));
            break;
        default:  // 6 and > 9
            break;
        }

        joan::orig::as_label(jo::self<C>(f.self)->bottom_bar)->draw(joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{7}));  // 0x00629bd0
        if (global<i32>(d::g_mission) == 6) {
            const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kStrings)->autosave_text_1(joan::orig::arg::u32(global<i32>(d::g_mission))));  // 0x004dd0e0
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(f.save)), joan::orig::arg::str(name));  // strcpy
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(f.save)), str(d::s_SAV_9cfb78));  // strcat ".SAV"
            u8* const entry = joan::orig::arg::as<u8*>(joan::orig::as_paths(obj(d::g_saves))->find(joan::orig::arg::str(f.save)));  // 0x00477640
            if (entry == nullptr || jo::view<i32>(jo::self<jo::GameFile>(entry)->u32_19a) != 6 || jo::view<i8>(jo::self<jo::GameFile>(entry)->u8_19e) != 1) {
                joan::orig::as_widget(jo::self<C>(f.self)->menu_buttons[8])->reset_child();  // 0x0062b660
            } else {
                joan::orig::as_panel(jo::self<C>(f.self)->menu_buttons[8])->reset_6b7cf0();                  // 0x006b7cf0
                joan::orig::as_button(jo::self<C>(f.self)->menu_buttons[8])->draw(2u, 7);  // 0x00622cc0
            }
        } else {
            joan::orig::as_button(jo::self<C>(f.self)->menu_buttons[8])->draw(2u, 7);
        }
        joan::orig::as_button(jo::self<C>(f.self)->menu_buttons[9])->draw(2u, 7);
        joan::orig::as_button(jo::self<C>(f.self)->menu_buttons[10])->draw(2u, 7);
        joan::orig::as_button(jo::self<C>(f.self)->menu_buttons[7])->draw(2u, 7);
        joan::orig::as_button(jo::self<C>(f.self)->view_toggle_button)->draw(2u, 7);
        joan::orig::as_button(jo::self<C>(f.self)->p_171)->draw(2u, 7);
    }
};

JOAN_REPLACE("game/b116_06", "ObjectiveMenu::draw", 0x0071f690, &H::draw);
JOAN_CANVAS_SELF(0x0071f690);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_06
}  // namespace

// ---------------------------------------- from modules/game_ui8.cpp (game/ui8)
namespace {
namespace game_ui8 {

using joan::call::thiscall;


JOAN_COUNTER(c_cam, "ObjectiveMenu::draw_camera");

struct H {
    // 0x00720bc0 (jump table 0x00720c51 on +0x80 - 2). ret 0.
    void draw_camera() {
        c_cam.hit();
        if (static_cast<std::int8_t>(jo::self<jo::CampScreen>(this)->state[0]) == 0) return;
        switch (static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(jo::self<jo::CampScreen>(this)->visible)) - 2)) {
            case 0: joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_camera))->render(); break;   // 0x00614760
            case 1: joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_camera))->render(); break;
            case 3:
                if (static_cast<std::int8_t>(jo::self<jo::CampScreen>(this)->camera_view) != 0) joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->map_camera))->render();
                break;
            case 7: joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera))->render(); break;
            default: break;
        }
    }
};

#define U8(name, addr, fn) JOAN_REPLACE("game/ui8", name, addr, fn)
U8("ObjectiveMenu::draw_camera", 0x00720bc0, &H::draw_camera);
#undef U8

}  // namespace game_ui8
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::hook::x87::ftol;
using i8 = std::int8_t;
using i16 = std::int16_t;


void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_720ca0, "Menu::update_720ca0");
JOAN_COUNTER(c_7226a0, "Menu::open_7226a0");

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), returns this.
u32 make_color(u32* slot) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));
}

// ------------------------------------------------------------------ thiscall members
struct H {
    jo::CampScreen* self() { return jo::self<C>(this); }

    // 0x00720ca0
    i32 button(jo::Widget*& off, i32 key) {
        return joan::orig::arg::as<i32>(joan::orig::as_ui_button(off)->mouse(joan::orig::arg::i32(key), i32{0}, i32{-1}, i32{0}, i32{0}, i32{0}));  // 0x006218c0
    }
    void close_722950() { joan::orig::as_dialog(this)->close_if_open(); }  // 0x00722950
    void pick(i32 v) { joan::orig::as_camp_screen(this)->set_state_721a60(joan::orig::arg::u32(v)); }  // 0x00721a60
    u8 update_720ca0() {
        c_720ca0.hit();
        jo::CampScreen* const s = self();
        if (static_cast<i8>(s->state[0]) == 0) return 0;
        if (static_cast<i8>(global<u8>(d::g_a57ecc)) != 0) return 0;
        const i32 sub = static_cast<i8>(s->visible);
        switch (sub) {
            case 0: {
                if (static_cast<i8>(s->state[1]) != 0) {
                    if (button(jo::self<C>(this)->menu_buttons[9], 0) != 0) {
                        close_722950();
                        s->state[2] = 0;
                        joan::orig::as_dialog(obj(d::g_objective_menu))->close_if_open();  // 0x00722950
                        global<u8>(d::g_savemenu_load_mode) = 1;
                        joan::orig::as_save_menu(obj(d::g_save_menu))->open(static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{2})));  // 0x0074da40
                        return 1;
                    }
                    if (button(jo::self<C>(this)->menu_buttons[8], 0) != 0) {
                        if (global<u32>(d::g_mission) == 0) return 1;
                        s->state[3] = 1;
                        close_722950();
                        s->state[2] = 0;
                    }
                    if (button(jo::self<C>(this)->menu_buttons[10], 0) != 0) {
                        s->state[3] = 2;
                        close_722950();
                        s->state[2] = 0;
                    }
                    if (static_cast<i8>(s->state[3]) != 0) {
                        s->state[0] = 0;
                        s->visible = 0;
                        joan::orig::as_screen(this)->free_slots_and_parts();              // 0x0071f360
                        joan::orig::as_screen(this)->layout_main();              // 0x007215f0
                        joan::orig::as_menu(obj(d::g_hud))->slide_out();  // 0x006f26e0
                        joan::orig::as_ingame_frame(obj(d::g_hud))->free_backgrounds();  // 0x006f20f0
                        s->state[2] = 0;
                        s->state[1] = 0;
                        if (static_cast<i8>(s->state[3]) == 1) {
                            joan::orig::as_battle(obj(d::g_scenarios))->load_scenario_41cc80(joan::orig::arg::i32(global<u32>(d::g_mission)), joan::orig::arg::i32(global<u32>(d::g_stage_index)), i32{0});  // 0x0041cc80
                            joan::orig::as_game(obj(d::g_game_a50088))->set_state(joan::orig::arg::u32(i32{0xc}));  // 0x004d7e70
                            if (global<u32>(d::g_mission) == 6) {
                                char buf[0x108];
                                const char* const base = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->autosave_text_1(joan::orig::arg::u32(global<u32>(d::g_mission))));  // 0x004dd0e0
                                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(base));  // strcpy
                                std::strcat(static_cast<char*>(joan::orig::arg::mptr(buf)), str(d::s_SAV_9cfb78));  // strcat ".SAV"
                                void* const r = joan::orig::as_paths(obj(d::g_saves))->find(joan::orig::arg::str(buf));  // 0x00477640
                                joan::orig::as_progress(obj(d::g_progress))->set_image(static_cast<jo::GameFile*>(r));                  // 0x007563f0
                            }
                        } else if (static_cast<i8>(s->state[3]) == 2) {
                            if (global<i32>(d::g_mission) == joan::mission_count::last_mission()) {  // 8; [Fixes] MissionCount: the data's last
                                joan::orig::as_ending(obj(d::g_ending_screen))->open(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x006d19e0
                                return 1;
                            }
                            u8* const g = joan::orig::arg::as<u8*>(joan::orig::as_profile_list(obj(d::g_profile_list))->current());  // 0x004ca290
                            if (g != nullptr) {
                                joan::orig::as_units(kUnits)->collect();              // 0x0051ff00
                                joan::orig::as_scenario(obj(d::g_mission))->apply_list();     // 0x004cdbc0
                                joan::orig::as_game(obj(d::g_game_state))->copy_words_to();     // 0x004727b0
                                joan::orig::as_game(obj(d::g_game_state))->push_buffer();     // 0x00472b00
                                jo::self<jo::Profile>(g)->u32_38 = global<u32>(d::g_get_a596c4);
                                jo::self<jo::Profile>(g)->u32_3c = global<u32>(d::g_play_ms);
                                jo::view<u32>(jo::self<jo::Profile>(g)->i32_40) = global<u32>(d::g_command_points);
                                joan::orig::as_profile(g)->save_4c8810();                   // 0x004c8810
                            }
                            global<u8>(d::g_a58f95) = 1;
                            global<u8>(d::g_flag_a59298) = 3;
                            joan::orig::as_mode(obj(d::g_game_a50088))->set_499540(joan::orig::arg::u32(i32{0xd}));  // 0x00499540
                        }
                        s->state[3] = 0;
                    }
                } else if (button(jo::self<C>(this)->menu_buttons[7], 0x1b) != 0) {
                    if (static_cast<i8>(s->state[2]) != 0) global<u32>(d::g_screen_pending) = 1;
                    close_722950();
                    return 1;
                }
                if (button(jo::self<C>(this)->menu_buttons[0], 0) != 0) {
                    pick(static_cast<i8>(s->state[1]) != 0 ? 7 : 1);
                    return 1;
                }
                if (button(jo::self<C>(this)->menu_buttons[1], 0) != 0) { pick(2); return 1; }
                if (button(jo::self<C>(this)->menu_buttons[2], 0) != 0) { pick(3); return 1; }
                if (button(jo::self<C>(this)->menu_buttons[3], 0) != 0) { pick(4); return 1; }
                if (button(jo::self<C>(this)->menu_buttons[4], 0) != 0) { pick(5); return 1; }
                if (button(jo::self<C>(this)->menu_buttons[5], 0) != 0) { pick(8); return 1; }
                if (button(jo::self<C>(this)->menu_buttons[6], 0) != 0) { pick(9); return 1; }
                break;
            }
            case 1: joan::orig::as_obj_menu(this)->update_briefing(); break;  // 0x00724210
            case 2: joan::orig::as_list(this)->scroll_734120(); break;  // 0x00734120
            case 3: joan::orig::as_obj_menu(this)->update_shop(); break;  // 0x007380b0
            case 4: joan::orig::as_obj_menu(this)->update_army(); break;  // 0x007277a0
            case 5: joan::orig::as_obj_menu(this)->update_map(); break;  // 0x00730150
            case 6: break;
            case 7: joan::orig::as_obj_menu(this)->update_debrief(); break;  // 0x0072c740
            case 8: joan::orig::as_obj_menu(this)->update_strategy(); break;  // 0x00736120
            case 9: joan::orig::as_obj_menu(this)->update_siege(); break;  // 0x0073a320
            default: break;
        }
        if (static_cast<i8>(s->state[1]) != 0) return 1;
        if (static_cast<i8>(s->visible) == 6) return 1;
        if (joan::orig::arg::as<i32>(joan::orig::as_mode(obj(d::g_screen_mgr))->check_6f5a70()) == 0) return 1;  // 0x006f5a70
        close_722950();
        s->state[2] = 0;
        return 1;
    }

    // 0x007226a0
    void label(jo::Widget*& btn_off, std::uintptr_t text_thunk) {
        u32 slot = 0;
        const u32 color = make_color(&slot);
        const std::uintptr_t text = thiscall<std::uintptr_t>(text_thunk, obj(d::g_strings));  // the text getter returns a char*
        joan::orig::as_button(btn_off)->set_text(joan::orig::arg::str(text), img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(color), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
    }
    u32 open_7226a0(i32 mode_arg) {
        c_7226a0.hit();
        jo::CampScreen* const s = self();
        s->state[0] = 1;
        if (global<i32>(d::g_mission) == joan::mission_count::last_mission()) label(  // 8; [Fixes] MissionCount: the data's last
            jo::self<C>(this)->menu_buttons[10], fn::code_value(fn::Vict_open_5));  // 0x004dba60
        global<u8>(d::g_get_a591c7) = 1;
        s->state[1] = 0;
        s->state[2] = 0;
        const i32 mode = static_cast<i8>(mode_arg);
        if (mode == 1) {
            s->state[2] = 1;
            label(jo::self<C>(this)->menu_buttons[0], fn::code_value(fn::ObjMenu_open_4));  // 0x004dbb20
        } else if (mode == 2) {
            s->state[1] = 1;
            label(jo::self<C>(this)->menu_buttons[0], fn::code_value(fn::ObjMenu_open_10));  // 0x004dbd60
        } else {
            label(jo::self<C>(this)->menu_buttons[0], fn::code_value(fn::ObjMenu_open_4));  // 0x004dbb20
        }
        if (global<i32>(d::g_mission) <= 4) {
            joan::orig::as_widget_child(&jo::self<C>(this)->menu_buttons[6]->child)->disable();  // 0x006ca020
            joan::orig::as_widget_child(&jo::self<C>(this)->menu_buttons[5]->child)->disable();
        } else {
            joan::orig::as_widget_child(&jo::self<C>(this)->menu_buttons[6]->child)->enable();  // 0x006ca000
            joan::orig::as_widget_child(&jo::self<C>(this)->menu_buttons[5]->child)->enable();
        }
        s->state[3] = 0;
        joan::orig::as_screen(this)->layout_main();              // 0x007215f0
        joan::orig::as_ingame_frame(obj(d::g_hud))->slide_in();  // 0x006f24d0
        pick(0);                                      // 0x00721a60
        joan::orig::as_obj_menu(this)->load_masks();              // 0x0071efc0
        if (global<u8>(d::g_game_running) == 0) return 0;
        global<u8>(d::g_game_running) = 0;
        global<i16>(d::g_saved_game_speed) = static_cast<i16>(ftol(static_cast<double>(global<float>(d::g_game_speed))));
        joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(0.0f));  // 0x0042f930
        return 0;  // 0x0042f930 returns nothing; callers (and set_state_6f5cc0's callers) discard the result
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Menu::update_720ca0", 0x00720ca0, &H::update_720ca0);
B("Menu::open_7226a0", 0x007226a0, &H::open_7226a0);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;


JOAN_COUNTER(c_objscr, "ObjScreen::update_721270");

struct H {
    // ------------------------------------------------------------------ 0x00721270
    void objscreen_update_721270() {
        c_objscr.hit();
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->state[0]) == 0) return;
        if (global<i8>(d::g_a5d384) != 0) return;
        const fn::Code upd = fn::Obj_update_61f3b0;  // 0x0061f3b0
        thiscall<u32>(upd, static_cast<void*>(jo::self<C>(this)->top_bar));
        thiscall<u32>(upd, static_cast<void*>(jo::self<C>(this)->bottom_bar));
        thiscall<u32>(upd, static_cast<void*>(jo::self<C>(this)->side_bar));
        for (i32 i = 0; i < 0xa; ++i) thiscall<u32>(upd, static_cast<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]));
        for (i32 k = 0; k < 11; ++k) thiscall<u32>(upd, static_cast<void*>(menu_button(this, k)));  // +0x54..+0x7c
        thiscall<u32>(upd, static_cast<void*>(jo::self<C>(this)->view_toggle_button));
        thiscall<u32>(upd, static_cast<void*>(jo::self<C>(this)->p_171));
        joan::orig::as_obj_menu(this)->update_briefing_widgets();  // 0x00724620
        joan::orig::as_panel(this)->update_7343b0();  // 0x007343b0
        joan::orig::as_obj_menu(this)->update_shop_widgets();  // 0x00738340
        joan::orig::as_panel(this)->reset_buttons();  // 0x00728230
        joan::orig::as_screen(this)->nop_730ba0();  // 0x00730ba0
        joan::orig::as_screen(this)->nop_72c9f0();  // 0x0072c9f0
        joan::orig::as_obj_menu(this)->update_strategy_arrows();  // 0x00736480
        joan::orig::as_obj_menu(this)->spin_siege_model();  // 0x0073b040
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->visible) != 6) return;
        if (jo::self<C>(this)->menu_buttons[4]->y != 0x3e) return;
        {
            const float v = jo::self<C>(this)->menu_buttons[4]->scale;
            if (v < 0.0f || v > 0.0f) return;  // continue when equal or unordered
        }
        if (jo::self<C>(this)->bottom_bar->x != 0x400) return;
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->state[1]) != 0) {
            if (jo::view<i8>(jo::self<jo::CampScreen>(this)->state[3]) == 0) {
                jo::self<jo::CampScreen>(this)->state[0] = 0;
                jo::self<jo::CampScreen>(this)->visible = 0;
                joan::orig::as_screen(this)->free_slots_and_parts();  // 0x0071f360
                jo::self<jo::CampScreen>(this)->state[2] = 0;
                jo::self<jo::CampScreen>(this)->state[1] = 0;
            }
            return;
        }
        if (global<u32>(d::g_screen_pending) != 0 && global<u32>(d::g_screen_pending) != 2) {
            jo::self<jo::CampScreen>(this)->state[0] = 0;
            jo::self<jo::CampScreen>(this)->visible = 0;
            joan::orig::as_screen(this)->free_slots_and_parts();
            jo::self<jo::CampScreen>(this)->state[2] = 0;
            joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
            return;
        }
        if (global<i8>(d::g_menu_frame_state) == 2) {
            joan::orig::as_menu(obj(d::g_hud))->slide_out();  // 0x006f26e0
            return;
        }
        if (global<i8>(d::g_menu_frame_state) != 0) return;
        jo::self<jo::CampScreen>(this)->state[0] = 0;
        jo::self<jo::CampScreen>(this)->visible = 0;
        joan::orig::as_screen(this)->free_slots_and_parts();
        if (global<u8>(d::g_game_running) == 0) {
            global<u8>(d::g_game_running) = 1;
            const float v = static_cast<float>(global<i16>(d::g_saved_game_speed));
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(v));  // 0x0042f930
            joan::orig::as_cursor(obj(d::g_mouse))->set_pos();     // 0x00592bd0
        }
        jo::self<jo::CampScreen>(this)->state[2] = 0;
        jo::self<jo::CampScreen>(this)->state[1] = 0;
        if (global<u32>(d::g_screen_pending) == 2)
            joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
        else
            joan::orig::as_triple(obj(d::g_screen_mgr))->clear();  // 0x006f5e90
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("ObjScreen::update_721270", 0x00721270, &H::objscreen_update_721270);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b104_15.cpp (game/b104_15)
namespace {
namespace game_b104_15_2 {

using u32 = std::uint32_t;

using joan::call::thiscall;


JOAN_COUNTER(c_layout_main, "Screen::layout_main");

struct H {
    // 0x0061e020 / 0x0061f1b0 on the widget pointer member `off`.
    u32 m6(jo::Widget*& off, u32 a, u32 b, u32 c, u32 d, u32 e, u32 g) {
        joan::orig::as_widget(off)->move_to_61e020(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(g));  // 0x0061e020
        return 0;  // 0x0061e020 returns nothing; layout_main discards r
    }
    u32 f5(jo::Widget*& off, u32 a, u32 b, u32 c, u32 d, u32 e) {
        joan::orig::as_widget(off)->scale_to_61f1b0(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
        return 0;  // 0x0061f1b0 returns nothing; layout_main discards r
    }

    // ------------------------------------------------------------------------ 0x007215f0
    u32 layout_main() {
        c_layout_main.hit();
        u32 r;
        r = m6(jo::self<C>(this)->top_bar, 0xfffffc00u, 0x84u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->bottom_bar, 0x400u, 0x298u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->side_bar, 0xfffffe9cu, 0xb8u, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->side_bar, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        for (u32 i = 0; i < 10; ++i) {
            joan::orig::as_widget(jo::self<C>(this)->slot_buttons->buttons[i])->scale_to_61f1b0(0, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), 1, 0u);  // 0x0061f1b0
        }
        r = m6(jo::self<C>(this)->menu_buttons[0], 0x100u, 0x3eu, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[0], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->menu_buttons[1], 0x100u, 0x3eu, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[1], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->menu_buttons[2], 0x100u, 0x3eu, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[2], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->menu_buttons[3], 0x100u, 0x3eu, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[3], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->menu_buttons[4], 0x100u, 0x3eu, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[4], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->menu_buttons[5], 0x100u, 0x3eu, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[5], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->menu_buttons[6], 0x100u, 0x3eu, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[6], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->menu_buttons[7], 0x1ceu, 0x298u, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[7], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->menu_buttons[8], 0xa2u, 0x298u, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[8], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->menu_buttons[9], 0x19cu, 0x298u, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[9], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(jo::self<C>(this)->menu_buttons[10], 0x296u, 0x298u, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->menu_buttons[10], 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->view_toggle_button, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(jo::self<C>(this)->p_171, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        joan::orig::as_obj_menu(this)->reset_briefing_widgets();         // 0x00724670
        joan::orig::as_obj_menu(this)->reset_reward_widgets();         // 0x007349b0
        joan::orig::as_obj_menu(this)->reset_shop_widgets();         // 0x007388a0
        joan::orig::as_obj_menu(this)->reset_army_widgets();         // 0x00728290
        joan::orig::as_screen(this)->nop_730bc0();         // 0x00730bc0
        joan::orig::as_screen(this)->nop_72ca10();         // 0x0072ca10
        joan::orig::as_obj_menu(this)->reset_strategy_arrows();         // 0x007364c0
        (void)r;
        joan::orig::as_screen(this)->nop_73b140();  // 0x0073b140
        return 0;  // 0x0073b140 returns nothing; callers discard the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_15", name, addr, fn)
B("Screen::layout_main", 0x007215f0, &H::layout_main);
#undef B

}  // namespace game_b104_15_2
}  // namespace

// ---------------------------------------- from modules/game_b109_14.cpp (game/b109_14)
namespace {
namespace game_b109_14 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::hook::x87::ftol;
using i8 = std::int8_t;
using i16 = std::int16_t;

i32 sx16(i16 v) { return static_cast<i32>(v); }

JOAN_COUNTER(c_button, "draw_button_722990");

// ------------------------------------------------------------------ 0x00722990
u32 button_colour(const u8* button, float scale_const, u32* slot) {
    const float a = scale_const * jo::as_button(button)->scale;
    const i32 alpha = joan::hook::x87::ftol(static_cast<double>(a));
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, alpha));  // 0x0042a260
}

void blit_cs(i16 x, i16 y, u32 ctx, img::Addr name, img::Addr res, u32 colour) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(sx16(x)), joan::orig::arg::i16(sx16(y)), joan::orig::arg::u32(ctx), joan::orig::arg::str(str(name)), img::obj(res), joan::orig::arg::u32(colour), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}

static u32 JOAN_CDECL draw_button(u8* button, u32 ctx, u32 state_arg) {
    c_button.hit();
    const u32 state = state_arg & 0xff;
    if (state != 7 && (state & 1) != 0) return 0;
    i16 w4 = jo::as_button(button)->x;
    i16 w8 = jo::as_button(button)->y;
    i16 wc = jo::as_button(button)->x2;
    i16 w10 = jo::as_button(button)->y2;
    float f14 = 1.0f;
    float f18 = 1.0f;
    if (jo::as_button(button)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(// 0x00633e60
                      sx16(jo::as_button(button)->x)), joan::orig::arg::i16(sx16(jo::as_button(button)->y)), joan::orig::arg::i16(sx16(jo::as_button(button)->x2)), joan::orig::arg::i16(sx16(jo::as_button(button)->y2)), &f14, &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
    }
    i32 text_w = 0;
    if (jo::view<i8>(jo::as_button(button)->caption[0]) != 0) {
        text_w = sx16(joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(jo::as_button(button)->caption), -1, 0, 0, 1.0f));                                    // 0x00624f00
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->text_height(joan::orig::arg::i16(i32{2}), 1.0f);              // 0x006253f0 (height, unused)
    }
    const float k255 = global<float>(d::flt_255);
    if (jo::as_button(button)->enabled != 0) {
        if (jo::as_button(button)->pressed != 0) {
            u32 slot = 0;
            blit_cs(w4, w8, ctx, d::s_CS_DW, d::g_bmp_commele2, button_colour(button, k255, &slot));  // CS_DW
        } else if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(sx16(jo::as_button(button)->x)), joan::orig::arg::i32(// 0x00593980
                                 sx16(jo::as_button(button)->y)), joan::orig::arg::i32(sx16(jo::as_button(button)->x2)), joan::orig::arg::i32(sx16(jo::as_button(button)->y2)))) != 0) {
            u32 slot = 0;
            blit_cs(w4, w8, ctx, d::s_CS_OV, d::g_bmp_commele2, button_colour(button, k255, &slot));  // CS_OV
        } else {
            u32 slot = 0;
            blit_cs(w4, w8, ctx, d::s_CS_OT, d::g_bmp_commele2, button_colour(button, k255, &slot));  // CS_OT
        }
    } else {
        u32 slot = 0;
        blit_cs(w4, w8, ctx, d::s_CS_DS, d::g_bmp_commele2, button_colour(button, k255, &slot));  // CS_DS
    }
    if (jo::view<i8>(jo::as_button(button)->caption[0]) == 0) return 1;
    u32 label_slot = 0;
    const u32 label_colour = jo::as_button(button)->enabled != 0 ? button_colour(button, k255, &label_slot)
                                                    : button_colour(button, global<float>(d::flt_128), &label_slot);
    joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_3(joan::orig::arg::i32(sx16(jo::as_button(button)->x)), joan::orig::arg::i32(sx16(jo::as_button(button)->y)), joan::orig::arg::i32(// 0x00626130
                  sx16(jo::as_button(button)->x2)), joan::orig::arg::i32(sx16(jo::as_button(button)->y2) - 8), joan::orig::arg::u32(ctx), joan::orig::arg::str(jo::as_button(button)->caption), joan::orig::arg::u32(label_colour), i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);
    if (jo::as_button(button)->pressed == 0 &&
        joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(sx16(jo::as_button(button)->x)), joan::orig::arg::i32(sx16(jo::as_button(button)->y)), joan::orig::arg::i32(// 0x00593980
                      sx16(jo::as_button(button)->x2)), joan::orig::arg::i32(sx16(jo::as_button(button)->y2)))) == 0)
        return 1;
    {
        u32 slot = 0;
        const u32 colour = button_colour(button, k255, &slot);
        const i32 y = sx16(jo::as_button(button)->y) + 0x11;
        const i32 x = sx16(jo::as_button(button)->x) + (0x200 - text_w) / 2 - 0x20;
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), str(d::s_DIAMOND), img::obj(d::g_bmp_res_menu_b), joan::orig::arg::u32(colour), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // DIAMOND
    }
    {
        u32 slot = 0;
        const u32 colour = button_colour(button, k255, &slot);
        const i32 y = sx16(jo::as_button(button)->y) + 0x11;
        const i32 x = sx16(jo::as_button(button)->x) + (text_w + 0x200) / 2 + 0xa;
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), str(d::s_DIAMOND), img::obj(d::g_bmp_res_menu_b), joan::orig::arg::u32(colour), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // DIAMOND
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_14", name, addr, fn)
B("draw_button_722990", 0x00722990, &draw_button);
#undef B

}  // namespace game_b109_14
}  // namespace

// ---------------------------------------- from modules/game_b109_12.cpp (game/b109_12)
namespace {
namespace game_b109_12 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::hook::x87::Fpu;
using joan::hook::x87::ftol;
using i8 = std::int8_t;
using i16 = std::int16_t;


constexpr u32 kOne = 0x3f800000;  // 1.0f bits

// 0x0042a260 (thunk 0x004098b8): 4-byte colour from r, g, b, a; returns its address.
u32 make_color(u32 r, u32 g, u32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*jo::self<jo::Colour>(p));
}
// 0x00633500 (thunk 0x00401bbd) on 0xa46128, ret 0x44.
void draw_sprite(i32 x, i32 y, u32 surf, img::Addr name, img::Addr res, u32 color, i32 a7, i32 a8, i32 a9, i32 a10, i32 a11,
                 i32 a12, i32 a13, i32 a14, i32 a15, i32 a16, i32 a17) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(surf), joan::orig::arg::str(img::addr(name)), img::obj(res), joan::orig::arg::u32(color), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), joan::orig::arg::i16(a15), joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));
}

JOAN_COUNTER(c_mark, "mark_draw_723020");

// ---- 0x00723020 -----------------------------------------------------------------------------
struct MarkCtx {
    void* ctl;
    u32 surf;
    i16 lx, ly, lz, lw;
    float s14, s18;
};

i32 alpha_of(void* ctl) {
    const float a = global<float>(d::flt_255) * jo::as_button(ctl)->scale;  // 255.0f * alpha
    return joan::hook::x87::ftol(static_cast<double>(a));
}

void mark_sprites(MarkCtx& c) {
    const Fpu F{true};
    const i32 w = static_cast<i16>(joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(jo::as_button(c.ctl)->caption), -1, 0, 0, joan::orig::arg::f32(kOne)));  // 0x00624f00
    joan::orig::as_font(obj(d::g_font_goudy14_22_white))->text_height(joan::orig::arg::i16(i32{2}), joan::orig::arg::f32(kOne));  // 0x006253f0 (result dead)
    {
        const u32 color = make_color(0xff, 0xff, 0xff, alpha_of(c.ctl));
        const i32 v = i32{c.lx} + i32{c.lz} - w;
        const i32 x = joan::hook::x87::ftol(F.sub(F.mul(static_cast<double>(v), 0.5), 20.0));
        draw_sprite(x, i32{c.ly} + 8, c.surf, d::s_MARK, d::g_bmp_commele2, color, 3, 0, 2, 0, -1, -1, -1, -1, 0, -1,
                    -1);  // "MARK"
    }
    {
        const u32 color = make_color(0xff, 0xff, 0xff, alpha_of(c.ctl));
        const i32 v = i32{c.lz} + (w + i32{c.lx});
        const i32 x = joan::hook::x87::ftol(F.add(F.mul(static_cast<double>(v), 0.5), 6.0));
        draw_sprite(x, i32{c.ly} + 8, c.surf, d::s_MARK, d::g_bmp_commele2, color, 3, 0, 2, 0, -1, -1, -1, -1, 0, -1,
                    -1);  // "MARK"
    }
}

i32 scaled(float s) {  // 0x00590a20 on 0xa45fa8 (1.0f * s as double), short result
    const float v = global<float>(d::flt_1) * s;
    return static_cast<i16>(joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(v)))));
}

// Text colour: black (0,0,0,alpha) when [+0x8a] != 0 (0x007232f5), white (0xff,0xff,0xff,alpha) on
// the two [+0x8a] == 0 paths (0x00723572, 0x0072366c).
void mark_text(MarkCtx& c, u32 rgb) {
    const i32 dy = jo::as_button(c.ctl)->pressed != 0 ? scaled(c.s18) : 0;
    const i32 dx = jo::as_button(c.ctl)->pressed != 0 ? scaled(c.s14) : 0;
    const u32 color = make_color(rgb, rgb, rgb, alpha_of(c.ctl));
    joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_3(joan::orig::arg::i32(static_cast<i16>(i32{c.lx} + dx)), joan::orig::arg::i32(static_cast<i16>(i32{c.ly} + dy - 1)), joan::orig::arg::i32(c.lz), joan::orig::arg::i32(static_cast<i16>(i32{c.lw} - 1)), joan::orig::arg::u32(c.surf), joan::orig::arg::str(jo::as_button(c.ctl)->caption), joan::orig::arg::u32(color), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626130
}

i32 JOAN_CDECL mark_draw(void* ctl, u32 surf, u32 flags) {
    c_mark.hit();
    if ((flags & 0xff) != 7 && ((flags & 0xff) & 1) != 0) return 0;
    MarkCtx c{ctl, surf, jo::as_button(ctl)->x, jo::as_button(ctl)->y, jo::as_button(ctl)->x2, jo::as_button(ctl)->y2, 1.0f, 1.0f};
    if (jo::as_button(ctl)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::as_button(ctl)->x), joan::orig::arg::i16(jo::as_button(ctl)->y), joan::orig::arg::i16(jo::as_button(ctl)->x2), joan::orig::arg::i16(jo::as_button(ctl)->y2), &c.s14, &c.s18, &c.lx, &c.ly, &c.lz, &c.lw, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    if (jo::view<i8>(jo::as_button(ctl)->caption[0]) == 0) return 1;
    if (jo::as_button(ctl)->pressed != 0) {
        mark_sprites(c);
        mark_text(c, 0);
    } else if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{c.lx}, i32{c.ly}, i32{c.lz}, i32{c.lw})) !=
               0) {  // 0x00593980
        mark_sprites(c);
        mark_text(c, 0xff);
    } else {
        mark_text(c, 0xff);
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_12", name, addr, fn)
B("mark_draw_723020", 0x00723020, &mark_draw);
#undef B

}  // namespace game_b109_12
}  // namespace

// ---------------------------------------- from modules/game_b108_13_2.cpp (game/b108_13)
namespace {
namespace game_b108_13_2 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::hook::x87::ftol;
using i8 = std::int8_t;
using i16 = std::int16_t;


void* const kScreen = obj(d::g_mouse);
void* const kScaler = obj(d::g_sprites_a46128);
constexpr u32 kOne = 0x3f800000;  // 1.0f pushed as a dword

// 0x0042a260 Color::Color(r, g, b, a) on a 4-byte local; returns this
u32 color(u32 r, u32 g, u32 b, i32 a) {
    u32 c[2] = {0, 0};
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(c)->construct(r, g, b, a));
}
i32 alpha_of(const void* p) { return joan::hook::x87::ftol(global<float>(d::flt_255) * jo::as_button(p)->scale); }
i32 box_visible(i32 x0, i32 y0, i32 x1, i32 y1) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(kScreen)->inside(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1)));  // 0x00593980
}

JOAN_COUNTER(c_button_cb, "button_draw_cb_723870");
JOAN_COUNTER(c_draw_mask, "Slider::draw_mask");

// ------------------------------------------------------------------ 0x00723870 (cdecl)
i32 JOAN_CDECL button_draw_cb(void* p, i32 b, i32 c) {
    c_button_cb.hit();
    const u32 state = static_cast<u32>(c) & 0xffu;
    if (state != 7 && (state & 1u) != 0) return 0;
    if (jo::view<i8>(jo::as_button(p)->caption[0]) == 0) return 0;
    i16 x0 = jo::as_button(p)->x;
    i16 y0 = jo::as_button(p)->y;
    i16 x1 = jo::as_button(p)->x2;
    i16 y1 = jo::as_button(p)->y2;
    u32 sx = kOne;
    u32 sy = kOne;
    if (jo::as_button(p)->fit != 0) {
        joan::orig::as_sprite2d(kScaler)->fit_rect(joan::orig::arg::i16(jo::as_button(p)->x), joan::orig::arg::i16(jo::as_button(p)->y), joan::orig::arg::i16(jo::as_button(p)->x2), joan::orig::arg::i16(jo::as_button(p)->y2), &sx, &sy, &x0, &y0, &x1, &y1, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    }
    void* const btn = jo::as_button(p)->caption_box;
    joan::orig::as_font(btn)->measure(joan::orig::arg::str(jo::as_button(p)->caption), -1, 0, 0, joan::orig::arg::f32(kOne));  // 0x00624f00 (result unused)
    joan::orig::as_font(btn)->text_height(joan::orig::arg::i16(2), joan::orig::arg::f32(kOne));                       // 0x006253f0 (result unused)
    img::Addr name = d::s_M_OT;                                // "M_OT"
    if (jo::as_button(p)->enabled != 0 && box_visible(static_cast<i32>(jo::as_button(p)->x), static_cast<i32>(jo::as_button(p)->y), static_cast<i32>(jo::as_button(p)->x2), static_cast<i32>(jo::as_button(p)->y2)) != 0)
        name = d::s_M_OV;  // "M_OV"
    {
        const i32 alpha = alpha_of(p);
        const u32 col = color(0xff, 0xff, 0xff, alpha);
        joan::orig::as_sprite_queue(kScaler)->draw_named(joan::orig::arg::i16(static_cast<i32>(x0)), joan::orig::arg::i16(static_cast<i32>(y0)), joan::orig::arg::u32(b), joan::orig::arg::str(str(name)), obj(d::g_bmp_objmenu), joan::orig::arg::u32(col), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
    }
    const i32 alpha = alpha_of(p);
    const u32 col = color(0xff, 0xff, 0xff, alpha);
    joan::orig::as_text(jo::as_button(p)->caption_box)->draw(joan::orig::arg::i32(static_cast<i32>(jo::as_button(p)->x) + 0x32), joan::orig::arg::i32(static_cast<i32>(jo::as_button(p)->y) + 1), joan::orig::arg::i32(static_cast<i32>(jo::as_button(p)->x2) - 0xe), joan::orig::arg::i32(static_cast<i32>(jo::as_button(p)->y2) - 4), joan::orig::arg::u32(b), joan::orig::arg::str(jo::as_button(p)->caption), joan::orig::arg::u32(col), 2, 1, 1, 1, 2, 0, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), 0, joan::orig::arg::i32(-1));  // 0x00626320
    return 1;
}

struct H {
    // ------------------------------------------------------------------ 0x007240f0
    void draw_mask(u32 unused) {
        c_draw_mask.hit();
        (void)unused;
        if (!(jo::self<jo::CampScreen>(this)->brief_up->scale > global<float>(d::k_9cf5c4))) return;
        joan::orig::as_label(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_text))->draw(6u, 7u);  // 0x00629bd0
        const u32 col = color(0xff, 0xff, 0xff, 0xff);
        joan::orig::as_sprite_queue(kScaler)->draw_named(0, 0, 6u, str(d::s_mask), obj(d::g_bmp_objmenu), joan::orig::arg::u32(col), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
        if (static_cast<i32>(jo::self<C>(this)->brief_line) > -4) joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_up))->draw(4u, 7);  // 0x00622cc0
        if (static_cast<i32>(jo::self<C>(this)->brief_line) < static_cast<i32>(jo::self<C>(this)->brief_px) - 0xa) joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_down))->draw(4u, 7);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("button_draw_cb_723870", 0x00723870, &button_draw_cb);
B("Slider::draw_mask", 0x007240f0, &H::draw_mask);
#undef B

}  // namespace game_b108_13_2
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

using u16 = std::uint16_t;

u32 low_byte_1(std::uintptr_t eax) { return (static_cast<u32>(eax) & 0xffffff00u) | 1u; }  // al = 1; the rest of eax is whatever the original left (callers read al)

JOAN_COUNTER(c_scroller_724210, "Scroller::buttons_724210");

struct H {
    // -------------------------------------------------------------- 0x00724210
    std::uintptr_t scroll_step(void* list, i32 delta) {
        jo::self<jo::CampScreen>(this)->brief_line = static_cast<i16>(jo::self<jo::CampScreen>(this)->brief_line + delta);
        return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_label(list)->set_layout(joan::orig::arg::u32(i32{0x314}), joan::orig::arg::u32(i32{0x262}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(jo::view<u16>(jo::self<jo::CampScreen>(this)->brief_line)), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{-1})));  // 0x006299b0
    }
    u32 hovered(void* button) {  // 0x00593980
        return joan::orig::arg::as<u32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(jo::as_button(button)->x), static_cast<i32>(jo::as_button(button)->y), static_cast<i32>(jo::as_button(button)->x2), static_cast<i32>(jo::as_button(button)->y2)));
    }
    u32 clicked(void* button, i32 key) {  // 0x006218c0
        return joan::orig::arg::as<u32>(joan::orig::as_ui_button(button)->mouse(joan::orig::arg::i32(key), i32{0}, i32{-1}, i32{0}, i32{0}, i32{0}));
    }
    u32 scroller_buttons() {
        c_scroller_724210.hit();
        const std::uintptr_t self = reinterpret_cast<std::uintptr_t>(this);  // only its low byte is meaningful (al = 1)
        if (clicked(jo::self<jo::CampScreen>(this)->brief_up, 0) != 0) {
            if (jo::self<jo::CampScreen>(this)->brief_line > -4) scroll_step(jo::self<jo::CampScreen>(this)->brief_text, -1);
            jo::view<u32>(jo::self<jo::CampScreen>(this)->brief_hold_ms) = 0;
            return low_byte_1(self);
        }
        if (jo::as_button(jo::self<jo::CampScreen>(this)->brief_up)->pressed != 0) {
            const u32 r = hovered(jo::self<jo::CampScreen>(this)->brief_up);
            if (r != 0) {
                jo::self<jo::CampScreen>(this)->brief_hold_ms = jo::self<jo::CampScreen>(this)->brief_hold_ms + global<i32>(d::g_frame_elapsed_ms);
                if (jo::self<jo::CampScreen>(this)->brief_hold_ms < 100) return low_byte_1(self);
                jo::self<jo::CampScreen>(this)->brief_hold_ms = jo::self<jo::CampScreen>(this)->brief_hold_ms - 100;
                if (!(jo::self<jo::CampScreen>(this)->brief_line > -4)) return low_byte_1(static_cast<u32>(static_cast<i32>(jo::self<jo::CampScreen>(this)->brief_line)));
                return low_byte_1(scroll_step(jo::self<jo::CampScreen>(this)->brief_text, -1));
            }
        }
        if (clicked(jo::self<jo::CampScreen>(this)->brief_down, 0) != 0) {
            const i32 pos = jo::self<jo::CampScreen>(this)->brief_line;
            if (!(pos < jo::self<jo::CampScreen>(this)->brief_px - 10)) return low_byte_1(static_cast<u32>(pos));
            return low_byte_1(scroll_step(jo::self<jo::CampScreen>(this)->brief_text, 1));
        }
        if (jo::as_button(jo::self<jo::CampScreen>(this)->brief_down)->pressed != 0) {
            const u32 r = hovered(jo::self<jo::CampScreen>(this)->brief_down);
            if (r != 0) {
                jo::self<jo::CampScreen>(this)->brief_hold_ms = jo::self<jo::CampScreen>(this)->brief_hold_ms + global<i32>(d::g_frame_elapsed_ms);
                if (jo::self<jo::CampScreen>(this)->brief_hold_ms < 100) return low_byte_1(self);
                jo::self<jo::CampScreen>(this)->brief_hold_ms = jo::self<jo::CampScreen>(this)->brief_hold_ms - 100;
                if (!(jo::self<jo::CampScreen>(this)->brief_line < jo::self<jo::CampScreen>(this)->brief_px - 10)) return low_byte_1(self);
                return low_byte_1(scroll_step(jo::self<jo::CampScreen>(this)->brief_text, 1));
            }
        }
        if (clicked(jo::self<jo::CampScreen>(this)->menu_buttons[7], 0x1b) != 0) return low_byte_1(joan::orig::arg::as<u32>(joan::orig::as_camp_screen(this)->set_state_721a60(joan::orig::arg::u32(u8{0}))));  // 0x00721a60
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Scroller::buttons_724210", 0x00724210, &H::scroller_buttons);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;


u32 release_61f3b0(void* p) { return joan::orig::arg::as<u32>(joan::orig::as_widget(p)->update()); }  // 0x0061f3b0

struct H {
    // --------------------------------------------------------------------- 0x00724620
    u32 release3() {
        release_61f3b0(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_up));
        release_61f3b0(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_down));
        return release_61f3b0(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_text));
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Dlg::release3", 0x00724620, &H::release3);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using u32 = std::uint32_t;

using joan::call::thiscall;


struct H {
    // ------------------------------------------------------------------ 0x00724670 / 0x007349b0
    static u32 button_reset(void* b) { joan::orig::as_widget(b)->scale_to_61f1b0(0, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), 1, 0u); return 0; }  // 0x0061f1b0 (returns nothing; 0x00724670's only caller discards)
    u32 reset_buttons_724670() {
        button_reset(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_up));
        button_reset(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_down));
        return button_reset(jo::view<void*>(jo::self<jo::CampScreen>(this)->brief_text));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::reset_buttons_724670", 0x00724670, &H::reset_buttons_724670);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b109_05.cpp (game/b109_05)
//
//    - CampScreen::set_state 0x00721a60 (ret 4, byte state): leave-state switch (jump table
//      0x007223d1) on the signed byte +0x80, store, enter-state switch (jump table 0x007223f9).
//      Returns what the original leaves in eax (last callee's eax, or this on the default path).
namespace {
namespace game_b109_05 {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_camp_state, "CampScreen::set_state");

// ---------------------------------------------------------------------------------- 0x00721a60
std::uintptr_t move(void* o, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) {
    return joan::orig::arg::as<std::uintptr_t>(joan::orig::as_widget(o)->move_to_61e020(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(joan::orig::arg::i32(e)), joan::orig::arg::u32(f)));  // 0x0061e020
}
void anim(void* o, u32 a, u32 b, u32 c, u32 d, u32 e) {
    joan::orig::as_widget(o)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
}
constexpr u32 kSix = 0x40c00000u;    // 6.0f
constexpr u32 kX = 0x455ac000u;      // 3500.0f
constexpr u32 kY = 0xc4bb8000u;      // -1500.0f
constexpr u32 kFar = 0x477fff00u;    // 65535.0f

struct H {
    // ------------------------------------------------------------------------------ 0x00721a60
    std::uintptr_t camp_set_state(u32 arg) {  // returns a pointer-sized eax leftover (the facade returns std::uintptr_t)
        c_camp_state.hit();
        const u32 old = static_cast<u32>(static_cast<i32>(jo::view<i8>(jo::self<jo::CampScreen>(this)->visible)));
        if (old <= 9) {
            switch (old) {
                case 0:
                    for (i32 k = 0; k < 7; ++k) {  // +0x54..+0x6c
                        anim(menu_button(this, k), 0, kSix, 0, 0, 0);
                        move(menu_button(this, k), 0x100, 0x3e, kX, kY, 0, 0);
                    }
                    anim(jo::self<C>(this)->menu_buttons[8], 0, kSix, 0, 0, 0);
                    anim(jo::self<C>(this)->menu_buttons[9], 0, kSix, 0, 0, 0);
                    anim(jo::self<C>(this)->menu_buttons[10], 0, kSix, 0, 0, 0);
                    anim(jo::self<C>(this)->menu_buttons[7], 0, kSix, 0, 0, 0);
                    anim(jo::self<C>(this)->view_toggle_button, 0, kSix, 0, 0, 0);
                    anim(jo::self<C>(this)->p_171, 0, kSix, 0, 0, 0);
                    break;
                case 1: joan::orig::as_obj_menu(this)->reset_briefing(); break;  // 0x00724bf0
                case 2: joan::orig::as_screen(this)->reset_735550(); break;  // 0x00735550
                case 3: joan::orig::as_screen(this)->reset_738d00(); break;  // 0x00738d00
                case 4: joan::orig::as_obj_menu(this)->leave_army(); break;  // 0x00728d20
                case 5:
                    move(jo::self<C>(this)->menu_buttons[7], 0x1ce, 0x298, kX, kY, 0, 0);
                    anim(jo::self<C>(this)->view_toggle_button, 0, kSix, 0, 0, 0);
                    joan::orig::as_screen(this)->purge_items();  // 0x00731930
                    break;
                case 7: joan::orig::as_obj_menu(this)->leave_debrief(); break;  // 0x0072ca90
                case 8: joan::orig::as_screen(this)->reset_736770(); break;  // 0x00736770
                case 9:
                    move(jo::self<C>(this)->menu_buttons[7], 0x1ce, 0x298, kX, kY, 0, 0);
                    anim(jo::self<C>(this)->p_171, 0, kSix, 0, 0, 0);
                    joan::orig::as_obj_menu(this)->leave_siege();  // 0x0073b1c0
                    break;
                default: break;  // 6
            }
        }
        jo::self<jo::CampScreen>(this)->visible = static_cast<u8>(arg);
        const u32 state = static_cast<u32>(static_cast<i32>(static_cast<i8>(static_cast<u8>(arg))));
        if (state > 9) return reinterpret_cast<std::uintptr_t>(this);
        switch (state) {
            case 0: {
                static const u32 kDelay[7] = {0x1f4, 0x226, 0x258, 0x28a, 0x2bc, 0x2ee, 0x320};
                static const u32 kRow[7] = {0x24c, 0x20d, 0x1ce, 0x18f, 0x150, 0x111, 0xd2};
                for (i32 k = 0; k < 7; ++k) {
                    void* const o = menu_button(this, 6 - k);  // +0x6c - 4 k
                    anim(o, 1, kSix, 0, 0, kDelay[k]);
                    move(o, 0x100, kRow[k], kX, kY, 0, kDelay[k]);
                }
                move(jo::self<C>(this)->top_bar, 0, 0x84, kX, kY, 0, 0x15e);
                move(jo::self<C>(this)->bottom_bar, 0, 0x298, kX, kY, 0, 0x15e);
                if (jo::view<i8>(jo::self<jo::CampScreen>(this)->state[1]) == 0) {
                    anim(jo::self<C>(this)->menu_buttons[7], 1, kSix, 0, 0, 0x1f4);
                } else {
                    anim(jo::self<C>(this)->menu_buttons[8], 1, kSix, 0, 0, 0x1f4);
                    anim(jo::self<C>(this)->menu_buttons[9], 1, kSix, 0, 0, 0x1f4);
                    anim(jo::self<C>(this)->menu_buttons[10], 1, kSix, 0, 0, 0x1f4);
                }
                move(jo::self<C>(this)->menu_buttons[8], 0xa2, 0x298, kFar, 0, 0, 0x1f4);
                move(jo::self<C>(this)->menu_buttons[9], 0x19c, 0x298, kFar, 0, 0, 0x1f4);
                return move(jo::self<C>(this)->menu_buttons[10], 0x296, 0x298, kFar, 0, 0, 0x1f4);
            }
            case 1: return joan::orig::arg::as<u32>(joan::orig::as_obj_menu(this)->open_brief());  // 0x007246f0
            case 2: return joan::orig::arg::as<u32>(joan::orig::as_dialog(this)->reset_7353a0());  // 0x007353a0
            case 3: return joan::orig::arg::as<u32>(joan::orig::as_obj_menu(this)->enter_shop());  // 0x00738b50
            case 4: return joan::orig::arg::as<u32>(joan::orig::as_screen(this)->layout_728a90());  // 0x00728a90
            case 5:
                move(jo::self<C>(this)->menu_buttons[7], 0x219, 0x298, kX, kY, 0, 0);
                anim(jo::self<C>(this)->view_toggle_button, 1, kSix, 0, 0, 0);
                return joan::orig::arg::as<u32>(joan::orig::as_obj_menu(this)->spawn_hero_models());  // 0x00730be0
            case 6: {
                anim(jo::self<C>(this)->menu_buttons[7], 0, kSix, 0, 0, 0);
                anim(jo::self<C>(this)->menu_buttons[8], 0, kSix, 0, 0, 0);
                anim(jo::self<C>(this)->menu_buttons[9], 0, kSix, 0, 0, 0);
                anim(jo::self<C>(this)->menu_buttons[10], 0, kSix, 0, 0, 0);
                anim(jo::self<C>(this)->menu_buttons[0], 0, kSix, 0, 0, 0);
                move(jo::self<C>(this)->menu_buttons[0], 0x100, 0x3e, kX, kY, 0, 0);
                static const u32 kDelay6[6] = {0x32, 0x64, 0x96, 0xc8, 0xfa, 0x12c};
                for (i32 k = 0; k < 6; ++k) {
                    void* const o = menu_button(this, 1 + k);  // +0x58 + 4 k
                    anim(o, 0, kSix, 0, 0, kDelay6[k]);
                    move(o, 0x100, 0x3e, kX, kY, 0, kDelay6[k]);
                }
                move(jo::self<C>(this)->top_bar, 0xfffffc00u, 0x84, kX, kY, 0, 0x190);
                return move(jo::self<C>(this)->bottom_bar, 0x400, 0x298, kX, kY, 0, 0x190);
            }
            case 7: return joan::orig::arg::as<u32>(joan::orig::as_obj_menu(this)->enter_debrief());  // 0x0072ca30
            case 8: return joan::orig::arg::as<u32>(joan::orig::as_obj_menu(this)->enter_strategy());  // 0x007365e0
            default:                                          // 9
                move(jo::self<C>(this)->menu_buttons[7], 0x183, 0x298, kX, kY, 0, 0);
                anim(jo::self<C>(this)->p_171, 1, kSix, 0, 0, 0);
                return joan::orig::arg::as<u32>(joan::orig::as_screen(this)->reset_73b160());  // 0x0073b160
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_05", name, addr, fn)
// 0x006ba8b0 ArmoryPanel::refresh: NOT registered (see the header).
B("CampScreen::set_state_721a60", 0x00721a60, &H::camp_set_state);
#undef B

}  // namespace game_b109_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ObjMenu::zero() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b96_08::H::clear_278>(this)); }  // 0x71d290
auto joan::orig::ObjMenu::release_71e800() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_01::H::release_71e800>(this)); }  // 0x71e800
auto joan::orig::ObjMenu::load_masks() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_05::H::reload_masks>(this)); }  // 0x71efc0
auto joan::orig::Screen::free_slots_and_parts() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15::H::free_slots>(this)); }  // 0x71f360
auto joan::orig::ObjMenu::draw_camera() -> void { joan::bind::member<&::game_ui8::H::draw_camera>(this); }  // 0x720bc0
auto joan::orig::Screen::layout_main() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15_2::H::layout_main>(this)); }  // 0x7215f0
auto joan::orig::CampScreen::set_state_721a60(std::uint32_t a0) -> std::uintptr_t { return joan::bind::conv<std::uintptr_t>(joan::bind::member<&::game_b109_05::H::camp_set_state>(this, a0)); }  // 0x721a60
auto joan::orig::Menu::open_7226a0(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_12::H::open_7226a0>(this, a0)); }  // 0x7226a0
auto joan::orig::Dialog::close_if_open() -> void { joan::bind::member<&::game_b108_13::H::close_if_open>(this); }  // 0x722950
auto joan::orig::ObjMenu::build_briefing() -> void { joan::bind::member<&::game_b102_15::H::scroller_723c80>(this); }  // 0x723c80
auto joan::orig::ObjMenu::free_briefing() -> void { joan::bind::member<&::game_b100_16::H::release_723f90>(this); }  // 0x723f90
auto joan::orig::Slider::draw_mask(std::uint32_t a0) -> void { joan::bind::member<&::game_b108_13_2::H::draw_mask>(this, a0); }  // 0x7240f0
auto joan::orig::ObjMenu::update_briefing() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::scroller_buttons>(this)); }  // 0x724210
auto joan::orig::ObjMenu::update_briefing_widgets() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_16::H::release3>(this)); }  // 0x724620
auto joan::orig::ObjMenu::reset_briefing_widgets() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::reset_buttons_724670>(this)); }  // 0x724670
