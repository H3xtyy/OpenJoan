// interfac/optmenui.cpp: functions of the original d:\ww\interfac\optmenui.cpp (re/attribution.csv).
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
#include <cstddef>
#include <cmath>
#include "../canvas_api.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "view_range.hpp"
#include "graphics_page.hpp"
#include "mouse_options.hpp"
#include "remap_set.hpp"
#include "../platform/input.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using joan::hook::x87::Fpu;
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

namespace mouse = joan::optmenu::mouse;

// The two calls the rows are animated with (Widget::move_to 0x0061e020, Widget::scale_to 0x0061f1b0), for the cursor
// row's widgets, which are not members of OptMenu.
void widget_move(void* w, u32 a, u32 b, u32 c, u32 d, u32 e, u32 g) {
    joan::orig::as_widget(w)->move_to_61e020(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c),
                                             joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(g));
}
void widget_scale(void* w, u32 a, u32 b, u32 c, u32 d, u32 e) {
    joan::orig::as_widget(w)->scale_to_61f1b0(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c),
                                              static_cast<char>(d), joan::orig::arg::u32(e));
}

}  // namespace

// ---------------------------------------- from modules/game_b110_10.cpp (game/b110_10)
//
//    - OptMenu::build_controls_7435e0 0x007435e0 (ret 0, returns this as the original leaves it;
//      its only caller 0x0073ba80 ignores eax, interfac/optmenui.cpp): 25 text lists (0x00620ee0,
//      new 0x346), 4 selectors (0x006b22f0, new 0xad), 2 sliders (0x0075fab0, new 0x99) and two
//      10-entry key-binding label arrays allocated through 0x0058e7b0 (size 0x28,
//      "d:\ww\interfac\optmenui.cpp", line [0xa2c920] + 0x88 / + 0x90); then +0x114 = -1,
//      +0x110 = +0x10c = 0.
//
//  - 0x007435e0: new (0x007b1d35) + ctor SEH frames: normal path only (no callee throws). The
//    colour helper 0x0042a260 is called between the pushes of 0x00623ce0, before the font getter,
//    as in the original. Slider value: fld [0xa58027] * 10.0f + 10.0f (float ops) -> _ftol, done
//    before new(0x99) like the original; [0xa5802e] -> _ftol after 0x00760050.
namespace {
namespace game_b110_10 {

using joan::hook::x87::ftol;

using joan::orig::helpers::add32;

using joan::orig::helpers::game_new;  // operator new

void* const kStrings = obj(d::g_strings);

JOAN_COUNTER(c_opt_build, "OptMenu::build_controls_7435e0");

// 0x0042a260 colour(r, g, b, a) on a local 4-byte object, packed dword read back.
u32 make_colour(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
}

struct H {
    using M = jo::OptMenu;
    // The widget pointer member `p` of this OptMenu.
    template <class W>
    W*& m(W* M::*p) {
        return jo::self<M>(this)->*p;
    }

    // ------------------------------------------------------------------------------ 0x007435e0
    static void* new_list() {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86
        return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(mem)->ctor_620ee0()) : nullptr;  // 0x00620ee0
    }
    static void* new_selector() {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::ComboBox)));  // 0xad on x86
        return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(mem)->m_9dd7a0_ctor()) : nullptr;  // 0x006b22f0
    }
    static void* new_slider() {
        void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::ScrollBox)));  // 0x99 on x86 (ScrollBox: Slider is a view of it)
        return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(mem)->m_9df888_ctor()) : nullptr;  // 0x0075fab0
    }
    // 0x00621540(x0, y0, x1, y1, callback, 1, 0, 0, 1, 1, 1)
    static void place(void* list, i32 x0, i32 y0, i32 x1, i32 y1, std::uintptr_t callback) {
        joan::orig::as_button(list)->create(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), callback, i32{1}, i32{0}, i32{0}, i32{1}, i32{1}, i32{1});
    }
    static void set_text_of(void* w, void* t) {
        const u32 colour = make_colour(0xff, 0xff, 0xff, 0xff);
        joan::orig::as_button(w)->set_text(joan::orig::arg::str(t), obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(colour), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
    }
    // colour, then the text from the string table getter, then 0x00623ce0.
    template <class W>
    void text(W* M::*off, std::uintptr_t getter) {
        void* const t = thiscall<void*>(getter, kStrings);
        set_text_of(m(off), t);
    }
    template <class W>
    void scale(W* M::*off) {
        joan::orig::as_widget(m(off))->scale_to(0, joan::orig::arg::f32(u32{0x3f800000}), 0.0f, 1, 0u);  // 0x0061f1b0
    }
    // Bottom bar button list: new list, place, text, 0x0061f1b0.
    template <class W>
    void button(W* M::*off, i32 x0, i32 x1, std::uintptr_t getter) {
        m(off) = static_cast<W*>(new_list());
        place(m(off), x0, 0x2c8, x1, 0x2e6, fn::code_value(fn::mark_draw_73e130));  // 0x0073e130
        text(off, getter);
        scale(off);
    }
    // Option label list at the left edge.
    // [Fixes] MouseOptions: a label whose text is ours, and the cursor row (slider + label) under the camera slider.
    template <class W>
    void label_text(W* M::*off, i32 y0, i32 y1, const char* t) {
        m(off) = static_cast<W*>(new_list());
        place(m(off), -612, y0, -100, y1, fn::code_value(fn::draw_unit_info));  // 0x0073e990
        set_text_of(m(off), const_cast<char*>(t));
    }
    void build_cursor_row() {
        void* const s = new_slider();
        mouse::cursor_slider() = s;
        init_slider(s, mouse::kCursorMax + 1);
        joan::orig::as_scroll_box(s)->set_value(joan::orig::arg::u32(mouse::cursor_position()));  // 0x00761240
        void* const l = new_list();
        mouse::cursor_label() = l;
        place(l, -612, 0x15b, -100, 0x193, fn::code_value(fn::draw_unit_info));  // 0x0073e990
        set_text_of(l, const_cast<char*>("Cursor Sensitivity"));
    }
    template <class W>
    void label(W* M::*off, i32 y0, i32 y1, std::uintptr_t getter) {
        m(off) = static_cast<W*>(new_list());
        place(m(off), -612, y0, -100, y1, fn::code_value(fn::draw_unit_info));  // 0x0073e990
        text(off, getter);
    }
    template <class W>
    void selector(W* M::*off, i32 x) {
        m(off) = static_cast<W*>(new_selector());
        joan::orig::as_combo_box(m(off))->init_options(joan::orig::arg::u32(x), joan::orig::arg::u32(i32{0x3e}), i32{2}, joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{1}));  // 0x006b29b0
    }
    template <class W>
    void option(W* M::*off, std::uintptr_t getter) {
        void* const s = thiscall<void*>(getter, kStrings);
        joan::orig::as_combo_box(m(off))->add_6b3930(joan::orig::arg::str(s));  // 0x006b3930
    }
    template <class W>
    u32 select(W* M::*off, i32 v) { return joan::orig::arg::as<u32>(joan::orig::as_popup(m(off))->set_text_6b44e0(joan::orig::arg::u32(v))); }  // 0x006b44e0
    static void init_slider(void* w, i32 range) {
        joan::orig::as_scroll_box(w)->init(joan::orig::arg::u32(i32{0x1c0}), joan::orig::arg::u32(i32{0x3e}), joan::orig::arg::i32(range), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{1}));  // 0x00760050
    }
    template <class W>
    void slider(W* M::*off, i32 range) {
        m(off) = static_cast<W*>(new_slider());
        init_slider(m(off), range);
    }
    template <class W>
    void set_slider(W* M::*off, i32 v) { joan::orig::as_scroll_box(m(off))->set_value(joan::orig::arg::u32(v)); }  // 0x00761240
    void* alloc_labels(i32 line_add) {
        return joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<u32>(sizeof(jo::ButtonArray10)))  /* 10 button pointers: 0x28 on x86 */, str(d::s_d_ww_interfac_optmenui_cpp), joan::orig::arg::i32(add32(static_cast<i32>(global<i16>(d::g_a2c920)), line_add)));  // 0x0058e7b0
    }
    void key_label(jo::ButtonArray10* M::*table, i32 i, i32 x0, i32 y0, i32 x1, i32 y1, std::uintptr_t callback) {
        void* const list = new_list();
        m(table)->buttons[i] = static_cast<jo::Button*>(list);
        place(m(table)->buttons[i], x0, y0, x1, y1, callback);
        const u32 colour = make_colour(0xff, 0xff, 0xff, 0xff);
        joan::orig::as_button(m(table)->buttons[i])->set_text(joan::orig::arg::str(obj(d::kEmpty)), obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(colour), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
    }

    H* opt_build() {
        c_opt_build.hit();
        button(&M::key_btn_dc, 0x1b5, 0x24b, fn::code_value(fn::OptMenu_open_33));  // 0x004db550
        button(&M::key_btn_e0, 0x296, 0x32c, fn::code_value(fn::Menu_load_optmenu));  // 0x004db670
        button(&M::key_btn_d8, 0xd4, 0x16a, fn::code_value(fn::Menu_optmenu_0b));   // 0x004db130

        {
            const Fpu fpu{true};
            const i32 t = ftol(fpu.add(fpu.mul(static_cast<double>(global<float>(d::g_a58027)), 10.0), 10.0));
            slider(&M::key_slider_c8, mouse::on() ? 0x1f : 0x15);  // [Fixes] MouseOptions: ten more steps at the low end
            set_slider(&M::key_slider_c8, mouse::on() ? t + 10 : t);
        }
        joan::orig::as_order(obj(d::g_player_unit_recno))->set_clamped(joan::orig::arg::f32(global<u32>(d::g_a58027)));  // 0x00432250

        if (mouse::on()) {
            label_text(&M::key_btn_cc, 0x127, 0x15f, "Camera Sensitivity");
            build_cursor_row();
        } else {
            label(&M::key_btn_cc, 0x127, 0x15f, fn::code_value(fn::OptMenu_open_10));  // 0x004db220
        }
        selector(&M::key_combo_b8, 0x1c0);
        option(&M::key_combo_b8, fn::code_value(fn::TxtRes_optmenu_2f));  // 0x004db7f0
        option(&M::key_combo_b8, fn::code_value(fn::OptMenu_open_30));  // 0x004db820
        label(&M::key_btn_bc, 0x15b + mouse::row_shift(), 0x193 + mouse::row_shift(), fn::code_value(fn::Menu_optmenu_11));  // 0x004db250
        select(&M::key_combo_b8, add32(static_cast<i32>(global<i8>(d::g_cfg_invert_look)), 1));

        selector(&M::key_combo_e4, 0x1c0);
        option(&M::key_combo_e4, fn::code_value(fn::TxtRes_optmenu_35));  // 0x004db8e0
        option(&M::key_combo_e4, fn::code_value(fn::OptMenu_open_36));  // 0x004db910
        label(&M::key_btn_e8, 0x1c3 + mouse::row_shift(), 0x1fb + mouse::row_shift(), fn::code_value(fn::OptMenu_open_34));  // 0x004db8b0
        select(&M::key_combo_e4, add32(static_cast<i32>(global<i8>(d::g_a5802c)), 1));

        slider(&M::key_slider_d0, 0xb4);
        set_slider(&M::key_slider_d0, ftol(static_cast<double>(global<float>(d::g_a5802e))));
        label(&M::key_btn_d4, 0x18f + mouse::row_shift(), 0x1c7 + mouse::row_shift(), fn::code_value(fn::Menu_optmenu_31));  // 0x004db850

        selector(&M::key_combo_c0, 0x1c0);
        option(&M::key_combo_c0, fn::code_value(fn::OptMenu_open_0c));  // 0x004db160
        option(&M::key_combo_c0, fn::code_value(fn::OptMenu_open_0d));  // 0x004db190
        label(&M::key_btn_c4, 0xf3, 0x12b, fn::code_value(fn::TxtRes_optmenu_0f));  // 0x004db1f0
        if (*static_cast<u32*>(global<void*>(d::g_keys_a51806)) != 0) option(&M::key_combo_c0, fn::code_value(fn::Menu_optmenu_0e));  // 0x004db1c0
        if (global<u32>(d::g_cfg_control_scheme) == 0) {
            select(&M::key_combo_c0, 1);
        } else if (global<u32>(d::g_cfg_control_scheme) == 3) {
            select(&M::key_combo_c0, 2);
        } else if (global<u32>(d::g_cfg_control_scheme) == 1) {
            if (*static_cast<u32*>(global<void*>(d::g_keys_a51806)) != 0) {
                select(&M::key_combo_c0, global<u32>(d::g_cfg_control_scheme) == 1 ? 3 : 1);
            } else {
                select(&M::key_combo_c0, 1);
            }
        }

        button(&M::credit_btn_f4, 0x1b5, 0x24b, fn::code_value(fn::OptMenu_open_33));  // 0x004db550
        button(&M::credit_btn_f8, 0xd4, 0x16a, fn::code_value(fn::OptMenu_open_1f));   // 0x004db4f0
        button(&M::credit_btn_fc, 0x296, 0x32c, fn::code_value(fn::Menu_optmenu_20));  // 0x004db520

        selector(&M::credit_combo_100, 0x74);
        option(&M::credit_combo_100, fn::code_value(fn::Menu_optmenu_28));  // 0x004db6a0
        option(&M::credit_combo_100, fn::code_value(fn::OptMenu_open_29));  // 0x004db6d0
        option(&M::credit_combo_100, fn::code_value(fn::TxtRes_optmenu_2a));  // 0x004db700
        option(&M::credit_combo_100, fn::code_value(fn::Menu_optmenu_2b));  // 0x004db730
        option(&M::credit_combo_100, fn::code_value(fn::OptMenu_open_2c));  // 0x004db760
        option(&M::credit_combo_100, fn::code_value(fn::Menu_optmenu_32));  // 0x004db880
        option(&M::credit_combo_100, fn::code_value(fn::OptMenu_open_38));  // 0x004db970
        select(&M::credit_combo_100, 1);

        jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_lines) = alloc_labels(0x88);
        for (i32 i = 0; i < 10; ++i) key_label(&M::credit_lines, i, 0x74, 0x46, 0x244, 0x64, fn::code_value(fn::draw_slot_button_73f490));  // 0x0073f490
        jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_labels) = alloc_labels(0x90);
        for (i32 i = 0; i < 10; ++i)
            key_label(&M::credit_labels, i, 0x400, add32(i * 0x28, 0x12c), 0x546, add32(i * 0x28, 0x154),
                      fn::code_value(fn::opt_mark_draw_73fde0));  // 0x0073fde0

        jo::self<jo::OptMenu>(this)->key_slot = -1;
        jo::self<jo::OptMenu>(this)->keys = nullptr;
        jo::view<u32>(jo::self<jo::OptMenu>(this)->key_count) = 0;
        return this;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(double) == 8 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b110_10", name, addr, fn)
B("OptMenu::build_controls_7435e0", 0x007435e0, &H::opt_build);
#undef B

}  // namespace game_b110_10
}  // namespace

// ---------------------------------------- from modules/game_b106_10.cpp (game/b106_10)
//
//      0x00744ce0 OptMenu::release()              (thiscall, ret 0) - destroys and deletes
//                 the 17 sub-objects at +0xb8..+0x100, the two 10-element pointer arrays at
//                 +0x104 / +0x108 (elements destroyed unconditionally, as in the original,
//                 then the array freed via 0x0058ed70 with optmenui.cpp / line global
//                 0x00a2c924 + 0x81 / 0x8c), frees +0x110 (line + 0x9e), clears +0x10c.
//                 Returns this (the eax the original leaves).
namespace {
namespace game_b106_10 {

using M = jo::OptMenu;

// The widget pointer at layout offset `off` (offsetof(OptMenu, member): every slot is a pointer member).
void*& widget_at(void* self, std::size_t off) { return *reinterpret_cast<void**>(static_cast<u8*>(self) + off); }

JOAN_COUNTER(c_optmenu_release, "OptMenu::release_744ce0");

struct H {
    // 0x00744ce0
    void* optmenu_release() {
        c_optmenu_release.hit();
        u8* self = reinterpret_cast<u8*>(this);
        struct Slot {
            std::size_t off;  // offsetof(OptMenu, <widget pointer>)
            std::uintptr_t destroy;  // code addresses (fn::code_value)
            std::uintptr_t del;
        };
        static const Slot kSlots[] = {
            {offsetof(M, key_btn_dc), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},  {offsetof(M, key_btn_e0), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},
            {offsetof(M, key_btn_d8), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},  {offsetof(M, key_combo_c0), fn::code_value(fn::ComboBox_free_parts), fn::code_value(fn::Obj741240_scalar_deleting_dtor)},
            {offsetof(M, key_btn_c4), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},  {offsetof(M, key_slider_c8), fn::code_value(fn::Panel_destroy_children), fn::code_value(fn::Obj_scalar_dtor_741290)},
            {offsetof(M, key_btn_cc), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},  {offsetof(M, key_slider_d0), fn::code_value(fn::Panel_destroy_children), fn::code_value(fn::Obj_scalar_dtor_741290)},
            {offsetof(M, key_btn_d4), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},  {offsetof(M, key_combo_b8), fn::code_value(fn::ComboBox_free_parts), fn::code_value(fn::Obj741240_scalar_deleting_dtor)},
            {offsetof(M, key_btn_bc), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},  {offsetof(M, key_combo_e4), fn::code_value(fn::ComboBox_free_parts), fn::code_value(fn::Obj741240_scalar_deleting_dtor)},
            {offsetof(M, key_btn_e8), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},  {offsetof(M, credit_btn_f4), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},
            {offsetof(M, credit_btn_f8), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},  {offsetof(M, credit_btn_fc), fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0)},
            {offsetof(M, credit_combo_100), fn::code_value(fn::ComboBox_free_parts), fn::code_value(fn::Obj741240_scalar_deleting_dtor)},
        };
        // thunks: 0x004072c0 -> 0x006217e0, 0x004055e2 -> 0x006b33a0 (deleting dtor),
        //         0x0040824c -> 0x006b3130, 0x00405f0b -> 0x00741240,
        //         0x004063c0 -> 0x00760440, 0x00409ddb -> 0x00741290
        for (const Slot& e : kSlots) {
            if (widget_at(self, e.off) == nullptr) continue;
            thiscall<u32>(e.destroy, widget_at(self, e.off));
            void* p = widget_at(self, e.off);
            if (p != nullptr) thiscall<u32>(e.del, p, 1);
            widget_at(self, e.off) = nullptr;
        }
        if (void*& w = mouse::cursor_slider(); w != nullptr) {  // [Fixes] MouseOptions
            thiscall<u32>(fn::code_value(fn::Panel_destroy_children), w);
            thiscall<u32>(fn::code_value(fn::Obj_scalar_dtor_741290), w, 1);
            w = nullptr;
        }
        if (void*& w = mouse::cursor_label(); w != nullptr) {
            thiscall<u32>(fn::code_value(fn::MenuButton_clear), w);
            thiscall<u32>(fn::code_value(fn::ComboBox_scalar_dtor_6b33a0), w, 1);
            w = nullptr;
        }
        release_array(self, &M::credit_lines, 0x81);
        release_array(self, &M::credit_labels, 0x8c);
        if (jo::view<void*>(jo::self<jo::OptMenu>(self)->keys) != nullptr) {
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::OptMenu>(self)->keys)), str(d::s_d_ww_interfac_optmenui_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a2c924)) + 0x9e));  // 0x0058ed70
            jo::view<void*>(jo::self<jo::OptMenu>(self)->keys) = nullptr;
        }
        jo::view<u32>(jo::self<jo::OptMenu>(self)->key_count) = 0;
        return this;
    }

  private:
    static void release_array(u8* self, jo::ButtonArray10* M::*off, i32 line) {
        if (jo::self<M>(self)->*off == nullptr) return;
        for (i32 i = 0; i < 10; ++i) {
            joan::orig::as_menu_button((jo::self<M>(self)->*off)->buttons[i])->clear();  // 0x006217e0
            void* p = (jo::self<M>(self)->*off)->buttons[i];
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b33a0(1u);  // 0x006b33a0
            (jo::self<M>(self)->*off)->buttons[i] = nullptr;
        }
        joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::self<M>(self)->*off), str(d::s_d_ww_interfac_optmenui_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a2c924)) + line));  // 0x0058ed70
        jo::self<M>(self)->*off = nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_10", name, addr, fn)
B("OptMenu::release", 0x00744ce0, &H::optmenu_release);
#undef B

}  // namespace game_b106_10
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
//
//    - OptMenu::draw_745720 0x00745720 (ret 4, interfac/optmenui.cpp, arg unused): returns this.
//
//  - 0x00745720: [ebp-8] is set only for selection 1..3 of the +0xc0 combobox (proven always
//    1..3): the ctor memsets +0x70..+0xac to 0 (0x6b2318); the list (mode 2, 0x6b29b0 arg 3) gets two
//    unconditional add_item calls (0x743f89 / 0x743fa2; the first sets +0x80 = 1 in mode 2) and
//    one conditional; the only other writer, set_selection 0x6b44e0, is called on it with
//    1 / 2 / 3 (0x7440c6..0x744134) or by the mode-2 click handler 0x6b4280 wrapping inside
//    1..count (<= 3); remove_item 0x6b3b90 is only used on another object (+0x20e8 at
//    0x752a85 / 0x753096). 0 used for the dead default.
namespace {
namespace game_b108_19 {

using M = jo::OptMenu;
using joan::orig::helpers::addr_of;

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), ret 0x10; returns this.
u32 make_color(u32 r, u32 g, u32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*jo::self<jo::Colour>(p));
}

