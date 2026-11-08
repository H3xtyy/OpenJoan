// game/unknown_mainmenu.cpp: functions whose original source file is unknown (formerly game/_unplaced/mainmenu.cpp;
// re/attribution.csv: between ww/interfac/list.cpp and ww/interfac/minires.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../fp/fma.hpp"
#include "../original/helpers.hpp"
#include "../callconv.hpp"
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

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;

using joan::orig::helpers::fbits;

using u16 = std::uint16_t;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b109_16.cpp (game/b109_16)
namespace {
namespace game_b109_16 {

using joan::orig::helpers::gf;  // float constant in .rdata

const Fpu fpu{true};

using joan::call::thiscall;

// 0x0042a260 (thunk 0x004098b8): 4-byte colour ctor (r, g, b, a), ret 0x10; returns this.
u32 make_color(u32 r, u32 g, u32 b, u32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*jo::as_colour(p));
}

JOAN_COUNTER(c_label, "label_draw_707e30");

// ---- 0x00707e30 -------------------------------------------------------------------------------
void label_frame(u32 surf, i16 x, i16 y, img::Addr name) {  // 0x00633500 with the frame style
    const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(static_cast<i32>(x)), joan::orig::arg::i16(static_cast<i32>(y)), surf, joan::orig::arg::str(str(name)), joan::orig::arg::mptr(global<void*>(d::g_a3cf6d)), joan::orig::arg::u32(color), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
void label_mark(u32 surf, i32 x, i32 y) {
    const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), surf, str(d::s_L_MARK), joan::orig::arg::mptr(global<void*>(d::g_a3cf6d)), joan::orig::arg::u32(color), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500 "L_MARK"
}
i32 centre(i16 x, i16 x2) { return static_cast<i32>(x) + (static_cast<i32>(x2) - static_cast<i32>(x)) / 2; }

u32 JOAN_CDECL label_draw(u8* ctl, u32 surf, u32 flags) {
    c_label.hit();
    if ((flags & 0xff) != 7 && (flags & 0xff & 1) != 0) return 0;
    i16 x = jo::as_button(ctl)->x, y = jo::as_button(ctl)->y, x2 = jo::as_button(ctl)->x2, y2 = jo::as_button(ctl)->y2;
    float sx = 1.0f, sy = 1.0f;
    if (jo::as_button(ctl)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(static_cast<i32>(jo::as_button(ctl)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(ctl)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(ctl)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(ctl)->y2)), &sx, &sy, &x, &y, &x2, &y2, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    i32 w = 0;
    if (jo::view<i8>(jo::as_button(ctl)->caption[0]) != 0) {
        w = joan::orig::as_font(obj(d::g_font_goudy_bold_24))->measure(joan::orig::arg::str(jo::as_button(ctl)->caption), -1, 0, 0, joan::orig::arg::f32(fbits(0x3f800000)));  // 0x00624f00
        joan::orig::as_font(obj(d::g_font_goudy_bold_24))->text_height(joan::orig::arg::i16(i32{2}), joan::orig::arg::f32(fbits(0x3f800000)));  // 0x006253f0 (result unused)
    }
    if (jo::as_button(ctl)->enabled != 0) {
        if (jo::as_button(ctl)->pressed != 0) {
            label_frame(surf, x, y, d::s_L_DW);  // "L_DW"
            if (w != 0) {
                label_mark(surf, centre(x, x2) - w / 2 - 0x28, static_cast<i32>(y) + 0xc);
                label_mark(surf, centre(x, x2) + w / 2, static_cast<i32>(y) + 0xc);
            }
        } else if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(x), static_cast<i32>(y), static_cast<i32>(x2), static_cast<i32>(y2))) != 0) {  // 0x00593980
            label_frame(surf, x, y, d::s_L_OV);  // "L_OV"
            if (w != 0) {
                label_mark(surf, centre(x, x2) - w / 2 - 0x28, static_cast<i32>(y) + 0xc);
                label_mark(surf, centre(x, x2) + w / 2, static_cast<i32>(y) + 0xc);
            }
        } else {
            label_frame(surf, x, y, d::s_L_OT);  // "L_OT"
        }
    } else {
        label_frame(surf, x, y, d::s_L_OT);  // "L_OT"
    }
    if (jo::view<i8>(jo::as_button(ctl)->caption[0]) == 0) return 1;
    bool lit = false;
    if (jo::as_button(ctl)->enabled != 0) {
        if (jo::as_button(ctl)->pressed != 0)
            lit = true;
        else
            lit = joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(x), static_cast<i32>(y), static_cast<i32>(x2), static_cast<i32>(y2))) != 0;  // 0x00593980
    }
    i32 dy = 0, dx = 0;
    if (jo::as_button(ctl)->pressed != 0)
        dy = joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(fpu.mul(gf(d::flt_1), sy))));  // 0x00590a20
    if (jo::as_button(ctl)->pressed != 0)
        dx = joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(fpu.mul(gf(d::flt_1), sx))));  // 0x00590a20
    const u32 color = lit ? make_color(0xff, 0xff, 0xff, 0xff) : make_color(0, 0, 0, 0xff);
    joan::orig::as_text(obj(d::g_font_goudy_bold_24))->draw_line(joan::orig::arg::i32(centre(x, x2) - w / 2 + dx), joan::orig::arg::i32(static_cast<i32>(y) + dy + 0xb), surf, joan::orig::arg::str(jo::as_button(ctl)->caption), joan::orig::arg::u32(color), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(fbits(0x3f800000)), joan::orig::arg::f32(fbits(0x3f800000)));  // 0x006255d0
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_16", name, addr, fn)
B("label_draw_707e30", 0x00707e30, &label_draw);
#undef B

}  // namespace game_b109_16
}  // namespace

// ---------------------------------------- from modules/game_b109_19.cpp (game/b109_19)
namespace {
namespace game_b109_19 {

using joan::call::thiscall;

u32 color(i32 r, i32 g, i32 b, i32 a) {  // 0x0042a260 colour ctor (ret 0x10)
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));
}

JOAN_COUNTER(c_button, "draw_mode_button_708760");

// ------------------------------------------------------------------ 0x00708760
// "M_xx" sprite through 0x00633500 (ret 0x44).
void mode_sprite(i16 x, i16 y, u32 ctx, img::Addr name) {
    const u32 c = color(0xff, 0xff, 0xff, 0xff);
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(str(name)), joan::orig::arg::mptr(global<void*>(d::g_a3cf6d)), joan::orig::arg::u32(c), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
void mode_sprite_i(i32 x, i32 y, u32 ctx, img::Addr name) {
    const u32 c = color(0xff, 0xff, 0xff, 0xff);
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(str(name)), joan::orig::arg::mptr(global<void*>(d::g_a3cf6d)), joan::orig::arg::u32(c), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
void mode_arrows(i16 w4, i16 w8, i16 wc, i32 label_w, u32 ctx) {
    const i32 mid = w4 + (wc - w4) / 2;
    mode_sprite_i(mid - label_w / 2 - 0x49, w8 + 0xe, ctx, d::s_M_LA);  // "M_LA"
    mode_sprite_i(mid + label_w / 2 + 5, w8 + 0xe, ctx, d::s_M_RA);     // "M_RA"
}
i32 mode_scale(float s) {  // 0x00590a20 (ret 8) on 1.0 * s, ax sign-extended
    const float v = global<float>(d::flt_1) * s;
    return static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(v)))));
}

static u32 JOAN_CDECL draw_mode_button(u8* button, u32 ctx, u32 state_arg) {
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
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(jo::as_button(button)->x), joan::orig::arg::i16(jo::as_button(button)->y), joan::orig::arg::i16(jo::as_button(button)->x2), joan::orig::arg::i16(jo::as_button(button)->y2), &f14, &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));  // 0x00633e60
    }
    i32 label_w = 0;
    if (jo::view<i8>(jo::as_button(button)->caption[0]) != 0) {
        label_w = static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_font(obj(d::g_font_goudy_bold_20))->measure(joan::orig::arg::str(jo::as_button(button)->caption), -1, 0, 0, 1.0f)));                         // 0x00624f00
        joan::orig::as_font(obj(d::g_font_goudy_bold_20))->text_height(2, 1.0f);                    // 0x006253f0
    }
    if (jo::as_button(button)->enabled != 0) {
        if (jo::as_button(button)->pressed != 0) {
            mode_sprite(w4, w8, ctx, d::s_M_DW);  // "M_DW"
            if (label_w != 0) mode_arrows(w4, w8, wc, label_w, ctx);
        } else if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{w4}, i32{w8}, i32{wc}, i32{w10})) != 0) {  // 0x00593980
            mode_sprite(w4, w8, ctx, d::s_M_OV);  // "M_OV"
            if (label_w != 0) mode_arrows(w4, w8, wc, label_w, ctx);
        } else {
            mode_sprite(w4, w8, ctx, d::s_M_OT);  // "M_OT"
        }
    } else {
        mode_sprite(w4, w8, ctx, d::s_M_OT);  // "M_OT"
    }
    if (jo::view<i8>(jo::as_button(button)->caption[0]) != 0) {
        bool lit = false;
        if (jo::as_button(button)->enabled != 0) {
            if (jo::as_button(button)->pressed != 0) {
                lit = true;
            } else if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(i32{w4}, i32{w8}, i32{wc}, i32{w10})) != 0) {  // 0x00593980
                lit = true;
            }
        }
        const i32 dy = jo::as_button(button)->pressed != 0 ? mode_scale(f18) : 0;
        const i32 dx = jo::as_button(button)->pressed != 0 ? mode_scale(f14) : 0;
        const u32 c = lit ? color(0xff, 0xff, 0xff, 0xff) : color(0, 0, 0, 0xff);
        const i32 x = w4 + (wc - w4) / 2 - label_w / 2 + dx;
        const i32 y = w8 + dy + 5;
        joan::orig::as_text(obj(d::g_font_goudy_bold_20))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(jo::as_button(button)->caption), joan::orig::arg::u32(c), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);  // 0x006255d0
    }
    return 1;
}

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_19", name, addr, fn)
B("draw_mode_button_708760", 0x00708760, &draw_mode_button);
#undef B

}  // namespace game_b109_19
}  // namespace

