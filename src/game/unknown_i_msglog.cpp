// game/unknown_i_msglog.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_msglog.cpp;
// re/attribution.csv: between ww/interfac/i_letter.cpp and ww/interfac/i_sms.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::call::thiscall;
using joan::img::obj;

using joan::hook::global;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using u16 = std::uint16_t;

using joan::hook::x87::ftol;

using joan::call::cdecl_call;
using joan::img::str;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {


JOAN_COUNTER(c_panel_show, "Panel::show_6e19b0");

struct H {
    // ------------------------------------------------------------------------ 0x006d65b0 / 0x006e0e20
    void* self_ret() { return this; }

    // ------------------------------------------------------------------------ 0x006e19b0
    std::uintptr_t show_6e19b0(i8 on, i8 mode) {
        c_panel_show.hit();
        if (static_cast<i32>(on) != 0) {
            joan::orig::as_msg_log(this)->load_6e1570();  // 0x006e1570
            if (static_cast<i32>(mode) == 1) jo::self<jo::MessageLog>(this)->mode = 1;
            jo::view<u16>(jo::self<jo::MessageLog>(this)->page_count) = 0;
            for (i32 i = 0; i < 9; ++i) {
                const u16 n = static_cast<u16>(joan::orig::arg::as<u32>(joan::orig::as_msg_log(this)->pages_in_section(joan::orig::arg::i32(i))));  // 0x006e2910
                jo::view<u16>(jo::self<jo::MessageLog>(this)->page_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::MessageLog>(this)->page_count) + n);
            }
            const i32 w47 = static_cast<i32>(jo::self<jo::MessageLog>(this)->page);
            const i32 w4b = static_cast<i32>(jo::self<jo::MessageLog>(this)->page_count);
            if ((w47 == 0 && w4b != 0) || w47 > w4b) {
                jo::self<jo::MessageLog>(this)->page = jo::self<jo::MessageLog>(this)->page_count;
                if (static_cast<i32>(jo::self<jo::MessageLog>(this)->page) % 2 == 0) jo::view<u16>(jo::self<jo::MessageLog>(this)->page) = static_cast<u16>(jo::view<u16>(jo::self<jo::MessageLog>(this)->page) - 1);
                jo::self<jo::MessageLog>(this)->entry = static_cast<u16>(
                    joan::orig::arg::as<u32>(joan::orig::as_msg_log(this)->entry_at(static_cast<i32>(jo::self<jo::MessageLog>(this)->page))));  // 0x006e29a0
            }
            if (global<u8>(d::g_game_running) != 0) {
                global<u8>(d::g_game_running) = 0;
                global<u16>(d::g_saved_game_speed) = static_cast<u16>(
                    joan::hook::x87::ftol(static_cast<double>(global<float>(d::g_game_speed))));
                joan::orig::as_game(obj(d::g_game_a50088))->set_speed(0u);  // 0x0042f930
            }
            joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();                   // 0x00592c30
            const char* const v = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->igamemnu_text_15());     // 0x004daec0
            joan::orig::as_ingame_frame(obj(d::g_hud))->set_title(joan::orig::arg::str(v));                // 0x006f2ac0
            joan::orig::as_ingame_frame(obj(d::g_hud))->slide_in();                   // 0x006f24d0
            joan::orig::as_widget(jo::view<void*>(jo::self<jo::MessageLog>(this)->title))->move_to(0x400, 0x2c8, joan::orig::arg::f32(0x455ac000u), joan::orig::arg::f32(0xc4bb8000u), 1, 0u);  // 0x0061e020
            joan::orig::as_widget(jo::view<void*>(jo::self<jo::MessageLog>(this)->close_button))->scale_to(0, joan::orig::arg::f32(0x40c00000u), 0.0f, 1, 0u);                      // 0x0061f1b0
            joan::orig::as_widget(jo::view<void*>(jo::self<jo::MessageLog>(this)->title))->move_to(0, 0x2c8, joan::orig::arg::f32(0x455ac000u), joan::orig::arg::f32(0xc4bb8000u), 0, 0u);
            joan::orig::as_widget(jo::view<void*>(jo::self<jo::MessageLog>(this)->close_button))->scale_to(1, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0xc8u);
            jo::self<jo::MessageLog>(this)->open = on;
        } else {
            joan::orig::as_widget(jo::view<void*>(jo::self<jo::MessageLog>(this)->title))->move_to(0x400, 0x2c8, joan::orig::arg::f32(0x455ac000u), joan::orig::arg::f32(0xc4bb8000u), 0, 0x96u);
            joan::orig::as_widget(jo::view<void*>(jo::self<jo::MessageLog>(this)->close_button))->scale_to(0, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);
            joan::orig::as_cursor(obj(d::g_mouse))->set_pos();  // 0x00592bd0
        }
        return joan::orig::arg::as<u32>(joan::orig::as_parser(obj(d::g_mouse))->reset());  // 0x00595a20
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::self_6e0e20", 0x006e0e20, &H::self_ret);
B("Panel::show_6e19b0", 0x006e19b0, &H::show_6e19b0);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {


void* const kRes = obj(d::g_texture_array);
void* const kText = obj(d::g_strings);

using joan::orig::helpers::game_new;                       // operator new

// new Widget (a Button, 0x346 bytes on i686) / new TextDesc (Image2D, 0xb7 bytes on i686), null when operator new fails.
void* new_widget() {
    void* const m = game_new(static_cast<u32>(sizeof(jo::Button)));  // the full button object (0x620ee0 constructs it)
    return m != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(m)->ctor_620ee0()) : nullptr;  // 0x00620ee0
}
void* new_text() {
    void* const m = game_new(static_cast<u32>(sizeof(jo::Image2D)));
    return m != nullptr ? joan::orig::as_image2d(m)->construct() : nullptr;  // 0x00629400
}

// Colour record 0x0042a260(r, g, b, a) on a 4-byte local; returns its dword.
u32 colour(i32 r, i32 g, i32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));  // 0x0042a260
    return jo::view<u32>(*jo::as_colour(p));
}