JOAN_COUNTER(c_optdraw, "OptMenu::draw_745720");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x00745720
    std::uintptr_t opt_draw(u32 /*unused*/) {
        c_optdraw.hit();
        auto d = [&](fn::Code target, auto off) { thiscall<u32>(target, jo::self<M>(this)->*off, i32{1}, i32{7}); };
        auto btn = [&](jo::Button* M::*off) { joan::orig::as_button(jo::self<M>(this)->*off)->draw(1u, 7); };  // 0x00622cc0
        constexpr fn::Code kSlider = fn::HlBar_draw, kList = fn::SpellBtn_draw;  // 0x760b70 0x6b3ec0
        btn(&M::key_btn_dc);
        btn(&M::key_btn_e0);
        btn(&M::key_btn_d8);
        d(kSlider, &M::key_slider_c8);
        btn(&M::key_btn_cc);
        if (mouse::cursor_slider() != nullptr) {  // [Fixes] MouseOptions
            thiscall<u32>(kSlider, mouse::cursor_slider(), i32{1}, i32{7});
            joan::orig::as_button(mouse::cursor_label())->draw(1u, 7);
        }
        d(kSlider, &M::key_slider_d0);
        btn(&M::key_btn_d4);
        d(kList, &M::key_combo_c0);
        btn(&M::key_btn_c4);
        d(kList, &M::key_combo_b8);
        btn(&M::key_btn_bc);
        d(kList, &M::key_combo_e4);
        btn(&M::key_btn_e8);
        btn(&M::credit_btn_f4);
        btn(&M::credit_btn_f8);
        btn(&M::credit_btn_fc);
        d(kList, &M::credit_combo_100);
        // The set the page shows: the input method's, or the one its Keyboard / Gamepad button picked (remap_set.hpp).
        const i32 mode = joan::optmenu::remap::mode(this);
        for (i32 i = 0; i < jo::self<jo::OptMenu>(this)->key_count; ++i) {
            joan::orig::as_button(jo::self<jo::OptMenu>(this)->credit_lines->buttons[i])->draw(1u, 7);  // 0x00622cc0
            joan::orig::as_button(jo::self<jo::OptMenu>(this)->credit_labels->buttons[i])->draw(1u, 7);  // 0x00622cc0
            if (jo::self<jo::OptMenu>(this)->page != 5) continue;
            u32 color = make_color(0xff, 0xff, 0xff, 0xff);
            const char* text = joan::orig::arg::as<const char*>(joan::orig::as_race_res(obj(d::g_keys_a48820))->record(joan::orig::arg::i32(jo::self<jo::OptMenu>(this)->keys->ids[i])));  // 0x004cb580
            joan::orig::as_button(jo::self<jo::OptMenu>(this)->credit_lines->buttons[i])->set_text(joan::orig::arg::str(text), img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(color), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
            if (jo::self<jo::OptMenu>(this)->key_slot == i) {
                color = make_color(0xff, 0xff, 0xff, 0xff);
                text = joan::orig::as_opt_menu(obj(d::g_strings))->open_2d();  // 0x004db790
            } else {
                color = make_color(0xff, 0xff, 0xff, 0xff);
                text = joan::orig::arg::as<const char*>(joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(mode), joan::orig::arg::i32(jo::self<jo::OptMenu>(this)->keys->ids[i])));  // 0x004cb5b0
            }
            joan::orig::as_button(jo::self<jo::OptMenu>(this)->credit_labels->buttons[i])->set_text(joan::orig::arg::str(text), img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(color), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});
        }
        return addr_of(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("OptMenu::draw_745720", 0x00745720, &H::opt_draw);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b109_05.cpp (game/b109_05)
//
//    - OptMenu::handle_input 0x00745b10 (ret 0, returns al 0/1; caller 0x0073da5f ignores eax,
//      interfac/optmenui.cpp): enables / disables the five sliders and selectors from the
//      mouse-in-rect tests, key binding capture while a slot is selected (+0x114 >= 0), else the
//      button / selector dispatch for page 3 (game options) and page 5 (key bindings).
//
//  - 0x00745b10: the "device" local [ebp-8] is set only for selector values 1 / 2 / 3 of the
//    widget at +0xc0 (mode 2 selector, 2-3 options: the ctor memsets it, the first option add
//    sets the value to 1, every 0x006b44e0 call on it passes 1..3, the arrows wrap inside
//    1..count). The uninitialised case cannot happen; it is given 0 deterministically.
//    Index -1 reads ([+0x110][-1] after a clear in the same pass) are reproduced as in the
//    original. Float stores: fild / fdiv 10.0 at PC24 via Fpu{true}; fild -> float store is one
//    rounding (static_cast<float> of the int); fld * 10.0f + 10.0f are float ops -> ftol.
namespace {
namespace game_b109_05 {

using u16 = std::uint16_t;

using M = jo::OptMenu;

// The widget pointer at layout offset `off` (offsetof(OptMenu, member): the hover table mixes widget classes).
void* widget_at(const void* self, std::size_t off) {
    return *reinterpret_cast<void* const*>(static_cast<const u8*>(self) + off);
}

JOAN_COUNTER(c_opt_input, "OptMenu::handle_input");

// ---------------------------------------------------------------------------------- 0x00745b10
constexpr img::Addr kInput = d::g_mouse;
constexpr img::Addr kBind = d::g_keys_a48820;

struct H {
    // ------------------------------------------------------------------------------ 0x00745b10
    // Mouse-in-rect test of widget `w` (0x00593980(0, y, 0x400, h)) as 0 / 1.
    u32 hover(std::size_t w) { return hover_widget(widget_at(this, w)); }
    u32 hover_widget(void* const o) {
        return joan::orig::arg::as<i32>(joan::orig::as_point(obj(kInput))->inside(i32{0}, static_cast<i32>(jo::self<jo::Button>(o)->y), i32{0x400}, static_cast<i32>(jo::self<jo::Button>(o)->y2))) != 0
                   ? 1u
                   : 0u;
    }
    i32 slot() { return static_cast<i32>(jo::self<jo::OptMenu>(this)->key_slot); }
    jo::Button* slot_button(jo::ButtonArray10* M::*table) { return (jo::self<M>(this)->*table)->buttons[slot()]; }
    // Deselects the current key slot (both labels) and sets the slot to -1.
    void clear_slot() {
        slot_button(&M::credit_lines)->held = 0;
        slot_button(&M::credit_labels)->held = 0;
        jo::self<jo::OptMenu>(this)->key_slot = -1;
    }
    // 0x004cba40(dev, binding, a, b) on the binding table, then clear.
    void bind(u32 dev, u32 a, u32 b) {
        joan::orig::as_bindings(obj(kBind))->assign(joan::orig::arg::i32(dev), joan::orig::arg::i32(jo::self<M>(this)->keys->ids[slot()]), joan::orig::arg::i32(a), joan::orig::arg::i32(b));
        clear_slot();
    }
    static bool pressed(u32 mask) {
        return (static_cast<u32>(global<u16>(d::g_a51802)) & mask) == 0 &&
               (static_cast<u32>(global<u16>(d::g_key_modifiers)) & mask) != 0;
    }
    // Key part shared by the three devices: d5a scan code, else Shift / Ctrl / Alt edges.
    void bind_key_or_modifier(u32 dev) {
        if (global<u32>(d::g_last_key_scancode) != 0) {
            bind(dev, 0, global<u32>(d::g_last_key_scancode));
        } else if (pressed(0x3)) {
            bind(3, 0, 0x2a);
        } else if (pressed(0xc)) {
            bind(3, 0, 0x1d);
        } else if (pressed(0x30)) {
            bind(3, 0, 0x38);
        }
    }
    bool click(jo::Button* M::*w, u32 first) {
        return joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::self<M>(this)->*w)->mouse(joan::orig::arg::i32(first), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(~u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}))) != 0;  // 0x006218c0
    }
    void select_page() {
        const u32 v = jo::view<u32>(jo::self<jo::OptMenu>(this)->credit_combo_100->selected) - 1u;
        joan::orig::as_opt_menu(this)->list_keys(static_cast<std::int8_t>(joan::orig::arg::i32(v <= 6 ? v : 0u)));  // 0x007478a0
    }
    u8 opt_input() {
        c_opt_input.hit();
        // Hover enables: the label byte of the partner widget, then the widget's own state.
        struct Pair {
            std::size_t widget;  // offsetof(OptMenu, <selector / slider pointer>)
            jo::Button* M::*label;
        };
        static const Pair kPairs[5] = {{offsetof(M, key_combo_c0), &M::key_btn_c4}, {offsetof(M, key_slider_c8), &M::key_btn_cc},
                                       {offsetof(M, key_combo_b8), &M::key_btn_bc}, {offsetof(M, key_combo_e4), &M::key_btn_e8},
                                       {offsetof(M, key_slider_d0), &M::key_btn_d4}};
        static const std::uintptr_t kSet[5] = {fn::code_value(fn::Obj6b3_set_flag), fn::code_value(fn::Obj760_set_flag), fn::code_value(fn::Obj6b3_set_flag), fn::code_value(fn::Obj6b3_set_flag), fn::code_value(fn::Obj760_set_flag)};
        for (i32 k = 0; k < 5; ++k) {
            const u8 on = static_cast<u8>(hover(kPairs[k].widget));
            (jo::self<M>(this)->*kPairs[k].label)->held = on;
            const u32 again = hover(kPairs[k].widget);
            thiscall<u32>(kSet[k], widget_at(this, kPairs[k].widget), again);  // 0x006b3e50 / 0x00760b00
        }
        if (void* const w = mouse::cursor_slider(); w != nullptr) {  // [Fixes] MouseOptions
            static_cast<jo::Button*>(mouse::cursor_label())->held = static_cast<u8>(hover_widget(w));
            thiscall<u32>(fn::code_value(fn::Obj760_set_flag), w, hover_widget(w));  // 0x00760b00
        }
        // [ebp-8]: device from the selector at +0xc0 (always 1..3, see the header).
        // The set the key bindings page edits (remap_set.hpp; the original: the input method of INPUT OPTIONS).
        const i32 device = joan::optmenu::remap::mode(this);
        // Binding a pad button: the pad stays off the menu meanwhile (its A / B / D-pad would click, go back and hop),
        // and only a button pressed after the slot was picked counts (A, which picked it, is still down).
        static u32 held_buttons = 0;
        static i32 held_for = -1;
        if (slot() >= 0 && device == 1) {
            joan::platform::input::pad_menu_hold();
            if (held_for != slot()) {
                held_buttons = 0;
                if (*global<const u32*>(d::g_keys_a51806) != 0)
                    for (i32 i = 0; i < 0xe; ++i)
                        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_ai(obj(d::g_keys_a51806))->is_ready(joan::orig::arg::i32(i)))) != 0) held_buttons |= 1u << i;  // 0x00597ed0
            }
        }
        held_for = slot();
        if (slot() >= 0) {
            if (global<i32>(d::g_key) == 0x1b) {
                clear_slot();
                return 1;
            }
            if (device == 0) {
                bind_key_or_modifier(0);
            } else if (device == 1) {
                if (*global<const u32*>(d::g_keys_a51806) != 0) {
                    for (i32 i = 0; i < 0xe; ++i) {
                        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_ai(obj(d::g_keys_a51806))->is_ready(joan::orig::arg::i32(i)))) == 0) {  // 0x00597ed0
                            held_buttons &= ~(1u << i);
                        } else if ((held_buttons & (1u << i)) == 0) {
                            bind(1, 1, static_cast<u32>(i));
                            break;
                        }
                    }
                    if (slot() < 0) return 1;
                }
                bind_key_or_modifier(1);
            } else if (device == 3) {
                bind_key_or_modifier(3);
                if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(kInput))->value(i32{0})) != 0 && jo::self<jo::OptMenu>(this)->credit_combo_100->selected == 2) {  // 0x00594700
                    bind(3, 2, 0);
                }
                if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(kInput))->value(i32{1})) != 0 && jo::self<jo::OptMenu>(this)->credit_combo_100->selected == 2) {
                    bind(3, 2, 1);
                }
            }
            return 1;
        }
        const i32 page = static_cast<i32>(jo::self<jo::OptMenu>(this)->page);
        if (page == 3) {
            if (click(&M::key_btn_d8, 0)) {
                joan::optmenu::remap::reset(this);  // the page opens on the input method's set (remap_set.hpp)
                select_page();
                joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(i32{5})));  // 0x0073cc00
                return 1;
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_menu(jo::self<M>(this)->key_combo_c0)->step_6b4280()) != 0) return 1;  // 0x006b4280
            if (joan::orig::arg::as<i32>(joan::orig::as_slider(jo::self<M>(this)->key_slider_c8)->step_760df0()) != 0) {           // 0x00760df0
                const i32 v = joan::orig::arg::as<i32>(joan::orig::as_scroll_box(jo::self<M>(this)->key_slider_c8)->value()) - (mouse::on() ? 20 : 10);  // 0x00761270
                global<float>(d::g_a58027) = static_cast<float>(Fpu{true}.div(static_cast<double>(v), 10.0));
                joan::orig::as_order(obj(d::g_player_unit_recno))->set_clamped(joan::orig::arg::f32(global<u32>(d::g_a58027)));  // 0x00432250
                return 1;
            }
            if (void* const w = mouse::cursor_slider();
                w != nullptr && joan::orig::arg::as<i32>(joan::orig::as_slider(w)->step_760df0()) != 0) {  // [Fixes] MouseOptions
                mouse::set_cursor_position(joan::orig::arg::as<i32>(joan::orig::as_scroll_box(w)->value()));  // 0x00761270
                return 1;
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_slider(jo::self<M>(this)->key_slider_d0)->step_760df0()) != 0) {
                global<float>(d::g_a5802e) = static_cast<float>(joan::orig::arg::as<i32>(joan::orig::as_scroll_box(jo::self<M>(this)->key_slider_d0)->value()));
                return 1;
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_menu(jo::self<M>(this)->key_combo_b8)->step_6b4280()) != 0) {
                global<u8>(d::g_cfg_invert_look) = static_cast<u8>(jo::view<u32>(jo::self<jo::OptMenu>(this)->key_combo_b8->selected) - 1u);
                return 1;
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_menu(jo::self<M>(this)->key_combo_e4)->step_6b4280()) != 0) {
                global<u8>(d::g_a5802c) = static_cast<u8>(jo::view<u32>(jo::self<jo::OptMenu>(this)->key_combo_e4->selected) - 1u);
                return 1;
            }
            if (click(&M::key_btn_dc, 0)) {  // defaults
                global<u32>(d::g_cfg_control_scheme) = 3;
                joan::orig::as_popup(jo::self<M>(this)->key_combo_c0)->set_text_6b44e0(joan::orig::arg::u32(global<u32>(d::g_cfg_control_scheme) - 1u));  // 0x006b44e0
                global<u32>(d::g_a58027) = 0;
                global<u32>(d::g_a5802e) = 0x42b40000u;  // 90.0f
                joan::orig::as_order(obj(d::g_player_unit_recno))->set_clamped(joan::orig::arg::f32(global<u32>(d::g_a58027)));
                const Fpu fpu{true};
                const double t = fpu.add(fpu.mul(static_cast<double>(global<float>(d::g_a58027)), 10.0), 10.0);
                joan::orig::as_scroll_box(jo::self<M>(this)->key_slider_c8)->set_value(joan::orig::arg::u32(joan::hook::x87::ftol(t) + (mouse::on() ? 10 : 0)));  // 0x00761240
                if (void* const w = mouse::cursor_slider(); w != nullptr) {  // [Fixes] MouseOptions: the original's speed
                    mouse::set_cursor_position(mouse::kCursorDefault);
                    joan::orig::as_scroll_box(w)->set_value(joan::orig::arg::u32(mouse::kCursorDefault));
                }
                joan::orig::as_scroll_box(jo::self<M>(this)->key_slider_d0)->set_value(joan::orig::arg::u32(i32{0x5a}));
                global<u8>(d::g_cfg_invert_look) = 0;
                joan::orig::as_popup(jo::self<M>(this)->key_combo_b8)->set_text_6b44e0(joan::orig::arg::u32(i32{1}));
                joan::orig::as_popup(jo::self<M>(this)->key_combo_e4)->set_text_6b44e0(joan::orig::arg::u32(i32{1}));
            }
            if (click(&M::key_btn_e0, 0x1b)) {
                joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(i32{0})));
                return 1;
            }
            return 0;
        }
        if (page == 5) {
            for (i32 i = 0; i < jo::self<jo::OptMenu>(this)->key_count; ++i) {
                void* const b = jo::self<M>(this)->credit_lines->buttons[i];
                if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(b)->mouse(joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(~u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}))) != 0) {
                    jo::view<u8>(jo::self<jo::OptMenu>(this)->key_slot) = static_cast<u8>(i);
                    jo::self<M>(this)->credit_lines->buttons[i]->held = 1;
                    jo::self<M>(this)->credit_labels->buttons[i]->held = 1;
                    return 1;
                }
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_menu(jo::self<M>(this)->credit_combo_100)->step_6b4280()) != 0) {
                select_page();
                return 1;
            }
            if (joan::optmenu::remap::input(this)) {  // Keyboard / Gamepad switched: the category's keys of that set
                select_page();
                return 1;
            }
            if (click(&M::credit_btn_f8, 0)) {
                joan::orig::as_bindings(obj(kBind))->save_snapshot();  // 0x004cc380
                joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(i32{3})));
                return 1;
            }
            if (click(&M::credit_btn_f4, 0)) {
                joan::orig::as_bindings(obj(kBind))->restore(joan::orig::arg::i32(device));  // 0x004cb410
                return 1;
            }
            if (click(&M::credit_btn_fc, 0x1b)) {
                joan::orig::as_bindings(obj(kBind))->load();  // 0x004cc220
                joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(i32{3})));
                return 1;
            }
        }
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_05", name, addr, fn)
B("OptMenu::handle_input_745b10", 0x00745b10, &H::opt_input);
#undef B

}  // namespace game_b109_05
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
//
//    - 0x00746ff0 OptMenu::release_all   (thiscall, ret 0; interfac/optmenui.cpp) -> the eax the
//      original leaves (10 = loop counter, this, or 0x004ceef0).
//
//  - 0x00746ff0: the 10-step loop increments its counter through eax, so the eax left for the
//    no-slide paths is 10 (or `this`), not the last 0x0061f3b0 result (the caller ignores it).
namespace {
namespace game_b108_16 {

using M = jo::OptMenu;

// The widget pointer at layout offset `off` (offsetof(OptMenu, member): the list mixes widget classes).
void* widget_at(const void* self, std::size_t off) {
    return *reinterpret_cast<void* const*>(static_cast<const u8*>(self) + off);
}

JOAN_COUNTER(c_opt_release, "OptMenu::release_all");

u32 release_61f3b0(void* p) { return joan::orig::arg::as<u32>(joan::orig::as_widget(p)->update()); }  // 0x0061f3b0

struct H {
    // --------------------------------------------------------------------- 0x00746ff0
    std::uintptr_t opt_release_all() {
        c_opt_release.hit();
        const u32 v8 = jo::view<u32>(jo::self<jo::OptMenu>(this)->key_btn_dc->scale_arg);
        const u32 vc = jo::view<u32>(jo::self<jo::OptMenu>(this)->credit_lines->buttons[0]->move_arg);
        const std::size_t offs[17] = {offsetof(M, key_btn_dc),    offsetof(M, key_btn_e0),       offsetof(M, key_btn_d8),
                                      offsetof(M, key_slider_c8), offsetof(M, key_btn_cc),       offsetof(M, key_slider_d0),
                                      offsetof(M, key_btn_d4),    offsetof(M, key_combo_c0),     offsetof(M, key_btn_c4),
                                      offsetof(M, key_combo_b8),  offsetof(M, key_btn_bc),       offsetof(M, key_combo_e4),
                                      offsetof(M, key_btn_e8),    offsetof(M, credit_btn_f4),    offsetof(M, credit_btn_f8),
                                      offsetof(M, credit_btn_fc), offsetof(M, credit_combo_100)};
        for (i32 k = 0; k < 17; ++k) release_61f3b0(widget_at(this, offs[k]));
        if (mouse::cursor_slider() != nullptr) {  // [Fixes] MouseOptions
            release_61f3b0(mouse::cursor_slider());
            release_61f3b0(mouse::cursor_label());
        }
        i32 i = 0;
        for (; i < 10; ++i) {
            release_61f3b0(jo::view<void**>(jo::self<jo::OptMenu>(this)->credit_lines)[i]);
            release_61f3b0(jo::view<void**>(jo::self<jo::OptMenu>(this)->credit_labels)[i]);
        }
        // The loop counter is incremented through eax, so eax = 10 when the loop exits.
        std::uintptr_t r = static_cast<u32>(i);
        bool slide = false;
        if (v8 != 0 && jo::view<u32>(jo::self<jo::OptMenu>(this)->key_btn_dc->scale_arg) == 0) {
            slide = true;
        } else if (vc != 0) {
            r = reinterpret_cast<std::uintptr_t>(this);
            if (jo::view<u32>(jo::self<jo::OptMenu>(this)->credit_lines->buttons[0]->move_arg) == 0) slide = true;
        }
        if (slide) r = joan::orig::arg::as<std::uintptr_t>(joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_BUTTON_SLIDE_MANY)));  // 0x004ceef0 "BUTTON_SLIDE_MANY"
        return r;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("OptMenu::release_all", 0x00746ff0, &H::opt_release_all);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b104_15.cpp (game/b104_15)
//
//      0x00747200 OptMenu::layout_747200          (thiscall, ret 0) - returns last 0x0061f1b0 eax.
namespace {
namespace game_b104_15 {

using M = jo::OptMenu;

JOAN_COUNTER(c_layout_747200, "OptMenu::layout_747200");

// Optional offset on the light / options screens: -0x30 in menu mode 9.
u32 menu_dy() { return global<i8>(d::g_game_mode) == 9 ? 0xffffffd0u : 0u; }

struct H {
    // 0x0061e020 / 0x0061f1b0 on the widget pointer member `off`.
    template <class W>
    u32 m6(W* M::*off, u32 a, u32 b, u32 c, u32 d, u32 e, u32 g) {
        joan::orig::as_widget(jo::self<M>(this)->*off)->move_to_61e020(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(g));  // 0x0061e020
        return 0;  // move_to returns nothing; the layout results are never read
    }
    template <class W>
    u32 f5(W* M::*off, u32 a, u32 b, u32 c, u32 d, u32 e) {
        joan::orig::as_widget(jo::self<M>(this)->*off)->scale_to_61f1b0(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
        return 0;  // scale_to returns nothing; the layout results are never read
    }

    // ------------------------------------------------------------------------ 0x00747200
    u32 layout_747200() {
        c_layout_747200.hit();
        const u32 d = menu_dy();
        u32 r;
        r = m6(&M::key_combo_c0, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_combo_c0, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_btn_c4, 0xfffffd9cu, u32(d + 0xf3), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_btn_c4, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_slider_c8, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_slider_c8, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_btn_cc, 0xfffffd9cu, u32(d + 0x127), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_btn_cc, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        if (void* const w = mouse::cursor_slider(); w != nullptr) {  // [Fixes] MouseOptions
            widget_move(w, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
            widget_scale(w, 0u, 0x3f800000u, 0u, 0x1u, 0u);
            widget_move(mouse::cursor_label(), 0xfffffd9cu, u32(d + 0x15b), 0x3f800000u, 0u, 0x1u, 0u);
            widget_scale(mouse::cursor_label(), 0u, 0x3f800000u, 0u, 0x1u, 0u);
        }
        r = m6(&M::key_combo_b8, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_combo_b8, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_btn_bc, 0xfffffd9cu, u32(d + 0x15b + mouse::row_shift()), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_btn_bc, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_combo_e4, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_combo_e4, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_btn_e8, 0xfffffd9cu, u32(d + 0x1c3 + mouse::row_shift()), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_btn_e8, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_slider_d0, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_slider_d0, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_btn_d4, 0xfffffd9cu, u32(d + 0x18f + mouse::row_shift()), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_btn_d4, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_btn_e0, 0x296u, u32(d + 0x2c8), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_btn_e0, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_btn_dc, 0x1b5u, u32(d + 0x2c8), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_btn_dc, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::key_btn_d8, 0xd4u, u32(d + 0x2c8), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::key_btn_d8, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::credit_btn_f4, 0x1b5u, u32(d + 0x2c8), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::credit_btn_f4, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::credit_btn_f8, 0xd4u, u32(d + 0x2c8), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::credit_btn_f8, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::credit_btn_fc, 0x296u, u32(d + 0x2c8), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::credit_btn_fc, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::credit_combo_100, 0x74u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::credit_combo_100, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        for (u32 i = 0; i < 10; ++i) {
            joan::orig::as_widget(jo::self<M>(this)->credit_lines->buttons[i])->move_to_61e020(joan::orig::arg::i16(0x74u), joan::orig::arg::i16(u32(d + 0x46)), joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), static_cast<char>(0x1u), joan::orig::arg::u32(0u));  // 0x0061e020
            joan::orig::as_widget(jo::self<M>(this)->credit_lines->buttons[i])->scale_to_61f1b0(joan::orig::arg::i16(0u), joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), static_cast<char>(0x1u), joan::orig::arg::u32(0u));  // 0x0061f1b0
            joan::orig::as_widget(jo::self<M>(this)->credit_labels->buttons[i])->move_to_61e020(joan::orig::arg::i16(0x400u), joan::orig::arg::i16(u32(d + (i * 0x28) + 0x12c)), joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), static_cast<char>(0x1u), joan::orig::arg::u32(0u));  // 0x0061e020
            joan::orig::as_widget(jo::self<M>(this)->credit_labels->buttons[i])->scale_to_61f1b0(joan::orig::arg::i16(0u), joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), static_cast<char>(0x1u), joan::orig::arg::u32(0u));  // 0x0061f1b0
        }
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_15", name, addr, fn)
B("OptMenu::layout_747200", 0x00747200, &H::layout_747200);
#undef B

}  // namespace game_b104_15
}  // namespace

// ---------------------------------------- from modules/game_b111_17.cpp (fix/uninit)
//
//    - OptionsMenu::list_keys 0x007478a0 (thiscall, ret 4, eax = final loop index): counts the
//      bindable key names in the range of the category arg, (re)allocates +0x110 (count dwords)
//      and stores the key indices. The device mode comes from the selector +0xc0 (+0x80:
//      1 -> 0, 2 -> 3, 3 -> 1); any other selection leaves [ebp-0xc] uninitialised and passes
//      it to 0x004cb5b0. The fix uses 0 - the mode the menu setup (0x007440b2) selects for an
//      unknown control type (it maps an unknown [0xa58023] to selection 1, i.e. mode 0).
namespace {
namespace game_b111_17 {


JOAN_COUNTER(c_list_keys, "OptionsMenu::list_keys");

struct H {
    // -------------------------------------------------------------- 0x007478a0
    bool key_listed(i32 mode, i32 key) {
        const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(mode), joan::orig::arg::i32(key)));  // 0x004cb5b0
        if (name != nullptr && joan::crt::stricmp(joan::orig::arg::str(name), str(d::s_Not_Available)) == 0) return false;  // 'Not Available'
        return joan::crt::stricmp(joan::orig::arg::str(name), str(d::s_Escape)) != 0;  // 'Escape'
    }
    u32 list_keys(i8 category) {
        c_list_keys.hit();
        // The set the page shows (remap_set.hpp; the original: the input method of INPUT OPTIONS, fix/uninit 0 for
        // a selection other than 1..3).
        const i32 mode = joan::optmenu::remap::mode(this);
        i32 lo = 1;
        i32 hi = 0;
        switch (static_cast<u32>(static_cast<i32>(category))) {
            case 0: lo = 1; hi = 0xa; break;
            case 1: lo = 0xb; hi = 0x15; break;
            case 2: lo = 0x16; hi = 0x20; break;
            case 3: lo = 0x23; hi = 0x29; break;
            case 4: lo = 0x2a; hi = 0x2e; break;
            case 5: lo = 0x2f; hi = 0x33; break;
            case 6: lo = 0x35; hi = 0x37; break;
            default: lo = 1; hi = global<i32>(d::g_a48830); break;
        }
        i16 count = 0;
        for (i32 j = lo; j <= hi; ++j)
            if (key_listed(mode, j)) count = static_cast<i16>(count + 1);
        if (jo::self<jo::OptMenu>(this)->keys != nullptr) {
            jo::view<u32>(jo::self<jo::OptMenu>(this)->key_count) = 0;
            joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(jo::self<jo::OptMenu>(this)->keys), str(d::s_d_ww_interfac_optmenui_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a2c928)) + 0x43));  // 0x0058ed70
            jo::self<jo::OptMenu>(this)->keys = nullptr;
        }
        jo::self<jo::OptMenu>(this)->keys = joan::orig::arg::as<jo::KeyIdArray10*>(joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(static_cast<i32>(count) << 2), str(d::s_d_ww_interfac_optmenui_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a2c928)) + 0x47)));  // 0x0058e7b0
        jo::self<jo::OptMenu>(this)->key_count = count;
        i16 k = 0;
        i32 j = lo;
        for (; j <= hi; ++j) {
            if (!key_listed(mode, j)) continue;
            jo::view<i32>(jo::self<jo::OptMenu>(this)->keys->ids[static_cast<i32>(k)]) = j;
            k = static_cast<i16>(k + 1);
        }
        return static_cast<u32>(j);
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: device mode 0 for a selector value outside 1..3 (garbage [ebp-0xc] originally).
FIX("OptionsMenu::list_keys", 0x007478a0, &H::list_keys);
#undef FIX

}  // namespace game_b111_17
}  // namespace

// ---------------------------------------- from modules/game_b109_05.cpp (game/b109_05)
//
//    - OptPanel::init 0x0073ba80 (ret 0, void: caller 0x0046f687 ignores eax): frame / long bars,
//      four OPTMENU buttons + one with callback 0x00405dda, then nine layout calls.
namespace {
namespace game_b109_05_2 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;
using M = jo::OptMenu;

void* new_(u32 size) { return joan::crt::operator_new(joan::orig::arg::u32(size)); }  // operator new

JOAN_COUNTER(c_opt_panel_init, "OptPanel::init");

// ---------------------------------------------------------------------------------- 0x00721a60
void move(void* o, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) {
    joan::orig::as_widget(o)->move_to_61e020(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(f));  // 0x0061e020
}
void anim(void* o, u32 a, u32 b, u32 c, u32 d, u32 e) {
    joan::orig::as_widget(o)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
}
constexpr u32 kOne = 0x3f800000u;    // 1.0f

// ---------------------------------------------------------------------------------- 0x0073ba80
// Colour ctor 0x0042a260(a, b, c, d) on a local, the 4 bytes are passed on by value.
u32 colour(u32 a, u32 b, u32 c, u32 d) {
    u32 local = 0;
    const u32* const r = joan::orig::arg::as<const u32*>(joan::orig::as_colour(&local)->construct(a, b, c, d));  // 0x0042a260
    return *r;
}
void* new_obj(u32 size, std::uintptr_t ctor) {
    void* const mem = new_(size);
    return mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
}
void bar(void* self, jo::Image2D* M::*field, u32 x, u32 y, img::Addr name, u32 mx, u32 my) {
    jo::self<M>(self)->*field = static_cast<jo::Image2D*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::Image2D)), fn::code_value(fn::Widget629_ctor)));  // 0x00629400
    const u32 c = colour(0, 0, 0, 0xff);
    joan::orig::as_label(jo::self<M>(self)->*field)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::mptr(jo::self<M>(self)->u32_18[3]), joan::orig::arg::str(str(name)), joan::orig::arg::i32(c), joan::orig::arg::i32(u32{3}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{2}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(~u32{0}), joan::orig::arg::i32(~u32{0}), joan::orig::arg::i32(~u32{0}), joan::orig::arg::i32(~u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(~u32{0}), joan::orig::arg::i32(~u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(u32{1}));                                                   // 0x00629660
    move(jo::self<M>(self)->*field, mx, my, kOne, 0, 1, 0);
}
void button(void* self, jo::Button* M::*field, u32 x, u32 y, u32 w, u32 h, std::uintptr_t cb, std::uintptr_t label_thunk) {
    jo::self<M>(self)->*field = static_cast<jo::Button*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0)));  // 0x00620ee0
    joan::orig::as_button(jo::self<M>(self)->*field)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), cb, joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(u32{1}));                                                   // 0x00621540
    const u32 c = colour(0xff, 0xff, 0xff, 0xff);
    const std::uintptr_t label = thiscall<std::uintptr_t>(label_thunk, obj(d::g_strings));  // the label getter returns a char*
    joan::orig::as_button(jo::self<M>(self)->*field)->set_text(joan::orig::arg::str(label), img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(c), joan::orig::arg::i32(u32{1}), joan::orig::arg::i32(u32{2}), joan::orig::arg::i32(u32{0}), joan::orig::arg::i32(u32{2}), joan::orig::arg::i32(u32{2}));  // 0x00623ce0
    anim(jo::self<M>(self)->*field, 0, kOne, 0, 1, 0);
}

struct H {
    // ------------------------------------------------------------------------------ 0x0073ba80
    void opt_panel_init() {
        c_opt_panel_init.hit();
        jo::self<jo::OptPanel>(this)->u32_18[0] = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_optmenu));
        jo::self<jo::OptPanel>(this)->images[0] = 0;
        jo::self<jo::OptPanel>(this)->u32_18[1] = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_optmenuf));
        jo::self<jo::OptPanel>(this)->images[1] = 0;
        jo::self<jo::OptPanel>(this)->u32_18[2] = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_optmenub));
        jo::self<jo::OptPanel>(this)->images[2] = 0;
        jo::self<jo::OptPanel>(this)->u32_18[3] = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_commele2));
        jo::self<jo::OptPanel>(this)->images[3] = 0;
        jo::self<jo::OptPanel>(this)->images[4] = 0;
        jo::self<jo::OptPanel>(this)->images[5] = 0;
        bar(this, &M::title, 0x1cc, 0x6c, d::s_FRAME, 0x1cc, 0xfffffe40u);              // FRAME
        bar(this, &M::frame_top, 0, 0xb4, d::s_LONGBAR, 0xfffffc00u, 0xb4);            // LONGBAR
        bar(this, &M::frame_bottom, 0, 0x2c8, d::s_LONGBAR, 0x400, 0x2c8);                 // LONGBAR
        button(this, &M::tab_button_0, 0x100, 0x3e, 0x300, 0x7e, fn::code_value(fn::draw_cs_73ee40), fn::code_value(fn::OptMenu_open_23));  // 0x0073ee40, 0x004db5b0
        button(this, &M::tab_button_1, 0x100, 0x3e, 0x300, 0x7e, fn::code_value(fn::draw_cs_73ee40), fn::code_value(fn::OptMenu_open_24));  // 0x004db5e0
        button(this, &M::tab_button_2, 0x100, 0x3e, 0x300, 0x7e, fn::code_value(fn::draw_cs_73ee40), fn::code_value(fn::OptMenu_open_25));  // 0x004db610
        button(this, &M::tab_button_3, 0x100, 0x3e, 0x300, 0x7e, fn::code_value(fn::draw_cs_73ee40), fn::code_value(fn::OptMenu_open_26));  // 0x004db640
        button(this, &M::tab_button_4, 0x1ce, 0x2c8, 0x232, 0x2e6, fn::code_value(fn::mark_draw_73e130), fn::code_value(fn::Menu_load_optmenu));  // 0x0073e130, 0x004db670
        jo::view<u8>(jo::self<jo::OptPanel>(this)->page) = 1;
        joan::orig::as_opt_menu(this)->build_buttons();  // 0x00748b10
        joan::orig::as_opt_menu(this)->build_controls_742470();  // 0x00742470
        joan::orig::as_opt_menu(this)->build_controls_7435e0();  // 0x007435e0
        joan::orig::as_menu(this)->create_controls_740440();  // 0x00740440
        joan::optmenu::graphics::create(this);  // GRAPHICS OPTIONS and the openjoan.ini rows (graphics_page.hpp)
        joan::optmenu::remap::create(this);     // the key bindings page's Keyboard / Gamepad button (remap_set.hpp)
        joan::orig::as_panel(this)->slide_in_74c2e0();  // 0x0074c2e0
        joan::orig::as_slider(this)->open();  // 0x007433d0
        joan::orig::as_panel(this)->slide_in_7480e0();  // 0x007480e0
        joan::orig::as_panel(this)->slide_in_7420e0();  // 0x007420e0
        joan::orig::as_opt_menu(this)->apply();  // 0x0073ca80
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_05", name, addr, fn)
B("OptPanel::init_73ba80", 0x0073ba80, &H::opt_panel_init);
#undef B

}  // namespace game_b109_05_2
}  // namespace