// ---------------------------------------- from modules/game_b107_05.cpp (game/b107_05)
namespace {
namespace game_b107_05 {

using joan::call::thiscall;

JOAN_COUNTER(c_menu_init, "MainMenu::init_709090");

struct H {

    // ---- 0x00709090 ----
    static u32 color(u32 a, u32 b, u32 c, u32 d) {
        u32 col = 0;
        return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&col)->construct(a, b, c, d));  // 0x0042a260
    }
    // Button::create 0x00621540 + text: (x - 0x200, y, (short)x, x + w, callback, 1,0,0,1,1,1),
    // then colour (ff,ff,ff,ff), text getter on 0xa57c08, Button::set_text(text, tbl, colour, 1,2,0,2,2).
    void button(jo::Button& btn, u32 x, u32 y, u32 right, std::uintptr_t callback, std::uintptr_t text_fn, img::Addr table) {
        jo::Button* const b = &btn;
        joan::orig::as_button(b)->create(joan::orig::arg::i32(x - 0x200u), joan::orig::arg::i32(y), joan::orig::arg::i32(static_cast<u32>(static_cast<u16>(x))), joan::orig::arg::i32(x + right), callback, 1, 0, 0, 1, 1, 1);  // 0x00621540
        const u32 col = color(0xffu, 0xffu, 0xffu, 0xffu);
        const char* const text = thiscall<const char*>(text_fn, obj(d::g_strings));
        joan::orig::as_button(b)->set_text(text, img::obj(table), joan::orig::arg::i32(col), 1, 2, 0, 2, 2);  // 0x00623ce0
    }
    // Label::create 0x00629660: (x, y, font +0x31, name, colour(0,0,0,ff), 3,0,2,0,-1,-1,-1,-1,0,-1,-1,0,1,1)
    void label(jo::Image2D& image, u32 x, u32 y, img::Addr name) {
        const u32 col = color(0u, 0u, 0u, 0xffu);
        joan::orig::as_label(&image)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::mptr(jo::self<jo::MainMenu>(this)->font), joan::orig::arg::str(img::addr(name)), joan::orig::arg::i32(col), 3, 0, 2, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0, 1, 1);  // 0x00629660
    }
    void scale(jo::Image2D& image, u32 factor) {
        joan::orig::as_widget(&image)->scale_to(0, joan::orig::arg::f32(factor), 0.0f, 1, 0u);  // 0x0061f1b0
    }

    u32 menu_init_709090() {
        c_menu_init.hit();
        jo::self<jo::MainMenu>(this)->font = img::obj(d::g_bmp_res_menu1024);
        jo::self<jo::MainMenu>(this)->font2 = img::obj(d::g_bmp_res_menu_b);
        jo::self<jo::MainMenu>(this)->images[0] = nullptr;
        jo::self<jo::MainMenu>(this)->images[1] = nullptr;
        jo::self<jo::MainMenu>(this)->images[2] = nullptr;
        jo::self<jo::MainMenu>(this)->images[3] = nullptr;
        jo::self<jo::MainMenu>(this)->images[4] = nullptr;
        jo::self<jo::MainMenu>(this)->images[5] = nullptr;
        jo::self<jo::MainMenu>(this)->images[6] = nullptr;
        jo::self<jo::MainMenu>(this)->flags[2] = 0;
        std::memset(static_cast<void*>(jo::self<jo::MainMenu>(this)->cheat_buf), 0, sizeof(jo::MainMenu::cheat_buf));  // memset
        jo::self<jo::MainMenu>(this)->cheat_len = 0;
        jo::self<jo::MainMenu>(this)->u32_3450 = 0;
        // Both branches on [0xa48858] store the same sizes.
        jo::self<jo::MainMenu>(this)->layout_width = 0x400;
        jo::self<jo::MainMenu>(this)->layout_height = 0x300;
        const u32 x = global<u32>(d::g_a48858) == 0 ? 0u : 0x1f4u;

        button(jo::self<jo::MainMenu>(this)->button2, x, 0xe0, 0x128, fn::code_value(fn::label_draw_707e30), fn::code_value(fn::MainMenu_open_1), d::g_font_goudy_bold_24);   // 0x00707e30, 0x004de1a0
        button(jo::self<jo::MainMenu>(this)->button3, x, 0x12c, 0x174, fn::code_value(fn::label_draw_707e30), fn::code_value(fn::MainMenu_open_2), d::g_font_goudy_bold_24);  // 0x004de1d0
        button(jo::self<jo::MainMenu>(this)->button6, x, 0x178, 0x1c0, fn::code_value(fn::label_draw_707e30), fn::code_value(fn::MainMenu_open_03), d::g_font_goudy_bold_24); // 0x004de200
        button(jo::self<jo::MainMenu>(this)->button5, x, 0x1c4, 0x20c, fn::code_value(fn::label_draw_707e30), fn::code_value(fn::MainMenu_open_0b), d::g_font_goudy_bold_24); // 0x004de380
        button(jo::self<jo::MainMenu>(this)->button12, x, 0x210, 0x258, fn::code_value(fn::label_draw_707e30), fn::code_value(fn::MainMenu_open_04), d::g_font_goudy_bold_24); // 0x004de230
        button(jo::self<jo::MainMenu>(this)->button7, x, 0xe0, 0x110, fn::code_value(fn::draw_mode_button_708760), fn::code_value(fn::MainMenu_open_5), d::g_font_goudy_bold_20);  // 0x00708760, 0x004de260
        button(jo::self<jo::MainMenu>(this)->button8, x, 0x114, 0x144, fn::code_value(fn::draw_mode_button_708760), fn::code_value(fn::Menu_mainmenu_6), d::g_font_goudy_bold_20); // 0x004de290
        button(jo::self<jo::MainMenu>(this)->button9, x, 0x148, 0x178, fn::code_value(fn::draw_mode_button_708760), fn::code_value(fn::Menu_mainmenu_7), d::g_font_goudy_bold_20); // 0x004de2c0
        button(jo::self<jo::MainMenu>(this)->button10, x, 0x17c, 0x1ac, fn::code_value(fn::draw_mode_button_708760), fn::code_value(fn::MainMenu_open_8), d::g_font_goudy_bold_20); // 0x004de2f0
        button(jo::self<jo::MainMenu>(this)->button11, x, 0x1b0, 0x1e0, fn::code_value(fn::draw_mode_button_708760), fn::code_value(fn::MainMenu_open_9), d::g_font_goudy_bold_20); // 0x004de320

        label(jo::self<jo::MainMenu>(this)->img_main, 0u, 0u, d::s_main);             // "main"
        label(jo::self<jo::MainMenu>(this)->img_alab_l, 0xffffff44u, 0u, d::s_alab_l);    // "alab_l"
        label(jo::self<jo::MainMenu>(this)->img_alab_r, 0x400u, 0u, d::s_alab_r);         // "alab_r"
        label(jo::self<jo::MainMenu>(this)->img_alab_s, 0u, 0u, d::s_alab_s);             // "alab_s"
        label(jo::self<jo::MainMenu>(this)->img_gold_l, 0x2du, 0u, d::s_gold_l);          // "gold_l"
        label(jo::self<jo::MainMenu>(this)->img_gold_r, 0x2f3u, 0u, d::s_gold_r);         // "gold_r"
        label(jo::self<jo::MainMenu>(this)->img_lea_l, 0xfffffef8u, 0u, d::s_lea_l);    // "lea_l"
        label(jo::self<jo::MainMenu>(this)->img_lea_r, 0x400u, 0u, d::s_lea_r);         // "lea_r"
        label(jo::self<jo::MainMenu>(this)->img_sphere, 0x20u, 0u, d::s_sphere);          // "sphere"
        label(jo::self<jo::MainMenu>(this)->img_stripe, 0xf0u, 0x2e8u, d::s_stripe);      // "stripe"
        label(jo::self<jo::MainMenu>(this)->img_logo_bar, 0x400u, 0x10u, d::s_logo_bar);      // "logo_bar"
        label(jo::self<jo::MainMenu>(this)->img_logo, 0x120u, 0x1au, d::s_logo);      // "logo"
        label(jo::self<jo::MainMenu>(this)->img_joan, 0xffffffd3u, 0x96u, d::s_joan); // "joan"

        scale(jo::self<jo::MainMenu>(this)->img_main, 0x40700000u);  // 3.75f
        scale(jo::self<jo::MainMenu>(this)->img_alab_s, 0x40700000u);
        scale(jo::self<jo::MainMenu>(this)->img_gold_l, 0x40700000u);
        scale(jo::self<jo::MainMenu>(this)->img_gold_r, 0x40700000u);
        scale(jo::self<jo::MainMenu>(this)->img_logo, 0x40700000u);
        scale(jo::self<jo::MainMenu>(this)->img_sphere, 0x40700000u);
        scale(jo::self<jo::MainMenu>(this)->img_stripe, 0x40700000u);
        scale(jo::self<jo::MainMenu>(this)->img_joan, 0x3f89249eu);  // 1.0714285f

        u8* const state = &jo::self<jo::MainMenu>(this)->open;  // open .. u8_341d: 12 consecutive bytes
        for (i32 i = 0; i < 12; ++i) state[i] = 0;
        jo::self<jo::MainMenu>(this)->buttons_off = 1;
        jo::self<jo::MainMenu>(this)->intro_delay_ms = 0;
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_05", name, addr, fn)
B("MainMenu::init_709090", 0x00709090, &H::menu_init_709090);
#undef B

}  // namespace game_b107_05
}  // namespace