JOAN_COUNTER(c_longbar, "LongBar::init_6e0e40");
JOAN_COUNTER(c_msglog, "MsgLog::load_6e1570");

struct H {
    // ------------------------------------------------------------------ 0x006e0e40
    void longbar_6e0e40() {
        c_longbar.hit();
        jo::self<jo::MessageLog>(this)->sprites = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_msglog));
        jo::self<jo::MessageLog>(this)->font = img::obj(d::g_bmp_commele2);
        jo::self<jo::MessageLog>(this)->icons = static_cast<jo::BmpRes*>(img::obj(d::g_icons));
        jo::view<u8>(jo::self<jo::MessageLog>(this)->open) = 0;
        jo::view<void*>(jo::self<jo::MessageLog>(this)->title) = new_text();
        const u32 black = colour(0, 0, 0, 0xff);
        joan::orig::as_label(jo::view<void*>(jo::self<jo::MessageLog>(this)->title))->create(0x400, 0x2c8, joan::orig::arg::mptr(img::obj(d::g_bmp_commele2)), str(d::s_LONGBAR), joan::orig::arg::i32(black), joan::orig::arg::i32(// 0x00629660
                      3), 0, 2, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, 1, 1);                             // "LONGBAR"
        jo::view<void*>(jo::self<jo::MessageLog>(this)->close_button) = new_widget();
        joan::orig::as_button(jo::view<void*>(jo::self<jo::MessageLog>(this)->close_button))->create(0x1ce, 0x2c8, 0x231, 0x2e6, fn::code_value(fn::draw_mark_6e2ab0), joan::orig::arg::i32(// 0x00621540
                      1), 1, 0, 1, 1, 1);                                                     // cb 0x006e2ab0
        const u32 white = colour(0xff, 0xff, 0xff, 0xff);
        const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(kText)->igamemnu_text_14());  // 0x004dae90 (IGAMEMNU 14)
        joan::orig::as_button(jo::view<void*>(jo::self<jo::MessageLog>(this)->close_button))->set_text(joan::orig::arg::str(text), img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(white), 1, 2, 0, 2, 2);  // 0x00623ce0
    }

    // ------------------------------------------------------------------ 0x006e1570
    void arrow_button(jo::Button*& slot, i32 x, img::Addr ot, img::Addr dw, img::Addr ov) {
        if (slot != nullptr) return;
        jo::view<void*>(slot) = new_widget();
        joan::orig::as_menu_button(slot)->setup(joan::orig::arg::i16(x), joan::orig::arg::i16(0x28c), joan::orig::arg::mptr(jo::self<jo::MessageLog>(this)->font), joan::orig::arg::str(str(ot)), joan::orig::arg::str(str(dw)), joan::orig::arg::str(str(ov)), joan::orig::arg::str(// 0x006211d0
                      0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));
    }
    void load_res(void*& slot, img::Addr name) {
        if (slot != nullptr) return;
        slot = joan::orig::as_texture_manager(kRes)->load(joan::orig::arg::str(str(name)), 1, 0u);  // 0x006099c0
    }
    void msglog_6e1570() {
        c_msglog.hit();
        const u32 state = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kRes)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kRes)->set_mode(1u);                 // 0x0060a740
        load_res(jo::self<jo::MessageLog>(this)->res[0], d::s_MSGLOG);   // "MSGLOG"
        load_res(jo::self<jo::MessageLog>(this)->res[1], d::s_MSGLOG_B);   // "MSGLOG_B"
        load_res(jo::self<jo::MessageLog>(this)->res[2], d::s_COMMELE2);   // "COMMELE2"
        load_res(jo::self<jo::MessageLog>(this)->res[3], d::s_MESSAGE);   // "MESSAGE"
        load_res(jo::self<jo::MessageLog>(this)->res_41, d::s_COMMELE2);  // "COMMELE2"
        joan::orig::as_texture_manager(kRes)->set_mode(joan::orig::arg::u32(state));  // 0x0060a740
        arrow_button(jo::self<jo::MessageLog>(this)->prev_button, 0x78, d::s_aw_l_ot, d::s_aw_l_dw, d::s_aw_l_ov);   // aw_l_ot / dw / ov
        arrow_button(jo::self<jo::MessageLog>(this)->next_button, 0x364, d::s_aw_r_ot, d::s_aw_r_dw, d::s_aw_r_ov);  // aw_r_ot / dw / ov
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("LongBar::init_6e0e40", 0x006e0e40, &H::longbar_6e0e40);
B("MsgLog::load_6e1570", 0x006e1570, &H::msglog_6e1570);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {


JOAN_COUNTER(c_release_children, "Obj::release_children");

struct H {
    // ------------------------------------------------------------------ 0x006e1070
    void release_children() {
        c_release_children.hit();
        joan::orig::as_panel(this)->release();  // 0x006e1810
        if (jo::self<jo::MessageLog>(this)->title != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::MessageLog>(this)->title);
            if (p != nullptr) joan::orig::as_image2d(p)->destroy(1u);  // 0x006bd440
            jo::self<jo::MessageLog>(this)->title = nullptr;
        }
        if (jo::self<jo::MessageLog>(this)->close_button != nullptr) {
            void* const p = jo::view<void*>(jo::self<jo::MessageLog>(this)->close_button);
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b33a0(1u);  // 0x006b33a0
            jo::self<jo::MessageLog>(this)->close_button = nullptr;
        }
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Obj::release_children", 0x006e1070, &H::release_children);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b116_07.cpp (game/b116_07)
namespace {
namespace game_b116_07 {

constexpr u32 kM1 = 0xffffffffu;

i32 gsb(img::Addr a) { return static_cast<i32>(global<i8>(a)); }

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite list

// Colour helpers: 0x0042a260 (thunk 0x004098b8, r, g, b, a) and 0x005e9640 (thunk 0x00407577,
// argb dword) on a 4-byte stack object, the dword read back through the returned pointer.
u32 rgba(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
}
// 0x00633500 (thunk 0x00401bbd, 17 args, ret 0x44) on the sprite list.
void sprite(u32 x, u32 y, u32 a3, const char* name, void* holder, u32 colour, u32 a7, u32 a8, u32 a9,
            u32 a10, u32 a11, u32 a12, u32 a13, u32 a14, u32 a15, u32 a16, u32 a17) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a3), joan::orig::arg::str(name), joan::orig::arg::mptr(holder), joan::orig::arg::u32(colour), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), joan::orig::arg::i16(a15), joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));  // 0x00633500
}