// ---------------------------------------- from modules/game_b107_14.cpp (game/b107_14)
//
//    - 0x0073c410 Obj73c410::release_all (thiscall, ret 0): 0x0073c8a0, then for +0x28/+0x2c/+0x30
//      (0x00629830 + deleting dtor 0x006bd440) and +0x34..+0x44 (0x006217e0 + deleting dtor
//      0x006b33a0) release / delete / clear, then 0x00749e00, 0x007429c0, 0x00744ce0,
namespace {
namespace game_b107_14 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using M = jo::OptMenu;

JOAN_COUNTER(c_release_all, "Obj73c410::release_all");

struct H {
    // 0x0073c410
    template <auto Field, fn::Code Release, fn::Code Delete>
    void drop() {
        if (jo::self<M>(this)->*Field != nullptr) {
            thiscall<u32>(Release, static_cast<void*>(jo::self<M>(this)->*Field));
            void* const p = jo::self<M>(this)->*Field;
            if (p != nullptr)
                thiscall<u32>(Delete, p, 1);
            jo::self<M>(this)->*Field = nullptr;
        }
    }
    u32 release_all() {
        c_release_all.hit();
        joan::orig::as_opt_menu(this)->free_images();                              // 0x0073c8a0
        drop<&M::title, fn::Image2d_release, fn::Sprite_scalar_dtor>();                         // 0x00629830 / 0x006bd440
        drop<&M::frame_top, fn::Image2d_release, fn::Sprite_scalar_dtor>();
        drop<&M::frame_bottom, fn::Image2d_release, fn::Sprite_scalar_dtor>();
        drop<&M::tab_button_0, fn::MenuButton_clear, fn::ComboBox_scalar_dtor_6b33a0>();                         // 0x006217e0 / 0x006b33a0
        drop<&M::tab_button_1, fn::MenuButton_clear, fn::ComboBox_scalar_dtor_6b33a0>();
        drop<&M::tab_button_2, fn::MenuButton_clear, fn::ComboBox_scalar_dtor_6b33a0>();
        drop<&M::tab_button_3, fn::MenuButton_clear, fn::ComboBox_scalar_dtor_6b33a0>();
        drop<&M::tab_button_4, fn::MenuButton_clear, fn::ComboBox_scalar_dtor_6b33a0>();
        joan::orig::as_screen(this)->release_749e00();                              // 0x00749e00
        joan::orig::as_screen(this)->release_7429c0();                              // 0x007429c0
        joan::orig::as_opt_menu(this)->release();                              // 0x00744ce0
        return joan::orig::arg::as<u32>(joan::orig::as_screen(this)->release_740df0());                       // 0x00740df0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_14", name, addr, fn)
B("Obj73c410::release_all", 0x0073c410, &H::release_all);
#undef B

}  // namespace game_b107_14
}  // namespace

// ---------------------------------------- from modules/game_b104_15.cpp (game/b104_15)
//
//      0x00741240 Obj741240::scalar_deleting_dtor (thiscall, ret 4) - returns this.
namespace {
namespace game_b104_15_2 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

struct H {
    // ------------------------------------------------------------------------ 0x00741240
    H* scalar_dtor(u32 flags) {
        joan::orig::as_combo_box(this)->destruct();  // 0x006b2340
        if ((flags & 1) != 0) joan::crt::operator_delete(joan::orig::arg::mptr(static_cast<void*>(this)));  // operator delete
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_15", name, addr, fn)
B("Obj741240::scalar_deleting_dtor", 0x00741240, &H::scalar_dtor);
#undef B

}  // namespace game_b104_15_2
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
//
//    - Obj::scalar_dtor_741290 0x00741290 (flags), ret 4: 0x75fb00, delete when flags & 1.
namespace {
namespace game_b100_16 {

using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;

using joan::orig::helpers::op_delete;

struct H {
    // ------------------------------------------------------------------ 0x00741290
    void* scalar_dtor_741290(u32 flags) {
        joan::orig::as_scroll_box(this)->destruct();  // 0x0075fb00
        if ((flags & 1u) != 0) op_delete(this);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Obj::scalar_dtor_741290", 0x00741290, &H::scalar_dtor_741290);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
//
//      0x0075fab0 Widget_9df888::ctor        (thiscall, ret 0)
namespace {
namespace game_b95_08 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

struct H {

    // ------------------------------------------------------------------ 0x0075fab0
    void* widget_9df888_ctor() {
        joan::orig::as_gadget(this)->ctor();  // 0x0061de00
        jo::self<jo::ScrollBox>(this)->vtable = obj(d::vt_ScrollBox);
        jo::ScrollBox* const s = jo::self<jo::ScrollBox>(this);
        std::memset(&s->built, 0, static_cast<std::size_t>(reinterpret_cast<u8*>(s + 1) - reinterpret_cast<u8*>(&s->built)));  // +0x70..end
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Widget_9df888::ctor", 0x0075fab0, &H::widget_9df888_ctor);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
//
//    - free functions (cdecl): ArrowBtn::on_click 0x0075fb70 (p): byte p[0x73] = 1, returns 1
//      (callback passed to 0x0062b8d0 by 0x00760050); key filters 0x00763c90 / 0x00763cd0
//      (a, flags): (flags & 2) != 0.
namespace {
namespace game_b103_16 {

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::orig::helpers::addr;

using joan::orig::helpers::game_new;  // operator new

JOAN_COUNTER(c_init_760050, "ArrowList::init_760050");

struct H {
    void* new_widget(u32 size, std::uintptr_t ctor_thunk) {
        void* mem = game_new(size);
        return mem != nullptr ? thiscall<void*>(ctor_thunk, mem) : nullptr;
    }

    void init_760050(u32 x, u32 y, i32 count, u32 v, u32 a, u32 b) {
        c_init_760050.hit();
        const u32 self = static_cast<u32>(addr(this));  // only bits 16..31 are used below, as garbage the callees ignore (they read the low words)
        jo::view<u16>(jo::self<jo::ScrollBox>(this)->x) = static_cast<u16>(x);
        jo::view<u16>(jo::self<jo::ScrollBox>(this)->y) = static_cast<u16>(y);
        jo::view<u16>(jo::self<jo::ScrollBox>(this)->x2) = static_cast<u16>(jo::self<jo::ScrollBox>(this)->x + 0x1d0);
        jo::view<u16>(jo::self<jo::ScrollBox>(this)->y2) = static_cast<u16>(jo::self<jo::ScrollBox>(this)->y + 0x34);
        jo::self<jo::ScrollBox>(this)->u32_74 = v;
        jo::self<jo::ScrollBox>(this)->u32_78 = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_commele2));
        jo::view<void*>(jo::self<jo::ScrollBox>(this)->up) = new_widget(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0));  // 0x00620ee0
        jo::view<void*>(jo::self<jo::ScrollBox>(this)->down) = new_widget(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0));
        jo::view<void*>(jo::self<jo::ScrollBox>(this)->body) = new_widget(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0));
        jo::view<void*>(jo::self<jo::ScrollBox>(this)->bar) = new_widget(static_cast<std::uint32_t>(sizeof(jo::Scroll)), fn::code_value(fn::Widget62b_ctor));  // 0x0062b6c0
        const u32 hi = self & 0xffff0000u;  // upper halves of edx/ecx/eax (all held this)
        joan::orig::as_button(jo::view<void*>(jo::self<jo::ScrollBox>(this)->body))->create(joan::orig::arg::i32(hi | jo::view<u16>(jo::self<jo::ScrollBox>(this)->x)), joan::orig::arg::i32(hi | jo::view<u16>(jo::self<jo::ScrollBox>(this)->y)), joan::orig::arg::i32(hi | jo::view<u16>(jo::self<jo::ScrollBox>(this)->x2)), joan::orig::arg::i32(hi | jo::view<u16>(jo::self<jo::ScrollBox>(this)->y2)), fn::code_value(fn::Button_draw_75fba0), 1, 0, 0, 1, 1, 1);  // 0x00621540, callback thunk of 0x0075fba0
        jo::self<jo::ScrollBox>(this)->body->sprite_set = jo::self<jo::ScrollBox>(this)->u32_78;
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::ScrollBox>(this)->up))->setup(joan::orig::arg::i16(static_cast<u32>(jo::self<jo::ScrollBox>(this)->x + 0x23)), joan::orig::arg::i16(static_cast<u32>(jo::self<jo::ScrollBox>(this)->y + 0xb)), joan::orig::arg::mptr(jo::self<jo::ScrollBox>(this)->u32_78), joan::orig::arg::str(img::addr(d::s_aw_l_ot)), joan::orig::arg::str(img::addr(d::s_aw_l_dw)), joan::orig::arg::str(img::addr(d::s_aw_l_ov)), joan::orig::arg::str(img::addr(d::s_aw_l_ov)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)));  // 0x006211d0 aw_l_*
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::ScrollBox>(this)->down))->setup(joan::orig::arg::i16(static_cast<u32>(jo::self<jo::ScrollBox>(this)->x + 0x186)), joan::orig::arg::i16(static_cast<u32>(jo::self<jo::ScrollBox>(this)->y + 0xb)), joan::orig::arg::mptr(jo::self<jo::ScrollBox>(this)->u32_78), joan::orig::arg::str(img::addr(d::s_aw_r_ot)), joan::orig::arg::str(img::addr(d::s_aw_r_dw)), joan::orig::arg::str(img::addr(d::s_aw_r_ov)), joan::orig::arg::str(img::addr(d::s_aw_l_ov)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)));  // aw_r_* (+ aw_l_ov)
        joan::orig::as_text_box(jo::view<void*>(jo::self<jo::ScrollBox>(this)->bar))->init(joan::orig::arg::i16(static_cast<u32>(jo::self<jo::ScrollBox>(this)->x + 0x50)), joan::orig::arg::i16(static_cast<u32>(jo::self<jo::ScrollBox>(this)->y + 0xc)), joan::orig::arg::i16(static_cast<u32>(jo::self<jo::ScrollBox>(this)->x + 0x17c)), joan::orig::arg::i16(static_cast<u32>(jo::self<jo::ScrollBox>(this)->y + 0x24)), joan::orig::arg::i16(1u), fn::code_value(fn::ArrowBtn_on_click_75fb70), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)));  // 0x0062b8d0, callback thunk of 0x0075fb70 (the callback, a code address passed whole)
        joan::orig::as_scroll(jo::view<void*>(jo::self<jo::ScrollBox>(this)->bar))->set_62be10(0u, joan::orig::arg::u32(static_cast<u32>(count - 1)), 0u);  // 0x0062be10
        joan::orig::as_scroll_box(this)->set_value(0u);  // 0x00761240
        jo::view<u8>(jo::self<jo::ScrollBox>(this)->enabled) = static_cast<u8>(a);
        jo::view<u8>(jo::self<jo::ScrollBox>(this)->i8_8) = static_cast<u8>(b);
        jo::view<u32>(jo::self<jo::ScrollBox>(this)->scale) = 0x3f800000;
        jo::view<u32>(jo::self<jo::ScrollBox>(this)->built) = 1;
    }
};

i32 JOAN_CDECL key_filter(u32, u32 flags) { return (flags & 0xff & 2) != 0 ? 1 : 0; }

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("ArrowList::init_760050", 0x00760050, &H::init_760050);
B("Key::filter_763cd0", 0x00763cd0, &key_filter);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
//
//      0x00760440 Panel::destroy_children()                     thiscall
namespace {
namespace game_b92_06 {

using joan::call::thiscall;

struct H {
    template <class W>
    void delete_child(W* jo::ScrollBox::*off, std::uintptr_t dtor) {
        jo::ScrollBox* const s = jo::self<jo::ScrollBox>(this);
        if (s->*off == nullptr) return;
        void* const p = s->*off;
        if (p != nullptr) thiscall<void*>(dtor, p, 1);
        s->*off = nullptr;
    }

    // 0x00760440
    void destroy_children() {
        if (jo::self<jo::ScrollBox>(this)->built == 0) return;
        jo::self<jo::ScrollBox>(this)->u32_78 = nullptr;
        delete_child(&jo::ScrollBox::up, fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));  // 0x006b33a0
        delete_child(&jo::ScrollBox::down, fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        delete_child(&jo::ScrollBox::body, fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        delete_child(&jo::ScrollBox::bar, fn::code_value(fn::Obj562960_scalar_deleting_dtor));  // 0x00562960
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Panel::destroy_children", 0x00760440, &H::destroy_children);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
//
//      0x00761240 Obj761::forward(a) (ret 4)       - 0x0062be50 on +0x7c
namespace {
namespace game_b91_04 {

using joan::call::thiscall;

struct H {
    // 0x00761240
    std::uint32_t forward_761240(std::uint32_t a) { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_scroll(jo::view<void*>(jo::self<jo::ScrollBox>(this)->bar))->set_pos(joan::orig::arg::i32(a))); }  // 0x0062be50
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj761::forward", 0x00761240, &H::forward_761240);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b104_15_3.cpp (game/b104_15)
namespace {
namespace game_b104_15_3 {

using joan::hook::global;
using i8 = std::int8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using M = jo::OptMenu;

JOAN_COUNTER(c_layout_747bc0, "OptMenu::layout_747bc0");
JOAN_COUNTER(c_layout_74b6b0, "OptMenu::layout_74b6b0");

// Optional offset on the light / options screens: -0x30 in menu mode 9.
u32 menu_dy() { return global<i8>(d::g_game_mode) == 9 ? 0xffffffd0u : 0u; }

struct H {
    // 0x0061e020 / 0x0061f1b0 on the widget pointer member `off`.
    template <class W>
    u32 m6(W* M::*off, u32 a, u32 b, u32 c, u32 d, u32 e, u32 g) {
        joan::orig::as_widget(jo::self<M>(this)->*off)->move_to_61e020(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(g));  // 0x0061e020
        return 0;  // move_to returns nothing; the layout results are never read
    }
    template <class W>
    u32 f5(W* M::*off, u32 a, u32 b, u32 c, u32 d, u32 e) {
        joan::orig::as_widget(jo::self<M>(this)->*off)->scale_to_61f1b0(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
        return 0;  // scale_to returns nothing; the layout results are never read
    }

    // ------------------------------------------------------------------------ 0x00747bc0
    u32 layout_747bc0() {
        c_layout_747bc0.hit();
        const u32 d = menu_dy();
        u32 r;
        r = f5(&M::key_btn_d4, 0x1u, 0x40800000u, 0u, 0u, 0x1f4u);
        r = m6(&M::key_btn_d4, 0xffffff9cu, u32(d + 0x18f + mouse::row_shift()), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::key_btn_bc, 0x1u, 0x40800000u, 0u, 0u, 0x1f4u);
        r = m6(&M::key_btn_bc, 0xffffff9cu, u32(d + 0x15b + mouse::row_shift()), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::key_btn_e8, 0x1u, 0x40800000u, 0u, 0u, 0x1f4u);
        r = m6(&M::key_btn_e8, 0xffffff9cu, u32(d + 0x1c3 + mouse::row_shift()), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::key_btn_cc, 0x1u, 0x40800000u, 0u, 0u, 0x1f4u);
        r = m6(&M::key_btn_cc, 0xffffff9cu, u32(d + 0x127), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::key_btn_c4, 0x1u, 0x40800000u, 0u, 0u, 0x1f4u);
        r = m6(&M::key_btn_c4, 0xffffff9cu, u32(d + 0xf3), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::key_btn_dc, 0x1u, 0x40c00000u, 0u, 0u, 0x1f4u);
        r = m6(&M::key_btn_dc, 0x1b5u, u32(d + 0x2c8), 0x477fff00u, 0u, 0u, 0x1f4u);
        r = f5(&M::key_btn_e0, 0x1u, 0x40c00000u, 0u, 0u, 0x1f4u);
        r = m6(&M::key_btn_e0, 0x296u, u32(d + 0x2c8), 0x477fff00u, 0u, 0u, 0x1f4u);
        r = f5(&M::key_btn_d8, 0x1u, 0x40c00000u, 0u, 0u, 0x1f4u);
        r = m6(&M::key_btn_d8, 0xd4u, u32(d + 0x2c8), 0x477fff00u, 0u, 0u, 0x1f4u);
        r = m6(&M::title, 0x1ccu, u32(d - 0x30), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::key_combo_e4, 0x1u, 0x40800000u, 0u, 0u, 0x226u);
        r = m6(&M::key_combo_e4, 0x1c0u, u32(d + 0x1bc + mouse::row_shift()), 0x455ac000u, 0xc4bb8000u, 0u, 0x226u);
        r = f5(&M::key_slider_d0, 0x1u, 0x40800000u, 0u, 0u, 0x258u);
        r = m6(&M::key_slider_d0, 0x1c0u, u32(d + 0x188 + mouse::row_shift()), 0x455ac000u, 0xc4bb8000u, 0u, 0x258u);
        r = f5(&M::key_combo_b8, 0x1u, 0x40800000u, 0u, 0u, 0x28au);
        r = m6(&M::key_combo_b8, 0x1c0u, u32(d + 0x154 + mouse::row_shift()), 0x455ac000u, 0xc4bb8000u, 0u, 0x28au);
        if (void* const w = mouse::cursor_slider(); w != nullptr) {  // [Fixes] MouseOptions
            widget_scale(mouse::cursor_label(), 0x1u, 0x40800000u, 0u, 0u, 0x1f4u);
            widget_move(mouse::cursor_label(), 0xffffff9cu, u32(d + 0x15b), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
            widget_scale(w, 0x1u, 0x40800000u, 0u, 0u, 0x2a3u);
            widget_move(w, 0x1c0u, u32(d + 0x154), 0x455ac000u, 0xc4bb8000u, 0u, 0x2a3u);
        }
        r = f5(&M::key_slider_c8, 0x1u, 0x40800000u, 0u, 0u, 0x2bcu);
        r = m6(&M::key_slider_c8, 0x1c0u, u32(d + 0x120), 0x455ac000u, 0xc4bb8000u, 0u, 0x2bcu);
        r = f5(&M::key_combo_c0, 0x1u, 0x40800000u, 0u, 0u, 0x2eeu);
        r = m6(&M::key_combo_c0, 0x1c0u, u32(d + 0xec), 0x455ac000u, 0xc4bb8000u, 0u, 0x2eeu);
        return r;
    }

    // ------------------------------------------------------------------------ 0x0074b6b0
    u32 layout_74b6b0() {
        c_layout_74b6b0.hit();
        const u32 d = menu_dy();
        u32 r;
        r = m6(&M::snd_slider_48, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_slider_48, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_btn_4c, 0xfffffd9cu, u32(d + 0xf3), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_btn_4c, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_slider_50, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_slider_50, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_btn_54, 0xfffffd9cu, u32(d + 0x127), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_btn_54, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_slider_58, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_slider_58, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_btn_5c, 0xfffffd9cu, u32(d + 0x15b), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_btn_5c, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_slider_60, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_slider_60, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_btn_64, 0xfffffd9cu, u32(d + 0x18f), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_btn_64, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_combo_78, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_combo_78, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_btn_7c, 0xfffffd9cu, u32(d + 0x1c3), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_btn_7c, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_slider_68, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_slider_68, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_btn_6c, 0xfffffd9cu, u32(d + 0x1f7), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_btn_6c, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_slider_70, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_slider_70, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_btn_74, 0xfffffd9cu, u32(d + 0x22b), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_btn_74, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_combo_80, 0x1c0u, u32(d + 0x3e), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_combo_80, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_btn_84, 0xfffffd9cu, u32(d + 0x25f), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_btn_84, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_btn_8c, 0x219u, u32(d + 0x2c8), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_btn_8c, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        r = m6(&M::snd_btn_88, 0x183u, u32(d + 0x2c8), 0x3f800000u, 0u, 0x1u, 0u);
        r = f5(&M::snd_btn_88, 0u, 0x3f800000u, 0u, 0x1u, 0u);
        joan::optmenu::graphics::hide_now(this);  // graphics_page.hpp
        joan::optmenu::remap::hide_now(this);     // remap_set.hpp
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_15", name, addr, fn)
B("OptMenu::layout_747bc0", 0x00747bc0, &H::layout_747bc0);
B("OptMenu::layout_74b6b0", 0x0074b6b0, &H::layout_74b6b0);
#undef B

}  // namespace game_b104_15_3
}  // namespace

// ---------------------------------------- from modules/game_b103_16_2.cpp (game/b103_16)
namespace {
namespace game_b103_16_2 {

using joan::hook::global;
using i8 = std::int8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;
using M = jo::OptMenu;

// sound
void slide_sound() {
    joan::orig::as_sound(obj(d::g_sfx_player))->play_def(img::str(d::s_BUTTON_SLIDE_MANY));  // 0x004ceef0 'BUTTON_SLIDE_MANY'
}
u32 slide_dy() { return static_cast<i8>(global<u8>(d::g_game_mode)) == 9 ? 0xffffffd0u : 0u; }

JOAN_COUNTER(c_slide_7480e0, "Panel::slide_in_7480e0");
JOAN_COUNTER(c_slide_74c2e0, "Panel::slide_in_74c2e0");

struct H {
    void slide_7480e0() {
        c_slide_7480e0.hit();
        const u32 dy = slide_dy();
        if (jo::self<jo::OptMenu>(this)->slide_state != 0) slide_sound();
        joan::orig::as_widget(jo::self<M>(this)->key_btn_d4)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_btn_d4)->move_to(-612, joan::orig::arg::i16(dy + 0x18f + mouse::row_shift()), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->key_btn_bc)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_btn_bc)->move_to(-612, joan::orig::arg::i16(dy + 0x15b + mouse::row_shift()), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->key_combo_e4)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_btn_e8)->move_to(-612, joan::orig::arg::i16(dy + 0x1c3 + mouse::row_shift()), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->key_btn_cc)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_btn_cc)->move_to(-612, joan::orig::arg::i16(dy + 0x127), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->key_btn_c4)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_btn_c4)->move_to(-612, joan::orig::arg::i16(dy + 0xf3), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->key_btn_dc)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_btn_e0)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_btn_d8)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_combo_c0)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_combo_c0)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        if (void* const w = mouse::cursor_slider(); w != nullptr) {  // [Fixes] MouseOptions
            joan::orig::as_widget(mouse::cursor_label())->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);
            joan::orig::as_widget(mouse::cursor_label())->move_to(-612, joan::orig::arg::i16(dy + 0x15b), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);
            joan::orig::as_widget(w)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 75, 0x0u);
            joan::orig::as_widget(w)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 75, 0x0u);
        }
        joan::orig::as_widget(jo::self<M>(this)->key_slider_c8)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 50, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_slider_c8)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 50, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->key_combo_b8)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x64u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_combo_b8)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x64u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->key_slider_d0)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, -106, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_slider_d0)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), -106, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->key_combo_e4)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0xc8u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->key_combo_e4)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0xc8u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->title)->move_to(0x1cc, joan::orig::arg::i16(dy - 0x1c0), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0xfau);  // 0x0061e020
    }

    void slide_74c2e0() {
        c_slide_74c2e0.hit();
        const u32 dy = slide_dy();
        if (jo::self<jo::OptMenu>(this)->slide_state != 0) slide_sound();
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_88)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_8c)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_4c)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_4c)->move_to(-612, joan::orig::arg::i16(dy + 0xf3), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_54)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_54)->move_to(-612, joan::orig::arg::i16(dy + 0x127), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_5c)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_5c)->move_to(-612, joan::orig::arg::i16(dy + 0x15b), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_64)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_64)->move_to(-612, joan::orig::arg::i16(dy + 0x18f), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_7c)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_7c)->move_to(-612, joan::orig::arg::i16(dy + 0x1c3), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_6c)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_6c)->move_to(-612, joan::orig::arg::i16(dy + 0x1f7), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_74)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_74)->move_to(-612, joan::orig::arg::i16(dy + 0x22b), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_84)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_btn_84)->move_to(-612, joan::orig::arg::i16(dy + 0x25f), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_48)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_48)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_50)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x32u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_50)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x32u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_58)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x64u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_58)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x64u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_60)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x96u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_60)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x96u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_combo_78)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0xc8u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_combo_78)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0xc8u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_68)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0xfau);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_68)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0xfau);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_70)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x12cu);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_slider_70)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x12cu);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->snd_combo_80)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x15eu);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->snd_combo_80)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x15eu);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->title)->move_to(0x1cc, joan::orig::arg::i16(dy - 0x1c0), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x190u);  // 0x0061e020
        // VIDEO OPTIONS and GRAPHICS OPTIONS share these widgets: leaving either slides all of ours out too.
        joan::optmenu::graphics::video_out(this);     // graphics_page.hpp
        joan::optmenu::graphics::graphics_out(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Panel::slide_in_7480e0", 0x007480e0, &H::slide_7480e0);
B("Panel::slide_in_74c2e0", 0x0074c2e0, &H::slide_74c2e0);
#undef B

}  // namespace game_b103_16_2
}  // namespace

// ---------------------------------------- from modules/game_b101_15.cpp (game/b101_15)
namespace {
namespace game_b101_15 {

using joan::hook::global;
using i8 = std::int8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;

u32 button_anim(void* b, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    joan::orig::as_widget(b)->scale_to_61f1b0(joan::orig::arg::i16(a0), joan::orig::arg::f32(a1), joan::orig::arg::f32(a2), static_cast<char>(a3), joan::orig::arg::u32(a4));  // 0x0061f1b0
    return 0;  // scale_to returns nothing; CreditsScreen::layout results are never read
}
u32 button_move(void* b, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
    joan::orig::as_widget(b)->move_to_61e020(joan::orig::arg::i16(a0), joan::orig::arg::i16(a1), joan::orig::arg::f32(a2), joan::orig::arg::f32(a3), static_cast<char>(a4), joan::orig::arg::u32(a5));  // 0x0061e020
    return 0;  // move_to returns nothing; CreditsScreen::layout results are never read (only Panel::set_page)
}

JOAN_COUNTER(c_credits_layout, "CreditsScreen::layout");

struct H {
    // 0x00748540 (ret)
    u32 credits_layout() {
        c_credits_layout.hit();
        const u32 dy = static_cast<i32>(global<i8>(d::g_game_mode)) == 9 ? 0xffffffd0u : 0u;
        button_anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_f4), 1u, 0x40c00000u, 0u, 0u, 0x1f4u);
        button_move(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_f4), 0x1b5u, dy + 0x2c8u, 0x477fff00u, 0u, 0u, 0x1f4u);
        button_anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_f8), 1u, 0x40c00000u, 0u, 0u, 0x1f4u);
        button_move(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_f8), 0xd4u, dy + 0x2c8u, 0x477fff00u, 0u, 0u, 0x1f4u);
        button_anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_fc), 1u, 0x40c00000u, 0u, 0u, 0x1f4u);
        button_move(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_fc), 0x296u, dy + 0x2c8u, 0x477fff00u, 0u, 0u, 0x1f4u);
        u32 delay = 0x32;
        for (i32 i = 9; i >= 0; --i) {
            const u32 y = dy + static_cast<u32>(i) * 0x28u + 0x12cu;
            button_anim(jo::self<jo::OptMenu>(this)->credit_lines->buttons[i], 1u, 0x40c00000u, 0u, 0u, delay + 0x1f4u);
            button_move(jo::self<jo::OptMenu>(this)->credit_lines->buttons[i], 0x74u, y, 0x455ac000u, 0xc4bb8000u, 0u, delay + 0x1f4u);
            button_anim(jo::self<jo::OptMenu>(this)->credit_labels->buttons[i], 1u, 0x40c00000u, 0u, 0u, delay + 0x1f4u);
            button_move(jo::self<jo::OptMenu>(this)->credit_labels->buttons[i], 0x244u, y, 0x455ac000u, 0xc4bb8000u, 0u, delay + 0x1f4u);
            delay += 0x32;
        }
        button_anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_combo_100), 1u, 0x40c00000u, 0u, 0u, delay + 0x1f4u);
        joan::optmenu::remap::slide_in(this, delay + 0x1f4u);  // the Keyboard / Gamepad button beside it (remap_set.hpp)
        return button_move(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_combo_100), 0x74u, dy + 0xecu, 0x455ac000u, 0xc4bb8000u, 0u, delay + 0x1f4u);
    }
};

#define B15(name, addr, fn) JOAN_REPLACE("game/b101_15", name, addr, fn)
B15("CreditsScreen::layout", 0x00748540, &H::credits_layout);
#undef B15

}  // namespace game_b101_15
}  // namespace

// ---------------------------------------- from modules/game_b101_16.cpp (game/b101_16)
namespace {
namespace game_b101_16 {

using joan::hook::global;
using i8 = std::int8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::img::obj;

using joan::img::str;

using M = jo::OptMenu;

void* const kSounds = obj(d::g_sfx_player);

// 0x0061f1b0 (0, 6.0f, 0, 0, delay) / 0x0061e020 (x, y, a, b, 0, delay) on a button.
void button_anim(void* b, i32 delay) {
    joan::orig::as_widget(b)->scale_to(0, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, joan::orig::arg::u32(delay));  // 0x0061f1b0
}
void button_slide(void* b, i32 x, i32 y, u32 a, u32 c, i32 delay) {
    joan::orig::as_widget(b)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(a), joan::orig::arg::f32(c), 0, joan::orig::arg::u32(delay));  // 0x0061e020
}

JOAN_COUNTER(c_slide_in, "Menu::slide_in_748830");

struct H {
    // ------------------------------------------------------------------ 0x00748830
    i32 slide_in_748830() {
        c_slide_in.hit();
        i32 dy = 0;
        if (static_cast<i32>(global<i8>(d::g_game_mode)) == 9) dy = -0x30;
        if (static_cast<i32>(jo::self<jo::OptMenu>(this)->slide_state) != 0)
            joan::orig::as_sound(kSounds)->play_def(str(d::s_BUTTON_SLIDE_MANY));  // 0x004ceef0 "BUTTON_SLIDE_MANY"
        i32 delay = 0;
        button_anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_f4), delay);
        button_slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_f4), 0x1b5, dy + 0x2c8, 0x477fff00u, 0u, delay);
        button_anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_f8), delay);
        button_slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_f8), 0xd4, dy + 0x2c8, 0x477fff00u, 0u, delay);
        button_anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_fc), delay);
        button_slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_btn_fc), 0x296, dy + 0x2c8, 0x477fff00u, 0u, delay);
        button_anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_combo_100), delay);
        button_slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->credit_combo_100), 0x74, dy + 0x3e, 0x455ac000u, 0xc4bb8000u, delay);
        joan::optmenu::remap::slide_out(this, static_cast<u32>(delay));  // remap_set.hpp
        delay += 0x32;
        i32 i = 0;
        for (; i < 10; ++i) {
            button_anim(jo::self<M>(this)->credit_lines->buttons[i], delay);
            button_slide(jo::self<M>(this)->credit_lines->buttons[i], 0x74, dy + 0x46, 0x455ac000u, 0xc4bb8000u, delay);
            button_anim(jo::self<M>(this)->credit_labels->buttons[i], delay);
            button_slide(jo::self<M>(this)->credit_labels->buttons[i], 0x400, dy + i * 0x28 + 0x12c, 0x455ac000u, 0xc4bb8000u, delay);
            delay += 0x32;
        }
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_16", name, addr, fn)
B("Menu::slide_in_748830", 0x00748830, &H::slide_in_748830);
#undef B

}  // namespace game_b101_16
}  // namespace