// ---------------------------------------- from modules/game_b101_15.cpp (game/b101_15)
namespace {
namespace game_b101_15 {

using joan::call::thiscall;

void* const kWindows = obj(d::g_texture_array);

u32 button_free(void* b) { return joan::orig::arg::as<u32>(joan::orig::as_menu_button(b)->clear()); }  // 0x006217e0

JOAN_COUNTER(c_release, "Screen::release");

struct H {
    // 0x0070a060 (ret), vtable slot
    u32 release() {
        c_release.hit();
        jo::self<jo::MainMenu>(this)->font = nullptr;
        jo::self<jo::MainMenu>(this)->font2 = nullptr;
        for (i32 i = 0; i < 7; ++i) {
            if (jo::self<jo::MainMenu>(this)->images[i] != nullptr) {
                joan::orig::as_texture_manager(kWindows)->release(joan::orig::arg::mptr(jo::self<jo::MainMenu>(this)->images[i]));  // 0x0060a550
                jo::self<jo::MainMenu>(this)->images[i] = nullptr;
            }
        }
        button_free(&jo::self<jo::MainMenu>(this)->button2);
        button_free(&jo::self<jo::MainMenu>(this)->button3);
        button_free(&jo::self<jo::MainMenu>(this)->button6);
        button_free(&jo::self<jo::MainMenu>(this)->button5);
        button_free(&jo::self<jo::MainMenu>(this)->button12);
        button_free(&jo::self<jo::MainMenu>(this)->button7);
        button_free(&jo::self<jo::MainMenu>(this)->button8);
        button_free(&jo::self<jo::MainMenu>(this)->button9);
        button_free(&jo::self<jo::MainMenu>(this)->button10);
        button_free(&jo::self<jo::MainMenu>(this)->button11);
        jo::Image2D* const images = &jo::self<jo::MainMenu>(this)->img_main;  // img_main .. img_joan: 13 consecutive Image2D
        for (i32 i = 0; i < 13; ++i) joan::orig::as_image2d(&images[i])->release();  // 0x00629830
        return 1;
    }
};

#define B15(name, addr, fn) JOAN_REPLACE("game/b101_15", name, addr, fn)
B15("Screen::release", 0x0070a060, &H::release);
#undef B15

}  // namespace game_b101_15
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using joan::orig::helpers::memset_;  // memset

using joan::call::thiscall;

void* const kSfx = obj(d::g_sfx_player);

JOAN_COUNTER(c_cheats, "Screen::update_cheats");

// ---------------------------------------------------------------------------- CRT (direct)
using joan::orig::helpers::strcpy_;
using joan::orig::helpers::stricmp_;

struct H {
    // ------------------------------------------------------------------ 0x0070a370
    i32 update_cheats(u32) {
        if (jo::view<i8>(jo::self<jo::MainMenu>(this)->flags[2]) == 0) return 0;
        c_cheats.hit();
        if (joan::orig::arg::as<i8>(joan::orig::as_opt_menu(obj(d::g_opt_menu))->is_open()) == 0 &&  // 0x004303a0
            joan::orig::arg::as<i8>(joan::orig::as_save_menu(obj(d::g_save_menu))->is_open()) == 0 &&  // 0x00430370
            global<i8>(d::g_a59a6c) == 0 && global<u32>(d::g_key) >= 0x41 && global<u32>(d::g_key) <= 0x7a) {
            char word[0x20] = {};  // [ebp-0x28]
            strcpy_(word, str(d::s_coronation));  // "coronation"
            jo::view<u8>(jo::self<jo::MainMenu>(this)->cheat_buf[static_cast<i32>(jo::view<i16>(jo::self<jo::MainMenu>(this)->cheat_len))]) = global<u8>(d::g_key);
            jo::self<jo::MainMenu>(this)->cheat_len = static_cast<u16>(jo::self<jo::MainMenu>(this)->cheat_len + 1);
            word[jo::view<i16>(jo::self<jo::MainMenu>(this)->cheat_len)] = 0;
            if (stricmp_(word, jo::self<jo::MainMenu>(this)->cheat_buf) != 0) {
                memset_(jo::self<jo::MainMenu>(this)->cheat_buf, 0, sizeof(jo::MainMenu::cheat_buf));
                jo::self<jo::MainMenu>(this)->cheat_len = 0;
            }
            if (stricmp_(str(d::s_coronation), jo::self<jo::MainMenu>(this)->cheat_buf) == 0) {
                global<u8>(d::g_a59a6c) = 1;
                memset_(jo::self<jo::MainMenu>(this)->cheat_buf, 0, sizeof(jo::MainMenu::cheat_buf));
                jo::self<jo::MainMenu>(this)->cheat_len = 0;
                joan::orig::as_sound(kSfx)->play_def(str(d::s_OBJECTIVE_ADD));  // 0x004ceef0 "OBJECTIVE_ADD"
            }
            return 1;
        }
        if (jo::view<i8>(jo::self<jo::MainMenu>(this)->buttons_off) != 0) return 1;
        if (jo::view<i8>(jo::self<jo::MainMenu>(this)->difficulty_0_chosen) != 0 || jo::view<i8>(jo::self<jo::MainMenu>(this)->difficulty_1_chosen) != 0 || jo::view<i8>(jo::self<jo::MainMenu>(this)->difficulty_2_chosen) != 0 || jo::view<i8>(jo::self<jo::MainMenu>(this)->difficulty_3_chosen) != 0) return 1;
        if (jo::view<i8>(jo::self<jo::MainMenu>(this)->panel_options) == 0) {
            if (jo::self<jo::MainMenu>(this)->slide != 0) {
                if (button(jo::self<jo::MainMenu>(this)->button11, 0x1b)) {
                    jo::self<jo::MainMenu>(this)->back_requested = 1;
                    jo::self<jo::MainMenu>(this)->open = 1;
                    jo::self<jo::MainMenu>(this)->busy = 0;
                }
                if (button(jo::self<jo::MainMenu>(this)->button7, 0)) jo::self<jo::MainMenu>(this)->difficulty_0_chosen = 1;
                if (button(jo::self<jo::MainMenu>(this)->button8, 0)) jo::self<jo::MainMenu>(this)->difficulty_1_chosen = 1;
                if (button(jo::self<jo::MainMenu>(this)->button9, 0)) jo::self<jo::MainMenu>(this)->difficulty_2_chosen = 1;
                if (button(jo::self<jo::MainMenu>(this)->button10, 0)) jo::self<jo::MainMenu>(this)->difficulty_3_chosen = 1;
            } else {
                if (button(jo::self<jo::MainMenu>(this)->button2, 0)) select(jo::view<u8>(jo::self<jo::MainMenu>(this)->slide));
                if (button(jo::self<jo::MainMenu>(this)->button3, 0)) select(jo::self<jo::MainMenu>(this)->panel_save);
                if (button(jo::self<jo::MainMenu>(this)->button6, 0)) select(jo::self<jo::MainMenu>(this)->panel_options);
                if (button(jo::self<jo::MainMenu>(this)->button5, 0)) select(jo::self<jo::MainMenu>(this)->panel_quit_game);
                if (button(jo::self<jo::MainMenu>(this)->button12, 0)) select(jo::self<jo::MainMenu>(this)->panel_quit);
            }
        }
        if (global<u32>(d::g_key) == 0x53) {
            joan::orig::as_main_menu(this)->reset_widgets();                         // 0x0070d670
            joan::orig::as_cursor(obj(d::g_mouse_cursor))->set_596db0(2);           // 0x00596db0
            joan::orig::as_battle(obj(d::g_scenarios))->load_scenario_41cc80(1, 1, 0);     // 0x0041cc80
            joan::orig::as_game(obj(d::g_game_a50088))->set_state(0xcu);         // 0x004d7e70
        }
        return 0;
    }
    bool button(jo::Button& btn, i32 key) {
        return joan::orig::arg::as<i32>(joan::orig::as_ui_button(&btn)->mouse(joan::orig::arg::i32(key), 0, joan::orig::arg::i32(-1), 0, 0, 0)) != 0;  // 0x006218c0
    }
    void select(u8& panel) {
        panel = 1;
        jo::self<jo::MainMenu>(this)->open = 1;
        jo::self<jo::MainMenu>(this)->busy = 0;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("Screen::update_cheats", 0x0070a370, &H::update_cheats);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b116_14.cpp (game/b116_14)
namespace {
namespace game_b116_14 {

constexpr img::Addr kMouse = d::g_mouse;

constexpr img::Addr kTextRes = d::g_strings;    // localised text resource

constexpr u32 kOne = 0x3f800000u;  // 1.0f pushed as an immediate

// Not joan::call::thiscall: a __thiscall function pointer, not a pointer to member (a class-type R is returned
// as a free function returns it); the target is an fn:: name or a computed address (see src/call.hpp).
template <class R = void, class T, class... A>
R thiscall(T address, const void* self, A... args) {
    return reinterpret_cast<R(JOAN_THISCALL*)(void*, A...)>(joan::fn::target(address))(const_cast<void*>(self), args...);
}
i32 sx(i16 v) { return static_cast<i32>(v); }

// x87 arithmetic at PC24 with the x87's own exponent range: the exact result hi + lo (|lo| <= half
// an ulp of hi) rounded to a 24-bit significand WITHOUT float overflow / gradual underflow (a
// register value; only the final fstp rounds to the float format). Fpu{true} rounds to the float
// format at every step, which differs when an intermediate leaves the float range (e.g. a large
// tick * dt in Dialogue::draw that the following / 1000 brings back). hi is finite and normal for
// every operand combination here (|values| within 2^-200..2^200), NaN / inf pass through.
double pc24(double hi, double lo) {
    if (hi == 0.0 || !(hi - hi == 0.0)) return hi;  // zero, inf, NaN
    std::uint64_t bits;
    std::memcpy(&bits, &hi, sizeof bits);
    const i32 e = static_cast<i32>((bits >> 52) & 0x7ff) - 1023;
    auto pow2 = [](i32 n) {
        const std::uint64_t b = static_cast<std::uint64_t>(n + 1023) << 52;
        double d;
        std::memcpy(&d, &b, sizeof d);
        return d;
    };
    const double down = pow2(-e), up = pow2(e);  // exact scalings into [1, 2) and back
    return joan::hook::x87::round_to_float(hi * down, lo * down) * up;
}
double mul24(double a, double b) {
    const double p = a * b;
    return pc24(p, joan::fp::fma_exact(a, b, -p));
}

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite drawer

JOAN_COUNTER(c_main, "MainMenu::draw");

// 0x0042a260 colour ctor (thiscall (r, g, b, a), ret 0x10) into `slot`; returns the dword.
u32 make_colour(u32* slot, u32 r, u32 g, u32 b, u32 a) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
}

// Sprite flush after a menu: 0x00631d50 ? 0x00631d80 : 0x00631dc0 (returns that eax).
u32 flush_sprites() {
    if (joan::orig::arg::as<i32>(joan::orig::as_sprite_queue(obj(kSprites))->begin_frame()) != 0) return joan::orig::arg::as<u32>(joan::orig::as_sprite_queue(obj(kSprites))->end_frame());  // 0x00631d50 / 0x00631d80
    return joan::orig::arg::as<u32>(joan::orig::as_sprite_queue(obj(kSprites))->release_list());                                                      // 0x00631dc0
}

// 0x00629bd0 (frame draw, ret 8) / 0x00622cc0 (button draw, ret 8) with (a, 7).
void frame(void* f, i32 a) { joan::orig::as_label(f)->draw(joan::orig::arg::u32(a), joan::orig::arg::u32(i32{7})); }   // 0x00629bd0
void button(void* b, i32 a) { joan::orig::as_button(b)->draw(joan::orig::arg::u32(a), 7); }  // 0x00622cc0

struct H {
    // -----------------------------------------------------------------------------------------
    // 0x0070a8b0 MainMenu::draw(char)
    u32 main_draw(u32 a) {
        joan::canvas::ScreenScope canvas_scope(0x0070a8b0);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        if (jo::view<i8>(jo::self<jo::MainMenu>(this)->flags[2]) == 0) return 0;
        c_main.hit();
        joan::orig::as_cursor(obj(kMouse))->reset_pos();  // 0x00592c30
        u32 slot0 = 0;
        const u32 black = make_colour(&slot0, 0u, 0u, 0u, 0xffu);
        joan::orig::as_sprite2d(obj(kSprites))->queue(i32{0}, i32{0}, i32{0x400}, i32{0x300}, joan::orig::arg::u32(i32{0xa}), joan::orig::arg::u32(black), joan::orig::arg::mptr(i32{0}), joan::orig::arg::i32(// 0x00632a70
                      i32{3}), i32{0}, i32{0}, i32{0}, i32{-1}, i32{-1}, i32{-1}, i32{-1}, i32{0});
        frame(&jo::self<jo::MainMenu>(this)->img_main, 0xa);
        frame(&jo::self<jo::MainMenu>(this)->img_sphere, 0xa);
        frame(&jo::self<jo::MainMenu>(this)->img_joan, 0xa);
        frame(&jo::self<jo::MainMenu>(this)->img_lea_l, 0xa);
        frame(&jo::self<jo::MainMenu>(this)->img_lea_r, 0xa);
        frame(&jo::self<jo::MainMenu>(this)->img_gold_l, 0xa);
        frame(&jo::self<jo::MainMenu>(this)->img_gold_r, 0xa);
        const u32 c = static_cast<u8>(a);
        if (joan::orig::arg::as<i8>(joan::orig::as_opt_menu(obj(d::g_opt_menu))->is_open()) != 0) {  // 0x004303a0
            joan::orig::as_opt_menu(obj(d::g_opt_menu))->draw(joan::orig::arg::u32(c));      // 0x0073d2c0
            flush_sprites();
        } else if (joan::orig::arg::as<i8>(joan::orig::as_save_menu(obj(d::g_save_menu))->is_open()) != 0) {  // 0x00430370
            joan::orig::as_save_menu(obj(d::g_save_menu))->draw_74e170(joan::orig::arg::u32(c));             // 0x0074e170
            flush_sprites();
        }
        frame(&jo::self<jo::MainMenu>(this)->img_logo_bar, 0xa);
        frame(&jo::self<jo::MainMenu>(this)->img_logo, 0xa);
        button(&jo::self<jo::MainMenu>(this)->button2, 0xa);
        button(&jo::self<jo::MainMenu>(this)->button3, 0xa);
        button(&jo::self<jo::MainMenu>(this)->button6, 0xa);
        button(&jo::self<jo::MainMenu>(this)->button5, 0xa);
        button(&jo::self<jo::MainMenu>(this)->button12, 0xa);
        if (jo::self<jo::MainMenu>(this)->slide != 0) {
            button(&jo::self<jo::MainMenu>(this)->button7, 0xa);
            button(&jo::self<jo::MainMenu>(this)->button8, 0xa);
            button(&jo::self<jo::MainMenu>(this)->button9, 0xa);
            button(&jo::self<jo::MainMenu>(this)->button10, 0xa);
            button(&jo::self<jo::MainMenu>(this)->button11, 0xa);
        }
        frame(&jo::self<jo::MainMenu>(this)->img_alab_s, 0xa);
        frame(&jo::self<jo::MainMenu>(this)->img_alab_l, 0xa);
        frame(&jo::self<jo::MainMenu>(this)->img_alab_r, 0xa);
        frame(&jo::self<jo::MainMenu>(this)->img_stripe, 0xa);
        // fld 255.0f (0x009cfb68); fmul [+0x3147]; _ftol
        const i32 alpha = joan::hook::x87::ftol(mul24(static_cast<double>(global<float>(d::flt_255)),
                                                      static_cast<double>(jo::self<jo::MainMenu>(this)->img_stripe.scale)));
        u32 slot1 = 0;
        const u32 colour = make_colour(&slot1, 0u, 0u, 0u, static_cast<u32>(alpha));
        const char* const text = joan::orig::as_txt_res(obj(kTextRes))->mainmenu_text_0a();  // 0x004de350
        const i32 y2 = sx(jo::self<jo::MainMenu>(this)->img_stripe.y2) + 0xc;
        const i32 y1 = sx(jo::self<jo::MainMenu>(this)->img_stripe.y2) - 8;
        joan::orig::as_text(obj(d::g_font_goudy12_bold_black))->draw_3(i32{0}, joan::orig::arg::i32(y1), i32{0x400}, joan::orig::arg::i32(y2), joan::orig::arg::u32(i32{0xa}), joan::orig::arg::str(text), joan::orig::arg::u32(colour), i32{0}, i32{1}, joan::orig::arg::i32(// 0x00626130
                      i32{2}), i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
        return 1;
    }
};

JOAN_REPLACE("game/b116_14", "MainMenu::draw", 0x0070a8b0, &H::main_draw);
JOAN_CANVAS_SELF(0x0070a8b0);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_14
}  // namespace

// ---------------------------------------- from modules/game_b110_07.cpp (game/b110_07)
namespace {
namespace game_b110_07 {

using joan::call::thiscall;
using joan::orig::helpers::gd;  // double constant in .rdata

// fld dword / fcomp dword / test ah,0x40 / je skip: taken when equal or unordered.
bool feq(float a, float b) { return !(a < b) && !(a > b); }

constexpr u32 kF4 = 0x40800000;      // 4.0f
constexpr u32 kF3 = 0x40400000;      // 3.0f
constexpr u32 kF2 = 0x40000000;      // 2.0f
constexpr u32 kF1 = 0x3f800000;      // 1.0f
constexpr u32 kF10 = 0x41200000;     // 10.0f
constexpr u32 kF12000 = 0x463b8000;  // 12000.0f
constexpr u32 kF3500 = 0x455ac000;   // 3500.0f
constexpr u32 kFm1500 = 0xc4bb8000;  // -1500.0f
constexpr u32 kFm4000 = 0xc57a0000;  // -4000.0f
constexpr u32 kF7680 = 0x45f00000;   // 7680.0f
constexpr u32 kF1600 = 0x44c80000;   // 1600.0f
constexpr u32 kF1200 = 0x44960000;   // 1200.0f

constexpr img::Addr kSlideMany = d::s_BUTTON_SLIDE_MANY;  // "BUTTON_SLIDE_MANY"
constexpr img::Addr kSlideOut = d::s_MENU_SLIDE_OUT;   // "MENU_SLIDE_OUT"
constexpr img::Addr kSlideIn = d::s_MENU_SLIDE_IN;    // "MENU_SLIDE_IN"

JOAN_COUNTER(c_menu, "Menu::update_70acb0");

struct H {
    jo::MainMenu* m() { return jo::self<jo::MainMenu>(this); }
    template <class M>
    static i8 b(const M& flag) { return jo::view<i8>(flag); }  // the byte read signed
    static double wd(i16 v) { return static_cast<double>(static_cast<i32>(v)); }  // fild of the movsx word

    // 0x0061f1b0 (thunk 0x004034e0, ret 0x14): (a, f, 0, 0, 0)
    template <class W>
    void move(W& widget, i32 a, u32 fb) {
        joan::orig::as_widget(&widget)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(fbits(fb)), 0.0f, 0, 0u);
    }
    // 0x0061e020 (thunk 0x00403454, ret 0x18): (x, y, f, g, 0, 0)
    template <class W>
    void slide(W& widget, i32 x, i32 y, u32 fb, u32 gb) {
        joan::orig::as_widget(&widget)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(fbits(fb)), joan::orig::arg::f32(fbits(gb)), 0, 0u);
    }
    // 0x0061e2a0 (thunk 0x00403aad, ret 4)
    void slide_v(jo::Button& button, u32 fb) { joan::orig::as_widget(&button)->set_move_accel(joan::orig::arg::bits(fbits(fb))); }
    static void sound(img::Addr name) {
        joan::orig::as_sound(obj(d::g_sfx_player))->play_def(str(name));  // 0x004ceef0
    }
    void self_70d670() { joan::orig::as_main_menu(this)->reset_widgets(); }  // 0x0070d670

    // Right-hand button chain (0x0070b0ba..0x0070b3bd == 0x0070b94f..0x0070bd1b up to the last test).
    // Returns the final "img_main.scale == 0.0" test.
    bool right_chain() {
        if (feq(m()->img_stripe.scale, 0.0f) && !(static_cast<double>(m()->img_joan.scale) >= gd(d::dbl_0_3)) && feq(m()->img_sphere.scale, 1.0f))
            move(m()->img_sphere, 0, kF4);
        if (feq(m()->img_sphere.scale, 0.0f) && feq(m()->img_logo.scale, 1.0f)) move(m()->img_logo, 0, kF4);
        if (feq(m()->img_logo.scale, 0.0f) && m()->img_logo_bar.x == 0) slide(m()->img_logo_bar, 0x400, 0x10, kF7680, 0);
        if (m()->img_logo_bar.x == 0x400 && feq(m()->img_gold_l.scale, 1.0f) && feq(m()->img_gold_r.scale, 1.0f)) {
            move(m()->img_gold_l, 0, kF4);
            move(m()->img_gold_r, 0, kF4);
        }
        if (feq(m()->img_gold_l.scale, 0.0f) && feq(m()->img_gold_r.scale, 0.0f) && feq(m()->img_alab_s.scale, 1.0f)) {
            slide(m()->img_lea_l, -0x108, 0, kF1600, 0);
            slide(m()->img_lea_r, 0x400, 0, kF1600, 0);
            move(m()->img_alab_s, 0, kF4);
        }
        if (m()->img_lea_l.x == -0x108 && m()->img_lea_r.x == 0x400 && feq(m()->img_alab_s.scale, 0.0f) && m()->img_alab_l.x == 0) {
            slide(m()->img_alab_l, -0xbc, 0, kF1200, 0);
            slide(m()->img_alab_r, 0x400, 0, kF1200, 0);
        }
        if (m()->img_alab_l.x == -0xbc && m()->img_alab_r.x == 0x400 && feq(m()->img_main.scale, 1.0f)) move(m()->img_main, 0, kF4);
        return feq(m()->img_main.scale, 0.0f);
    }

    // Slide-out step: previous button's word <= -51.2 and next button at rest -> slide next out.
    void out_step(const jo::Button& prev, jo::Button& next, i32 y) {
        if (wd(prev.x) <= gd(d::dbl_m51_2) && next.x == 0 && feq(next.move_accel, 0.0f))
            slide(next, -0x200, y, kF10, kF12000);
    }
    // Slide-in step for button cur, then start the next one when cur passed -300.
    void in_step(jo::Button& cur, jo::Button& next, i32 y) {
        if (wd(cur.x) >= gd(d::dbl_m409_6) && cur.x < 0) slide_v(cur, kFm4000);
        if (cur.x >= -0x12c && next.x == -0x200) slide(next, 0, y, kF3500, kFm1500);
    }
    void in_last(jo::Button& cur) {
        if (wd(cur.x) >= gd(d::dbl_m409_6) && cur.x < 0) slide_v(cur, kFm4000);
    }

    // Sub-panel request (+0x3415 / +0x3416 / +0x3417 share the 1.0 / 0.0 head).
    // Returns 0 = not active, 1 = closing buttons, 2 = closed (0.0 reached).
    int panel_head(const u8& flag) {
        if (b(flag) != 1 || b(m()->open) != 1 || m()->button12.x != -0x200) return 0;
        if (feq(m()->img_joan.scale, 1.0f) && b(m()->img_joan.scaling) == 0) {
            move(m()->img_joan, 0, kF4);
            move(m()->img_stripe, 0, kF4);
            return 1;
        }
        return feq(m()->img_joan.scale, 0.0f) ? 2 : 1;
    }

    void update() {
        c_menu.hit();
        // 0x0070acbf
        if (b(m()->slide) == 1 && b(m()->open) == 1) {
            if (b(m()->back_requested) != 0) {
                if (b(m()->busy) == 0 && m()->button12.x == -0x200 && m()->button11.x == -0x200) {
                    m()->slide = 0;
                    m()->difficulty_0_chosen = 0;
                    m()->difficulty_1_chosen = 0;
                    m()->difficulty_2_chosen = 0;
                    m()->difficulty_3_chosen = 0;
                    m()->back_requested = 0;
                    m()->open = 0;
                    m()->busy = 1;
                }
            } else if (m()->button12.x == -0x200 && b(m()->busy) == 0) {
                m()->difficulty_0_chosen = 0;
                m()->difficulty_1_chosen = 0;
                m()->difficulty_2_chosen = 0;
                m()->difficulty_3_chosen = 0;
                m()->open = 0;
                m()->busy = 1;
            }
        }
        // 0x0070add5
        if (panel_head(m()->panel_save) == 2) {
            if (joan::orig::arg::as<u8>(joan::orig::as_save_menu(obj(d::g_save_menu))->is_open()) == 0 && b(m()->busy) == 0) {  // 0x00430370
                global<u8>(d::g_savemenu_load_mode) = 1;
                joan::orig::as_save_menu(obj(d::g_save_menu))->open(static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x0074da40
                m()->busy = 1;
            }
            if (joan::orig::arg::as<u8>(joan::orig::as_save_menu(obj(d::g_save_menu))->is_open()) == 0) {  // 0x00430370
                m()->panel_save = 0;
                m()->open = 0;
                m()->busy = 0;
            }
        }
        // 0x0070aef3
        if (panel_head(m()->panel_options) == 2) {
            if (joan::orig::arg::as<u8>(joan::orig::as_opt_menu(obj(d::g_opt_menu))->is_open()) == 0 && b(m()->busy) == 0) {  // 0x004303a0
                joan::orig::as_opt_menu(obj(d::g_opt_menu))->open(static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})));                // 0x0073c9d0
                m()->busy = 1;
            }
            if (joan::orig::arg::as<u8>(joan::orig::as_opt_menu(obj(d::g_opt_menu))->is_open()) == 0) {  // 0x004303a0
                m()->panel_options = 0;
                m()->open = 0;
                m()->busy = 0;
            }
        }
        // 0x0070b005
        if (panel_head(m()->panel_quit_game) == 2) {
            if (right_chain()) {
                self_70d670();
                joan::orig::as_game(obj(d::g_game_a50088))->set_state(joan::orig::arg::u32(i32{0xf}));  // 0x004d7e70
            }
        }
        // 0x0070b3bd
        if (b(m()->panel_quit) == 1 && b(m()->open) == 1 && m()->button12.x == -0x200) {
            joan::orig::as_quit_popup(obj(d::g_quit_popup))->show(joan::orig::arg::u32(i32{1}));  // 0x006e4c00
            self_70d670();
        }
        // 0x0070b401
        if ((m()->button12.x != 0 || m()->button2.x != 0) && (m()->button11.x != 0 || m()->button7.x != 0))
            m()->buttons_off = 1;
        else
            m()->buttons_off = 0;
        if (b(m()->buttons_off) == 0) {
            jo::Button jo::MainMenu::* const buttons[10] = {&jo::MainMenu::button2, &jo::MainMenu::button3, &jo::MainMenu::button6, &jo::MainMenu::button5, &jo::MainMenu::button12, &jo::MainMenu::button7, &jo::MainMenu::button8, &jo::MainMenu::button9, &jo::MainMenu::button10, &jo::MainMenu::button11};
            for (i32 i = 0; i < 10; ++i) joan::orig::as_widget_child(&(m()->*buttons[i]).child)->enable();  // 0x006ca000
        } else {
            jo::Button jo::MainMenu::* const buttons[10] = {&jo::MainMenu::button2, &jo::MainMenu::button3, &jo::MainMenu::button5, &jo::MainMenu::button6, &jo::MainMenu::button12, &jo::MainMenu::button7, &jo::MainMenu::button8, &jo::MainMenu::button9, &jo::MainMenu::button10, &jo::MainMenu::button11};
            for (i32 i = 0; i < 10; ++i) joan::orig::as_widget_child(&(m()->*buttons[i]).child)->disable();  // 0x006ca020
        }
        // 0x0070b57e: release cached resources 10 s after +0x341f
        if (b(m()->flags[2]) == 0 && jo::self<jo::MainMenu>(this)->release_time > 0u &&
            global<u32>(d::g_game_time_ms) - jo::self<jo::MainMenu>(this)->release_time > 0x2710u) {
            for (i32 i = 0; i < 6; ++i) {
                if (jo::self<jo::MainMenu>(this)->images[i] != nullptr) {
                    joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(jo::self<jo::MainMenu>(this)->images[i]));  // 0x0060a550
                    jo::self<jo::MainMenu>(this)->images[i] = nullptr;
                }
            }
            jo::self<jo::MainMenu>(this)->release_time = 0;
        }
        // 0x0070b69b
        if (b(m()->difficulty_0_chosen) != 0 || b(m()->difficulty_1_chosen) != 0 || b(m()->difficulty_2_chosen) != 0 || b(m()->difficulty_3_chosen) != 0) {
            // selection made: slide the left column out, then the right one
            if (m()->button7.x == 0 && feq(m()->button7.move_accel, 0.0f)) {
                slide(m()->button7, -0x200, 0xe0, kF10, kF12000);
                sound(kSlideMany);
            }
            out_step(m()->button7, m()->button8, 0x114);
            out_step(m()->button8, m()->button9, 0x148);
            out_step(m()->button9, m()->button10, 0x17c);
            out_step(m()->button10, m()->button11, 0x1b0);
            if (m()->button10.x == -0x200 && feq(m()->img_stripe.scale, 1.0f) && feq(m()->img_joan.scale, 1.0f)) {
                move(m()->img_stripe, 0, kF4);
                move(m()->img_joan, 0, kF1);
                sound(kSlideOut);
            }
            if (right_chain()) {
                if (b(m()->difficulty_0_chosen) != 0) {
                    global<u8>(d::g_difficulty) = 0;
                    m()->difficulty_0_chosen = 0;
                } else if (b(m()->difficulty_1_chosen) != 0) {
                    global<u8>(d::g_difficulty) = 1;
                    m()->difficulty_1_chosen = 0;
                } else if (b(m()->difficulty_2_chosen) != 0) {
                    global<u8>(d::g_difficulty) = 2;
                    m()->difficulty_2_chosen = 0;
                } else if (b(m()->difficulty_3_chosen) != 0) {
                    global<u8>(d::g_difficulty) = 3;
                    m()->difficulty_3_chosen = 0;
                }
                self_70d670();
                joan::orig::as_unit_list(obj(d::g_profile_list))->clear_4c9f00();           // 0x004c9f00
                joan::orig::as_profile_list(obj(d::g_profile_list))->refresh(i32{0});   // 0x004c9cd0
                m()->slide = 0;
                joan::orig::as_cursor(obj(d::g_mouse_cursor))->set_596db0(i32{2});   // 0x00596db0
                global<u8>(d::g_a58f95) = 0;
                joan::orig::as_battle(obj(d::g_scenarios))->load_scenario_41cc80(i32{1}, i32{1}, i32{0});  // 0x0041cc80
                joan::orig::as_game(obj(d::g_game_a50088))->set_state(joan::orig::arg::u32(i32{0xc}));  // 0x004d7e70
            }
        } else {
            // 0x0070bd20
            if (b(m()->panel_save) == 0 && b(m()->panel_options) == 0 && b(m()->panel_quit_game) == 0) {
                if (jo::view<i32>(jo::self<jo::MainMenu>(this)->intro_delay_ms) < 1000) {
                    jo::self<jo::MainMenu>(this)->intro_delay_ms = jo::self<jo::MainMenu>(this)->intro_delay_ms + global<u32>(d::g_frame_elapsed_ms);  // add wraps
                    return;  // 0x0070bd7d: straight to the epilogue
                }
                if (feq(m()->img_main.scale, 0.0f) && b(m()->img_main.scaling) == 0) {
                    move(m()->img_main, 1, kF3);
                    sound(kSlideIn);
                }
                if (feq(m()->img_main.scale, 1.0f) && m()->img_alab_l.x == -0xbc) {
                    slide(m()->img_alab_l, 0, 0, kF1200, 0);
                    slide(m()->img_alab_r, 0x344, 0, kF1200, 0);
                }
                if (m()->img_alab_l.x == 0 && m()->img_alab_r.x == 0x344 && m()->img_lea_l.x == -0x108) {
                    slide(m()->img_lea_l, 0, 0, kF1600, 0);
                    slide(m()->img_lea_r, 0x2f8, 0, kF1600, 0);
                }
                if (m()->img_alab_l.x == 0 && m()->img_alab_r.x == 0x344 && feq(m()->img_alab_s.scale, 0.0f)) move(m()->img_alab_s, 1, kF4);
                if (m()->img_lea_l.x == 0 && m()->img_lea_r.x == 0x2f8 && feq(m()->img_alab_s.scale, 1.0f) && feq(m()->img_gold_l.scale, 0.0f)) {
                    move(m()->img_gold_l, 1, kF4);
                    move(m()->img_gold_r, 1, kF4);
                }
                if (feq(m()->img_gold_l.scale, 1.0f) && feq(m()->img_gold_r.scale, 1.0f) && m()->img_logo_bar.x == 0x400)
                    slide(m()->img_logo_bar, 0, 0x10, kF7680, 0);
                if (m()->img_logo_bar.x == 0 && feq(m()->img_logo.scale, 0.0f)) move(m()->img_logo, 1, kF4);
                if (feq(m()->img_logo.scale, 1.0f) && feq(m()->img_sphere.scale, 0.0f)) move(m()->img_sphere, 1, kF4);
                if (feq(m()->img_logo.scale, 1.0f) && feq(m()->img_joan.scale, 0.0f)) move(m()->img_joan, 1, kF1);
                if (feq(m()->img_sphere.scale, 1.0f) && static_cast<double>(m()->img_joan.scale) > gd(d::dbl_0_7) && feq(m()->img_stripe.scale, 0.0f))
                    move(m()->img_stripe, 1, kF2);
            }
            // 0x0070c0f8
            if (b(m()->open) == 0) {
                if (b(m()->slide) == 0) {
                    if (feq(m()->img_stripe.scale, 1.0f) && m()->button2.x == -0x200) {
                        slide(m()->button2, 0, 0xe0, kF3500, kFm1500);
                        sound(kSlideMany);
                    }
                    in_step(m()->button2, m()->button3, 0x12c);
                    in_step(m()->button3, m()->button6, 0x178);
                    in_step(m()->button6, m()->button5, 0x1c4);
                    in_step(m()->button5, m()->button12, 0x210);
                    in_last(m()->button12);
                } else {
                    // 0x0070c3cb
                    if (m()->button7.x == -0x200) {
                        slide(m()->button7, 0, 0xe0, kF3500, kFm1500);
                        sound(kSlideMany);
                    }
                    in_step(m()->button7, m()->button8, 0x114);
                    in_step(m()->button8, m()->button9, 0x148);
                    in_step(m()->button9, m()->button10, 0x17c);
                    in_step(m()->button10, m()->button11, 0x1b0);
                    in_last(m()->button11);
                }
            } else {
                // 0x0070c663
                if (m()->button2.x == 0 && feq(m()->button2.move_accel, 0.0f)) {
                    sound(kSlideMany);
                    slide(m()->button2, -0x200, 0xe0, kF10, kF12000);
                }
                out_step(m()->button2, m()->button3, 0x12c);
                out_step(m()->button3, m()->button6, 0x178);
                out_step(m()->button6, m()->button5, 0x1c4);
                out_step(m()->button5, m()->button12, 0x210);
                if (b(m()->slide) != 0) {
                    if (m()->button7.x == 0 && feq(m()->button7.move_accel, 0.0f)) {
                        slide(m()->button7, -0x200, 0xe0, kF10, kF12000);
                        sound(kSlideMany);
                    }
                    out_step(m()->button7, m()->button8, 0x114);
                    out_step(m()->button8, m()->button9, 0x148);
                    out_step(m()->button9, m()->button10, 0x17c);
                    out_step(m()->button10, m()->button11, 0x1b0);
                }
            }
        }
        // 0x0070ca5f: per-frame tick of every button
        jo::Image2D jo::MainMenu::* const images[13] = {&jo::MainMenu::img_main, &jo::MainMenu::img_alab_l, &jo::MainMenu::img_alab_r, &jo::MainMenu::img_lea_l, &jo::MainMenu::img_lea_r, &jo::MainMenu::img_alab_s, &jo::MainMenu::img_gold_l,
                                                        &jo::MainMenu::img_gold_r, &jo::MainMenu::img_logo_bar, &jo::MainMenu::img_logo, &jo::MainMenu::img_sphere, &jo::MainMenu::img_stripe, &jo::MainMenu::img_joan};
        jo::Button jo::MainMenu::* const buttons[10] = {&jo::MainMenu::button2, &jo::MainMenu::button3, &jo::MainMenu::button6, &jo::MainMenu::button5, &jo::MainMenu::button12, &jo::MainMenu::button7, &jo::MainMenu::button8, &jo::MainMenu::button9, &jo::MainMenu::button10, &jo::MainMenu::button11};
        for (i32 i = 0; i < 13; ++i) joan::orig::as_widget(&(m()->*images[i]))->update();   // 0x0061f3b0
        for (i32 i = 0; i < 10; ++i) joan::orig::as_widget(&(m()->*buttons[i]))->update();  // 0x0061f3b0
    }
};

JOAN_REPLACE("game/b110_07", "Menu::update_70acb0", 0x0070acb0, &H::update);

}  // namespace game_b110_07
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {


void* const kImages = obj(d::g_texture_array);

JOAN_COUNTER(c_menu_images, "MainMenu::load_images");

u32 image_mode() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kImages)->mode()); }                // 0x0060a720
void set_image_mode(u32 m) { joan::orig::as_texture_manager(kImages)->set_mode(joan::orig::arg::u32(m)); }               // 0x0060a740
jo::Texture* load_image(const char* name) { return joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(kImages)->load(joan::orig::arg::str(name), 1, 0u)); }  // 0x006099c0