JOAN_COUNTER(c_log, "MessageLog::draw_6e1130");

struct H {
    // 0x006e1130
    void log_draw() {
        joan::canvas::ScreenScope canvas_scope(0x006e1130);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_log.hit();
        if (static_cast<i32>(jo::self<jo::MessageLog>(this)->open) == 0) return;
        joan::orig::as_menu(obj(d::g_hud))->reset_fades(static_cast<std::int8_t>(joan::orig::arg::i32(u32{0})));  // 0x006f21c0
        if (gsb(d::g_menu_frame_state) != 2) return;
        joan::orig::as_label(jo::self<jo::MessageLog>(this)->title)->draw(5u, 7u);  // 0x00629bd0
        joan::orig::as_button(jo::self<jo::MessageLog>(this)->close_button)->draw(5u, 7);  // 0x00622cc0
        const u32 c = rgba(0xff, 0xff, 0xff, 0xff);
        sprite(0x48, 0x88, 2, &img::global<const char>(d::s_cover), jo::self<jo::MessageLog>(this)->sprites, c, 3, 0, 0, 0, kM1, kM1,
               kM1, kM1, 0, kM1, kM1);  // 'cover'
        joan::orig::as_msg_log(this)->draw_page(joan::orig::arg::i32(static_cast<i32>(jo::self<jo::MessageLog>(this)->page)));      // 0x006e22e0
        joan::orig::as_msg_log(this)->draw_page(joan::orig::arg::i32(static_cast<i32>(jo::self<jo::MessageLog>(this)->page) + 1));  // 0x006e22e0
    }
};