// ---------------------------------------- from modules/game_b109_11.cpp (game/b109_11)
namespace {
namespace game_b109_11 {

using joan::hook::global;
using i8 = std::int8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::hook::x87::Fpu;

using joan::call::cdecl_call;

void* const kStrings = obj(d::g_strings);

using joan::hook::x87::ftol;

using M = jo::OptMenu;

void* const kFont = obj(d::g_font_goudy14_22_white);

JOAN_COUNTER(c_options, "OptionsPage::build_748b10");

struct H {
    // ------------------------------------------------------------ 0x00748b10
    static void* make(u32 size, std::uintptr_t ctor_thunk) {
        void* const mem = joan::crt::operator_new(joan::orig::arg::u32(size));  // operator new
        return mem != nullptr ? thiscall<void*>(ctor_thunk, mem) : nullptr;
    }
    // Button (0x00620ee0) at +off: 0x00621540 layout, 0x00623ce0 caption (string getter of
    // 0xa57c08, font 0xa51608, white).
    template <class W>
    void opt_button(W* M::*off, i32 x, i32 y, i32 w, i32 h, std::uintptr_t callback, std::uintptr_t text_thunk) {
        jo::self<M>(this)->*off = static_cast<W*>(make(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0)));  // 0x00620ee0
        joan::orig::as_button(jo::self<M>(this)->*off)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), callback, i32{1}, i32{0}, i32{0}, i32{1}, i32{1}, i32{1});  // 0x00621540
        alignas(4) u32 slot = 0;
        const u32 color = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&slot)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));  // 0x0042a260
        void* const text = thiscall<void*>(text_thunk, kStrings);
        joan::orig::as_button(jo::self<M>(this)->*off)->set_text(joan::orig::arg::str(text), kFont, joan::orig::arg::i32(color), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
    }
    // Slider (0x0075fab0) at +off: 0x00760050(0x1c0, 0x3e, range, 0, 1, 1) then 0x00761240(value).
    template <class W>
    void opt_slider_create(W* M::*off, i32 range) {
        jo::self<M>(this)->*off = static_cast<W*>(make(static_cast<std::uint32_t>(sizeof(jo::ScrollBox)), fn::code_value(fn::Widget_9df888_ctor)));  // 0x0075fab0
        joan::orig::as_scroll_box(jo::self<M>(this)->*off)->init(joan::orig::arg::u32(i32{0x1c0}), joan::orig::arg::u32(i32{0x3e}), joan::orig::arg::i32(range), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{1}));  // 0x00760050
    }
    template <class W>
    void opt_slider_set(W* M::*off, i32 value) { joan::orig::as_scroll_box(jo::self<M>(this)->*off)->set_value(joan::orig::arg::u32(value)); }  // 0x00761240
    // Choice list (0x006b22f0) at +off.
    template <class W>
    void opt_list_create(W* M::*off) {
        jo::self<M>(this)->*off = static_cast<W*>(make(static_cast<std::uint32_t>(sizeof(jo::ComboBox)), fn::code_value(fn::Widget_9dd7a0_ctor)));  // 0x006b22f0
        joan::orig::as_combo_box(jo::self<M>(this)->*off)->init_options(joan::orig::arg::u32(i32{0x1c0}), joan::orig::arg::u32(i32{0x3e}), i32{2}, joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{1}));  // 0x006b29b0
    }
    template <class W>
    void opt_list_add(W* M::*off, std::uintptr_t text_thunk) {
        void* const text = thiscall<void*>(text_thunk, kStrings);
        joan::orig::as_combo_box(jo::self<M>(this)->*off)->add_6b3930(joan::orig::arg::str(text));  // 0x006b3930
    }

    void options_build() {
        c_options.hit();
        const Fpu fpu{true};
        opt_button(&M::snd_btn_88, 0x183, 0x2c8, 0x1e7, 0x2e6, fn::code_value(fn::mark_draw_73e130), fn::code_value(fn::OptMenu_open_33));  // 0x0073e130, 0x004db550
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptionsPage>(this)->snd_btn_88))->scale_to(0, 1.0f, 0.0f, 1, 0u);  // 0x0061f1b0
        opt_button(&M::snd_btn_8c, 0x219, 0x2c8, 0x27d, 0x2e6, fn::code_value(fn::mark_draw_73e130), fn::code_value(fn::Menu_load_optmenu));  // 0x0073e130, 0x004db670
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptionsPage>(this)->snd_btn_8c))->scale_to(0, 1.0f, 0.0f, 1, 0u);  // 0x0061f1b0

        // +0x48: [0xa58060] / d[0x9df790] + d[0x9df780]
        opt_slider_create(&M::snd_slider_48, 0x65);
        {
            double v = fpu.div(static_cast<double>(global<float>(d::g_a58060)), global<double>(d::dbl_0_02));
            v = fpu.add(v, global<double>(d::dbl_9df780));
            opt_slider_set(&M::snd_slider_48, ftol(v));
        }
        joan::orig::as_sys_en(obj(d::g_sys3d))->set_brightness(joan::orig::arg::f32(global<u32>(d::g_a58060)));  // 0x00602a50

        opt_button(&M::snd_btn_4c, -0x264, 0xf3, -0x64, 0x12b, fn::code_value(fn::draw_unit_info), fn::code_value(fn::TxtRes_optmenu_16));  // 0x0073e990, 0x004db340
        i32 v14 = 0;
        {
            const float a = global<float>(d::g_a58064);
            const float t1 = global<float>(d::flt_1);
            double v;
            if (a > t1) {
                v = fpu.sub(static_cast<double>(a), static_cast<double>(t1));
                v = fpu.div(v, global<double>(d::dbl_0_04));
                v = fpu.add(v, global<double>(d::dbl_9df780));
            } else {
                v = fpu.sub(static_cast<double>(a), global<double>(d::dbl_0_5));
                v = fpu.div(v, global<double>(d::dbl_0_01));
            }
            v14 = ftol(v);
        }
        opt_slider_create(&M::snd_slider_50, 0x65);
        opt_slider_set(&M::snd_slider_50, v14);
        joan::orig::as_sys_en(obj(d::g_sys3d))->set_contrast(joan::orig::arg::f32(global<u32>(d::g_a58064)));  // 0x00602ad0

        opt_button(&M::snd_btn_54, -0x264, 0x127, -0x64, 0x15f, fn::code_value(fn::draw_unit_info), fn::code_value(fn::TxtRes_optmenu_17));  // 0x0073e990, 0x004db370
        {
            const float a = global<float>(d::g_a58068);
            const float t1 = global<float>(d::flt_1);
            const float t2 = global<float>(d::flt_3);
            double v;
            if (!(a > t1)) {
                v = fpu.sub(static_cast<double>(a), global<double>(d::dbl_0_5));
                v = fpu.div(v, global<double>(d::dbl_0_01));
            } else if (!(a > t2)) {
                v = fpu.sub(static_cast<double>(a), static_cast<double>(t1));
                v = fpu.div(v, global<double>(d::dbl_0_8));
                v = fpu.add(v, global<double>(d::dbl_9df780));
            } else {
                v = fpu.sub(static_cast<double>(a), static_cast<double>(t2));
                v = fpu.div(v, global<double>(d::dbl_0_12));
                v = fpu.add(v, global<double>(d::dbl_9df740));
            }
            v14 = ftol(v);
        }
        opt_slider_create(&M::snd_slider_58, 0x65);
        opt_slider_set(&M::snd_slider_58, v14);
        joan::orig::as_d3d(obj(d::g_sys3d))->set_gamma(joan::orig::arg::f32(global<u32>(d::g_a58068)));  // 0x00602b50

        opt_button(&M::snd_btn_5c, -0x264, 0x15b, -0x64, 0x193, fn::code_value(fn::draw_unit_info), fn::code_value(fn::OptMenu_open_18));  // 0x0073e990, 0x004db3a0
        joan::orig::as_d3d(obj(d::g_sys3d))->apply_gamma();  // 0x00602cd0
        // Widened ranges (view_range.hpp); the original: ([0xa58050] - 0x401) * 0x14 / 0x3c00 and [0xa58058] / 0x14.
        joan::optmenu::clamp_view_ranges();
        v14 = joan::optmenu::draw_to_slider(global<i32>(d::g_cfg_far_limit));
        opt_slider_create(&M::snd_slider_60, 0x15);
        opt_slider_set(&M::snd_slider_60, v14);

        opt_button(&M::snd_btn_64, -0x264, 0x18f, -0x64, 0x1c7, fn::code_value(fn::draw_unit_info), fn::code_value(fn::Menu_optmenu_19));  // 0x0073e990, 0x004db3d0
        if (static_cast<i32>(global<i8>(d::g_game_mode)) != 1)
            global<float>(d::g_far_clip) = static_cast<float>(global<i32>(d::g_cfg_far_limit));
        v14 = joan::optmenu::grass_to_slider(global<i32>(d::g_cfg_grass_density));
        opt_slider_create(&M::snd_slider_68, 0x65);
        opt_slider_set(&M::snd_slider_68, v14);

        opt_button(&M::snd_btn_6c, -0x264, 0x1f7, -0x64, 0x22f, fn::code_value(fn::draw_unit_info), fn::code_value(fn::OptMenu_open_1a));  // 0x0073e990, 0x004db400
        v14 = global<i32>(d::g_a5805c);
        opt_slider_create(&M::snd_slider_70, 0x65);
        opt_slider_set(&M::snd_slider_70, v14);

        opt_button(&M::snd_btn_74, -0x264, 0x22b, -0x64, 0x263, fn::code_value(fn::draw_unit_info), fn::code_value(fn::OptMenu_open_1b));  // 0x0073e990, 0x004db430
        opt_list_create(&M::snd_combo_78);
        opt_list_add(&M::snd_combo_78, fn::code_value(fn::OptMenu_open_2));  // 0x004daf50
        opt_list_add(&M::snd_combo_78, fn::code_value(fn::OptMenu_open_1));  // 0x004daf20
        joan::orig::as_popup(jo::view<void*>(jo::self<jo::OptionsPage>(this)->snd_combo_78))->set_text_6b44e0(joan::orig::arg::u32(static_cast<i32>(global<i8>(d::g_cfg_grass_enable) != 0 ? 1 : 0) + 1));  // 0x006b44e0

        opt_button(&M::snd_btn_7c, -0x264, 0x1c3, -0x64, 0x1fb, fn::code_value(fn::draw_unit_info), fn::code_value(fn::Menu_optmenu_1c));  // 0x0073e990, 0x004db460
        opt_list_create(&M::snd_combo_80);
        opt_list_add(&M::snd_combo_80, fn::code_value(fn::OptMenu_open_12));  // 0x004db280
        opt_list_add(&M::snd_combo_80, fn::code_value(fn::OptMenu_open_13));  // 0x004db2b0
        opt_list_add(&M::snd_combo_80, fn::code_value(fn::Menu_optmenu_14));  // 0x004db2e0
        opt_list_add(&M::snd_combo_80, fn::code_value(fn::OptMenu_open_15));  // 0x004db310
        joan::orig::as_popup(jo::view<void*>(jo::self<jo::OptionsPage>(this)->snd_combo_80))->set_text_6b44e0(joan::orig::arg::u32(static_cast<i32>(global<i8>(d::g_a58054)) + 1));  // 0x006b44e0

        opt_button(&M::snd_btn_84, -0x264, 0x25f, -0x64, 0x297, fn::code_value(fn::draw_unit_info), fn::code_value(fn::OptMenu_open_1d));  // 0x0073e990, 0x004db490
        if (global<jo::World*>(d::g_world) != nullptr && global<jo::World*>(d::g_world)->scene != nullptr)
            joan::orig::as_scene(global<jo::World*>(d::g_world)->scene)->set_fog_flag(static_cast<i32>(global<i8>(d::g_a58054) != 0 ? 1 : 0));  // 0x005ee140
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_11", name, addr, fn)
B("OptionsPage::build_748b10", 0x00748b10, &H::options_build);
#undef B

}  // namespace game_b109_11
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using M = jo::OptMenu;

JOAN_COUNTER(c_rel_749e00, "Screen::release_749e00");

// Release kinds: pre-call thunk, scalar-deleting-dtor thunk.
constexpr fn::Code kPre6217e0 = fn::MenuButton_clear;  // 0x006217e0
constexpr fn::Code kDel6b33a0 = fn::ComboBox_scalar_dtor_6b33a0;  // 0x006b33a0
constexpr fn::Code kPre6b3130 = fn::ComboBox_free_parts;  // 0x006b3130
constexpr fn::Code kDel741240 = fn::Obj741240_scalar_deleting_dtor;  // 0x00741240
constexpr fn::Code kPre760440 = fn::Panel_destroy_children;  // 0x00760440
constexpr fn::Code kDel741290 = fn::Obj_scalar_dtor_741290;  // 0x00741290

struct H {
    // one release block: if (p = +off) { pre(p); p = +off; if (p) del(p, 1); +off = 0; }
    // Returns the eax the original block leaves: this when skipped, else the re-read pointer
    // (0) or the scalar dtor's result.
    template <class W>
    std::uintptr_t release(W* M::*off, std::uintptr_t pre, std::uintptr_t del) {
        W*& slot = jo::self<M>(this)->*off;
        if (slot == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        thiscall<u32>(pre, static_cast<void*>(slot));
        W* const p = slot;
        const std::uintptr_t eax = p != nullptr ? thiscall<u32>(del, static_cast<void*>(p), 1u) : 0u;
        slot = nullptr;
        return eax;
    }
    std::uintptr_t a(jo::Button* M::*off) { return release(off, fn::code_value(kPre6217e0), fn::code_value(kDel6b33a0)); }
    std::uintptr_t b(jo::ComboBox* M::*off) { return release(off, fn::code_value(kPre6b3130), fn::code_value(kDel741240)); }
    std::uintptr_t c(jo::ScrollBox* M::*off) { return release(off, fn::code_value(kPre760440), fn::code_value(kDel741290)); }

    // ------------------------------------------------------------------ 0x00749e00
    std::uintptr_t release_749e00() {
        c_rel_749e00.hit();
        c(&M::snd_slider_48);
        a(&M::snd_btn_4c);
        c(&M::snd_slider_50);
        a(&M::snd_btn_54);
        c(&M::snd_slider_58);
        a(&M::snd_btn_5c);
        c(&M::snd_slider_60);
        a(&M::snd_btn_64);
        c(&M::snd_slider_68);
        a(&M::snd_btn_6c);
        c(&M::snd_slider_70);
        a(&M::snd_btn_74);
        b(&M::snd_combo_78);
        a(&M::snd_btn_7c);
        b(&M::snd_combo_80);
        a(&M::snd_btn_84);
        a(&M::snd_btn_88);
        joan::optmenu::graphics::release(this);  // graphics_page.hpp
        joan::optmenu::remap::release(this);     // remap_set.hpp
        return a(&M::snd_btn_8c);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Screen::release_749e00", 0x00749e00, &H::release_749e00);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using M = jo::OptMenu;

JOAN_COUNTER(c_panel_74a580, "Panel::enable_74a580");

struct H {
    // ------------------------------------------------------------------ 0x0074a580
    void panel_74a580(u32 /*unused*/) {
        c_panel_74a580.hit();
        joan::orig::as_scroll_box(jo::self<M>(this)->snd_slider_48)->draw(1, 7);  // 0x00760b70
        joan::orig::as_button(jo::self<M>(this)->snd_btn_4c)->draw(1u, 7);  // 0x00622cc0
        joan::orig::as_scroll_box(jo::self<M>(this)->snd_slider_50)->draw(1, 7);
        joan::orig::as_button(jo::self<M>(this)->snd_btn_54)->draw(1u, 7);
        joan::orig::as_scroll_box(jo::self<M>(this)->snd_slider_58)->draw(1, 7);
        joan::orig::as_button(jo::self<M>(this)->snd_btn_5c)->draw(1u, 7);
        joan::orig::as_scroll_box(jo::self<M>(this)->snd_slider_60)->draw(1, 7);
        joan::orig::as_button(jo::self<M>(this)->snd_btn_64)->draw(1u, 7);
        joan::orig::as_scroll_box(jo::self<M>(this)->snd_slider_68)->draw(1, 7);
        joan::orig::as_button(jo::self<M>(this)->snd_btn_6c)->draw(1u, 7);
        joan::orig::as_scroll_box(jo::self<M>(this)->snd_slider_70)->draw(1, 7);
        joan::orig::as_button(jo::self<M>(this)->snd_btn_74)->draw(1u, 7);
        joan::orig::as_combo_box(jo::self<M>(this)->snd_combo_78)->draw(1u, static_cast<std::uint8_t>(joan::orig::arg::u32(7u)));  // 0x006b3ec0
        joan::orig::as_button(jo::self<M>(this)->snd_btn_7c)->draw(1u, 7);
        joan::orig::as_combo_box(jo::self<M>(this)->snd_combo_80)->draw(1u, static_cast<std::uint8_t>(joan::orig::arg::u32(7u)));
        joan::orig::as_button(jo::self<M>(this)->snd_btn_84)->draw(1u, 7);
        joan::orig::as_button(jo::self<M>(this)->snd_btn_88)->draw(1u, 7);
        joan::orig::as_button(jo::self<M>(this)->snd_btn_8c)->draw(1u, 7);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Panel::enable_74a580", 0x0074a580, &H::panel_74a580);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b109_14.cpp (game/b109_14)
namespace {
namespace game_b109_14 {

using joan::hook::global;
using i8 = std::int8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::hook::x87::Fpu;

using M = jo::OptMenu;
i32 sx16(i16 v) { return static_cast<i32>(v); }

struct alignas(4) Config {  // 0x00459e30 / 0x0045b240 (frame space in the original: 0xb08 bytes)
    u8 bytes[0xb08];
};

JOAN_COUNTER(c_options, "Options::update_74a700");

struct H {
    i32 pressed(void* button, i32 key) {
        return joan::orig::arg::as<i32>(joan::orig::as_ui_button(button)->mouse(joan::orig::arg::i32(key), i32{0}, i32{-1}, i32{0}, i32{0}, i32{0}));  // 0x006218c0
    }

    // -------------------------------------------------------------- 0x0074a700
    template <class W>
    i32 hover(W* M::*off) {
        W* const c = jo::self<M>(this)->*off;
        const i32 r = joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{0}, joan::orig::arg::i32(sx16(jo::self<jo::Button>(c)->y)), i32{0x400}, joan::orig::arg::i32(// 0x00593980
                                    sx16(jo::self<jo::Button>(c)->y2))));
        return r != 0 ? 1 : 0;
    }
    template <class W>
    void hover_pair(W* M::*src, jo::Button* M::*dst, std::uintptr_t setter) {
        (jo::self<M>(this)->*dst)->held = static_cast<u8>(hover(src));
        const i32 h = hover(src);
        thiscall<u32>(setter, jo::self<M>(this)->*src, h);
    }
    i32 changed(jo::ScrollBox* M::*off) { return joan::orig::arg::as<i32>(joan::orig::as_slider(jo::self<M>(this)->*off)->step_760df0()); }  // 0x00760df0
    i32 value(jo::ScrollBox* M::*off) { return joan::orig::arg::as<i32>(joan::orig::as_scroll_box(jo::self<M>(this)->*off)->value()); }    // 0x00761270
    void set_value(jo::ScrollBox* M::*off, i32 v) { joan::orig::as_scroll_box(jo::self<M>(this)->*off)->set_value(joan::orig::arg::u32(v)); }  // 0x00761240
    static float f32(double v) { return static_cast<float>(v); }
    static bool not_c2() { return global<i8>(d::g_game_mode) != 1; }
    static void sound_on(i32 on) { joan::orig::as_grass(obj(d::g_dyn_grass))->set_enabled(static_cast<char>(joan::orig::arg::i32(on))); }  // 0x00464600
    static void music_style(i32 v) {
        if (global<jo::World*>(d::g_world) != nullptr && global<jo::World*>(d::g_world)->scene != nullptr)
            joan::orig::as_scene(global<jo::World*>(d::g_world)->scene)->set_fog_flag(joan::orig::arg::i32(v));  // 0x005ee140
    }
    u8 options_update() {
        c_options.hit();
        const Fpu fpu{true};
        // The original runs this for VIDEO OPTIONS only. Its widgets are now split between VIDEO OPTIONS (brightness,
        // contrast, gamma) and GRAPHICS OPTIONS (draw distance, dynamic grass x3, shadow; graphics_page.hpp): each page
        // handles its own, so a widget slid off the screen never reacts.
        const bool video = jo::self<jo::OptMenu>(this)->page != joan::optmenu::graphics::kPage;
        const bool graphics = !video;
        if (video) {
            hover_pair(&M::snd_slider_48, &M::snd_btn_4c, fn::code_value(fn::Obj760_set_flag));  // 0x00760b00
            hover_pair(&M::snd_slider_50, &M::snd_btn_54, fn::code_value(fn::Obj760_set_flag));
            hover_pair(&M::snd_slider_58, &M::snd_btn_5c, fn::code_value(fn::Obj760_set_flag));
        } else {
            hover_pair(&M::snd_slider_60, &M::snd_btn_64, fn::code_value(fn::Obj760_set_flag));
            hover_pair(&M::snd_slider_68, &M::snd_btn_6c, fn::code_value(fn::Obj760_set_flag));
            hover_pair(&M::snd_slider_70, &M::snd_btn_74, fn::code_value(fn::Obj760_set_flag));
            hover_pair(&M::snd_combo_78, &M::snd_btn_7c, fn::code_value(fn::Obj6b3_set_flag));  // 0x006b3e50
            hover_pair(&M::snd_combo_80, &M::snd_btn_84, fn::code_value(fn::Obj6b3_set_flag));
        }
        if (video && changed(&M::snd_slider_48) != 0) {
            const i32 v = value(&M::snd_slider_48) - 0x32;
            global<float>(d::g_a58060) = f32(fpu.mul(static_cast<double>(v), global<double>(d::dbl_0_02)));
            joan::orig::as_sys_en(obj(d::g_sys3d))->set_brightness(joan::orig::arg::f32(global<u32>(d::g_a58060)));  // 0x00602a50
            joan::orig::as_d3d(obj(d::g_sys3d))->apply_gamma();                             // 0x00602cd0
            return 1;
        }
        if (video && changed(&M::snd_slider_50) != 0) {
            if (value(&M::snd_slider_50) > 0x32) {
                const i32 v = value(&M::snd_slider_50) - 0x32;
                global<float>(d::g_a58064) =
                    f32(fpu.add(fpu.mul(static_cast<double>(v), global<double>(d::dbl_0_04)), global<double>(d::dbl_1)));
            } else {
                const i32 v = value(&M::snd_slider_50);
                global<float>(d::g_a58064) =
                    f32(fpu.add(fpu.mul(static_cast<double>(v), global<double>(d::dbl_0_01)), global<double>(d::dbl_0_5)));
            }
            joan::orig::as_sys_en(obj(d::g_sys3d))->set_contrast(joan::orig::arg::f32(global<u32>(d::g_a58064)));  // 0x00602ad0
            joan::orig::as_d3d(obj(d::g_sys3d))->apply_gamma();                             // 0x00602cd0
            return 1;
        }
        if (video && changed(&M::snd_slider_58) != 0) {
            if (!(value(&M::snd_slider_58) > 0x32)) {
                const i32 v = value(&M::snd_slider_58);
                global<float>(d::g_a58068) =
                    f32(fpu.add(fpu.mul(static_cast<double>(v), global<double>(d::dbl_0_01)), global<double>(d::dbl_0_5)));
            } else if (!(value(&M::snd_slider_58) > 0x4b)) {
                const i32 v = value(&M::snd_slider_58) - 0x32;
                global<float>(d::g_a58068) =
                    f32(fpu.add(fpu.mul(static_cast<double>(v), global<double>(d::dbl_0_08)), global<double>(d::dbl_1)));
            } else {
                const i32 v = value(&M::snd_slider_58) - 0x4b;
                global<float>(d::g_a58068) =
                    f32(fpu.add(fpu.mul(static_cast<double>(v), global<double>(d::dbl_0_12)), global<double>(d::dbl_3)));
            }
            joan::orig::as_d3d(obj(d::g_sys3d))->set_gamma(joan::orig::arg::f32(global<u32>(d::g_a58068)));  // 0x00602b50
            joan::orig::as_d3d(obj(d::g_sys3d))->apply_gamma();                             // 0x00602cd0
            return 1;
        }
        if (graphics && changed(&M::snd_slider_60) != 0) {
            const i32 v = joan::optmenu::draw_from_slider(value(&M::snd_slider_60));  // the original: step * 0x3c00 / 0x14 + 0x401
            if (not_c2()) global<float>(d::g_far_clip) = static_cast<float>(v);
            global<i32>(d::g_cfg_far_limit) = v;
            return 1;
        }
        if (graphics && joan::orig::arg::as<i32>(joan::orig::as_menu(jo::self<M>(this)->snd_combo_78)->step_6b4280()) != 0) {  // 0x006b4280
            if (jo::view<u32>(jo::self<jo::OptMenu>(this)->snd_combo_78->selected) == 1) {
                if (not_c2()) sound_on(0);
                global<u8>(d::g_cfg_grass_enable) = 0;
            } else {
                if (not_c2()) sound_on(1);
                global<u8>(d::g_cfg_grass_enable) = 1;
            }
            return 1;
        }
        if (graphics && changed(&M::snd_slider_68) != 0) {
            const i32 v = joan::optmenu::grass_from_slider(value(&M::snd_slider_68));  // the original: step * 0x14
            if (v != global<i32>(d::g_cfg_grass_density)) {
                if (not_c2()) joan::orig::as_grass(obj(d::g_dyn_grass))->reserve(joan::orig::arg::i32(v));  // 0x004644d0
                global<i32>(d::g_cfg_grass_density) = v;
                if (v == 0) {
                    joan::orig::as_popup(jo::self<M>(this)->snd_combo_78)->set_text_6b44e0(joan::orig::arg::u32(i32{1}));  // 0x006b44e0
                    if (not_c2()) sound_on(0);
                    global<u8>(d::g_cfg_grass_enable) = 0;
                } else {
                    joan::orig::as_popup(jo::self<M>(this)->snd_combo_78)->set_text_6b44e0(joan::orig::arg::u32(i32{2}));
                    if (not_c2()) sound_on(1);
                    global<u8>(d::g_cfg_grass_enable) = 1;
                }
            }
            return 1;
        }
        if (graphics && changed(&M::snd_slider_70) != 0) {
            global<i32>(d::g_a5805c) = value(&M::snd_slider_70);
            return 1;
        }
        if (graphics && joan::orig::arg::as<i32>(joan::orig::as_menu(jo::self<M>(this)->snd_combo_80)->step_6b4280()) != 0) {  // 0x006b4280
            switch (jo::view<u32>(jo::self<jo::OptMenu>(this)->snd_combo_80->selected) - 1u) {     // jump table 0x0074b25f
                case 0:
                    global<u8>(d::g_a58054) = 0;
                    music_style(0);
                    break;
                case 1:
                    global<u8>(d::g_a58054) = 1;
                    music_style(1);
                    break;
                case 2:
                    global<u8>(d::g_a58054) = 2;
                    music_style(1);
                    break;
                case 3:
                    global<u8>(d::g_a58054) = 3;
                    music_style(1);
                    break;
                default:
                    break;
            }
            return 1;
        }
        if (pressed(jo::self<M>(this)->snd_btn_88, 0) != 0) {
            Config cfg;
            joan::orig::as_config(&cfg)->reset();  // 0x00459e30
            joan::orig::as_config(&cfg)->check_caps();  // 0x0045b240
            if (video) {
                global<u32>(d::g_a58060) = jo::view<u32>(jo::self<jo::Config>(&cfg)->brightness);
                joan::orig::as_sys_en(obj(d::g_sys3d))->set_brightness(joan::orig::arg::f32(global<u32>(d::g_a58060)));  // 0x00602a50
                joan::orig::as_d3d(obj(d::g_sys3d))->apply_gamma();                             // 0x00602cd0
                set_value(&M::snd_slider_48, joan::hook::x87::ftol(fpu.add(
                                    fpu.div(static_cast<double>(global<float>(d::g_a58060)), global<double>(d::dbl_0_02)),
                                    global<double>(d::dbl_9df780))));
            }
            if (graphics) {
                global<u32>(d::g_cfg_far_limit) = jo::self<jo::Config>(&cfg)->draw_distance;
                joan::optmenu::clamp_view_ranges();  // the reset defaults are below the widened ranges (view_range.hpp)
                if (not_c2()) global<float>(d::g_far_clip) = static_cast<float>(global<i32>(d::g_cfg_far_limit));
                set_value(&M::snd_slider_60, joan::optmenu::draw_to_slider(global<i32>(d::g_cfg_far_limit)));  // the original: ([0xa58050] - 0x401) / 0x300
            }
            if (video) {
                global<u32>(d::g_a58064) = jo::view<u32>(jo::self<jo::Config>(&cfg)->contrast);
                joan::orig::as_sys_en(obj(d::g_sys3d))->set_contrast(joan::orig::arg::f32(global<u32>(d::g_a58064)));  // 0x00602ad0
                joan::orig::as_d3d(obj(d::g_sys3d))->apply_gamma();                             // 0x00602cd0
                {
                    const float g = global<float>(d::g_a58064);
                    const float one = global<float>(d::flt_1);
                    if (!(g > one))
                        set_value(&M::snd_slider_50, joan::hook::x87::ftol(fpu.div(fpu.sub(static_cast<double>(global<float>(d::g_a58064)),
                                                                            global<double>(d::dbl_0_5)),
                                                                   global<double>(d::dbl_0_01))));
                    else
                        set_value(&M::snd_slider_50, joan::hook::x87::ftol(fpu.add(
                                            fpu.div(fpu.sub(static_cast<double>(global<float>(d::g_a58064)),
                                                            static_cast<double>(one)),
                                                    global<double>(d::dbl_0_04)),
                                            global<double>(d::dbl_9df780))));
                }
            }
            if (graphics) {
                global<u8>(d::g_cfg_grass_enable) = jo::self<jo::Config>(&cfg)->grass_enabled;
                joan::orig::as_popup(jo::self<M>(this)->snd_combo_78)->set_text_6b44e0(joan::orig::arg::u32((jo::view<i8>(jo::self<jo::Config>(&cfg)->grass_enabled) != 0 ? i32{1} : i32{0}) + 1));  // 0x006b44e0
                if (not_c2()) sound_on(static_cast<i32>(jo::view<i8>(jo::self<jo::Config>(&cfg)->grass_enabled)));
            }
            if (video) {
                global<u32>(d::g_a58068) = jo::view<u32>(jo::self<jo::Config>(&cfg)->gamma);
                joan::orig::as_d3d(obj(d::g_sys3d))->set_gamma(joan::orig::arg::f32(global<u32>(d::g_a58068)));  // 0x00602b50
                joan::orig::as_d3d(obj(d::g_sys3d))->apply_gamma();                             // 0x00602cd0
                {
                    const float g = global<float>(d::g_a58068);
                    const float one = global<float>(d::flt_1);
                    const float three = global<float>(d::flt_3);
                    if (!(g > one))
                        set_value(&M::snd_slider_58, joan::hook::x87::ftol(fpu.div(fpu.sub(static_cast<double>(global<float>(d::g_a58068)),
                                                                            global<double>(d::dbl_0_5)),
                                                                   global<double>(d::dbl_0_01))));
                    else if (!(global<float>(d::g_a58068) > three))
                        set_value(&M::snd_slider_58, joan::hook::x87::ftol(fpu.add(
                                            fpu.div(fpu.sub(static_cast<double>(global<float>(d::g_a58068)),
                                                            static_cast<double>(one)),
                                                    global<double>(d::dbl_0_8)),
                                            global<double>(d::dbl_9df780))));
                    else
                        set_value(&M::snd_slider_58, joan::hook::x87::ftol(fpu.add(
                                            fpu.div(fpu.sub(static_cast<double>(global<float>(d::g_a58068)),
                                                            static_cast<double>(three)),
                                                    global<double>(d::dbl_0_12)),
                                            global<double>(d::dbl_9df740))));
                }
            }
            if (graphics) {
                global<u32>(d::g_cfg_grass_density) = jo::self<jo::Config>(&cfg)->view_distance;
                joan::optmenu::clamp_view_ranges();
                if (not_c2()) joan::orig::as_grass(obj(d::g_dyn_grass))->reserve(joan::orig::arg::i32(global<i32>(d::g_cfg_grass_density)));  // 0x004644d0
                set_value(&M::snd_slider_68, joan::optmenu::grass_to_slider(global<i32>(d::g_cfg_grass_density)));  // the original: view_distance / 0x14
                global<u8>(d::g_a58054) = jo::view<u8>(jo::self<jo::Config>(&cfg)->detail);
                joan::orig::as_popup(jo::self<M>(this)->snd_combo_80)->set_text_6b44e0(joan::orig::arg::u32(static_cast<i32>(global<i8>(d::g_a58054)) + 1));  // 0x006b44e0
                music_style(static_cast<i32>(global<i8>(d::g_a58054)));
                global<u32>(d::g_a5805c) = jo::self<jo::Config>(&cfg)->grass_density_percent;
                set_value(&M::snd_slider_70, global<i32>(d::g_a5805c));
            }
            joan::optmenu::graphics::reset_defaults(this, video ? 1 : joan::optmenu::graphics::kPage);  // graphics_page.hpp
        }
        if (pressed(jo::self<M>(this)->snd_btn_8c, 0x1b) != 0) {
            joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(i32{0})));  // 0x0073cc00
            return 1;
        }
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(Config) == 0xb08, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_14", name, addr, fn)
B("Options::update_74a700", 0x0074a700, &H::options_update);
#undef B

}  // namespace game_b109_14
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::img::str;

using M = jo::OptMenu;

// The widget pointer at layout offset `off` (offsetof(OptMenu, member): the list mixes widget classes).
void* widget_at(const void* self, std::size_t off) {
    return *reinterpret_cast<void* const*>(static_cast<const u8*>(self) + off);
}

void* const kSnd = obj(d::g_sfx_player);

void btn_refresh(void* b) { joan::orig::as_widget(b)->update(); }  // 0x0061f3b0
void snd_play(const char* name) { joan::orig::as_sound(kSnd)->play_def(name); }                        // 0x004ceef0

JOAN_COUNTER(c_slide_btns, "SlidePanel::refresh_btns");

struct H {
    // ------------------------------------------------------------------------ 0x0074b550
    void slide_btns() {
        c_slide_btns.hit();
        const u32 was = jo::self<jo::OptMenu>(this)->snd_slider_48->scale_arg;
        static const std::size_t kOffs[18] = {
            offsetof(M, snd_slider_48), offsetof(M, snd_btn_4c), offsetof(M, snd_slider_50),
            offsetof(M, snd_btn_54), offsetof(M, snd_slider_58), offsetof(M, snd_btn_5c),
            offsetof(M, snd_slider_60), offsetof(M, snd_btn_64), offsetof(M, snd_combo_78),
            offsetof(M, snd_btn_7c), offsetof(M, snd_slider_68), offsetof(M, snd_btn_6c),
            offsetof(M, snd_slider_70), offsetof(M, snd_btn_74), offsetof(M, snd_combo_80),
            offsetof(M, snd_btn_84), offsetof(M, snd_btn_88), offsetof(M, snd_btn_8c)};
        for (i32 k = 0; k < 18; ++k) btn_refresh(widget_at(this, kOffs[k]));
        joan::optmenu::graphics::update(this);  // graphics_page.hpp
        joan::optmenu::remap::update(this);     // remap_set.hpp
        if (was != 0 && jo::self<jo::OptMenu>(this)->snd_slider_48->scale_arg == 0) snd_play(str(d::s_BUTTON_SLIDE_MANY));  // "BUTTON_SLIDE_MANY"
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("SlidePanel::refresh_btns", 0x0074b550, &H::slide_btns);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b104_16.cpp (game/b104_16)
namespace {
namespace game_b104_16 {

using joan::hook::global;
using i8 = std::int8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

using i32 = std::int32_t;

using M = jo::OptMenu;

JOAN_COUNTER(c_layout_items, "Panel::layout_items");

// 0x0061f1b0 through thunk 0x004034e0 (ret 0x14)
void item_mode(void* item, u32 scale_bits, u32 dur) {
    joan::orig::as_widget(item)->scale_to(1, joan::orig::arg::f32(scale_bits), 0.0f, 0, joan::orig::arg::u32(dur));  // 0x0061f1b0
}
// 0x0061e020 through thunk 0x00403454 (ret 0x18)
i32 item_move(void* item, i32 x, i32 y, u32 a3, u32 a4, u32 dur) {
    joan::orig::as_widget(item)->move_to_61e020(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(a3), joan::orig::arg::f32(a4), static_cast<char>(u32{0}), joan::orig::arg::u32(dur));  // 0x0061e020
    return 0;  // move_to returns nothing; Panel::layout_items results are never read (only Panel::set_page)
}

constexpr u32 k6f = 0x40c00000;      // 6.0f
constexpr u32 k4f = 0x40800000;      // 4.0f
constexpr u32 k65535f = 0x477fff00;  // 65535.0f
constexpr u32 k3500f = 0x455ac000;   // 3500.0f
constexpr u32 kM1500f = 0xc4bb8000;  // -1500.0f

struct H {
    // ------------------------------------------------------------------ 0x0074bc50
    i32 layout_items() {
        c_layout_items.hit();
        i32 dy = 0;
        if (static_cast<i32>(global<i8>(d::g_game_mode)) == 9) dy = -0x30;

        item_mode(jo::self<M>(this)->snd_btn_88, k6f, 0x1f4);
        item_move(jo::self<M>(this)->snd_btn_88, 0x183, dy + 0x2c8, k65535f, 0, 0x1f4);
        item_mode(jo::self<M>(this)->snd_btn_8c, k6f, 0x1f4);
        item_move(jo::self<M>(this)->snd_btn_8c, 0x219, dy + 0x2c8, k65535f, 0, 0x1f4);

        // VIDEO OPTIONS (graphics_page.hpp): Resolution, Borderless, Brightness, Contrast, Gamma, VSync, Max FPS,
        // UI Scale. The
        // original's other rows (draw distance, dynamic grass x3, shadow) are on GRAPHICS OPTIONS; the openjoan.ini rows
        // slide in through graphics::video_in. Rows 2..4: the original widgets at their new place.
        namespace gp = joan::optmenu::graphics;
        item_mode(jo::self<M>(this)->snd_btn_4c, k4f, 0x1f4);
        item_move(jo::self<M>(this)->snd_btn_4c, -100, dy + gp::video_row(2), k3500f, kM1500f, 0x1f4);
        item_mode(jo::self<M>(this)->snd_btn_54, k4f, 0x1f4);
        item_move(jo::self<M>(this)->snd_btn_54, -100, dy + gp::video_row(3), k3500f, kM1500f, 0x1f4);
        item_mode(jo::self<M>(this)->snd_btn_5c, k4f, 0x1f4);
        item_move(jo::self<M>(this)->snd_btn_5c, -100, dy + gp::video_row(4), k3500f, kM1500f, 0x1f4);

        item_move(jo::self<M>(this)->title, 0x1cc, dy + gp::kVideoFrameY, k3500f, kM1500f, 0x1f4);  // the panel behind the value column, for 7 rows

        item_mode(jo::self<M>(this)->snd_slider_58, k4f, 0x2bc);
        item_move(jo::self<M>(this)->snd_slider_58, 0x1c0, dy + gp::video_row(4) - 7, k3500f, kM1500f, 0x2bc);
        item_mode(jo::self<M>(this)->snd_slider_50, k4f, 0x2ee);
        item_move(jo::self<M>(this)->snd_slider_50, 0x1c0, dy + gp::video_row(3) - 7, k3500f, kM1500f, 0x2ee);
        item_mode(jo::self<M>(this)->snd_slider_48, k4f, 0x320);
        item_move(jo::self<M>(this)->snd_slider_48, 0x1c0, dy + gp::video_row(2) - 7, k3500f, kM1500f, 0x320);
        gp::video_in(this);
        return 0;  // the last item_move's 0, as before
    }
};

#define B104(name, addr, fn) JOAN_REPLACE("game/b104_16", name, addr, fn)
B104("Panel::layout_items", 0x0074bc50, &H::layout_items);
#undef B104

}  // namespace game_b104_16
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    H* zero_124() {  // 0x0073ba40
        memset_(this, 0, sizeof(jo::OptMenu));  // 0x124
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Struct124::zero", 0x0073ba40, &H::zero_124);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::img::obj;

using joan::img::str;

using joan::call::thiscall;

void* const kMenus = obj(d::g_texture_array);

JOAN_COUNTER(c_optmenu, "OptMenu::load");

jo::Texture* menu_load(const char* name) {
    return joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(kMenus)->load(joan::orig::arg::str(name), i32{1}, joan::orig::arg::u32(i32{0})));  // 0x006099c0
}
u32 menus_get() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kMenus)->mode()); }                // 0x0060a720
void menus_set(u32 v) { joan::orig::as_texture_manager(kMenus)->set_mode_60a740(v); }      // 0x0060a740

struct H {
    // ------------------------------------------------------------------------ 0x0073c740
    u32 optmenu_load() {
        c_optmenu.hit();
        const u32 saved = menus_get();
        menus_set(1);
        if (jo::self<jo::OptMenu>(this)->images[0] == 0) jo::self<jo::OptMenu>(this)->images[0] = menu_load(str(d::s_OPTMENU));        // "OPTMENU"
        if (jo::self<jo::OptMenu>(this)->images[1] == 0) jo::self<jo::OptMenu>(this)->images[1] = menu_load(str(d::s_OPTMENUF));        // "OPTMENUF"
        if (jo::self<jo::OptMenu>(this)->images[2] == 0) jo::self<jo::OptMenu>(this)->images[2] = menu_load(str(d::s_OPTMENUB));        // "OPTMENUB"
        if (jo::self<jo::OptMenu>(this)->images[3] == 0) jo::self<jo::OptMenu>(this)->images[3] = menu_load(str(d::s_COMMELE2));    // "COMMELE2"
        if (jo::self<jo::OptMenu>(this)->images[4] == 0) jo::self<jo::OptMenu>(this)->images[4] = menu_load(str(d::s_COMMELE3));  // "COMMELE3"
        if (static_cast<i32>(global<i8>(d::g_game_mode)) == 9 && jo::self<jo::OptMenu>(this)->images[5] == 0)
            jo::self<jo::OptMenu>(this)->images[5] = menu_load(str(d::s_MENU_B));  // "MENU_B"
        menus_set(saved);
        return 0;  // Textures::set_mode returns nothing; only caller Opt::open 0x0073c9d0, whose result is never read
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("OptMenu::load", 0x0073c740, &H::optmenu_load);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b92_06_2.cpp (game/b92_06)
namespace {
namespace game_b92_06_2 {

using joan::hook::global;

using u32 = std::uint32_t;

using i16 = std::int16_t;

using joan::img::obj;

using joan::img::str;

using joan::call::thiscall;

void* const kImages = obj(d::g_texture_array);
void* const kSounds = obj(d::g_sfx_player);

void image_release(void* h) { joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(h)); }                    // 0x0060a550
// 0x0061f1b0 (ret 0x14), raw dwords as pushed by the original.
using joan::orig::helpers::anim_set;
// 0x0061e020 (ret 0x18)
void anim_move(void* o, std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t e,
               std::uint32_t f) {
    joan::orig::as_widget(o)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(f));
}

JOAN_COUNTER(c_slider, "Slider::open");

struct H {
    static void release_at(jo::Texture*& image) {
        if (image != nullptr) {
            image_release(image);
            image = nullptr;
        }
    }

    // 0x0073c8a0
    void release6() {
        for (std::int32_t i = 0; i < 6; ++i) release_at(jo::self<jo::OptMenu>(this)->images[i]);
    }

    // 0x007433d0
    void slider_open() {
        c_slider.hit();
        const std::uint32_t d = global<std::int8_t>(d::g_game_mode) == 9 ? 0xffffffd0u : 0u;
        if (jo::self<jo::OptMenu>(this)->slide_state != 0)
            joan::orig::as_sound(kSounds)->play_def(str(d::s_BUTTON_SLIDE_MANY));  // 0x004ceef0 "BUTTON_SLIDE_MANY"
        anim_set(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_btn_119), 0, 0x40800000u, 0, 0, 0);
        anim_move(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_btn_119), 0xfffffd9cu, d + 0xf3u, 0x455ac000u, 0xc4bb8000u, 0, 0);
        anim_set(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_combo_ec), 0, 0x40800000u, 0, 0, 0);
        anim_move(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_btn_f0), 0xfffffd9cu, d + 0x127u, 0x455ac000u, 0xc4bb8000u, 0, 0);
        anim_set(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_btn_11d), 0, 0x40c00000u, 0, 0, 0);
        anim_set(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_combo_115), 0, 0x40800000u, 0, 0, 0);
        anim_move(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_combo_115), 0x1c0u, d + 0x3eu, 0x455ac000u, 0xc4bb8000u, 0, 0);
        anim_set(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_combo_ec), 0, 0x40800000u, 0, 0, 0x32u);
        anim_move(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_combo_ec), 0x1c0u, d + 0x3eu, 0x455ac000u, 0xc4bb8000u, 0, 0x32u);
        anim_move(jo::view<void*>(jo::self<jo::OptMenu>(this)->title), 0x1ccu, d - 0x1c0u, 0x455ac000u, 0xc4bb8000u, 0, 0x64u);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Images6::release", 0x0073c8a0, &H::release6);