static void JOAN_CDECL init_menu_colour() {  // 0x0070dfd0
    u32 tmp = 0;
    global<u32>(d::g_a6a5b8) = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(0xff, 0xff, 0xff, 0xcc));  // 0x0042a260
}

struct H {
    void menu_load_images() {  // 0x0070d370
        c_menu_images.hit();
        const u32 mode = image_mode();
        set_image_mode(1);
        if (jo::self<jo::MainMenu>(this)->images[0] == nullptr) {
            if (global<i32>(d::g_a48858) == 0) {
                jo::self<jo::MainMenu>(this)->images[0] = load_image(str(d::s_MENU1024));  // "MENU1024"
            } else {
                jo::self<jo::MainMenu>(this)->images[0] = load_image(str(d::s_MENU1024));  // "MENU1024"
            }
        }
        if (jo::self<jo::MainMenu>(this)->images[1] == nullptr) jo::self<jo::MainMenu>(this)->images[1] = load_image(str(d::s_MENU_A));  // "MENU_A"
        if (jo::self<jo::MainMenu>(this)->images[2] == nullptr) jo::self<jo::MainMenu>(this)->images[2] = load_image(str(d::s_MENU_G));  // "MENU_G"
        if (jo::self<jo::MainMenu>(this)->images[3] == nullptr) jo::self<jo::MainMenu>(this)->images[3] = load_image(str(d::s_MENU_J));  // "MENU_J"
        if (jo::self<jo::MainMenu>(this)->images[4] == nullptr) jo::self<jo::MainMenu>(this)->images[4] = load_image(str(d::s_MENU_S));  // "MENU_S"
        if (jo::self<jo::MainMenu>(this)->images[5] == nullptr) jo::self<jo::MainMenu>(this)->images[5] = load_image(str(d::s_MENU_SP));  // "MENU_SP"
        if (jo::self<jo::MainMenu>(this)->images[6] == nullptr) jo::self<jo::MainMenu>(this)->images[6] = load_image(str(d::s_MENU_B));  // "MENU_B"
        set_image_mode(mode);
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("MainMenu::load_images", 0x0070d370, &H::menu_load_images);
B94("init_menu_colour", 0x0070dfd0, &init_menu_colour);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b82_15.cpp (game/b82_15)
namespace {
namespace game_b82_15 {

void* const kSound = obj(d::g_texture_array);

JOAN_COUNTER(c_sounds, "Interface::free_sounds");

struct H {
    // 0x0070d510. ret 0.
    void free_sounds() {
        c_sounds.hit();
        for (std::int32_t i = 0; i < 7; ++i) {
            if (jo::self<jo::MainMenu>(this)->images[i] != nullptr) {
                joan::orig::as_texture_manager(kSound)->release(joan::orig::arg::mptr(jo::self<jo::MainMenu>(this)->images[i]));  // 0x0060a550
                jo::self<jo::MainMenu>(this)->images[i] = nullptr;
            }
        }
    }

    // 0x0070dae0. ret 0.
    void clear_flags() {
        jo::self<jo::MainMenu>(this)->flags[2] = 0;
        jo::self<jo::MainMenu>(this)->flags[3] = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b82_15", name, addr, fn)
B("Interface::free_sounds", 0x0070d510, &H::free_sounds);
B("Interface::clear_flags", 0x0070dae0, &H::clear_flags);
#undef B

}  // namespace game_b82_15
}  // namespace

// ---------------------------------------- from modules/game_ui8.cpp (game/ui8)
namespace {
namespace game_ui8 {

using joan::call::thiscall;

JOAN_COUNTER(c_reset, "Interface::reset_widgets");

// Widget::scale_to(target 0, speed, accel 0, instant 1, arg 0) via thunk 0x004034e0.
void scale_now(void* widget, std::uint32_t speed) {
    joan::orig::as_widget(widget)->scale_to(0, joan::orig::arg::f32(speed), 0.0f, 1, 0u);  // 0x0061f1b0
}
// Widget::move_to(x, y, speed, accel 0, instant 1, arg 0) via thunk 0x00403454.
void move_now(void* widget, std::uint32_t x, std::uint32_t y, std::uint32_t speed) {
    joan::orig::as_widget(widget)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(speed), 0.0f, 1, 0u);  // 0x0061e020
}

struct H {
    jo::MainMenu* m() { return jo::self<jo::MainMenu>(this); }

    // 0x0070d670. ret 0.
    void reset_widgets() {
        c_reset.hit();
        jo::self<jo::MainMenu>(this)->release_time = global<std::uint32_t>(d::g_game_time_ms);
        joan::orig::as_main_menu(this)->free_images();  // 0x0070d510
        joan::orig::as_main_menu(this)->clear_flags();  // 0x0070dae0
        scale_now(&m()->img_main, 0x40700000u);  // 3.75
        move_now(&m()->img_alab_l, 0xffffff44u, 0u, 0x44304000u);
        move_now(&m()->img_alab_r, 0x400u, 0u, 0x44304000u);
        move_now(&m()->img_lea_l, 0xfffffef8u, 0u, 0x44778000u);
        move_now(&m()->img_lea_r, 0x400u, 0u, 0x44778000u);
        scale_now(&m()->img_alab_s, 0x40700000u);
        scale_now(&m()->img_gold_l, 0x40700000u);
        scale_now(&m()->img_gold_r, 0x40700000u);
        move_now(&m()->img_logo_bar, 0x400u, 0x10u, 0x45700000u);
        scale_now(&m()->img_logo, 0x40700000u);
        scale_now(&m()->img_sphere, 0x40700000u);
        scale_now(&m()->img_stripe, 0x40700000u);
        scale_now(&m()->img_joan, 0x3f89249eu);
        move_now(&m()->button2, 0xfffffe00u, 0xe0u, 0x442f0000u);
        move_now(&m()->button3, 0xfffffe00u, 0x12cu, 0x442f0000u);
        move_now(&m()->button6, 0xfffffe00u, 0x178u, 0x442f0000u);
        move_now(&m()->button5, 0xfffffe00u, 0x1c4u, 0x442f0000u);
        move_now(&m()->button12, 0xfffffe00u, 0x210u, 0x442f0000u);
        move_now(&m()->button7, 0xfffffe00u, 0xe0u, 0x442f0000u);
        move_now(&m()->button8, 0xfffffe00u, 0x114u, 0x442f0000u);
        move_now(&m()->button9, 0xfffffe00u, 0x148u, 0x442f0000u);
        move_now(&m()->button10, 0xfffffe00u, 0x17cu, 0x442f0000u);
        move_now(&m()->button11, 0xfffffe00u, 0x1b0u, 0x442f0000u);
        std::uint8_t* const state = &m()->open;  // open .. u8_341d: 12 consecutive bytes
        for (std::int32_t i = 0; i < 12; ++i) state[i] = 0;
        jo::self<jo::MainMenu>(this)->intro_delay_ms = 0;
    }
};

#define U8(name, addr, fn) JOAN_REPLACE("game/ui8", name, addr, fn)
U8("Interface::reset_widgets", 0x0070d670, &H::reset_widgets);
#undef U8

}  // namespace game_ui8
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::call::thiscall;

JOAN_COUNTER(c_reset_70db10, "Obj::reset_70db10");

struct H {
    // ------------------------------------------------------------------ 0x0070db10
    std::uintptr_t reset_70db10() {
        c_reset_70db10.hit();
        joan::orig::as_main_menu(this)->load_images();  // 0x0070d370
        joan::orig::as_main_menu(this)->set_flag_13();  // 0x0070dcb0
        jo::self<jo::MainMenu>(this)->release_time = global<u32>(d::g_game_time_ms);
        u8* const state = &jo::self<jo::MainMenu>(this)->open;  // open .. u8_341d: 12 consecutive bytes
        for (i32 i = 0; i < 12; ++i) state[i] = 0;
        jo::self<jo::MainMenu>(this)->intro_delay_ms = 0;
        jo::self<jo::MainMenu>(this)->buttons_off = 1;
        static jo::Button jo::MainMenu::* const kButtons[10] = {&jo::MainMenu::button2, &jo::MainMenu::button3, &jo::MainMenu::button6, &jo::MainMenu::button5, &jo::MainMenu::button12, &jo::MainMenu::button7, &jo::MainMenu::button8, &jo::MainMenu::button9, &jo::MainMenu::button10, &jo::MainMenu::button11};
        std::uintptr_t r = 0;
        for (i32 i = 0; i < 10; ++i) r = joan::orig::arg::as<u32>(joan::orig::as_widget_child(&(jo::self<jo::MainMenu>(this)->*kButtons[i]).child)->disable());  // 0x006ca020
        return r;
    }
};

static u32 packed_ffff00cc() {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(0xffu, 0xffu, 0u, 0xccu));  // 0x0042a260
}
static void JOAN_CDECL init_a6a598() { global<u32>(d::g_a6a598) = packed_ffff00cc(); }  // 0x0070de50
static void JOAN_CDECL init_a6a5a4() { global<u32>(d::g_a6a5a4) = packed_ffff00cc(); }  // 0x0070e0f0

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::reset_70db10", 0x0070db10, &H::reset_70db10);
B("init_a6a598", 0x0070de50, &init_a6a598);
B("init_a6a5a4", 0x0070e0f0, &init_a6a5a4);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {


const u32* color_ctor(void* c, u32 r, u32 g, u32 b, u32 a) {
    return joan::orig::arg::as<const u32*>(joan::orig::as_colour(c)->construct(r, g, b, a));  // 0x0042a260 (returns this)
}

u32 JOAN_CDECL color_a6a5a8() {  // 0x0070df70
    u32 tmp = 0;
    const u32* const p = color_ctor(&tmp, 0xff, 0, 0, 0xcc);
    const u32 v = *p;
    global<u32>(d::g_a6a5a8) = v;
    return v;
}

struct H {
    std::uintptr_t set_13() {  // 0x0070dcb0
        jo::self<jo::MainMenu>(this)->flags[2] = 1;
        jo::self<jo::MainMenu>(this)->flags[3] = 0;
        return reinterpret_cast<std::uintptr_t>(this);
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::set_13", 0x0070dcb0, &H::set_13);
B98("Globals::color_a6a5a8", 0x0070df70, &color_a6a5a8);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

// ---- static initialiser wrappers (direct calls, no thunks) ----
template <fn::Code Body>
u32 JOAN_CDECL init_fwd() {
    return cdecl_call<u32>(Body);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("init_70dce0", 0x0070dce0, &init_fwd<fn::set_pi_a6a584>);
B("init_70dd40", 0x0070dd40, &init_fwd<fn::init_a6a588>);
B("init_70dd90", 0x0070dd90, &init_fwd<fn::init_a6a58c>);
B("init_70dde0", 0x0070dde0, &init_fwd<fn::init_a6a590>);
B("init_70de30", 0x0070de30, &init_fwd<fn::init_a6a598>);
B("init_70de90", 0x0070de90, &init_fwd<fn::init_a6a5b4>);
B("init_70def0", 0x0070def0, &init_fwd<fn::set_color_a6a59c>);
B("init_70df50", 0x0070df50, &init_fwd<fn::Globals_color_a6a5a8>);
B("init_70dfb0", 0x0070dfb0, &init_fwd<fn::init_menu_colour>);
B("init_70e010", 0x0070e010, &init_fwd<fn::init_colour_a6a5ac>);
B("init_70e070", 0x0070e070, &init_fwd<fn::init_colour_a6a5a0>);
B("init_70e0d0", 0x0070e0d0, &init_fwd<fn::init_a6a5a4>);
B("init_70e130", 0x0070e130, &init_fwd<fn::init_a6a5b0>);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

// 0x0042a260 Color::Color(b0, b1, b2, b3) on a 4-byte local; returns its address.
u32 make_color(i32 b0, i32 b1, i32 b2, i32 b3) {
    u32 c = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&c)->construct(b0, b1, b2, b3));
}

// ---------------------------------------------------------------------------- 0x006def90 / 0x0070dd00
float pi_value() {
    const double a = joan::orig::arg::as<double>(joan::orig::libc::atan(joan::orig::arg::f64(1.0)));  // atan
    return static_cast<float>(Fpu{true}.mul(a, global<double>(d::dbl_4)));  // * 4.0
}
static void JOAN_CDECL set_pi_a6a584() { global<float>(d::g_pi_a6a584) = pi_value(); }

// ---------------------------------------------------------------------------- 0x0070df10
static u32 JOAN_CDECL set_color_a6a59c() {
    const u32 c = make_color(0, 0xff, 0, 0xcc);
    global<u32>(d::g_a6a59c) = c;
    return c;
}

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("set_pi_a6a584", 0x0070dd00, &set_pi_a6a584);
B95("set_color_a6a59c", 0x0070df10, &set_color_a6a59c);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

// 0x0070dd60
void JOAN_CDECL init_a6a588() { global<float>(d::g_a6a588) = global<float>(d::g_pi_a6a584) / global<float>(d::flt_2); }

// 0x0070e030
u32 JOAN_CDECL init_colour_a6a5ac() {
    u32 colour;
    const u32 v = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&colour)->construct(0xffu, 0xffu, 0xffu, 0xccu));  // 0x0042a260
    global<u32>(d::g_a6a5ac) = v;
    return v;
}

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("init_a6a588", 0x0070dd60, &init_a6a588);
B97("init_colour_a6a5ac", 0x0070e030, &init_colour_a6a5ac);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {

// ---------------------------------------------------------------------------- static initialisers
u32 packed_colour(u32 a, u32 b, u32 c, u32 d) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(a, b, c, d));  // 0x0042a260
}
static void JOAN_CDECL init_a6a58c() { global<float>(d::g_a6a58c) = global<float>(d::g_pi_a6a584) / global<float>(d::flt_3); }  // 0x0070ddb0
static void JOAN_CDECL init_a6a5b4() { global<u32>(d::g_a6a5b4) = packed_colour(0u, 0u, 0xffu, 0xccu); }      // 0x0070deb0
static void JOAN_CDECL init_a6a5b0() { global<u32>(d::g_a6a5b0) = packed_colour(0xffu, 0xffu, 0xffu, 0xccu); }  // 0x0070e150

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("init_a6a58c", 0x0070ddb0, &init_a6a58c);
B("init_a6a5b4", 0x0070deb0, &init_a6a5b4);
B("init_a6a5b0", 0x0070e150, &init_a6a5b0);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

// 0x0070de00
void JOAN_CDECL init_a6a590() { global<float>(d::g_a6a590) = global<float>(d::g_pi_a6a584) / global<float>(d::flt_4); }

// 0x0070e090
u32 JOAN_CDECL init_colour_a6a5a0() {
    u32 colour;
    const u32 v = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&colour)->construct(0xffu, 0xffu, 0xffu, 0xccu));  // 0x0042a260
    global<u32>(d::g_a6a5a0) = v;
    return v;
}

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("init_a6a590", 0x0070de00, &init_a6a590);
B96("init_colour_a6a5a0", 0x0070e090, &init_colour_a6a5a0);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::MainMenu::load_images() -> void { joan::bind::member<&::game_b94_08::H::menu_load_images>(this); }  // 0x70d370
auto joan::orig::MainMenu::free_images() -> void { joan::bind::member<&::game_b82_15::H::free_sounds>(this); }  // 0x70d510
auto joan::orig::MainMenu::reset_widgets() -> void { joan::bind::member<&::game_ui8::H::reset_widgets>(this); }  // 0x70d670
auto joan::orig::MainMenu::clear_flags() -> void { joan::bind::member<&::game_b82_15::H::clear_flags>(this); }  // 0x70dae0
auto joan::orig::MainMenu::reset() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::reset_70db10>(this)); }  // 0x70db10
auto joan::orig::MainMenu::set_flag_13() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::set_13>(this)); }  // 0x70dcb0