JOAN_REPLACE("game/b116_07", "MessageLog::draw", 0x006e1130, &H::log_draw);
JOAN_CANVAS_SELF(0x006e1130);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_07
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {


JOAN_COUNTER(c_dialog_step, "Dialog::step_6e1250");

struct H {
    // ------------------------------------------------------------------ 0x006e1250
    static i32 pressed(void* button, i32 key) {
        return joan::orig::arg::as<i32>(joan::orig::as_ui_button(button)->mouse(joan::orig::arg::i32(key), i32{0}, i32{-1}, i32{0}, i32{0}, i32{0}));  // 0x006218c0
    }

    bool dialog_step() {
        c_dialog_step.hit();
        if (jo::self<jo::MessageLog>(this)->open == 0) return false;
        if (global<i8>(d::g_menu_frame_state) != 2) return true;
        if (jo::view<u32>(jo::self<jo::MessageLog>(this)->turn_kind) == 0) {
            if (pressed(jo::self<jo::MessageLog>(this)->prev_button, 0) != 0 && static_cast<i32>(jo::self<jo::MessageLog>(this)->page) > 1) {
                jo::self<jo::MessageLog>(this)->page = static_cast<i16>(jo::view<u16>(jo::self<jo::MessageLog>(this)->page) - 2u);
            } else if (pressed(jo::self<jo::MessageLog>(this)->next_button, 0) != 0) {
                if (!(static_cast<i32>(jo::self<jo::MessageLog>(this)->page) + 2 > static_cast<i32>(jo::self<jo::MessageLog>(this)->page_count))) {
                    jo::self<jo::MessageLog>(this)->page = static_cast<i16>(jo::view<u16>(jo::self<jo::MessageLog>(this)->page) + 2u);
                }
            }
        }
        if (pressed(jo::self<jo::MessageLog>(this)->close_button, 0x1b) != 0) {
            joan::orig::as_panel(this)->show_6e19b0(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x006e19b0
            if (jo::view<i8>(jo::self<jo::MessageLog>(this)->mode) != 0) global<u32>(d::g_screen_pending) = 1;
            return true;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_mode(obj(d::g_screen_mgr))->check_6f5a70()) != 0) {  // 0x006f5a70
            joan::orig::as_panel(this)->show_6e19b0(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));
        }
        return true;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("Dialog::step_6e1250", 0x006e1250, &H::dialog_step);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b108_12.cpp (game/b108_12)
namespace {
namespace game_b108_12 {

JOAN_COUNTER(c_6e13c0, "Obj::update_6e13c0");

// ------------------------------------------------------------------ thiscall members
struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // 0x006bfe00
    static void upd(void* o) { joan::orig::as_widget(o)->update(); }  // 0x0061f3b0
    static bool is_zero_or_nan(float v, float c) { return !(v < c) && !(v > c); }

    // 0x006e13c0
    void update_6e13c0() {
        c_6e13c0.hit();
        if (static_cast<i8>(self()[0]) == 0) return;
        if (static_cast<i8>(global<u8>(d::g_menu_frame_state)) == 2) {
            upd(jo::self<jo::MessageLog>(this)->close_button);
            upd(jo::self<jo::MessageLog>(this)->title);
            if (jo::self<jo::MessageLog>(this)->title->x != 0x400) return;
            if (!is_zero_or_nan(jo::self<jo::MessageLog>(this)->close_button->scale, 0.0f)) return;
            if (global<u32>(d::g_screen_pending) == 0 || global<u32>(d::g_screen_pending) == 2) {
                joan::orig::as_menu(obj(d::g_hud))->slide_out();  // 0x006f26e0
                return;
            }
            self()[0] = 0;
            joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
            return;
        }
        joan::orig::as_panel(obj(d::g_hud))->update_6f2340();  // 0x006f2340
        if (static_cast<i8>(global<u8>(d::g_menu_frame_state)) != 0) return;
        if (global<u32>(d::g_screen_pending) != 0) {
            joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
        } else {
            if (global<u8>(d::g_game_running) == 0) {
                global<u8>(d::g_game_running) = 1;
                joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(static_cast<float>(global<i16>(d::g_saved_game_speed))));  // 0x0042f930
            }
            joan::orig::as_triple(obj(d::g_screen_mgr))->clear();  // 0x006f5e90
        }
        joan::orig::as_panel(this)->release();  // 0x006e1810
        jo::self<jo::MessageLog>(this)->page = 0;
        jo::self<jo::MessageLog>(this)->u16_45 = 0;
        self()[0] = 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_12", name, addr, fn)
B("Obj::update_6e13c0", 0x006e13c0, &H::update_6e13c0);
#undef B

}  // namespace game_b108_12
}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {


struct H {
    void nop0() {}

    // 0x006e1550
    std::uintptr_t byte_0_6e1550() { return (reinterpret_cast<std::uintptr_t>(this) & ~std::uintptr_t{0xffu}) | jo::view<u8>(jo::self<jo::MessageLog>(this)->open); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("Obj::nop_6e1530", 0x006e1530, &H::nop0);
B("Obj::byte_0_6e1550", 0x006e1550, &H::byte_0_6e1550);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

void* const kImages = obj(d::g_texture_array);


void image_release(void* h) { joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(h)); }                    // 0x0060a550

struct H {
    void release_at(void*& slot) {
        if (slot != nullptr) {
            image_release(slot);
            slot = nullptr;
        }
    }

    // 0x006e1810
    void panel_release() {
        release_at(jo::self<jo::MessageLog>(this)->res[0]);
        release_at(jo::self<jo::MessageLog>(this)->res[1]);
        release_at(jo::self<jo::MessageLog>(this)->res[2]);
        release_at(jo::self<jo::MessageLog>(this)->res[3]);
        release_at(jo::self<jo::MessageLog>(this)->res_41);
        if (jo::view<void*>(jo::self<jo::MessageLog>(this)->prev_button) != nullptr) {
            joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::MessageLog>(this)->prev_button))->scalar_dtor_6b33a0(1u);  // 0x006b33a0
            jo::self<jo::MessageLog>(this)->prev_button = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::MessageLog>(this)->next_button) != nullptr) {
            joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::MessageLog>(this)->next_button))->scalar_dtor_6b33a0(1u);
            jo::self<jo::MessageLog>(this)->next_button = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Panel::release", 0x006e1810, &H::panel_release);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

JOAN_COUNTER(c_backdrop, "Panel::refresh_backdrop");

struct H {
    // ------------------------------------------------------------------ 0x006e1c60
    void refresh_backdrop() {
        c_backdrop.hit();
        if (jo::self<jo::MessageLog>(this)->turn_kind == 0) return;
        i32 box[4] = {};  // [ebp-0x14], [ebp-0x18], [ebp-0x1c], [ebp-0x20] (low words zeroed)
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->scaled_rect(joan::orig::arg::i16(0x68), joan::orig::arg::i16(0x98), joan::orig::arg::i16(0x398), joan::orig::arg::i16(0x2a7), 1.0f, 1.0f, &box[0], &box[1], &box[2], &box[3]);  // 0x00633ce0
        alignas(alignof(void*)) u8 surface[sizeof(jo::Image)];  // 0x34 bytes on i686
        joan::orig::as_image(static_cast<void*>(surface))->construct();  // 0x0060fad0 ctor (memset 0x34)
        const i32 x0 = *reinterpret_cast<const i16*>(&box[0]);  // the low words
        const i32 y0 = *reinterpret_cast<const i16*>(&box[1]);
        const i32 x1 = *reinterpret_cast<const i16*>(&box[2]);
        const i32 y1 = *reinterpret_cast<const i16*>(&box[3]);
        joan::orig::as_image(static_cast<void*>(surface))->create(joan::orig::arg::u32(x1 - x0 + 1), joan::orig::arg::u32(y1 - y0 + 1), 0u, joan::orig::arg::i32(global<u32>(d::g_a5cc7c)));  // 0x0060fb60
        i32 rect[4] = {x0, y0, x1 + 1, y1 + 1};
        i32 point[2] = {0, 0};
        com_call<u32>(global<void*>(d::g_game_device), 0x70 / 4, global<void*>(d::g_vga_back), static_cast<void*>(rect), 1u,
                      jo::as_image(static_cast<void*>(surface))->surface, static_cast<void*>(point));  // CopyRects
        joan::orig::as_msg_log(this)->capture_page();  // 0x006e1ea0
        i32 rect2[4] = {0, 0, x1 - x0 + 1, y1 - y0 + 1};
        i32 point2[2] = {x0, y0};
        com_call<u32>(global<void*>(d::g_game_device), 0x70 / 4, jo::as_image(static_cast<void*>(surface))->surface, static_cast<void*>(rect2), 1u,
                      global<void*>(d::g_vga_back), static_cast<void*>(point2));  // CopyRects
        joan::orig::as_image(static_cast<void*>(surface))->release();  // 0x0060fd20
        joan::orig::as_image(static_cast<void*>(surface))->dtor();  // 0x0060fb10 dtor
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Panel::refresh_backdrop", 0x006e1c60, &H::refresh_backdrop);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b115_11.cpp (fix/uninit)
namespace {
namespace game_b115_11 {

// COM call through the vtable (stdcall, this first).
template <class R, class... A>
R com_call(void* self, u32 slot_offset, A... args) {
    void* const* const vt = joan::call::vtable_of(self);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vt[joan::call::x86_vslot(slot_offset)])(self, args...);
}

void* const kRender = obj(d::g_sprites_a46128);
void* const kTextures = obj(d::g_texture_array);

JOAN_COUNTER(c_page, "PageTurn::capture_6e1ea0 (fix/uninit)");

struct H {
    // ======================================================== 0x006e1ea0
    u32 page_capture() {
        c_page.hit();
        const u32 saved = jo::view<u32>(jo::self<jo::MessageLog>(this)->turn_kind);
        jo::view<u32>(jo::self<jo::MessageLog>(this)->turn_kind) = 0;
        joan::orig::as_msg_log(this)->draw();  // 0x006e1130
        jo::view<u32>(jo::self<jo::MessageLog>(this)->turn_kind) = saved;
        if (joan::orig::arg::as<i32>(joan::orig::as_sprite_queue(kRender)->begin_frame()) != 0) {  // 0x00631d50
            joan::orig::as_sprite_queue(kRender)->end_frame();          // 0x00631d80
        } else {
            joan::orig::as_sprite_queue(kRender)->release_list();  // 0x00631dc0
        }
        i16 l = 0, t = 0, r = 0, b = 0;  // [ebp-0xc] / -0x10 / -0x14 / -0x18
        const float sx = 1.0f, sy = 1.0f;
        char name[8] = {};  // [ebp-0x28] (6 bytes used; set but never read)
        std::memcpy(name, img::obj(d::s_Page1), 6);  // "Page1"
        const i32 kind = jo::self<jo::MessageLog>(this)->turn_kind;
        if (kind == 1) {
            joan::orig::as_sprite2d(kRender)->scaled_rect(joan::orig::arg::i16(0x68), joan::orig::arg::i16(0x98), joan::orig::arg::i16(0x1ff), joan::orig::arg::i16(0x277), joan::orig::arg::f32(sy), joan::orig::arg::f32(sx), &l, &t, &r, &b);  // 0x00633ce0
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(name))), str(d::s_Page2));             // strcpy "Page2"
        } else if (kind == 2) {
            joan::orig::as_sprite2d(kRender)->scaled_rect(joan::orig::arg::i16(0x200), joan::orig::arg::i16(0x98), joan::orig::arg::i16(0x397), joan::orig::arg::i16(0x2a7), joan::orig::arg::f32(sy), joan::orig::arg::f32(sx), &l, &t, &r, &b);
        }
        i32 rect[4] = {l, t, r, b};  // [ebp-0x3c]
        i32 point[2] = {0, 0};       // [ebp-0x44]
        if (jo::self<jo::MessageLog>(this)->page_texture != nullptr) {
            joan::orig::texture_ref::release(joan::orig::arg::mptr(jo::self<jo::MessageLog>(this)->page_texture));  // 0x0042cef0
            jo::self<jo::MessageLog>(this)->page_texture = nullptr;
        }
        jo::self<jo::MessageLog>(this)->page_texture = joan::orig::arg::as<jo::RenderTexture*>(joan::orig::as_texture_manager(kTextures)->create_2d());  // 0x006094c0
        joan::orig::as_texture(jo::self<jo::MessageLog>(this)->page_texture)->create(0x200u, 0x400u, 1u, joan::orig::arg::u32(global<u32>(d::g_a5cc7c)));  // 0x00605e20
        void* surface = nullptr;  // fix/uninit: [ebp-0x48] stays garbage if GetSurfaceLevel fails
        void* const tex = jo::view<void*>(jo::self<jo::MessageLog>(this)->page_texture->d3d_texture);
        com_call<i32>(tex, 0x3c, 0u, &surface);  // GetSurfaceLevel(0, &surface)
        void* const dev = global<void*>(d::g_game_device);
        com_call<i32>(dev, 0x70, global<void*>(d::g_vga_back), static_cast<void*>(rect), 1u, surface,
                      static_cast<void*>(point));  // CopyRects
        // fix/uninit: the [ebp-0x2c] object is NULL -> the UV / texture rewrite is skipped.
        if (surface == nullptr) return 0;
        return com_call<u32>(surface, 0x8);  // Release
    }
};

#define F(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: [ebp-0x2c] never written -> NULL (UV rewrite skipped); NULL surface not released.
F("PageTurn::capture_6e1ea0", 0x006e1ea0, &H::page_capture);
#undef F

}  // namespace game_b115_11
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {


JOAN_COUNTER(c_draw_page, "Book::draw_page_6e22e0");

// ---- shared draw helpers ----------------------------------------------------------------
void* const kSprites = obj(d::g_sprites_a46128);
// 0x0042a260 (thunk 0x004098b8, ret 0x10): packs r, g, b, a into the 4-byte colour object.
u32 packed_colour(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));
}
// 0x00633500 (thunk 0x00401bbd, ret 0x44) sprite queue, with the 11 trailing constants
// (3, 0, mode, 0, -1, -1, -1, -1, 0, -1, -1) every caller here pushes.
void sprite(u32 x, u32 y, u32 a3, const void* name, const void* sheet, u32 colour, i32 mode) {
    joan::orig::as_sprite_queue(kSprites)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a3), joan::orig::arg::str(name), joan::orig::arg::mptr(sheet), joan::orig::arg::u32(colour), 3, 0, joan::orig::arg::i16(mode), 0, -1, -1, -1, -1, 0, -1, -1);
}