B("Slider::open", 0x007433d0, &H::slider_open);
#undef B

}  // namespace game_b92_06_2
}  // namespace

// ---------------------------------------- from modules/game_b108_14.cpp (game/b108_14)
namespace {
namespace game_b108_14 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::call::thiscall;
using M = jo::OptMenu;
i32 ftol(float v) { return joan::hook::x87::ftol(static_cast<double>(v)); }

JOAN_COUNTER(c_opt_open, "Opt::open_73c9d0");
JOAN_COUNTER(c_opt_apply, "Opt::apply_73ca80");
JOAN_COUNTER(c_opt_click, "Opt::click_742c80");

struct H {
    // ------------------------------------------------------------------------ 0x0073c9d0
    u32 opt_open(u8 mode) {
        c_opt_open.hit();
        jo::view<u8>(jo::self<jo::Opt>(this)->slide_state) = 1;
        jo::self<jo::Opt>(this)->slide_flag = mode;
        joan::orig::as_screen(this)->layout_73ded0();  // 0x0073ded0
        if (global<i8>(d::g_game_mode) == 9) joan::orig::as_ingame_frame(obj(d::g_hud))->slide_in();  // 0x006f24d0
        joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(i32{0})));        // 0x0073cc00
        u32 r = joan::orig::arg::as<u32>(joan::orig::as_opt_menu(this)->load());       // 0x0073c740
        if (global<u8>(d::g_game_running) != 0) {
            global<u8>(d::g_game_running) = 0;
            global<i16>(d::g_saved_game_speed) = static_cast<i16>(ftol(global<float>(d::g_game_speed)));
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(0u);  // 0x0042f930
            r = 0;  // set_speed returns nothing; Opt::open results are never read
        }
        return r;
    }

    // ------------------------------------------------------------------------ 0x0073ca80
    void opt_apply() {
        c_opt_apply.hit();
        if (jo::self<jo::Opt>(this)->slide_state == 0) return;
        joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(i32{6})));  // 0x0073cc00
        void* const sel = obj(d::g_player_unit_recno);
        const i32 v = jo::self<jo::OptMenu>(this)->key_combo_c0->selected;
        if (global<i32>(d::g_control_mode) == 4) {
            switch (v) {
                case 1:
                    global<u32>(d::g_control_base_mode) = 0;
                    global<u32>(d::g_cfg_control_scheme) = 0;
                    break;
                case 2:
                    global<u32>(d::g_control_base_mode) = 3;
                    global<u32>(d::g_cfg_control_scheme) = 3;
                    break;
                case 3:
                    global<u32>(d::g_control_base_mode) = 1;
                    global<u32>(d::g_cfg_control_scheme) = 1;
                    break;
                default: break;
            }
        } else {
            switch (v) {
                case 1:
                    joan::orig::as_unit(sel)->set_state(i32{0}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x0042e040
                    global<u32>(d::g_cfg_control_scheme) = 0;
                    break;
                case 2:
                    joan::orig::as_unit(sel)->set_state(i32{3}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));
                    global<u32>(d::g_cfg_control_scheme) = 3;
                    break;
                case 3:
                    joan::orig::as_unit(sel)->set_state(i32{1}, static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));
                    global<u32>(d::g_cfg_control_scheme) = 1;
                    break;
                default: break;
            }
        }
        if (jo::view<i8>(jo::self<jo::Opt>(this)->slide_flag) == 0) joan::orig::as_unit(sel)->state_sound();  // 0x0042fef0
    }

    // ------------------------------------------------------------------------ 0x00742c80
    i32 hit_box(jo::ComboBox* M::*off) {
        void* const b = jo::self<M>(this)->*off;
        return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{0}, static_cast<i32>(jo::self<jo::Button>(b)->y), i32{0x400}, static_cast<i32>(jo::self<jo::Button>(jo::self<M>(this)->*off)->y2)));  // 0x00593980
    }
    u8 opt_click() {
        c_opt_click.hit();
        jo::self<M>(this)->video_btn_119->held = hit_box(&M::video_combo_115) != 0 ? 1 : 0;
        joan::orig::as_combo_box(jo::self<M>(this)->video_combo_115)->set_highlight(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i32>(hit_box(&M::video_combo_115) != 0 ? 1 : 0))));  // 0x006b3e50
        jo::self<M>(this)->video_btn_f0->held = hit_box(&M::video_combo_ec) != 0 ? 1 : 0;
        joan::orig::as_combo_box(jo::self<M>(this)->video_combo_ec)->set_highlight(static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i32>(hit_box(&M::video_combo_ec) != 0 ? 1 : 0))));  // 0x006b3e50
        if (joan::orig::arg::as<i32>(joan::orig::as_menu(jo::self<M>(this)->video_combo_115)->step_6b4280()) != 0) {  // 0x006b4280
            const i32 v = jo::self<jo::OptMenu>(this)->video_combo_115->selected;
            global<u8>(d::g_difficulty) = v == 1 ? 0 : v == 2 ? 1 : v == 3 ? 2 : 3;
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_menu(jo::self<M>(this)->video_combo_ec)->step_6b4280()) != 0) {  // 0x006b4280
            global<u8>(d::g_a5802d) = static_cast<u8>(jo::self<jo::OptMenu>(this)->video_combo_ec->selected - 1);
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::self<M>(this)->video_btn_11d)->mouse(i32{0x1b}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) !=
            0) {                                       // 0x006218c0
            joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(i32{0})));  // 0x0073cc00
            return 1;
        }
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_14", name, addr, fn)
B("Opt::open_73c9d0", 0x0073c9d0, &H::opt_open);
B("Opt::apply_73ca80", 0x0073ca80, &H::opt_apply);
B("Opt::click_742c80", 0x00742c80, &H::opt_click);
#undef B

}  // namespace game_b108_14
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::img::str;

using joan::call::thiscall;

void* const kSound = obj(d::g_sfx_player);

JOAN_COUNTER(c_panel_741960, "Panel::update_741960");
JOAN_COUNTER(c_set_page, "Panel::set_page");

// 0x0061f1b0 (thunk 0x004034e0), ret 0x14 / 0x0061e020 (thunk 0x00403454), ret 0x18.
void anim(void* button, u32 on, u32 speed, u32 a, u32 b, u32 c) {
    joan::orig::as_widget(button)->scale_to(joan::orig::arg::i16(on), joan::orig::arg::f32(speed), joan::orig::arg::f32(a), static_cast<char>(b), joan::orig::arg::u32(c));
}
u32 slide(void* button, u32 x, u32 y, u32 f1, u32 f2, u32 flag, u32 delay) {
    joan::orig::as_widget(button)->move_to_61e020(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(f1), joan::orig::arg::f32(f2), static_cast<char>(flag), joan::orig::arg::u32(delay));  // 0x0061e020
    return 0;  // move_to returns nothing; Panel::set_page results are never read
}

struct H {
    // ------------------------------------------------------------------ 0x00741960
    u32 update_741960() {
        c_panel_741960.hit();
        const u32 slide_before = jo::view<u32>(jo::self<jo::OptMenu>(this)->game_btn_94->scale_arg);
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_98))->update();  // 0x0061f3b0
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_9c))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_slider_a0))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_a4))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_slider_b0))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_b4))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_slider_a8))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_ac))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_combo_90))->update();
        u32 eax = joan::orig::arg::as<u32>(joan::orig::as_widget(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_94))->update());
        if (slide_before != 0 && jo::view<u32>(jo::self<jo::OptMenu>(this)->game_btn_94->scale_arg) == 0)
            eax = joan::orig::arg::as<u32>(joan::orig::as_sound(kSound)->play_def(str(d::s_BUTTON_SLIDE_MANY)));  // 0x004ceef0 "BUTTON_SLIDE_MANY"
        return eax;
    }

    // ------------------------------------------------------------------ 0x0073cc00
    u32 set_page(char page) {
        c_set_page.hit();
        const i32 shift = global<i8>(d::g_game_mode) == 9 ? -0x30 : 0;
        const u32 y = static_cast<u32>(shift + 0x3e);
        const u32 left = 0x455ac000u, right = 0xc4bb8000u, speed = 0x40c00000u;
        u32 eax;
        const i32 current = jo::self<jo::OptMenu>(this)->page;
        switch (current) {
        case 0:
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_0), 0, speed, 0, 0, 0);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_0), 0x100, y, left, right, 0, 0);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_1), 0, speed, 0, 0, 0x32);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_1), 0x100, y, left, right, 0, 0x32);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_2), 0, speed, 0, 0, 0x64);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_2), 0x100, y, left, right, 0, 0x64);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_3), 0, speed, 0, 0, 0x96);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_3), 0x100, y, left, right, 0, 0x96);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_4), 0, speed, 0, 0, 0);
            joan::optmenu::graphics::tab_out(this, 0x19);  // GRAPHICS OPTIONS (graphics_page.hpp)
            eax = joan::orig::arg::as<u32>(joan::orig::as_sound(kSound)->play_def(str(d::s_BUTTON_SLIDE_MANY)));  // 0x004ceef0 "BUTTON_SLIDE_MANY"
            break;
        case 1:
        case joan::optmenu::graphics::kPage:  // GRAPHICS OPTIONS shares VIDEO OPTIONS' widgets and slide-out
            joan::orig::as_panel(this)->slide_in_74c2e0();  // 0x0074c2e0
            joan::optmenu::graphics::save();  // the openjoan.ini rows changed on the page
            eax = 0;  // returns nothing; set_page results are never read (every caller discards them)
            break;
        case 2:
            joan::orig::as_panel(this)->slide_in_7420e0();  // 0x007420e0
            eax = 0;  // returns nothing; set_page results are never read
            break;
        case 3:
            joan::orig::as_panel(this)->slide_in_7480e0();  // 0x007480e0
            eax = 0;  // returns nothing; set_page results are never read
            break;
        case 4:
            joan::orig::as_slider(this)->open();  // 0x007433d0
            eax = 0;  // Slider::open returns nothing; set_page results are never read
            break;
        case 5:
            eax = joan::orig::arg::as<u32>(joan::orig::as_menu(this)->slide_in_748830());  // 0x00748830
            break;
        default:
            eax = static_cast<u32>(static_cast<i32>(global<i8>(d::g_game_mode)));
            break;
        }
        jo::view<char>(jo::self<jo::OptMenu>(this)->page) = page;
        eax = (eax & 0xffffff00u) | static_cast<u8>(page);
        switch (static_cast<i32>(page)) {
        case 0: {
            // Five buttons now (graphics_page.hpp kTabY): VIDEO, GRAPHICS, AUDIO, INPUT, GAME; the original's four sat at
            // 0x140 / 0x190 / 0x1e0 / 0x230.
            namespace gp = joan::optmenu::graphics;
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_3), 1, speed, 0, 0, 0x1f4);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_3), 0x100, static_cast<u32>(shift + gp::kTabY[4]), left, right, 0, 0x1f4);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_2), 1, speed, 0, 0, 0x226);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_2), 0x100, static_cast<u32>(shift + gp::kTabY[3]), left, right, 0, 0x226);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_1), 1, speed, 0, 0, 0x258);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_1), 0x100, static_cast<u32>(shift + gp::kTabY[2]), left, right, 0, 0x258);
            gp::tab_in(this, 0x271);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_0), 1, speed, 0, 0, 0x28a);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_0), 0x100, static_cast<u32>(shift + gp::kTabY[0]), left, right, 0, 0x28a);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_4), 1, speed, 0, 0, 0x1f4);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_4), 0x1ce, static_cast<u32>(shift + 0x2c8), 0x477fff00u, 0, 0, 0x1f4);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->frame_top), 0, static_cast<u32>(shift + 0xb4), left, right, 0, 0xfa);
            return slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->frame_bottom), 0, static_cast<u32>(shift + 0x2c8), left, right, 0, 0xfa);
        }
        case 1:
            return joan::orig::arg::as<u32>(joan::orig::as_panel(this)->layout_items());  // 0x0074bc50
        case joan::optmenu::graphics::kPage:
            joan::optmenu::graphics::graphics_in(this);  // graphics_page.hpp
            return 0;
        case 2:
            return joan::orig::arg::as<u32>(joan::orig::as_opt_menu(this)->layout_741cd0());  // 0x00741cd0
        case 3:
            return joan::orig::arg::as<u32>(joan::orig::as_opt_menu(this)->layout_747bc0());  // 0x00747bc0
        case 4:
            return joan::orig::arg::as<u32>(joan::orig::as_panel(this)->layout_743170());  // 0x00743170
        case 5:
            return joan::orig::arg::as<u32>(joan::orig::as_opt_menu(this)->layout_credits());  // 0x00748540
        case 6:
            joan::optmenu::graphics::tab_out(this, 0x19);  // graphics_page.hpp
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_4), 0, speed, 0, 0, 0);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_0), 0, speed, 0, 0, 0);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_0), 0x100, y, left, right, 0, 0);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_1), 0, speed, 0, 0, 0x32);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_1), 0x100, y, left, right, 0, 0x32);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_2), 0, speed, 0, 0, 0x64);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_2), 0x100, y, left, right, 0, 0x64);
            anim(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_3), 0, speed, 0, 0, 0x96);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_3), 0x100, y, left, right, 0, 0x96);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->frame_top), 0xfffffc00u, static_cast<u32>(shift + 0xb4), left, right, 0, 0xc8);
            slide(jo::view<void*>(jo::self<jo::OptMenu>(this)->frame_bottom), 0x400, static_cast<u32>(shift + 0x2c8), left, right, 0, 0xc8);
            return joan::orig::arg::as<u32>(joan::orig::as_sound(kSound)->play_def(str(d::s_BUTTON_SLIDE_MANY)));
        default:
            return eax;
        }
    }
};

JOAN_REPLACE("game/b108_07", "Panel::set_page", 0x0073cc00, &H::set_page);
JOAN_REPLACE("game/b108_07", "Panel::update_741960", 0x00741960, &H::update_741960);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b116_14.cpp (game/b116_14)
namespace {
namespace game_b116_14 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using u16 = std::uint16_t;

// Not joan::call::thiscall: a __thiscall function pointer, not a pointer to member (a class-type R is returned
// as a free function returns it); the target is an fn:: name or a computed address (see src/call.hpp).
template <class R = void, class T, class... A>
R thiscall(T address, const void* self, A... args) {
    return reinterpret_cast<R(JOAN_THISCALL*)(void*, A...)>(joan::fn::target(address))(const_cast<void*>(self), args...);
}
i32 sx(i16 v) { return static_cast<i32>(v); }

constexpr u32 kOne = 0x3f800000u;  // 1.0f pushed as an immediate
constexpr img::Addr kMouse = d::g_mouse;
constexpr img::Addr kTextRes = d::g_strings;  // localised text resource

JOAN_COUNTER(c_option, "OptionMenu::draw");

// 0x0042a260 colour ctor (thiscall (r, g, b, a), ret 0x10) into `slot`; returns the dword.
u32 make_colour(u32* slot, u32 r, u32 g, u32 b, u32 a) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
}

// 0x00629bd0 (frame draw, ret 8) / 0x00622cc0 (button draw, ret 8) with (a, 7).
void frame(void* f, i32 a) { joan::orig::as_label(f)->draw(joan::orig::arg::u32(a), joan::orig::arg::u32(i32{7})); }   // 0x00629bd0
void button(void* b, i32 a) { joan::orig::as_button(b)->draw(joan::orig::arg::u32(a), 7); }  // 0x00622cc0

struct H {
    // -----------------------------------------------------------------------------------------
    // 0x0073d2c0 OptionMenu::draw(char)
    std::uintptr_t option_draw(u32 a) {
        joan::canvas::ScreenScope canvas_scope(0x0073d2c0);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        if (jo::self<jo::OptMenu>(this)->slide_state == 0) return reinterpret_cast<std::uintptr_t>(this);
        c_option.hit();
        joan::orig::as_cursor(obj(kMouse))->reset_pos();  // 0x00592c30
        if (global<i8>(d::g_game_mode) == 9) joan::orig::as_menu(obj(d::g_hud))->reset_fades(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x006f21c0
        frame(jo::view<void*>(jo::self<jo::OptMenu>(this)->title), 2);
        frame(jo::view<void*>(jo::self<jo::OptMenu>(this)->frame_top), 2);
        std::uintptr_t title = fn::code_value(fn::OptMenu_open_22);  // 0x004db580 (page 0 and default)
        switch (static_cast<u32>(static_cast<i32>(jo::self<jo::OptMenu>(this)->page))) {
            case 1: title = fn::code_value(fn::OptMenu_open_23); break;  // 0x004db5b0
            case 2: title = fn::code_value(fn::OptMenu_open_24); break;  // 0x004db5e0
            case 3: title = fn::code_value(fn::OptMenu_open_25); break;  // 0x004db610
            case 4: title = fn::code_value(fn::OptMenu_open_26); break;  // 0x004db640
            default: break;
        }
        u32 slot = 0;
        const u32 colour = make_colour(&slot, 0xffu, 0xffu, 0xffu, 0xffu);
        const char* const text = static_cast<i32>(jo::self<jo::OptMenu>(this)->page) == joan::optmenu::graphics::kPage
                                     ? joan::optmenu::graphics::title()  // graphics_page.hpp
                                     : thiscall<const char*>(title, obj(kTextRes));
        const u8* b = jo::view<u8*>(jo::self<jo::OptMenu>(this)->frame_top);
        const i32 y2 = sx(jo::self<jo::Image2D>(b)->y) + 0x1e;
        const i32 x2 = sx(jo::self<jo::Image2D>(b)->x) + 0x400;
        const u32 hi = static_cast<u32>(x2) & 0xffff0000u;
        const u32 y1 = hi | static_cast<u16>(jo::self<jo::Image2D>(b)->y);
        const u32 x1 = hi | static_cast<u16>(jo::self<jo::Image2D>(b)->x);
        joan::orig::as_text(obj(d::g_font_goudy_bold_18))->draw_3(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(i32{2}), text, joan::orig::arg::u32(colour), i32{0}, i32{1}, i32{2}, joan::orig::arg::i32(// 0x00626130
                      i32{0}), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
        frame(jo::view<void*>(jo::self<jo::OptMenu>(this)->frame_bottom), 2);
        button(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_0), 2);
        button(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_1), 2);
        button(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_2), 2);
        button(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_3), 2);
        button(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_4), 2);
        const u32 c = static_cast<u8>(a);
        joan::orig::as_opt_menu(this)->draw_745720(joan::orig::arg::u32(c));          // 0x00745720
        joan::orig::as_panel(this)->enable_all(joan::orig::arg::u32(c));          // 0x007412e0
        joan::orig::as_panel(this)->enable_74a580(joan::orig::arg::u32(c));          // 0x0074a580
        joan::optmenu::graphics::draw(this);  // GRAPHICS OPTIONS button and the openjoan.ini rows (graphics_page.hpp)
        joan::optmenu::remap::draw(this);     // the key bindings page's Keyboard / Gamepad button (remap_set.hpp)
        return joan::orig::arg::as<u32>(joan::orig::as_panel(this)->set_mode_742bf0(joan::orig::arg::u32(c)));   // 0x00742bf0
    }
};

JOAN_REPLACE("game/b116_14", "OptionMenu::draw", 0x0073d2c0, &H::option_draw);
JOAN_CANVAS_SELF(0x0073d2c0);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_14
}  // namespace

// ---------------------------------------- from modules/game_b111_10.cpp (fix/uninit)
namespace {
namespace game_b111_10 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::img::str;

using joan::call::thiscall;
using M = jo::OptMenu;

JOAN_COUNTER(c_settings_click, "Settings::click_73d830");

struct H {
    // ------------------------------------------------------------------ 0x0073d830
    // fix/uninit: on page 5 the original maps the combo selection (+0xc0)->+0x80 1/2/3 to
    // 0/3/1 in [ebp-0x8] and passes it to 0x004cb410, leaving the local uninitialised for any
    // other selection. 0x004cb410 acts only on 0, 1 and 3, and the sibling 0x0073ca80 does nothing
    // for other selections, so the value used is -1 (no effect). The combo holds 2 or 3 items and
    // is always set to 1..3 by its setup (0x00744xxx), so this path is not expected to occur.
    u8 settings_click() {
        c_settings_click.hit();
        if (jo::self<jo::OptMenu>(this)->slide_state == 0) return 0;
        if (jo::self<jo::OptMenu>(this)->page == 0) {
            struct Btn {
                jo::Button* M::*btn;
                i32 code, page;
            };
            const Btn btns[4] = {{&M::tab_button_0, 0, 1}, {&M::tab_button_1, 0, 2}, {&M::tab_button_3, 0, 4}, {&M::tab_button_2, 0, 3}};
            for (const Btn& b : btns) {
                if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::self<M>(this)->*b.btn)->mouse(joan::orig::arg::i32(b.code), i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
                    joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_IF_SETTING_BUTTON_CLICK));  // 0x004ceef0 "IF_SETTING_BUTTON_CLICK"
                    joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(b.page)));                      // 0x0073cc00
                    return 1;
                }
            }
            if (joan::optmenu::graphics::tab_clicked(this)) {  // GRAPHICS OPTIONS (graphics_page.hpp)
                joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_IF_SETTING_BUTTON_CLICK));  // 0x004ceef0
                joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(joan::optmenu::graphics::kPage)));  // 0x0073cc00
                return 1;
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_4))->mouse(i32{0x1b}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {
                if (jo::view<i8>(jo::self<jo::OptMenu>(this)->slide_flag) != 0) global<u32>(d::g_screen_pending) = 1;
                joan::orig::as_opt_menu(this)->apply();  // 0x0073ca80
                return 1;
            }
        }
        if (jo::self<jo::OptMenu>(this)->page != 6 && jo::self<jo::OptMenu>(this)->key_slot < 0 && joan::orig::arg::as<i32>(joan::orig::as_mode(obj(d::g_screen_mgr))->check_6f5a70()) != 0) {  // 0x006f5a70
            if (jo::self<jo::OptMenu>(this)->page == 5) {
                const i32 mode = joan::optmenu::remap::mode(this);  // the set shown (remap_set.hpp)
                joan::orig::as_bindings(obj(d::g_keys_a48820))->restore(joan::orig::arg::i32(mode));  // 0x004cb410
            }
            joan::orig::as_opt_menu(this)->apply();  // 0x0073ca80
            jo::self<jo::OptMenu>(this)->slide_flag = 0;
            return 1;
        }
        switch (jo::self<jo::OptMenu>(this)->page) {
            case 1:
            case joan::optmenu::graphics::kPage:  // both pages run the video widgets' input (gated by page in 0x0074a700)
                if (joan::optmenu::graphics::input(this, jo::self<jo::OptMenu>(this)->page)) break;  // graphics_page.hpp
                joan::orig::as_opt_menu(this)->update_page_1();  // 0x0074a700
                break;
            case 2: joan::orig::as_opt_menu(this)->update_page_2(); break;  // 0x007413e0
            case 3:
            case 5: joan::orig::as_opt_menu(this)->handle_input_745b10(); break;  // 0x00745b10
            case 4: joan::orig::as_opt_menu(this)->update_page_4(); break;  // 0x00742c80
            default: break;
        }
        return 1;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// [ebp-0x8] mode for a combo selection outside 1..3 -> -1 (0x004cb410 does nothing).
FIX("Settings::click_73d830", 0x0073d830, &H::settings_click);
#undef FIX

}  // namespace game_b111_10
}  // namespace

// ---------------------------------------- from modules/game_b108_17_2.cpp (game/b108_17)
namespace {
namespace game_b108_17_2 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::img::str;

using joan::call::thiscall;
using M = jo::OptMenu;

void* const kSnd = obj(d::g_sfx_player);
void* const kKeys = obj(d::g_mouse);

void btn_refresh(void* b) { joan::orig::as_widget(b)->update(); }  // 0x0061f3b0
i32 btn_clicked(void* b, i32 a, i32 c, i32 d, i32 e, i32 f, i32 g) {
    return joan::orig::arg::as<i32>(joan::orig::as_ui_button(b)->mouse(joan::orig::arg::i32(a), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g)));  // 0x006218c0
}
u32 keys_93980(i32 a, i32 b, i32 c, i32 d) { return joan::orig::arg::as<u32>(joan::orig::as_point(kKeys)->inside(joan::orig::arg::i32(a), joan::orig::arg::i32(b), joan::orig::arg::i32(c), joan::orig::arg::i32(d))); }  // 0x00593980
void snd_play(const char* name) { joan::orig::as_sound(kSnd)->play_def(name); }                        // 0x004ceef0

JOAN_COUNTER(c_slide_update, "SlidePanel::update");
JOAN_COUNTER(c_opt_handle, "OptPanel::handle");

struct H {
    // ------------------------------------------------------------------------ 0x0073db40
    void slide_close_reset() {
        if (global<u8>(d::g_game_running) == 0) {
            global<u8>(d::g_game_running) = 1;
            const float v = static_cast<float>(static_cast<i32>(global<i16>(d::g_saved_game_speed)));
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(v));  // 0x0042f930
            joan::orig::as_cursor(kKeys)->set_pos();                // 0x00592bd0
        }
    }
    void slide_update() {
        c_slide_update.hit();
        if (jo::self<jo::OptMenu>(this)->slide_state == 0) return;
        const u32 was = jo::view<u32>(jo::self<jo::OptMenu>(this)->tab_button_3->move_arg);
        btn_refresh(jo::view<void*>(jo::self<jo::OptMenu>(this)->title));
        btn_refresh(jo::view<void*>(jo::self<jo::OptMenu>(this)->frame_top));
        btn_refresh(jo::view<void*>(jo::self<jo::OptMenu>(this)->frame_bottom));
        btn_refresh(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_0));
        btn_refresh(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_1));
        btn_refresh(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_2));
        btn_refresh(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_3));
        btn_refresh(jo::view<void*>(jo::self<jo::OptMenu>(this)->tab_button_4));
        joan::orig::as_opt_menu(this)->release_all();  // 0x00746ff0
        joan::orig::as_panel(this)->update_741960();  // 0x00741960
        joan::orig::as_opt_menu(this)->refresh_buttons();  // 0x0074b550
        joan::orig::as_panel(this)->update_742ee0();  // 0x00742ee0
        if (was != 0 && jo::view<u32>(jo::self<jo::OptMenu>(this)->tab_button_3->move_arg) == 0) {
            const i32 want = joan::optmenu::graphics::kTabY[4] + (global<i8>(d::g_game_mode) != 9 ? 0 : -0x30);  // the GAME button on the OPTIONS list (graphics_page.hpp)
            if (static_cast<i32>(jo::self<jo::OptMenu>(this)->tab_button_3->target_y) == want) snd_play(str(d::s_BUTTON_SLIDE_MANY));  // "BUTTON_SLIDE_MANY"
        }
        if (jo::self<jo::OptMenu>(this)->page != 6) return;
        i32 off = 0;
        if (global<i8>(d::g_game_mode) == 9) off = -0x30;
        if (static_cast<i32>(jo::self<jo::OptMenu>(this)->tab_button_3->y) != off + 0x3e) return;
        const float pos = jo::self<jo::OptMenu>(this)->tab_button_3->scale;
        const float zero = global<float>(d::k_9cf5c4);
        if (pos < zero || pos > zero) return;  // test ah,0x40: continue when equal or unordered
        if (static_cast<i32>(jo::self<jo::OptMenu>(this)->frame_top->x) != -0x400) return;
        if (static_cast<i32>(jo::self<jo::OptMenu>(this)->frame_bottom->x) != 0x400) return;
        if (global<i8>(d::g_game_mode) == 9) {
            if (global<i32>(d::g_screen_pending) != 0 && global<i32>(d::g_screen_pending) != 2) {
                jo::view<u8>(jo::self<jo::OptMenu>(this)->slide_state) = 0;
                jo::view<u8>(jo::self<jo::OptMenu>(this)->page) = 0;
                joan::orig::as_opt_menu(this)->free_images();  // 0x0073c8a0
                jo::self<jo::OptMenu>(this)->slide_flag = 0;
                joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
                return;
            }
            if (global<i8>(d::g_menu_frame_state) == 2) {
                joan::orig::as_menu(obj(d::g_hud))->slide_out();  // 0x006f26e0
                return;
            }
            if (global<i8>(d::g_menu_frame_state) != 0) return;
            jo::view<u8>(jo::self<jo::OptMenu>(this)->slide_state) = 0;
            jo::view<u8>(jo::self<jo::OptMenu>(this)->page) = 0;
            joan::orig::as_opt_menu(this)->free_images();  // 0x0073c8a0
            slide_close_reset();
            jo::self<jo::OptMenu>(this)->slide_flag = 0;
            if (global<i32>(d::g_screen_pending) == 2)
                joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
            else
                joan::orig::as_triple(obj(d::g_screen_mgr))->clear();  // 0x006f5e90
            return;
        }
        jo::view<u8>(jo::self<jo::OptMenu>(this)->slide_state) = 0;
        jo::view<u8>(jo::self<jo::OptMenu>(this)->page) = 0;
        joan::orig::as_opt_menu(this)->free_images();  // 0x0073c8a0
        slide_close_reset();
        jo::self<jo::OptMenu>(this)->slide_flag = 0;
    }

    // ------------------------------------------------------------------------ 0x007413e0
    template <class W>
    u32 hover(W* M::*off) {  // 0x00593980(0, w +0xb, 0x400, w +0xf) != 0
        W* const b = jo::self<M>(this)->*off;
        return keys_93980(0, jo::self<jo::Button>(b)->y, 0x400, jo::self<jo::Button>(b)->y2) != 0 ? 1u : 0u;
    }
    static i32 flag2() { return global<i8>(d::g_flag2) != 0 ? 2 : 0; }
    u8 opt_handle() {
        c_opt_handle.hit();
        jo::self<jo::OptPanel>(this)->game_btn_94->held = static_cast<u8>(hover(&M::game_combo_90));
        joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_combo_90))->set_highlight(static_cast<std::int8_t>(joan::orig::arg::i32(hover(&M::game_combo_90))));  // 0x006b3e50
        jo::self<jo::OptPanel>(this)->game_btn_a4->held = static_cast<u8>(hover(&M::game_slider_a0));
        joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_a0))->set_highlight(static_cast<std::int8_t>(joan::orig::arg::i32(hover(&M::game_slider_a0))));  // 0x00760b00
        jo::self<jo::OptPanel>(this)->game_btn_ac->held = static_cast<u8>(hover(&M::game_slider_a8));
        joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_a8))->set_highlight(static_cast<std::int8_t>(joan::orig::arg::i32(hover(&M::game_slider_a8))));  // 0x00760b00
        jo::self<jo::OptPanel>(this)->game_btn_b4->held = static_cast<u8>(hover(&M::game_slider_b0));
        joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_b0))->set_highlight(static_cast<std::int8_t>(joan::orig::arg::i32(hover(&M::game_slider_b0))));  // 0x00760b00
        if (joan::orig::arg::as<i32>(joan::orig::as_menu(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_combo_90))->step_6b4280()) != 0) {  // 0x006b4280
            global<u8>(d::g_flag2) = jo::self<jo::OptPanel>(this)->game_combo_90->selected == 1 ? 0 : 1;
            joan::orig::as_sound(obj(d::g_sound3d_system))->set_distance_model(static_cast<char>(joan::orig::arg::i32(flag2())));  // 0x00454000
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_slider(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_a8))->step_760df0()) != 0) {                                // 0x00760df0
            global<i16>(d::g_sfx_volume) = joan::orig::arg::as<i16>(joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_a8))->value());              // 0x00761270
            joan::orig::as_party(kSnd)->set_scale(static_cast<i32>(global<i16>(d::g_sfx_volume)));     // 0x0045f6b0
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_slider(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_b0))->step_760df0()) != 0) {                                    // 0x00760df0
            global<i16>(d::g_a57ff4) = joan::orig::arg::as<i16>(joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_b0))->value());                  // 0x00761270
            joan::orig::as_volume(obj(d::g_mus))->set(static_cast<i32>(global<i16>(d::g_a57ff4)));  // 0x00586620
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_slider(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_a0))->step_760df0()) != 0) {                                    // 0x00760df0
            global<i16>(d::g_music_volume) = joan::orig::arg::as<i16>(joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_a0))->value());                  // 0x00761270
            joan::orig::as_sound(obj(d::g_snd))->set_volume_59b150(static_cast<i32>(global<i16>(d::g_music_volume)));  // 0x0059b150
            return 1;
        }
        if (btn_clicked(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_btn_98), 0, 0, -1, 0, 0, 0) != 0) {
            global<i16>(d::g_music_volume) = 0x4d;
            joan::orig::as_sound(obj(d::g_snd))->set_volume_59b150(static_cast<i32>(global<i16>(d::g_music_volume)));  // 0x0059b150
            joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_a0))->set_value(joan::orig::arg::u32(static_cast<i32>(global<i16>(d::g_music_volume))));   // 0x00761240
            global<i16>(d::g_a57ff4) = 0x27;
            joan::orig::as_volume(obj(d::g_mus))->set(static_cast<i32>(global<i16>(d::g_a57ff4)));  // 0x00586620
            joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_b0))->set_value(joan::orig::arg::u32(static_cast<i32>(global<i16>(d::g_a57ff4))));
            global<i16>(d::g_sfx_volume) = 0x64;
            joan::orig::as_party(kSnd)->set_scale(static_cast<i32>(global<i16>(d::g_sfx_volume)));  // 0x0045f6b0
            joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_slider_a8))->set_value(joan::orig::arg::u32(static_cast<i32>(global<i16>(d::g_sfx_volume))));
            joan::orig::as_popup(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_combo_90))->set_text_6b44e0(2u);  // 0x006b44e0
            global<u8>(d::g_flag2) = 1;
            joan::orig::as_sound(obj(d::g_sound3d_system))->set_distance_model(static_cast<char>(joan::orig::arg::i32(flag2())));  // 0x00454000
        }
        if (btn_clicked(jo::view<void*>(jo::self<jo::OptPanel>(this)->game_btn_9c), 0x1b, 0, -1, 0, 0, 0) != 0) {
            joan::orig::as_panel(this)->set_page(static_cast<char>(joan::orig::arg::i32(0)));  // 0x0073cc00
            return 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("SlidePanel::update", 0x0073db40, &H::slide_update);
B("OptPanel::handle", 0x007413e0, &H::opt_handle);
#undef B

}  // namespace game_b108_17_2
}  // namespace

// ---------------------------------------- from modules/game_b105_20_2.cpp (game/b105_20)
namespace {
namespace game_b105_20_2 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using M = jo::OptMenu;

JOAN_COUNTER(c_layout, "Screen::layout_73ded0");
JOAN_COUNTER(c_rel_740df0, "Screen::release_740df0");
JOAN_COUNTER(c_rel_7429c0, "Screen::release_7429c0");

// Release kinds: pre-call thunk, scalar-deleting-dtor thunk.
constexpr fn::Code kPre6217e0 = fn::MenuButton_clear;  // 0x006217e0
constexpr fn::Code kDel6b33a0 = fn::ComboBox_scalar_dtor_6b33a0;  // 0x006b33a0
constexpr fn::Code kPre6b3130 = fn::ComboBox_free_parts;  // 0x006b3130
constexpr fn::Code kDel741240 = fn::Obj741240_scalar_deleting_dtor;  // 0x00741240
constexpr fn::Code kPre760440 = fn::Panel_destroy_children;  // 0x00760440
constexpr fn::Code kDel741290 = fn::Obj_scalar_dtor_741290;  // 0x00741290

struct H {
    // one release block: if (p = +off) { pre(p); p = +off; if (p) del(p, 1); +off = 0; }
    // Returns the eax the original block leaves: this when skipped, else the re-read pointer
    // (0) or the scalar dtor's result.
    template <class W>
    std::uintptr_t release(W* M::*off, std::uintptr_t pre, std::uintptr_t del) {
        W*& slot = jo::self<M>(this)->*off;
        if (slot == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        thiscall<u32>(pre, static_cast<void*>(slot));
        W* const p = slot;
        const std::uintptr_t eax = p != nullptr ? thiscall<u32>(del, static_cast<void*>(p), 1u) : 0u;
        slot = nullptr;
        return eax;
    }
    std::uintptr_t a(jo::Button* M::*off) { return release(off, fn::code_value(kPre6217e0), fn::code_value(kDel6b33a0)); }
    std::uintptr_t b(jo::ComboBox* M::*off) { return release(off, fn::code_value(kPre6b3130), fn::code_value(kDel741240)); }
    std::uintptr_t c(jo::ScrollBox* M::*off) { return release(off, fn::code_value(kPre760440), fn::code_value(kDel741290)); }

    // ------------------------------------------------------------------ 0x0073ded0
    template <class W>
    void place(W* M::*field, u32 x, u32 y) {  // 0x0061e020(x, y, 1.0f, 0, 1, 0), ret 0x18
        joan::orig::as_widget(jo::self<M>(this)->*field)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);
    }
    void reset(jo::Button* M::*field) {  // 0x0061f1b0(0, 1.0f, 0, 1, 0), ret 0x14
        joan::orig::as_widget(jo::self<M>(this)->*field)->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);
    }
    u32 layout_73ded0() {
        c_layout.hit();
        i32 d = 0;
        if (static_cast<i32>(global<i8>(d::g_game_mode)) == 9) d = -0x30;
        place(&M::frame_top, 0xfffffc00u, static_cast<u32>(d + 0xb4));
        place(&M::frame_bottom, 0x400u, static_cast<u32>(d + 0x2c8));
        place(&M::title, 0x1ccu, static_cast<u32>(d - 0x1c0));
        place(&M::tab_button_0, 0x100u, static_cast<u32>(d + 0x3e));
        reset(&M::tab_button_0);
        place(&M::tab_button_1, 0x100u, static_cast<u32>(d + 0x3e));
        reset(&M::tab_button_1);
        place(&M::tab_button_2, 0x100u, static_cast<u32>(d + 0x3e));
        reset(&M::tab_button_2);
        place(&M::tab_button_3, 0x100u, static_cast<u32>(d + 0x3e));
        reset(&M::tab_button_3);
        place(&M::tab_button_4, 0x1ceu, static_cast<u32>(d + 0x2c8));
        reset(&M::tab_button_4);
        joan::orig::as_opt_menu(this)->layout_74b6b0();         // 0x0074b6b0
        joan::orig::as_opt_menu(this)->place_buttons();         // 0x00741a70
        joan::orig::as_opt_menu(this)->layout_747200();         // 0x00747200
        return joan::orig::arg::as<u32>(joan::orig::as_opt_menu(this)->place_buttons_742f90());  // 0x00742f90
    }

    // ------------------------------------------------------------------ 0x00740df0
    std::uintptr_t release_740df0() {
        c_rel_740df0.hit();
        a(&M::game_btn_98);
        a(&M::game_btn_9c);
        b(&M::game_combo_90);
        a(&M::game_btn_94);
        c(&M::game_slider_a0);
        a(&M::game_btn_a4);
        c(&M::game_slider_a8);
        a(&M::game_btn_ac);
        c(&M::game_slider_b0);
        return a(&M::game_btn_b4);
    }
    // ------------------------------------------------------------------ 0x007429c0
    std::uintptr_t release_7429c0() {
        c_rel_7429c0.hit();
        a(&M::video_btn_11d);
        b(&M::video_combo_115);
        a(&M::video_btn_119);
        b(&M::video_combo_ec);
        return a(&M::video_btn_f0);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Screen::layout_73ded0", 0x0073ded0, &H::layout_73ded0);
B("Screen::release_740df0", 0x00740df0, &H::release_740df0);
B("Screen::release_7429c0", 0x007429c0, &H::release_7429c0);
#undef B

}  // namespace game_b105_20_2
}  // namespace

// ---------------------------------------- from modules/game_b109_10.cpp (game/b109_10)
namespace {
namespace game_b109_10 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::img::obj;

using joan::hook::x87::Fpu;

using joan::img::str;

using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;

using joan::hook::x87::ftol;

using joan::call::thiscall;

// Colour(r, g, b, a) 0x0042a260 on a stack temporary, then its dword (bytes only are read).
u32 make_color(i32 r, i32 g, i32 b, i32 a) {
    u32 tmp = 0;
    const u32* const c = joan::orig::arg::as<const u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
    return *c;
}
// Sprite queue 0x00633500 (thunk 0x00401bbd) on 0xa46128, ret 0x44.
u32 sprite(i32 x, i32 y, u32 layer, const char* name, jo::BmpRes* sprites, u32 color, i32 a7, i32 a8, i32 a9, i32 a10, i32 a16,
           i32 a17) {
    return joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(layer), joan::orig::arg::str(name), joan::orig::arg::mptr(sprites), joan::orig::arg::u32(color), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));
}

JOAN_COUNTER(c_mark, "mark_draw_73e130");

// 0x0073e130
u32 JOAN_CDECL mark_draw(void* ctl, u32 surf, u32 flags) {
    c_mark.hit();
    if ((flags & 0xff) != 7 && ((flags & 0xff) & 1) != 0) return 0;
    i16 x = jo::self<jo::Button>(ctl)->x;
    i16 y = jo::self<jo::Button>(ctl)->y;
    i16 w = jo::self<jo::Button>(ctl)->x2;
    i16 h = jo::self<jo::Button>(ctl)->y2;
    float s14 = 1.0f, s18 = 1.0f;
    if (jo::self<jo::Button>(ctl)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(i32{jo::self<jo::Button>(ctl)->x}), joan::orig::arg::i16(i32{jo::self<jo::Button>(ctl)->y}), joan::orig::arg::i16(i32{jo::self<jo::Button>(ctl)->x2}), joan::orig::arg::i16(i32{jo::self<jo::Button>(ctl)->y2}), &s14, &s18, &x, &y, &w, &h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    if (jo::view<i8>(jo::self<jo::Button>(ctl)->caption[0]) == 0) return 1;
    const Fpu F{true};
    const char* const text = jo::self<jo::Button>(ctl)->caption;
    auto alpha = [&]() -> i32 {
        const float a = global<float>(d::flt_255) * jo::self<jo::Button>(ctl)->scale;
        return ftol(static_cast<double>(a));
    };
    auto marks = [&]() {
        const i32 tw = joan::orig::as_font(obj(d::g_font_goudy14_22_white))->measure(joan::orig::arg::str(text), -1, 0, 0, 1.0f);  // 0x00624f00
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->text_height(joan::orig::arg::i16(i32{2}), 1.0f);  // 0x006253f0, result unused
        {
            const u32 col = make_color(0xff, 0xff, 0xff, alpha());
            jo::BmpRes* const sprites = global<jo::BmpRes*>(d::g_a4ff1c);  // OptMenu::u32_18[3]
            const i32 sx = ftol(F.sub(F.mul(static_cast<double>(wsub(wadd(x, w), tw)), 0.5), 20.0));
            sprite(sx, i32{y} + 8, surf, str(d::s_MARK), sprites, col, 3, 0, 2, 0, -1, -1);  // "MARK"
        }
        {
            const u32 col = make_color(0xff, 0xff, 0xff, alpha());
            jo::BmpRes* const sprites = global<jo::BmpRes*>(d::g_a4ff1c);  // OptMenu::u32_18[3]
            const i32 sx = ftol(F.add(F.mul(static_cast<double>(wadd(wadd(tw, x), w)), 0.5), 6.0));
            sprite(sx, i32{y} + 8, surf, str(d::s_MARK), sprites, col, 3, 0, 2, 0, -1, -1);  // "MARK"
        }
    };
    auto caption = [&]() {
        i32 oy = 0, ox = 0;
        if (jo::self<jo::Button>(ctl)->pressed != 0) {
            const double s = global<float>(d::flt_1) * s18;
            oy = joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(s)));  // 0x00590a20
        }
        if (jo::self<jo::Button>(ctl)->pressed != 0) {
            const double s = global<float>(d::flt_1) * s14;
            ox = joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(s)));  // 0x00590a20
        }
        const u32 col = make_color(0, 0, 0, alpha());
        joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_3(joan::orig::arg::i32(wadd(x, ox)), joan::orig::arg::i32(wadd(y, oy) - 1), i32{w}, joan::orig::arg::i32(i32{h} - 1), joan::orig::arg::u32(surf), text, joan::orig::arg::u32(col), i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);  // 0x00626130
    };
    if (jo::self<jo::Button>(ctl)->pressed != 0) {
        marks();
        caption();
    } else if (jo::self<jo::Button>(ctl)->pressed == 0 &&
               joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{x}, i32{y}, i32{w}, i32{h})) != 0) {  // 0x00593980
        marks();
        caption();
    } else {
        caption();
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_10", name, addr, fn)
B("mark_draw_73e130", 0x0073e130, &mark_draw);
#undef B

}  // namespace game_b109_10
}  // namespace

// ---------------------------------------- from modules/game_b108_03.cpp (game/b108_03)
namespace {
namespace game_b108_03 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using u16 = std::uint16_t;

using joan::img::str;

using joan::call::thiscall;

JOAN_COUNTER(c_draw_unit_info, "draw_unit_info_73e990");

void* const kText = obj(d::g_sprites_a46128);

u32 rgba(u8* frame_slot, float alpha_scale) {
    // fld 255.0f; fmul [p+0x11] (float product); _ftol; 0x0042a260(0xff, 0xff, 0xff, a)
    const float prod = global<float>(d::flt_255) * alpha_scale;
    const i32 a = joan::hook::x87::ftol(static_cast<double>(prod));
    u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_colour(frame_slot)->construct(static_cast<u8>(0xff), static_cast<u8>(0xff), static_cast<u8>(0xff), static_cast<u8>(a)));  // 0x0042a260
    return jo::view<u32>(*jo::self<jo::Colour>(r));
}
// 0x00633500 (text draw, 17 args) with the common tail 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1.
void text_draw(i32 x, i32 y, u32 target, const char* font, void* style, u32 color) {
    joan::orig::as_sprite_queue(kText)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(target), joan::orig::arg::str(font), joan::orig::arg::mptr(style), joan::orig::arg::u32(color), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}