struct H {
    // 0x006e22e0
    void draw_page_6e22e0(i32 page) {
        c_draw_page.hit();
        const bool right = page % 2 == 0;  // [ebp-0x8]
        const i32 w4b = jo::self<jo::MessageLog>(this)->page_count;
        const i32 number = w4b % 2 != 0 ? w4b + 1 : w4b;  // [ebp-0xc]
        const u32 white = packed_colour(0xff, 0xff, 0xff, 0xff);  // [ebp-0x10]
        const u32 shade = packed_colour(0, 0, 0, 0xbb);           // [ebp-0x14]
        if (right) {
            sprite(0x200, 0x98, 2, str(d::s_page_r), jo::self<jo::MessageLog>(this)->sprites, white, 0);  // "page_r"
            const char* const s = joan::orig::as_msg_log(obj(d::g_strings))->open_1(joan::orig::arg::u32(page), joan::orig::arg::u32(number));  // 0x004de120
            joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_2(0x359, 0x28c, 2u, s, joan::orig::arg::u32(shade), 0, 1, 2, 0, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x3f800000u));  // 0x00626080
            joan::orig::as_button(jo::view<void*>(jo::self<jo::MessageLog>(this)->next_button))->draw(2u, 7);  // 0x00622cc0
        } else {
            sprite(0x68, 0x98, 2, str(d::s_page_l), jo::self<jo::MessageLog>(this)->sprites, white, 0);  // "page_l"
            const char* const s = joan::orig::as_msg_log(obj(d::g_strings))->open_1(joan::orig::arg::u32(page), joan::orig::arg::u32(number));
            joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_line(0xa4, 0x28c, 2u, s, joan::orig::arg::u32(shade), joan::orig::arg::i32(-1), 0, 1, 2, 0, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x3f800000u));  // 0x006255d0
            joan::orig::as_button(jo::view<void*>(jo::self<jo::MessageLog>(this)->prev_button))->draw(2u, 7);
        }
        const u32 first = joan::orig::arg::as<u32>(joan::orig::as_msg_log(this)->entry_at(joan::orig::arg::i32(page)));  // 0x006e29a0
        if (page <= 0 || first == 0) return;
        u8* entry = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_portraits))->at_4865f0(joan::orig::arg::i32(first)));  // 0x004865f0
        u32 kind = 0;  // [ebp-0x20]
        i16 x = 0;     // [ebp-0x24]
        if (right) {
            const char* const s = joan::orig::as_menu(obj(d::g_strings))->msglog_2(joan::orig::arg::u32(static_cast<i32>(jo::as_msg_entry(entry)->kind)));  // 0x004de160
            joan::orig::as_text(obj(d::g_font_goudy_bold_18))->draw_2(0x384, 0xa0, 2u, s, joan::orig::arg::u32(shade), 0, 1, 2, 0, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x3f800000u));
            kind = 2;
            x = 0x214;
        } else {
            const char* const s = joan::orig::as_menu(obj(d::g_strings))->msglog_2(joan::orig::arg::u32(static_cast<i32>(jo::as_msg_entry(entry)->kind)));
            joan::orig::as_text(obj(d::g_font_goudy_bold_18))->draw_line(0x7c, 0xa0, 2u, s, joan::orig::arg::u32(shade), joan::orig::arg::i32(-1), 0, 1, 2, 0, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x3f800000u));
            kind = 1;
            x = 0x7c;
        }
        i16 n = 0;  // [ebp-0x2c]
        while (entry != nullptr && static_cast<i32>(n) < 4) {
            n = static_cast<i16>(n + 1);
            const i16 y = static_cast<i16>((static_cast<i32>(n) - 1) * 0x70 + 0xca);  // [ebp-0x28]
            sprite(static_cast<u32>(static_cast<u16>(x)), static_cast<u32>(static_cast<u16>(y)), kind, str(d::s_ICON_1),
                   jo::self<jo::MessageLog>(this)->icons, white, 2);  // "ICON_1"
            const char* const pic = jo::view<const char*>(jo::as_msg_entry(entry)->extra);
            if (joan::orig::arg::as<i32>(joan::orig::libc::strncmp(joan::orig::arg::str(pic), str(d::s_M000), 4u)) != 0) {  // strncmp "M000"
                sprite(static_cast<u32>(static_cast<u16>(x)), static_cast<u32>(static_cast<u16>(y)), kind, pic,
                       jo::self<jo::MessageLog>(this)->icons, white, 2);
            } else {
                sprite(static_cast<u32>(static_cast<i32>(x) + 10), static_cast<u32>(static_cast<i32>(y) + 10), kind,
                       pic, jo::self<jo::MessageLog>(this)->icons, white, 2);
            }
            sprite(static_cast<u32>(static_cast<u16>(x)), static_cast<u32>(static_cast<u16>(y)), kind, str(d::s_ICON_2),
                   jo::self<jo::MessageLog>(this)->icons, white, 2);  // "ICON_2"
            joan::orig::as_text(obj(d::g_font_times_10))->draw(joan::orig::arg::i32(static_cast<i32>(x) + 0x50), joan::orig::arg::i32(static_cast<u32>(static_cast<u16>(y))), joan::orig::arg::i32(static_cast<i32>(x) + 0x15e), joan::orig::arg::i32(static_cast<i32>(y) + 0x64), joan::orig::arg::u32(kind), jo::as_msg_entry(entry)->text, joan::orig::arg::u32(shade), 2, 1, 1, 1, 2, 0, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x3f800000u), 1, joan::orig::arg::i32(-1));  // 0x00626320
            if (joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_portraits))->is_null_at(joan::orig::arg::i32(jo::as_msg_entry(entry)->next))) != 0) {  // 0x00486620
                entry = nullptr;
            } else {
                u8* const next = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_portraits))->at_4865f0(joan::orig::arg::i32(jo::as_msg_entry(entry)->next)));
                if (static_cast<i32>(jo::as_msg_entry(next)->kind) == static_cast<i32>(jo::as_msg_entry(entry)->kind))
                    entry = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_portraits))->at_4865f0(joan::orig::arg::i32(jo::as_msg_entry(entry)->next)));
                else
                    entry = nullptr;
            }
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Book::draw_page_6e22e0", 0x006e22e0, &H::draw_page_6e22e0);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