// ---------------------------------------------------------------------- 0x0073e990
i32 JOAN_CDECL draw_unit_info(u8* g, u32 target, u32 flags) {
    const u32 fl = flags & 0xff;
    if (fl != 7 && (fl & 1) != 0) return 0;
    c_draw_unit_info.hit();
    alignas(4) u32 w4 = jo::view<u16>(jo::self<jo::Button>(g)->x);  // word locals in their own dword slots
    alignas(4) u32 w8 = jo::view<u16>(jo::self<jo::Button>(g)->y);
    alignas(4) u32 wc = jo::view<u16>(jo::self<jo::Button>(g)->x2);
    alignas(4) u32 w10 = jo::view<u16>(jo::self<jo::Button>(g)->y2);
    alignas(4) float f14 = 1.0f;
    alignas(4) float f18 = 1.0f;
    if (jo::self<jo::Button>(g)->fit != 0)
        joan::orig::as_sprite2d(kText)->fit_rect(joan::orig::arg::i16(jo::view<u16>(jo::self<jo::Button>(g)->x)), joan::orig::arg::i16(jo::view<u16>(jo::self<jo::Button>(g)->y)), joan::orig::arg::i16(jo::view<u16>(jo::self<jo::Button>(g)->x2)), joan::orig::arg::i16(jo::view<u16>(jo::self<jo::Button>(g)->y2)), &f14, &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    void* const font = obj(d::g_font_goudy14_22_white);
    if (jo::view<i8>(jo::self<jo::Button>(g)->caption[0]) != 0) {
        joan::orig::as_font(font)->measure(joan::orig::arg::str(static_cast<void*>(jo::self<jo::Button>(g)->caption)), -1, 0, 0, 1.0f);  // 0x00624f00
        joan::orig::as_font(font)->text_height(joan::orig::arg::i16(2), 1.0f);                                         // 0x006253f0
    }
    alignas(4) u8 col[4];
    {
        const u32 c = rgba(col, jo::self<jo::Button>(g)->scale);  // both branches of the original are identical
        text_draw(static_cast<i16>(w4), static_cast<i16>(w8), target, str(d::s_S_OT), global<void*>(d::g_a3cf6d), c);
    }
    if (jo::view<i8>(jo::self<jo::Button>(g)->caption[0]) == 0) return 1;
    {
        const u32 c = rgba(col, jo::self<jo::Button>(g)->scale);
        joan::orig::as_text(font)->draw_2(joan::orig::arg::i32(static_cast<i32>(jo::self<jo::Button>(g)->x2) - 0x28), joan::orig::arg::i32(static_cast<i32>(jo::self<jo::Button>(g)->y) + 8), joan::orig::arg::u32(target), joan::orig::arg::str(jo::self<jo::Button>(g)->caption), joan::orig::arg::u32(c), 0, 1, 2, 0, 1.0f, 1.0f);  // 0x00626080
    }
    if (jo::view<i8>(jo::self<jo::Button>(g)->held) == 0) return 1;
    {
        const u32 c = rgba(col, jo::self<jo::Button>(g)->scale);
        text_draw(static_cast<i32>(jo::self<jo::Button>(g)->x2) - 0x23, static_cast<i32>(jo::self<jo::Button>(g)->y) + 0xe, target,
                  str(d::s_DIAMOND), global<void*>(d::g_a3cf6d), c);
    }
    {
        const u32 c = rgba(col, jo::self<jo::Button>(g)->scale);
        void* const style = global<void*>(d::g_a3cf6d);
        const i32 y = static_cast<i32>(jo::self<jo::Button>(g)->y) + 0xe;
        i32 x = static_cast<i32>(jo::self<jo::Button>(g)->x2) - 0x23;
        const i16 w = joan::orig::as_font(font)->measure(joan::orig::arg::str(static_cast<void*>(jo::self<jo::Button>(g)->caption)), -1, 0, 0, 1.0f);  // 0x00624f00
        x = x - static_cast<i32>(w) - 0x20;
        text_draw(x, y, target, str(d::s_DIAMOND), style, c);
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_03", name, addr, fn)
B("draw_unit_info", 0x0073e990, &draw_unit_info);
#undef B

}  // namespace game_b108_03
}  // namespace

// ---------------------------------------- from modules/game_b109_11_2.cpp (game/b109_11)
namespace {
namespace game_b109_11_2 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::img::str;

using joan::hook::x87::ftol;

using joan::call::thiscall;

void* const kText = obj(d::g_sprites_a46128);
void* const kFont = obj(d::g_font_goudy14_22_white);

JOAN_COUNTER(c_cs, "draw_cs_73ee40");

// ---------------------------------------------------------------- shared draw helpers
// fld 255.0f; fmul [g+0x11]; _ftol; 0x0042a260(r, g, b, a) on a 4-byte slot; returns the dword.
u32 rgba(u32* slot, i32 r, i32 g, i32 b, float alpha_scale) {
    const float prod = global<float>(d::flt_255) * alpha_scale;
    const i32 a = ftol(static_cast<double>(prod));
    const u32* res = joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
    return *res;
}
// 0x00633500 (sprite / text draw, 17 args) with the common tail 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1.
void sprite_draw(i32 x, i32 y, u32 target, const char* name, const void* sheet, u32 color) {
    joan::orig::as_sprite_queue(kText)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(target), joan::orig::arg::str(name), joan::orig::arg::mptr(sheet), joan::orig::arg::u32(color), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// 0x00626130 (text in rect, 13 args) with the tail 0, 1, 2, 0, 1.0f, 1.0f.
void text_rect(void* font, i32 x, i32 y, i32 w, i32 h, u32 target, const void* text, u32 color) {
    joan::orig::as_text(font)->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(target), joan::orig::arg::str(text), joan::orig::arg::u32(color), i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);  // 0x00626130
}
i32 ui_inside(i32 x0, i32 y0, i32 x1, i32 y1) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1)));  // 0x00593980
}

// ---------------------------------------------------------------- 0x0073ee40
i32 JOAN_CDECL draw_cs(u8* g, u32 target, u32 flags) {
    const u32 fl = flags & 0xff;
    if (fl != 7 && (fl & 1) != 0) return 0;
    c_cs.hit();
    alignas(4) i16 w4 = jo::self<jo::Button>(g)->x;
    alignas(4) i16 w8 = jo::self<jo::Button>(g)->y;
    alignas(4) i16 wc = jo::self<jo::Button>(g)->x2;
    alignas(4) i16 w10 = jo::self<jo::Button>(g)->y2;
    alignas(4) float f14 = 1.0f;
    alignas(4) float f18 = 1.0f;
    if (jo::self<jo::Button>(g)->fit != 0)
        joan::orig::as_sprite2d(kText)->fit_rect(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Button>(g)->x)), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Button>(g)->y)), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Button>(g)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Button>(g)->y2)), &f14, &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    i32 tw = 0;
    if (jo::view<i8>(jo::self<jo::Button>(g)->caption[0]) != 0) {
        tw = joan::orig::as_font(kFont)->measure(joan::orig::arg::str(static_cast<void*>(jo::self<jo::Button>(g)->caption)), -1, 0, 0, 1.0f);                        // 0x00624f00
        joan::orig::as_font(kFont)->text_height(joan::orig::arg::i16(i32{2}), 1.0f);  // 0x006253f0
    }
    alignas(4) u32 slot = 0;
    img::Addr frame = d::s_CS_OT;  // "CS_OT"
    if (jo::self<jo::Button>(g)->enabled != 0) {
        if (jo::self<jo::Button>(g)->pressed != 0)
            frame = d::s_CS_DW;  // "CS_DW"
        else if (ui_inside(static_cast<i32>(jo::self<jo::Button>(g)->x), static_cast<i32>(jo::self<jo::Button>(g)->y),
                           static_cast<i32>(jo::self<jo::Button>(g)->x2), static_cast<i32>(jo::self<jo::Button>(g)->y2)) != 0)
            frame = d::s_CS_OV;  // "CS_OV"
    }
    {
        const u32 c = rgba(&slot, 0xff, 0xff, 0xff, jo::self<jo::Button>(g)->scale);
        sprite_draw(static_cast<i32>(w4), static_cast<i32>(w8), target, str(frame), global<jo::BmpRes*>(d::g_a4ff1c), c);
    }
    if (jo::view<i8>(jo::self<jo::Button>(g)->caption[0]) == 0) return 1;
    {
        const u32 c = rgba(&slot, 0xff, 0xff, 0xff, jo::self<jo::Button>(g)->scale);
        text_rect(kFont, static_cast<i32>(jo::self<jo::Button>(g)->x), static_cast<i32>(jo::self<jo::Button>(g)->y) - 5,
                  static_cast<i32>(jo::self<jo::Button>(g)->x2), static_cast<i32>(jo::self<jo::Button>(g)->y2) - 0x14, target,
                  static_cast<void*>(jo::self<jo::Button>(g)->caption), c);
    }
    if (jo::self<jo::Button>(g)->pressed == 0 && ui_inside(static_cast<i32>(jo::self<jo::Button>(g)->x), static_cast<i32>(jo::self<jo::Button>(g)->y),
                                          static_cast<i32>(jo::self<jo::Button>(g)->x2), static_cast<i32>(jo::self<jo::Button>(g)->y2)) == 0)
        return 1;
    {
        const u32 c = rgba(&slot, 0xff, 0xff, 0xff, jo::self<jo::Button>(g)->scale);
        void* const style = global<void*>(d::g_a3cf6d);
        const i32 y = static_cast<i32>(jo::self<jo::Button>(g)->y) + 0x11;
        const i32 x = static_cast<i32>(jo::self<jo::Button>(g)->x) + (0x200 - tw) / 2 - 0x20;
        sprite_draw(x, y, target, str(d::s_DIAMOND), style, c);
    }
    {
        const u32 c = rgba(&slot, 0xff, 0xff, 0xff, jo::self<jo::Button>(g)->scale);
        void* const style = global<void*>(d::g_a3cf6d);
        const i32 y = static_cast<i32>(jo::self<jo::Button>(g)->y) + 0x11;
        const i32 x = static_cast<i32>(jo::self<jo::Button>(g)->x) + (tw + 0x200) / 2 + 0xa;
        sprite_draw(x, y, target, str(d::s_DIAMOND), style, c);
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_11", name, addr, fn)
B("draw_cs_73ee40", 0x0073ee40, &draw_cs);
#undef B

}  // namespace game_b109_11_2
}  // namespace

// ---------------------------------------- from modules/game_b109_17.cpp (game/b109_17)
namespace {
namespace game_b109_17 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using u16 = std::uint16_t;

using joan::hook::x87::Fpu;

using joan::img::str;

using joan::hook::x87::ftol;

using joan::call::thiscall;

JOAN_COUNTER(c_slot, "draw_slot_button_73f490");

// ------------------------------------------------------------------ shared draw helpers
// Colour object 0x0042a260 (r, g, b, a) through thunk 0x004098b8; returns the packed dword.
u32 rgba(i32 r, i32 g, i32 b, i32 a) {
    u32 slot = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&slot)->construct(r, g, b, a));  // 0x0042a260
}
// fld [0x9cfb68] (255.0f); fmul [button+0x11]; _ftol.
i32 alpha_of(const u8* button) {
    return ftol(static_cast<double>(global<float>(d::flt_255) * jo::self<jo::Button>(button)->scale));
}
// Sprite 0x00633500 on 0xa46128 (17 args, ret 0x44).
void sprite(i32 x, i32 y, u32 ctx, const char* name, jo::BmpRes* tex, u32 color, i32 e7, i32 e8, i32 e9, i32 e10) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(name), joan::orig::arg::mptr(tex), joan::orig::arg::u32(color), joan::orig::arg::i16(e7), joan::orig::arg::i16(e8), joan::orig::arg::i16(e9), joan::orig::arg::i16(e10), -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// Boxed text 0x00626130 (13 args, ret 0x34).
void text_box(img::Addr font, i32 x, i32 y, i32 w, i32 h, u32 ctx, const char* s, u32 color, i32 e8, i32 e9,
              i32 e10, i32 e11) {
    joan::orig::as_text(obj(font))->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(ctx), s, joan::orig::arg::u32(color), joan::orig::arg::i32(e8), joan::orig::arg::i32(e9), joan::orig::arg::i32(e10), joan::orig::arg::i32(e11), 1.0f, 1.0f);  // 0x00626130
}
// Text width 0x00624f00 (s, -1, 0, 0, 1.0f), ret 0x14; ax.
u16 text_width(img::Addr font, const char* s) {
    return static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_font(obj(font))->measure(joan::orig::arg::str(s), -1, 0, 0, 1.0f)));  // 0x00624f00
}
// Pressed offset 0x00590a20 on 0xa45fa8 (double), movsx ax.
i32 press_offset(float scale) {
    const double v = static_cast<double>(global<float>(d::flt_1) * scale);
    return static_cast<i16>(static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(v)))));  // 0x00590a20
}

// Button rectangle (+9/+0xb/+0xd/+0xf words), scaled by 0x00633e60 when +6 is set.
struct Rect {
    i16 x, y, w, h;
    float sx, sy;  // ebp-0x14 / ebp-0x18
};
Rect button_rect(u8* b) {
    Rect r{jo::self<jo::Button>(b)->x, jo::self<jo::Button>(b)->y, jo::self<jo::Button>(b)->x2, jo::self<jo::Button>(b)->y2, 1.0f, 1.0f};
    if (jo::self<jo::Button>(b)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(// 0x00633e60
                      static_cast<i32>(jo::self<jo::Button>(b)->x)), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Button>(b)->y)), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Button>(b)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Button>(b)->y2)), &r.sx, &r.sy, &r.x, &r.y, &r.w, &r.h, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
    }
    return r;
}
bool skip_state(u32 state_arg) {
    const u32 state = state_arg & 0xff;
    return state != 7 && (state & 1) != 0;
}
bool hover(const Rect& r) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(r.x), static_cast<i32>(r.y), static_cast<i32>(r.w), static_cast<i32>(r.h))) != 0;  // 0x00593980
}

// ------------------------------------------------------------------ 0x0073f490
void slot_sprite(const u8* b, const Rect& r, u32 ctx, img::Addr name) {
    const u32 c = rgba(0xff, 0xff, 0xff, alpha_of(b));
    sprite(r.x, r.y, ctx, str(name), global<jo::BmpRes*>(d::g_a4ff1c), c, 3, 0, 2, 0);
}
void slot_label(u8* b, const Rect& r, u32 ctx, i32 shade) {
    const i32 oy = jo::self<jo::Button>(b)->pressed != 0 ? press_offset(r.sy) : 0;  // ebp-0x78 / -0x80 / -0x88
    const i32 ox = jo::self<jo::Button>(b)->pressed != 0 ? press_offset(r.sx) : 0;  // ebp-0x7c / -0x84 / -0x8c
    const u32 c = rgba(shade, shade, shade, alpha_of(b));
    text_box(d::g_font_goudy14_22_white, static_cast<i32>(r.x) + ox, static_cast<i32>(r.y) + oy - 2, r.w,
             static_cast<i32>(r.h) - 2, ctx, static_cast<const char*>(jo::self<jo::Button>(b)->caption), c, 0, 1, 2, 0);
}

static u32 JOAN_CDECL draw_slot_button_73f490(u8* b, u32 ctx, u32 state) {
    c_slot.hit();
    if (skip_state(state)) return 0;
    const Rect r = button_rect(b);
    if (jo::view<i8>(jo::self<jo::Button>(b)->caption[0]) == 0) return 1;
    if (jo::self<jo::Button>(b)->pressed != 0) {
        slot_sprite(b, r, ctx, d::s_SLOT_DW);  // SLOT_DW
    } else if (jo::self<jo::Button>(b)->pressed == 0 && hover(r)) {
        slot_sprite(b, r, ctx, d::s_SLOT_OV);  // SLOT_OV
    } else if (jo::view<i8>(jo::self<jo::Button>(b)->held) != 0) {
        slot_sprite(b, r, ctx, d::s_SLOT_OV);  // SLOT_OV
    } else {
        slot_sprite(b, r, ctx, d::s_SLOT_OT);  // SLOT_OT
    }
    if (jo::view<i8>(jo::self<jo::Button>(b)->held) != 0) {
        const i32 tw = static_cast<i16>(text_width(d::g_font_goudy14_22_white, static_cast<const char*>(jo::self<jo::Button>(b)->caption)));
        joan::orig::as_font(obj(d::g_font_goudy14_22_white))->text_height(joan::orig::arg::i16(i32{2}), 1.0f);  // 0x006253f0, result dead
        const Fpu fpu{true};
        {
            const u32 c = rgba(0xff, 0xff, 0xff, alpha_of(b));
            const i32 s = static_cast<i32>(r.x) + static_cast<i32>(r.w) - tw;
            const i32 mx = ftol(fpu.sub(fpu.sub(fpu.mul(static_cast<double>(s), 0.5), 20.0), 4.0));
            sprite(mx, static_cast<i32>(r.y) + 8, ctx, str(d::s_MARK), global<jo::BmpRes*>(d::g_a4ff1c), c, 3, 0, 0, 0);  // MARK
        }
        {
            const u32 c = rgba(0xff, 0xff, 0xff, alpha_of(b));
            const i32 s = tw + static_cast<i32>(r.x) + static_cast<i32>(r.w);
            const i32 mx = ftol(fpu.add(fpu.mul(static_cast<double>(s), 0.5), 6.0));
            sprite(mx, static_cast<i32>(r.y) + 8, ctx, str(d::s_MARK), global<jo::BmpRes*>(d::g_a4ff1c), c, 3, 0, 0, 0);  // MARK
        }
        slot_label(b, r, ctx, 0xff);
    } else if (jo::self<jo::Button>(b)->pressed == 0 && hover(r)) {
        slot_label(b, r, ctx, 0xff);
    } else {
        slot_label(b, r, ctx, 0);
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_17", name, addr, fn)
B("draw_slot_button_73f490", 0x0073f490, &draw_slot_button_73f490);
#undef B

}  // namespace game_b109_17
}  // namespace

// ---------------------------------------- from modules/game_b109_15.cpp (game/b109_15)
namespace {
namespace game_b109_15 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::hook::x87::Fpu;

using joan::img::str;

using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;

using joan::hook::x87::ftol;

using joan::call::thiscall;
float fl(u32 bits) {
    float f;
    std::memcpy(&f, &bits, 4);
    return f;
}

constexpr u32 kOneF = 0x3f800000;  // 1.0f

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), ret 0x10; returns this.
u32 make_color(u32 r, u32 g, u32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*jo::self<jo::Colour>(p));
}
// alpha = _ftol(255.0f * ctl->+0x11)
i32 alpha_of(const void* ctl) {
    const float a = global<float>(d::flt_255) * jo::self<jo::Button>(ctl)->scale;
    return ftol(static_cast<double>(a));
}
// 0x00633500 (thunk 0x00401bbd) on 0xa46128, ret 0x44.
void draw_sprite(i32 x, i32 y, u32 surf, img::Addr name, jo::BmpRes* set, u32 color, i32 a7, i32 a8, i32 a9, i32 a10) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(surf), str(name), joan::orig::arg::mptr(set), joan::orig::arg::u32(color), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, -1, -1);
}
// 0x00633e60 (thunk 0x0040807b) on 0xa46128, ret 0x30.
void fit_rect(const void* ctl, u32* f14, u32* f18, i16* s4, i16* s8, i16* sc, i16* s10) {
    joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::self<jo::Button>(ctl)->x), joan::orig::arg::i16(jo::self<jo::Button>(ctl)->y), joan::orig::arg::i16(jo::self<jo::Button>(ctl)->x2), joan::orig::arg::i16(jo::self<jo::Button>(ctl)->y2), joan::orig::arg::mptr(f14), joan::orig::arg::mptr(f18), joan::orig::arg::mptr(s4), joan::orig::arg::mptr(s8), joan::orig::arg::mptr(sc), joan::orig::arg::mptr(s10), joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
}
// 0x00590a20 (thunk 0x00401497) on 0xa45fa8 with (double)(1.0f * scale), result movsx ax.
i32 scale_round(u32 scale_bits) {
    const double d = Fpu{true}.mul(1.0, static_cast<double>(fl(scale_bits)));
    return joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(d)));
}
// 0x00624f00 (thunk 0x00405f97): text width (text, -1, 0, 0, 1.0f), ret 0x14.
i32 text_width(void* font, void* text) {
    return joan::orig::as_font(font)->measure(joan::orig::arg::str(text), -1, 0, 0, joan::orig::arg::f32(kOneF));
}
// 0x006253f0 (thunk 0x004076d0): (2, 1.0f), ret 8; result unused.
void text_height(void* font) { joan::orig::as_font(font)->text_height(joan::orig::arg::i16(i32{2}), joan::orig::arg::f32(kOneF)); }
// 0x00626130 (thunk 0x00402e0f): text in a box, ret 0x34.
void draw_text(void* font, i32 x, i32 y, i16 w, i32 h, u32 surf, void* text, u32 color) {
    joan::orig::as_text(font)->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(surf), joan::orig::arg::str(text), joan::orig::arg::u32(color), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOneF), joan::orig::arg::f32(kOneF));
}

JOAN_COUNTER(c_optmark, "opt_mark_draw_73fde0");

// 0x0073fde0
u32 JOAN_CDECL opt_mark_draw(void* ctl, u32 surf, u32 flags) {
    c_optmark.hit();
    if ((flags & 0xff) != 7 && ((flags & 0xff) & 1) != 0) return 0;
    i16 s4 = jo::self<jo::Button>(ctl)->x;
    i16 s8 = jo::self<jo::Button>(ctl)->y;
    i16 sc = jo::self<jo::Button>(ctl)->x2;
    i16 s10 = jo::self<jo::Button>(ctl)->y2;
    u32 f14 = kOneF, f18 = kOneF;
    if (jo::self<jo::Button>(ctl)->fit != 0) fit_rect(ctl, &f14, &f18, &s4, &s8, &sc, &s10);
    if (jo::view<i8>(jo::self<jo::Button>(ctl)->caption[0]) == 0) return 1;
    void* const font = obj(d::g_font_goudy14_22_white);
    void* const text = jo::self<jo::Button>(ctl)->caption;
    constexpr img::Addr kMark = d::s_MARK;  // "MARK"
    {
        const u32 color = make_color(0xff, 0xff, 0xff, alpha_of(ctl));
        const img::Addr name = jo::view<i8>(jo::self<jo::Button>(ctl)->held) != 0 ? img::Addr{d::s_TB_OV} : img::Addr{d::s_TB_OT};  // "TB_OV" / "TB_OT"
        draw_sprite(s4, s8, surf, name, global<jo::BmpRes*>(d::g_a4ff1c), color, 3, 0, 2, 0);
    }
    auto label = [&]() {
        const i32 ry = jo::self<jo::Button>(ctl)->pressed != 0 ? scale_round(f18) : 0;
        const i32 rx = jo::self<jo::Button>(ctl)->pressed != 0 ? scale_round(f14) : 0;
        const u32 color = make_color(0xff, 0xff, 0xff, alpha_of(ctl));
        draw_text(font, wadd(s4, rx), wadd(s8, ry) - 2, sc, static_cast<i32>(s10) - 2, surf, text, color);
    };
    if (jo::view<i8>(jo::self<jo::Button>(ctl)->held) != 0) {
        const i32 w20 = text_width(font, text);
        text_height(font);
        u32 color = make_color(0xff, 0xff, 0xff, alpha_of(ctl));
        {
            const Fpu fpu{true};
            const double d = fpu.sub(
                fpu.sub(fpu.mul(static_cast<double>(wsub(static_cast<i32>(s4) + sc, w20)), 0.5), 20.0), 4.0);
            draw_sprite(ftol(d), static_cast<i32>(s8) + 8, surf, kMark, global<jo::BmpRes*>(d::g_a4ff1c), color, 3, 0, 0, 0);
        }
        color = make_color(0xff, 0xff, 0xff, alpha_of(ctl));
        {
            const Fpu fpu{true};
            const double d = fpu.add(fpu.mul(static_cast<double>(wadd(wadd(w20, s4), sc)), 0.5), 6.0);
            draw_sprite(ftol(d), static_cast<i32>(s8) + 8, surf, kMark, global<jo::BmpRes*>(d::g_a4ff1c), color, 3, 0, 0, 0);
        }
    }
    label();
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_15", name, addr, fn)
B("opt_mark_draw_73fde0", 0x0073fde0, &opt_mark_draw);
#undef B

}  // namespace game_b109_15
}  // namespace

// ---------------------------------------- from modules/game_b109_01.cpp (game/b109_01)
namespace {
namespace game_b109_01 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::call::cdecl_call;
using joan::orig::helpers::game_new;  // operator new

using joan::call::thiscall;
using M = jo::OptMenu;
using joan::orig::helpers::fbits;
// operator new + thiscall ctor (no args) as `p = new T` (null when the allocation failed).
void* new_obj(u32 size, std::uintptr_t ctor_thunk) {
    void* const mem = game_new(size);
    return mem != nullptr ? thiscall<void*>(ctor_thunk, mem) : nullptr;
}

JOAN_COUNTER(c_menu, "Menu::create_controls_740440");

// ---------------------------------------------------------------- 0x00740440 helpers
void* const kText = obj(d::g_strings);
u32 white() {
    u32 c = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&c)->construct(0xff, 0xff, 0xff, 0xff));  // 0x0042a260
}

struct H {
    // -------------------------------------------------------------- 0x0045d330
    // Original frame [ebp-0x150]..[ebp-0x10]: the 0x18-byte time buffer is followed by the
    // locals it can overrun (tod, date, local / system FILETIME - dead after the format call)
    // and then the live file object, exactly as in the original.
    struct PerfFrame {
        char tbuf[0x18];  // [ebp-0x150]
        float tod;        // [ebp-0x138]
        u32 date;         // [ebp-0x134]
        u32 lft[2];       // [ebp-0x130]
        u32 ft[2];        // [ebp-0x128]
        u8 file[sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110];  // [ebp-0x120] (a File: 0x112 bytes on x64)
    };
    static_assert(offsetof(PerfFrame, file) == 0x30 && (sizeof(void*) != 4 || sizeof(PerfFrame) == 0x140),
                  "PerfFrame: no padding between the frame slots (0x140 bytes on x86)");

    // -------------------------------------------------------------- 0x00740440
    void* button(jo::Button* M::*slot, i32 x, i32 y, i32 x2, i32 y2, std::uintptr_t cb) {
        jo::self<M>(this)->*slot = static_cast<jo::Button*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0)));  // 0x00620ee0
        joan::orig::as_button(jo::self<M>(this)->*slot)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), cb, 1, 0, 0, 1, 1, 1);  // 0x00621540
        return jo::self<M>(this)->*slot;
    }
    u32 label(jo::Button* M::*slot, std::uintptr_t text_thunk) {
        const u32 c = white();
        const char* const t = thiscall<const char*>(text_thunk, kText);
        return joan::orig::arg::as<u32>(joan::orig::as_button(jo::self<M>(this)->*slot)->set_text(joan::orig::arg::str(t), img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(c), 1, 2, 0, 2, 2));  // 0x00623ce0
    }
    void slider(jo::ScrollBox* M::*slot, img::Addr value) {
        jo::self<M>(this)->*slot = static_cast<jo::ScrollBox*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::ScrollBox)), fn::code_value(fn::Widget_9df888_ctor)));  // 0x0075fab0
        joan::orig::as_scroll_box(jo::self<M>(this)->*slot)->init(0x1c0u, 0x3eu, 0x65, 0u, 1u, 1u);  // 0x00760050
        joan::orig::as_scroll_box(jo::self<M>(this)->*slot)->set_value(joan::orig::arg::u32(static_cast<i32>(global<i16>(value))));  // 0x00761240
    }
    u32 create_controls() {
        c_menu.hit();
        button(&M::game_btn_98, 0x183, 0x2c8, 0x1e7, 0x2e6, fn::code_value(fn::mark_draw_73e130));
        label(&M::game_btn_98, fn::code_value(fn::OptMenu_open_33));  // 0x004db550
        joan::orig::as_widget(jo::self<M>(this)->game_btn_98)->scale_to(0, joan::orig::arg::f32(fbits(0x3f800000)), 0.0f, 1, 0u);  // 0x0061f1b0
        button(&M::game_btn_9c, 0x219, 0x2c8, 0x27d, 0x2e6, fn::code_value(fn::mark_draw_73e130));
        label(&M::game_btn_9c, fn::code_value(fn::Menu_load_optmenu));  // 0x004db670
        joan::orig::as_widget(jo::self<M>(this)->game_btn_9c)->scale_to(0, joan::orig::arg::f32(fbits(0x3f800000)), 0.0f, 1, 0u);  // 0x0061f1b0
        jo::self<jo::OptMenu>(this)->game_combo_90 = static_cast<jo::ComboBox*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::ComboBox)), fn::code_value(fn::Widget_9dd7a0_ctor)));  // 0x006b22f0
        joan::orig::as_combo_box(jo::self<M>(this)->game_combo_90)->init_options(0x1c0u, 0x3eu, 2, 1u, 1u);  // 0x006b29b0
        {
            const char* const t = joan::orig::as_opt_menu(kText)->open_2();  // 0x004daf50
            joan::orig::as_combo_box(jo::self<M>(this)->game_combo_90)->add_6b3930(joan::orig::arg::str(t));  // 0x006b3930
        }
        {
            const char* const t = joan::orig::as_opt_menu(kText)->open_1();  // 0x004daf20
            joan::orig::as_combo_box(jo::self<M>(this)->game_combo_90)->add_6b3930(joan::orig::arg::str(t));  // 0x006b3930
        }
        joan::orig::as_popup(jo::self<M>(this)->game_combo_90)->set_text_6b44e0(joan::orig::arg::u32(global<i8>(d::g_flag2) != 0 ? 2 : 1));  // 0x006b44e0
        button(&M::game_btn_94, -0x264, 0x18f, -100, 0x1c7, fn::code_value(fn::draw_unit_info));
        label(&M::game_btn_94, fn::code_value(fn::OptMenu_open_05));  // 0x004dafe0
        slider(&M::game_slider_a0, d::g_music_volume);
        button(&M::game_btn_a4, -0x264, 0xf3, -100, 0x12b, fn::code_value(fn::draw_unit_info));
        label(&M::game_btn_a4, fn::code_value(fn::Menu_optmenu_03));  // 0x004daf80
        slider(&M::game_slider_b0, d::g_a57ff4);
        button(&M::game_btn_b4, -0x264, 0x127, -100, 0x15f, fn::code_value(fn::draw_unit_info));
        label(&M::game_btn_b4, fn::code_value(fn::TxtRes_optmenu_1e));  // 0x004db4c0
        slider(&M::game_slider_a8, d::g_sfx_volume);
        button(&M::game_btn_ac, -0x264, 0x15b, -100, 0x193, fn::code_value(fn::draw_unit_info));
        return label(&M::game_btn_ac, fn::code_value(fn::OptMenu_open_04));  // 0x004dafb0
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_01", name, addr, fn)
B("Menu::create_controls_740440", 0x00740440, &H::create_controls);
#undef B

}  // namespace game_b109_01
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using u32 = std::uint32_t;

using u8 = std::uint8_t;

using joan::call::thiscall;

JOAN_COUNTER(c_enable_all, "Panel::enable_all");

struct H {
    u32 enable_all(u32) {  // 0x007412e0
        c_enable_all.hit();
        joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_slider_a0))->draw(1, 7);  // 0x00760b70
        joan::orig::as_button(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_a4))->draw(1u, 7);  // 0x00622cc0
        joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_slider_b0))->draw(1, 7);  // 0x00760b70
        joan::orig::as_button(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_b4))->draw(1u, 7);  // 0x00622cc0
        joan::orig::as_scroll_box(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_slider_a8))->draw(1, 7);  // 0x00760b70
        joan::orig::as_button(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_ac))->draw(1u, 7);  // 0x00622cc0
        joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_combo_90))->draw(1u, static_cast<std::uint8_t>(joan::orig::arg::u32(7)));  // 0x006b3ec0
        joan::orig::as_button(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_94))->draw(1u, 7);  // 0x00622cc0
        joan::orig::as_button(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_98))->draw(1u, 7);  // 0x00622cc0
        return joan::orig::arg::as<u32>(joan::orig::as_button(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_9c))->draw(1u, 7));  // 0x00622cc0
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Panel::enable_all", 0x007412e0, &H::enable_all);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::call::thiscall;

JOAN_COUNTER(c_place_buttons, "OptionMenu::place_buttons");

struct H {
    // ------------------------------------------------------------------ 0x00724670 / 0x007349b0
    static u32 button_reset(void* b) {
        joan::orig::as_widget(b)->scale_to_61f1b0(joan::orig::arg::i16(0u), joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), static_cast<char>(1u), joan::orig::arg::u32(0u));  // 0x0061f1b0
        return 0;  // scale_to returns nothing; OptionMenu::place_buttons results are never read
    }

    // ------------------------------------------------------------------ 0x00741a70
    u32 place_buttons() {
        c_place_buttons.hit();
        i32 dy = 0;
        if (static_cast<i32>(global<i8>(d::g_game_mode)) == 9) dy = -0x30;
        auto place = [](void* b, i32 x, i32 y) {
            joan::orig::as_widget(b)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);  // 0x0061e020
        };
        place(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_slider_a0), 0x1c0, dy + 0x3e);
        place(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_a4), -0x264, dy + 0xf3);
        place(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_slider_a8), 0x1c0, dy + 0x3e);
        place(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_ac), -0x264, dy + 0x15b);
        place(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_slider_b0), 0x1c0, dy + 0x3e);
        place(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_b4), -0x264, dy + 0x127);
        place(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_combo_90), 0x1c0, dy + 0x3e);
        place(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_94), -0x264, dy + 0x18f);
        place(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_9c), 0x219, dy + 0x2c8);
        button_reset(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_9c));
        place(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_98), 0x183, dy + 0x2c8);
        return button_reset(jo::view<void*>(jo::self<jo::OptMenu>(this)->game_btn_98));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("OptionMenu::place_buttons", 0x00741a70, &H::place_buttons);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b104_15_4.cpp (game/b104_15)
namespace {
namespace game_b104_15_4 {

using joan::hook::global;

using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::call::thiscall;
using M = jo::OptMenu;

JOAN_COUNTER(c_layout_741cd0, "OptMenu::layout_741cd0");

// Optional offset on the light / options screens: -0x30 in menu mode 9.
u32 menu_dy() { return global<i8>(d::g_game_mode) == 9 ? 0xffffffd0u : 0u; }

struct H {
    // 0x0061e020 / 0x0061f1b0 on the widget pointer member `off`.
    template <class W>
    u32 m6(W* M::*off, u32 a, u32 b, u32 c, u32 d, u32 e, u32 g) {
        joan::orig::as_widget(jo::self<M>(this)->*off)->move_to_61e020(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(g));  // 0x0061e020
        return 0;  // move_to returns nothing; the layout results are never read
    }
    template <class W>
    u32 f5(W* M::*off, u32 a, u32 b, u32 c, u32 d, u32 e) {
        joan::orig::as_widget(jo::self<M>(this)->*off)->scale_to_61f1b0(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
        return 0;  // scale_to returns nothing; the layout results are never read
    }

    // ------------------------------------------------------------------------ 0x00741cd0
    u32 layout_741cd0() {
        c_layout_741cd0.hit();
        const u32 d = menu_dy();
        u32 r;
        r = f5(&M::game_btn_94, 0x1u, 0x40800000u, 0u, 0u, 0x1f4u);
        r = m6(&M::game_btn_94, 0xffffff9cu, u32(d + 0x18f), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::game_btn_a4, 0x1u, 0x40800000u, 0u, 0u, 0x1f4u);
        r = m6(&M::game_btn_a4, 0xffffff9cu, u32(d + 0xf3), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::game_btn_ac, 0x1u, 0x40800000u, 0u, 0u, 0x1f4u);
        r = m6(&M::game_btn_ac, 0xffffff9cu, u32(d + 0x15b), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::game_btn_b4, 0x1u, 0x40800000u, 0u, 0u, 0x1f4u);
        r = m6(&M::game_btn_b4, 0xffffff9cu, u32(d + 0x127), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::game_btn_98, 0x1u, 0x40c00000u, 0u, 0u, 0x1f4u);
        r = m6(&M::game_btn_98, 0x183u, u32(d + 0x2c8), 0x477fff00u, 0u, 0u, 0x1f4u);
        r = f5(&M::game_btn_9c, 0x1u, 0x40c00000u, 0u, 0u, 0x1f4u);
        r = m6(&M::game_btn_9c, 0x219u, u32(d + 0x2c8), 0x477fff00u, 0u, 0u, 0x1f4u);
        r = m6(&M::title, 0x1ccu, u32(d - 0x64), 0x455ac000u, 0xc4bb8000u, 0u, 0x1f4u);
        r = f5(&M::game_combo_90, 0x1u, 0x40800000u, 0u, 0u, 0x226u);
        r = m6(&M::game_combo_90, 0x1c0u, u32(d + 0x188), 0x455ac000u, 0xc4bb8000u, 0u, 0x226u);
        r = f5(&M::game_slider_a8, 0x1u, 0x40800000u, 0u, 0u, 0x258u);
        r = m6(&M::game_slider_a8, 0x1c0u, u32(d + 0x154), 0x455ac000u, 0xc4bb8000u, 0u, 0x258u);
        r = f5(&M::game_slider_b0, 0x1u, 0x40800000u, 0u, 0u, 0x28au);
        r = m6(&M::game_slider_b0, 0x1c0u, u32(d + 0x120), 0x455ac000u, 0xc4bb8000u, 0u, 0x28au);
        r = f5(&M::game_slider_a0, 0x1u, 0x40800000u, 0u, 0u, 0x2bcu);
        r = m6(&M::game_slider_a0, 0x1c0u, u32(d + 0xec), 0x455ac000u, 0xc4bb8000u, 0u, 0x2bcu);
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_15", name, addr, fn)
B("OptMenu::layout_741cd0", 0x00741cd0, &H::layout_741cd0);
#undef B

}  // namespace game_b104_15_4
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16_3 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::call::thiscall;
using M = jo::OptMenu;

// sound
void slide_sound() {
    joan::orig::as_sound(obj(d::g_sfx_player))->play_def(img::str(d::s_BUTTON_SLIDE_MANY));  // 0x004ceef0 'BUTTON_SLIDE_MANY'
}
u32 slide_dy() { return static_cast<i8>(global<u8>(d::g_game_mode)) == 9 ? 0xffffffd0u : 0u; }

JOAN_COUNTER(c_slide_7420e0, "Panel::slide_in_7420e0");

struct H {
    void slide_7420e0() {
        c_slide_7420e0.hit();
        const u32 dy = slide_dy();
        if (jo::self<jo::OptMenu>(this)->slide_state != 0) slide_sound();
        joan::orig::as_widget(jo::self<M>(this)->game_btn_94)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->game_btn_94)->move_to(-612, joan::orig::arg::i16(dy + 0x18f), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->game_btn_a4)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->game_btn_a4)->move_to(-612, joan::orig::arg::i16(dy + 0xf3), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->game_btn_ac)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->game_btn_ac)->move_to(-612, joan::orig::arg::i16(dy + 0x15b), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->game_btn_b4)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->game_btn_b4)->move_to(-612, joan::orig::arg::i16(dy + 0x127), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->game_btn_98)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->game_btn_9c)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->game_slider_a0)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->game_slider_a0)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->game_slider_b0)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x32u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->game_slider_b0)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x32u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->game_slider_a8)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x64u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->game_slider_a8)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x64u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->game_combo_90)->scale_to(0x0, joan::orig::arg::f32(0x40800000), 0.0f, 0, 0x96u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<M>(this)->game_combo_90)->move_to(0x1c0, joan::orig::arg::i16(dy + 0x3e), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x96u);  // 0x0061e020
        joan::orig::as_widget(jo::self<M>(this)->title)->move_to(0x1cc, joan::orig::arg::i16(dy - 0x1c0), joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0xc8u);  // 0x0061e020
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Panel::slide_in_7420e0", 0x007420e0, &H::slide_7420e0);
#undef B

}  // namespace game_b103_16_3
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::call::cdecl_call;
using joan::orig::helpers::game_new;  // operator new

using joan::call::thiscall;
using joan::orig::helpers::add32;

void* const kStrings = obj(d::g_strings);

JOAN_COUNTER(c_options, "Options::build_742470");

// 0x0042a260 colour(r, g, b, a) on a local, packed dword read back.
u32 make_colour(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
}

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x00742470
    i32 build_742470() {
        c_options.hit();
        auto new_list = []() -> void* {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86
            return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(mem)->ctor_620ee0()) : nullptr;  // 0x00620ee0
        };
        auto new_slider = []() -> void* {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::ComboBox)));  // 0xad on x86
            return mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(mem)->m_9dd7a0_ctor()) : nullptr;  // 0x006b22f0
        };
        auto set_font = [](void* list, std::uintptr_t getter) {
            const u32 colour = make_colour(0xff, 0xff, 0xff, 0xff);
            void* const font = thiscall<void*>(getter, kStrings);
            joan::orig::as_button(list)->set_text(joan::orig::arg::str(font), obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(colour), 1, 2, 0, 2, 2);  // 0x00623ce0
        };
        jo::view<void*>(jo::self<jo::Options>(this)->video_btn_11d) = new_list();
        joan::orig::as_button(jo::view<void*>(jo::self<jo::Options>(this)->video_btn_11d))->create(0x1ce, 0x2c8, 0x232, 0x2e6, fn::code_value(fn::mark_draw_73e130), 1, 0, 0, 1, 1, 1);  // 0x00621540
        set_font(jo::view<void*>(jo::self<jo::Options>(this)->video_btn_11d), fn::code_value(fn::Menu_load_optmenu));  // 0x004db670
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::Options>(this)->video_btn_11d))->scale_to(0, joan::orig::arg::f32(0x3f800000), 0.0f, 1, 0u);  // 0x0061f1b0

        jo::view<void*>(jo::self<jo::Options>(this)->video_combo_115) = new_slider();
        joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::Options>(this)->video_combo_115))->init_options(0x1c0u, 0x3eu, 2, 1u, 1u);  // 0x006b29b0
        joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::Options>(this)->video_combo_115))->add_6b3930(joan::orig::arg::str(joan::orig::arg::as<void*>(joan::orig::as_menu(kStrings)->optmenu_06())));  // 0x006b3930, 0x004db010
        joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::Options>(this)->video_combo_115))->add_6b3930(joan::orig::arg::str(joan::orig::arg::as<void*>(joan::orig::as_opt_menu(kStrings)->open_07())));  // 0x004db040
        joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::Options>(this)->video_combo_115))->add_6b3930(joan::orig::arg::str(joan::orig::arg::as<void*>(joan::orig::as_opt_menu(kStrings)->open_08())));  // 0x004db070
        joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::Options>(this)->video_combo_115))->add_6b3930(joan::orig::arg::str(joan::orig::arg::as<void*>(joan::orig::as_txt_res(kStrings)->optmenu_33())));  // 0x004db0a0
        joan::orig::as_popup(jo::view<void*>(jo::self<jo::Options>(this)->video_combo_115))->set_text_6b44e0(joan::orig::arg::u32(add32(global<i8>(d::g_difficulty), 1)));      // 0x006b44e0

        jo::view<void*>(jo::self<jo::Options>(this)->video_btn_119) = new_list();
        joan::orig::as_button(jo::view<void*>(jo::self<jo::Options>(this)->video_btn_119))->create(joan::orig::arg::i32(-612), 0xf3, joan::orig::arg::i32(-100), 0x12b, fn::code_value(fn::draw_unit_info), 1, 0, 0, 1, 1, 1);
        set_font(jo::view<void*>(jo::self<jo::Options>(this)->video_btn_119), fn::code_value(fn::OptMenu_open_2e));  // 0x004db7c0

        jo::view<void*>(jo::self<jo::Options>(this)->video_combo_ec) = new_slider();
        joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::Options>(this)->video_combo_ec))->init_options(0x1c0u, 0x3eu, 2, 1u, 1u);
        joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::Options>(this)->video_combo_ec))->add_6b3930(joan::orig::arg::str(joan::orig::arg::as<void*>(joan::orig::as_opt_menu(kStrings)->open_1())));  // 0x004daf20
        joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::Options>(this)->video_combo_ec))->add_6b3930(joan::orig::arg::str(joan::orig::arg::as<void*>(joan::orig::as_opt_menu(kStrings)->open_2())));  // 0x004daf50

        jo::view<void*>(jo::self<jo::Options>(this)->video_btn_f0) = new_list();
        joan::orig::as_button(jo::view<void*>(jo::self<jo::Options>(this)->video_btn_f0))->create(joan::orig::arg::i32(-612), 0x127, joan::orig::arg::i32(-100), 0x15f, fn::code_value(fn::draw_unit_info), 1, 0, 0, 1, 1, 1);
        set_font(jo::view<void*>(jo::self<jo::Options>(this)->video_btn_f0), fn::code_value(fn::Menu_optmenu_37));  // 0x004db940
        return joan::orig::arg::as<i32>(joan::orig::as_popup(jo::view<void*>(jo::self<jo::Options>(this)->video_combo_ec))->set_text_6b44e0(joan::orig::arg::u32(add32(global<i8>(d::g_a5802d), 1))));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Options::build_742470", 0x00742470, &H::build_742470);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
namespace {
namespace game_b108_05 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using u8 = std::uint8_t;

using joan::call::thiscall;
using M = jo::OptMenu;

JOAN_COUNTER(c_set_mode, "Panel::set_mode_742bf0");
JOAN_COUNTER(c_layout, "Panel::layout_743170");

struct H {
    // ------------------------------------------------------------------ 0x00742bf0
    u32 set_mode_742bf0(u32 /*unused byte*/) {
        c_set_mode.hit();
        joan::orig::as_combo_box(jo::self<M>(this)->video_combo_115)->draw(joan::orig::arg::u32(i32{1}), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{7})));  // 0x006b3ec0
        joan::orig::as_button(jo::self<M>(this)->video_btn_119)->draw(1u, 7);  // 0x00622cc0
        joan::orig::as_button(jo::self<M>(this)->video_btn_11d)->draw(1u, 7);  // 0x00622cc0
        joan::orig::as_combo_box(jo::self<M>(this)->video_combo_ec)->draw(joan::orig::arg::u32(i32{1}), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{7})));   // 0x006b3ec0
        return joan::orig::arg::as<u32>(joan::orig::as_button(jo::self<M>(this)->video_btn_f0)->draw(1u, 7));  // 0x00622cc0
    }

    // ------------------------------------------------------------------ 0x00743170
    // 0x0061f1b0 (thunk 0x004034e0, ret 0x14) / 0x0061e020 (thunk 0x00403454, ret 0x18); the
    // float arguments are passed as their bit patterns (4.0f = 0x40800000, ...).
    static void anim(void* o, u32 f, i32 t) {
        joan::orig::as_widget(o)->scale_to(1, joan::orig::arg::f32(f), 0.0f, 0, joan::orig::arg::u32(t));
    }
    static u32 move(void* o, i32 x, i32 y, u32 f1, u32 f2, i32 t) {
        joan::orig::as_widget(o)->move_to_61e020(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(f1), joan::orig::arg::f32(f2), static_cast<char>(i32{0}), joan::orig::arg::u32(t));  // 0x0061e020
        return 0;  // move_to returns nothing; Panel::layout_743170 results are never read (only Panel::set_page)
    }
    u32 layout_743170() {
        c_layout.hit();
        i32 dy = 0;
        if (global<i8>(d::g_game_mode) == 9) dy = -0x30;
        joan::orig::as_popup(jo::self<M>(this)->video_combo_115)->set_text_6b44e0(joan::orig::arg::u32(static_cast<i32>(global<i8>(d::g_difficulty)) + 1));  // 0x006b44e0
        anim(jo::self<M>(this)->video_btn_119, 0x40800000u, 0x1f4);
        move(jo::self<M>(this)->video_btn_119, -100, dy + 0xf3, 0x455ac000u, 0xc4bb8000u, 0x1f4);
        anim(jo::self<M>(this)->video_btn_f0, 0x40800000u, 0x1f4);
        move(jo::self<M>(this)->video_btn_f0, -100, dy + 0x127, 0x455ac000u, 0xc4bb8000u, 0x1f4);
        anim(jo::self<M>(this)->video_btn_11d, 0x40c00000u, 0x1f4);
        move(jo::self<M>(this)->video_btn_11d, 0x1ce, dy + 0x2c8, 0x477fff00u, 0u, 0x1f4);
        move(jo::self<M>(this)->title, 0x1cc, dy - 0xcc, 0x455ac000u, 0xc4bb8000u, 0x1f4);
        anim(jo::self<M>(this)->video_combo_ec, 0x40800000u, 0x226);
        move(jo::self<M>(this)->video_combo_ec, 0x1c0, dy + 0x120, 0x455ac000u, 0xc4bb8000u, 0x226);
        anim(jo::self<M>(this)->video_combo_115, 0x40800000u, 0x258);
        return move(jo::self<M>(this)->video_combo_115, 0x1c0, dy + 0xec, 0x455ac000u, 0xc4bb8000u, 0x258);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Panel::set_mode_742bf0", 0x00742bf0, &H::set_mode_742bf0);
B("Panel::layout_743170", 0x00743170, &H::layout_743170);
#undef B

}  // namespace game_b108_05
}  // namespace

// ---------------------------------------- from modules/game_b108_15_2.cpp (game/b108_15)
namespace {
namespace game_b108_15_2 {

using u32 = std::uint32_t;

using joan::img::obj;

using joan::img::str;

using joan::call::thiscall;
using M = jo::OptMenu;

JOAN_COUNTER(c_panel_742ee0, "Panel::update_742ee0");

struct H {
    // ------------------------------------------------------------------ 0x00742ee0
    void panel_742ee0() {
        c_panel_742ee0.hit();
        const u32 before = jo::view<u32>(jo::self<jo::OptMenu>(this)->video_btn_119->scale_arg);
        joan::orig::as_widget(jo::self<M>(this)->video_btn_11d)->update();  // 0x0061f3b0
        joan::orig::as_widget(jo::self<M>(this)->video_combo_115)->update();
        joan::orig::as_widget(jo::self<M>(this)->video_btn_119)->update();
        joan::orig::as_widget(jo::self<M>(this)->video_combo_ec)->update();
        joan::orig::as_widget(jo::self<M>(this)->video_btn_f0)->update();
        if (before != 0 && jo::view<u32>(jo::self<jo::OptMenu>(this)->video_btn_119->scale_arg) == 0)
            joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(d::s_BUTTON_SLIDE_MANY));  // 0x004ceef0 "BUTTON_SLIDE_MANY"
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Panel::update_742ee0", 0x00742ee0, &H::panel_742ee0);
#undef B

}  // namespace game_b108_15_2
}  // namespace

// ---------------------------------------- from modules/game_b100_16_2.cpp (game/b100_16)
namespace {
namespace game_b100_16_2 {

using joan::hook::global;

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;

using joan::call::thiscall;

// 0x0061f1b0 (5 args) / 0x0061e020 (6 args) on a button.
u32 button_anim(void* b, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    joan::orig::as_widget(b)->scale_to_61f1b0(joan::orig::arg::i16(a0), joan::orig::arg::f32(a1), joan::orig::arg::f32(a2), static_cast<char>(a3), joan::orig::arg::u32(a4));  // 0x0061f1b0
    return 0;  // scale_to returns nothing; OptionMenu::place_buttons_742f90 results are never read
}
u32 button_reset(void* b) { return button_anim(b, 0u, 0x3f800000u, 0u, 1u, 0u); }
void button_place(void* b, i32 x, i32 y) {
    joan::orig::as_widget(b)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);  // 0x0061e020
}

JOAN_COUNTER(c_place_buttons, "OptionMenu::place_buttons_742f90");

struct H {
    // ------------------------------------------------------------------ 0x00742f90
    u32 place_buttons_742f90() {
        c_place_buttons.hit();
        i32 dy = 0;
        if (static_cast<i32>(global<i8>(d::g_game_mode)) == 9) dy = -0x30;
        button_place(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_combo_115), 0x1c0, dy + 0x3e);
        button_reset(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_combo_115));
        button_place(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_btn_119), -0x264, dy + 0xf3);
        button_reset(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_btn_119));
        button_place(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_btn_11d), 0x1ce, dy + 0x2c8);
        button_reset(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_btn_11d));
        button_place(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_combo_ec), 0x1c0, dy + 0x3e);
        button_reset(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_combo_ec));
        button_place(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_btn_f0), -0x264, dy + 0x127);
        return button_reset(jo::view<void*>(jo::self<jo::OptMenu>(this)->video_btn_f0));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("OptionMenu::place_buttons_742f90", 0x00742f90, &H::place_buttons_742f90);
#undef B

}  // namespace game_b100_16_2
}  // namespace

// ---------------------------------------- widened draw distance / grass distance ranges (view_range.hpp)
namespace joan::optmenu {

void clamp_view_ranges() {
    global<i32>(d::g_cfg_far_limit) = clamp_range(global<i32>(d::g_cfg_far_limit), kDrawMin, kDrawMax);
    global<i32>(d::g_cfg_grass_density) = clamp_range(global<i32>(d::g_cfg_grass_density), kGrassMin, kGrassMax);
}

}  // namespace joan::optmenu

// ---------------------------------------- hot-plugged game pads (platform layer, phase C4)
#include "joystick_option.hpp"
#include "../original/bind.hpp"

namespace joan::optmenu {

// The original enumerated its joysticks once, at start-up: the control-type selector (OptPanel +0xc0, a mode-2
// combo box: items +0x9c, count +0xa0, selection +0x80) offers "joystick" (third entry, Menu::optmenu_0e) only when
// one was there then. With pads that can come and go, the entry follows them.
void joystick_presence_changed(bool present) {
    namespace img = joan::img;
    void* const panel = img::obj(d::g_opt_menu);  // the options panel (OptPanel::init 0x0073ba80)
    jo::ComboBox* const combo = jo::self<jo::OptMenu>(panel)->key_combo_c0;
    if (combo == nullptr) return;  // not built yet: OptMenu::build_controls looks at the slot itself
    if (combo->style != 2) return;
    const std::int32_t count = combo->item_count;
    if (present && count == 2) {
        void* const text = thiscall<void*>(fn::code_value(fn::Menu_optmenu_0e), img::obj(d::g_strings));  // 0x004db1c0
        joan::orig::as_combo_box(combo)->add_6b3930(joan::orig::arg::str(text));  // 0x006b3930
    } else if (!present && count >= 3) {
        if (global<u32>(d::g_cfg_control_scheme) == 1) {  // the joystick type was active: back to the keyboard (opt_apply case 1)
            if (global<i32>(d::g_control_mode) == 4) {
                global<u32>(d::g_control_base_mode) = 0;
            } else {
                joan::orig::as_unit(img::obj(d::g_player_unit_recno))->set_state(i32{0}, static_cast<std::int8_t>(0));  // 0x0042e040
            }
            global<u32>(d::g_cfg_control_scheme) = 0;
        }
        if (combo->selected == 3) joan::orig::as_popup(combo)->set_text_6b44e0(1u);  // 0x006b44e0
        joan::orig::as_combo_box(combo)->remove_from(3);  // 0x006b3b90
    }
}

void auto_input_type(bool pad) {
    namespace img = joan::img;
    // Away from the pad: KEYBOARD + MOUSE (type 3), the keyboard type the mouse looks with.
    const u32 current = global<u32>(d::g_cfg_control_scheme);
    const u32 want = pad ? 1u : 3u;
    if (current == want) return;
    if (pad && *global<u32*>(d::g_keys_a51806) == 0) return;  // no joystick in slot 0 (yet)
    // As OptMenu::apply 0x0073ca80 sets the type the input method selector shows: in RTS mode the type to restore,
    // in action mode the controlled hero's state.
    if (global<i32>(d::g_control_mode) == 4)
        global<u32>(d::g_control_base_mode) = want;
    else
        joan::orig::as_unit(img::obj(d::g_player_unit_recno))->set_state(static_cast<i32>(want), static_cast<std::int8_t>(0));  // 0x0042e040
    global<u32>(d::g_cfg_control_scheme) = want;
    // The selector of INPUT OPTIONS (selection 1 / 2 / 3 = type 0 / 3 / 1, OptMenu::build_controls 0x007435e0).
    jo::ComboBox* const combo = jo::self<jo::OptMenu>(img::obj(d::g_opt_menu))->key_combo_c0;
    const u32 sel = want == 0 ? 1u : want == 3 ? 2u : 3u;
    if (combo != nullptr && combo->style == 2 && static_cast<std::int32_t>(sel) <= combo->item_count)
        joan::orig::as_popup(combo)->set_text_6b44e0(sel);  // 0x006b44e0
    hook::log_line("[Input] input method -> %s (the device last used)", pad ? "joystick" : (want == 3 ? "keyboard & mouse" : "keyboard"));
}

}  // namespace joan::optmenu

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::OptMenu::release_all_73c410() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_14::H::release_all>(this)); }  // 0x73c410
auto joan::orig::OptMenu::load() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::optmenu_load>(this)); }  // 0x73c740
auto joan::orig::OptMenu::free_images() -> void { joan::bind::member<&::game_b92_06_2::H::release6>(this); }  // 0x73c8a0
auto joan::orig::OptMenu::open(std::uint8_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_14::H::opt_open>(this, a0)); }  // 0x73c9d0
auto joan::orig::OptMenu::apply() -> void { joan::bind::member<&::game_b108_14::H::opt_apply>(this); }  // 0x73ca80
auto joan::orig::Panel::set_page(char a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_07::H::set_page>(this, a0)); }  // 0x73cc00
auto joan::orig::Screen::layout_73ded0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_20_2::H::layout_73ded0>(this)); }  // 0x73ded0
auto joan::orig::Menu::create_controls_740440() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_01::H::create_controls>(this)); }  // 0x740440
auto joan::orig::Screen::release_740df0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_20_2::H::release_740df0>(this)); }  // 0x740df0
auto joan::orig::Panel::enable_all(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::enable_all>(this, a0)); }  // 0x7412e0
auto joan::orig::OptMenu::update_page_2() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_17_2::H::opt_handle>(this)); }  // 0x7413e0
auto joan::orig::Panel::update_741960() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_07::H::update_741960>(this)); }  // 0x741960
auto joan::orig::OptMenu::place_buttons() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::place_buttons>(this)); }  // 0x741a70
auto joan::orig::OptMenu::layout_741cd0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15_4::H::layout_741cd0>(this)); }  // 0x741cd0
auto joan::orig::Panel::slide_in_7420e0() -> void { joan::bind::member<&::game_b103_16_3::H::slide_7420e0>(this); }  // 0x7420e0
auto joan::orig::OptMenu::build_controls_742470() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_08::H::build_742470>(this)); }  // 0x742470
auto joan::orig::Screen::release_7429c0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_20_2::H::release_7429c0>(this)); }  // 0x7429c0
auto joan::orig::Panel::set_mode_742bf0(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_05::H::set_mode_742bf0>(this, a0)); }  // 0x742bf0
auto joan::orig::OptMenu::update_page_4() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_14::H::opt_click>(this)); }  // 0x742c80
auto joan::orig::Panel::update_742ee0() -> void { joan::bind::member<&::game_b108_15_2::H::panel_742ee0>(this); }  // 0x742ee0
auto joan::orig::OptMenu::place_buttons_742f90() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_16_2::H::place_buttons_742f90>(this)); }  // 0x742f90
auto joan::orig::Panel::layout_743170() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_05::H::layout_743170>(this)); }  // 0x743170
auto joan::orig::Slider::open() -> void { joan::bind::member<&::game_b92_06_2::H::slider_open>(this); }  // 0x7433d0
auto joan::orig::OptMenu::build_controls_7435e0() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b110_10::H::opt_build>(this)); }  // 0x7435e0
auto joan::orig::OptMenu::release() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b106_10::H::optmenu_release>(this)); }  // 0x744ce0
auto joan::orig::OptMenu::draw_745720(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_19::H::opt_draw>(this, a0)); }  // 0x745720
auto joan::orig::OptMenu::handle_input_745b10() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b109_05::H::opt_input>(this)); }  // 0x745b10
auto joan::orig::OptMenu::release_all() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_16::H::opt_release_all>(this)); }  // 0x746ff0
auto joan::orig::OptMenu::layout_747200() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15::H::layout_747200>(this)); }  // 0x747200
auto joan::orig::OptMenu::list_keys(std::int8_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_17::H::list_keys>(this, a0)); }  // 0x7478a0
auto joan::orig::OptMenu::layout_747bc0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15_3::H::layout_747bc0>(this)); }  // 0x747bc0
auto joan::orig::Panel::slide_in_7480e0() -> void { joan::bind::member<&::game_b103_16_2::H::slide_7480e0>(this); }  // 0x7480e0
auto joan::orig::OptMenu::layout_credits() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_15::H::credits_layout>(this)); }  // 0x748540
auto joan::orig::Menu::slide_in_748830() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b101_16::H::slide_in_748830>(this)); }  // 0x748830
auto joan::orig::OptMenu::build_buttons() -> void { joan::bind::member<&::game_b109_11::H::options_build>(this); }  // 0x748b10
auto joan::orig::Screen::release_749e00() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b105_20::H::release_749e00>(this)); }  // 0x749e00
auto joan::orig::Panel::enable_74a580(std::uint32_t a0) -> void { joan::bind::member<&::game_b108_15::H::panel_74a580>(this, a0); }  // 0x74a580
auto joan::orig::OptMenu::update_page_1() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b109_14::H::options_update>(this)); }  // 0x74a700
auto joan::orig::OptMenu::refresh_buttons() -> void { joan::bind::member<&::game_b108_17::H::slide_btns>(this); }  // 0x74b550
auto joan::orig::OptMenu::layout_74b6b0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15_3::H::layout_74b6b0>(this)); }  // 0x74b6b0
auto joan::orig::Panel::layout_items() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b104_16::H::layout_items>(this)); }  // 0x74bc50
auto joan::orig::Panel::slide_in_74c2e0() -> void { joan::bind::member<&::game_b103_16_2::H::slide_74c2e0>(this); }  // 0x74c2e0
auto joan::orig::Widget::m_9df888_ctor() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b95_08::H::widget_9df888_ctor>(this)); }  // 0x75fab0
auto joan::orig::ScrollBox::init(std::uint32_t a0, std::uint32_t a1, std::int32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> void { joan::bind::member<&::game_b103_16::H::init_760050>(this, a0, a1, a2, a3, a4, a5); }  // 0x760050
auto joan::orig::Panel::destroy_children() -> void { joan::bind::member<&::game_b92_06::H::destroy_children>(this); }  // 0x760440
auto joan::orig::ScrollBox::set_value(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b91_04::H::forward_761240>(this, a0)); }  // 0x761240