void* const kRandom = obj(d::g_misc);

JOAN_COUNTER(c_random_round4, "Obj6e::random_round4");

struct H {
    i32 random_round4(i32 i) {  // 0x006e2910
        c_random_round4.hit();
        const i32 v = static_cast<const i32*>(img::obj(d::g_a53c86))[i];  // dword i of 0x00a53c86 (untyped bss)
        const i32 rem = v % 4;
        const i32 base = v - rem;
        const double q = Fpu{true}.div(static_cast<double>(base), static_cast<double>(global<float>(d::flt_4)));
        i32 r = joan::orig::arg::as<i32>(joan::orig::as_misc(kRandom)->round(joan::orig::arg::f64(q)));  // 0x00590a20
        if (rem != 0) r = r + 1;
        return r;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6e::random_round4", 0x006e2910, &H::random_round4);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {


JOAN_COUNTER(c_combo, "Combo::entry_at");

struct H {
    i32 combo_entry_at(i32 n) {  // 0x006e29a0
        c_combo.hit();
        if (n > static_cast<i32>(jo::self<jo::MessageLog>(this)->page_count)) return 0;
        i32 acc = 0;
        for (i32 i = 0; i < 9; ++i) {
            const i32 c = joan::orig::arg::as<i32>(joan::orig::as_msg_log(this)->pages_in_section(joan::orig::arg::i32(i)));  // 0x006e2910
            if (static_cast<i32>(static_cast<u32>(acc) + static_cast<u32>(c)) >= n) {
                void* const list = obj(d::g_portraits);
                u8* p = joan::orig::arg::as<u8*>(joan::orig::as_list(list)->at_4865f0(joan::orig::arg::i32(global<u32>(d::g_a53c62 + 4 * i))));  // 0x004865f0
                for (i32 j = 1; j <= static_cast<i32>((static_cast<u32>(n) - (static_cast<u32>(acc) + 1u)) << 2); ++j) {
                    p = joan::orig::arg::as<u8*>(joan::orig::as_list(list)->at_4865f0(joan::orig::arg::i32(jo::as_msg_entry(p)->next)));
                }
                return jo::view<i32>(jo::as_msg_entry(p)->rec_no);
            }
            acc = static_cast<i32>(static_cast<u32>(acc) + static_cast<u32>(c));
        }
        return 0;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Combo::entry_at", 0x006e29a0, &H::combo_entry_at);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b109_11.cpp (game/b109_11)
namespace {
namespace game_b109_11 {


void* const kRes = obj(d::g_misc);
void* const kText = obj(d::g_sprites_a46128);

JOAN_COUNTER(c_mark, "draw_mark_6e2ab0");

// ---------------------------------------------------------------- shared draw helpers
// fld 255.0f; fmul [g+0x11]; _ftol; 0x0042a260(r, g, b, a) on a 4-byte slot; returns the dword.
u32 rgba(u32* slot, i32 r, i32 g, i32 b, float alpha_scale) {
    const float prod = global<float>(d::flt_255) * alpha_scale;
    const i32 a = ftol(static_cast<double>(prod));
    const u32* res = joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
    return *res;
}
// 0x00633500 (sprite / text draw, 17 args) with the common tail 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1.
void sprite_draw(i32 x, i32 y, u32 target, const char* name, void* sheet, u32 color) {
    joan::orig::as_sprite_queue(kText)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(target), joan::orig::arg::str(name), joan::orig::arg::mptr(sheet), joan::orig::arg::u32(color), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// 0x00626130 (text in rect, 13 args) with the tail 0, 1, 2, 0, 1.0f, 1.0f.
void text_rect(void* font, i32 x, i32 y, i32 w, i32 h, u32 target, const void* text, u32 color) {
    joan::orig::as_text(font)->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(target), static_cast<const char*>(text), joan::orig::arg::u32(color), i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);  // 0x00626130
}
i32 round_scaled(float s) {
    return joan::orig::arg::as<i16>(joan::orig::as_misc(kRes)->round(joan::orig::arg::f64(static_cast<double>(global<float>(d::flt_1) * s))));  // 0x00590a20
}
i32 ui_inside(i32 x0, i32 y0, i32 x1, i32 y1) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1)));  // 0x00593980
}

// ---------------------------------------------------------------- 0x006e2ab0
i32 JOAN_CDECL draw_mark(u8* g, u32 target, u32 flags) {
    const u32 fl = flags & 0xff;
    if (fl != 7 && (fl & 1) != 0) return 0;
    c_mark.hit();
    alignas(4) i16 w4 = jo::as_button(g)->x;
    alignas(4) i16 w8 = jo::as_button(g)->y;
    alignas(4) i16 wc = jo::as_button(g)->x2;
    alignas(4) i16 w10 = jo::as_button(g)->y2;
    alignas(4) float f14 = 1.0f;
    alignas(4) float f18 = 1.0f;
    if (jo::as_button(g)->fit != 0)
        joan::orig::as_sprite2d(kText)->fit_rect(joan::orig::arg::i16(static_cast<i32>(jo::as_button(g)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(g)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(g)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(g)->y2)), &f14, &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    void* const font = jo::as_button(g)->caption_box;
    if (jo::view<i8>(jo::as_button(g)->caption[0]) == 0) return 1;
    void* const text = jo::as_button(g)->caption;
    alignas(4) u32 slot = 0;
    if (jo::as_button(g)->pressed != 0) {
        const i32 tw = joan::orig::as_font(font)->measure(joan::orig::arg::str(text), -1, 0, 0, 1.0f);  // 0x00624f00
        joan::orig::as_font(font)->text_height(joan::orig::arg::i16(i32{2}), 1.0f);                                      // 0x006253f0
        {
            const u32 c = rgba(&slot, 0xff, 0xff, 0xff, jo::as_button(g)->scale);
            sprite_draw((static_cast<i32>(w4) + static_cast<i32>(wc) - tw) / 2 - 0x14, static_cast<i32>(w8) + 8,
                        target, str(d::s_MARK), img::obj(d::g_bmp_commele2), c);
        }
        {
            const u32 c = rgba(&slot, 0xff, 0xff, 0xff, jo::as_button(g)->scale);
            sprite_draw((tw + static_cast<i32>(w4) + static_cast<i32>(wc)) / 2 + 6, static_cast<i32>(w8) + 8,
                        target, str(d::s_MARK), img::obj(d::g_bmp_commele2), c);
        }
        const i32 dy = jo::as_button(g)->pressed != 0 ? round_scaled(f18) : 0;
        const i32 dx = jo::as_button(g)->pressed != 0 ? round_scaled(f14) : 0;
        const u32 c = rgba(&slot, 0, 0, 0, jo::as_button(g)->scale);
        text_rect(font, static_cast<i32>(w4) + dx, static_cast<i32>(w8) + dy - 1, static_cast<i32>(wc),
                  static_cast<i32>(w10) - 1, target, text, c);
        return 1;
    }
    if (jo::as_button(g)->pressed == 0 &&
        ui_inside(static_cast<i32>(w4), static_cast<i32>(w8), static_cast<i32>(wc), static_cast<i32>(w10)) != 0) {
        const Fpu fpu{true};
        const i32 tw = joan::orig::as_font(font)->measure(joan::orig::arg::str(text), -1, 0, 0, 1.0f);  // 0x00624f00
        joan::orig::as_font(font)->text_height(joan::orig::arg::i16(i32{2}), 1.0f);                                      // 0x006253f0
        {
            const u32 c = rgba(&slot, 0xff, 0xff, 0xff, jo::as_button(g)->scale);
            const i32 iv = static_cast<i32>(w4) + static_cast<i32>(wc) - tw;
            double v = fpu.mul(static_cast<double>(iv), global<double>(d::dbl_0_5));
            v = fpu.sub(v, global<double>(d::dbl_20));
            sprite_draw(ftol(v), static_cast<i32>(w8) + 8, target, str(d::s_MARK), img::obj(d::g_bmp_commele2), c);
        }
        {
            const u32 c = rgba(&slot, 0xff, 0xff, 0xff, jo::as_button(g)->scale);
            const i32 iv = tw + static_cast<i32>(w4) + static_cast<i32>(wc);
            double v = fpu.mul(static_cast<double>(iv), global<double>(d::dbl_0_5));
            v = fpu.add(v, global<double>(d::dbl_9ddb80));
            sprite_draw(ftol(v), static_cast<i32>(w8) + 8, target, str(d::s_MARK), img::obj(d::g_bmp_commele2), c);
        }
    }
    // 0x006e2f45 and 0x006e3031 are the same tail.
    const i32 dy = jo::as_button(g)->pressed != 0 ? round_scaled(f18) : 0;
    const i32 dx = jo::as_button(g)->pressed != 0 ? round_scaled(f14) : 0;
    const u32 c = rgba(&slot, 0xff, 0xff, 0xff, jo::as_button(g)->scale);
    text_rect(font, static_cast<i32>(w4) + dx, static_cast<i32>(w8) + dy - 1, static_cast<i32>(wc),
              static_cast<i32>(w10) - 1, target, text, c);
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b109_11", name, addr, fn)
B("draw_mark_6e2ab0", 0x006e2ab0, &draw_mark);
#undef B

}  // namespace game_b109_11
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MsgLog::release() -> void { joan::bind::member<&::game_b95_08::H::release_children>(this); }  // 0x6e1070
auto joan::orig::MsgLog::load_6e1570() -> void { joan::bind::member<&::game_b102_15::H::msglog_6e1570>(this); }  // 0x6e1570
auto joan::orig::Panel::release() -> void { joan::bind::member<&::game_b92_06::H::panel_release>(this); }  // 0x6e1810
auto joan::orig::Panel::show_6e19b0(std::int8_t a0, std::int8_t a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b103_15::H::show_6e19b0>(this, a0, a1)); }  // 0x6e19b0
auto joan::orig::MsgLog::capture_page() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b115_11::H::page_capture>(this)); }  // 0x6e1ea0
auto joan::orig::MsgLog::draw_page(std::int32_t a0) -> void { joan::bind::member<&::game_b108_02::H::draw_page_6e22e0>(this, a0); }  // 0x6e22e0
auto joan::orig::MsgLog::pages_in_section(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_06::H::random_round4>(this, a0)); }  // 0x6e2910
auto joan::orig::MsgLog::entry_at(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b94_08::H::combo_entry_at>(this, a0)); }  // 0x6e29a0
