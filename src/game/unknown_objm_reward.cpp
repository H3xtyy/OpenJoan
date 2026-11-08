// game/unknown_objm_reward.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/objm_reward.cpp; re/attribution.csv: between ww/interfac/objmenub.cpp and ww/interfac/objmenuw.cpp).
// They move to their original file when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
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

using joan::call::cdecl_call;

using i16 = std::int16_t;

using joan::hook::x87::Fpu;

using joan::img::obj;

using i8 = std::int8_t;

using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_map_widgets, "Map::create_widgets_732580");

// 0x0042a260 (thunk 0x004098b8, ret 0x10): builds a 4-byte colour in `c`, returns &c.
u32 make_color(i32 a, i32 b, i32 c, i32 d) {
    u32 color = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&color)->construct(a, b, c, d));
}

struct H {
    // ------------------------------------------------------------------ 0x00732580
    static void* new_obj(u32 size, std::uintptr_t ctor) {
        void* const mem = joan::crt::operator_new(joan::orig::arg::u32(size));  // operator new
        return mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
    }
    u32 create_widgets_732580() {
        c_map_widgets.hit();
        jo::self<jo::CampScreen>(this)->reward_frame = static_cast<jo::Widget*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::Image2D)), fn::code_value(fn::Widget629_ctor)));  // 0x00629400
        {
            const u32 c = make_color(0, 0, 0, 0xff);
            joan::orig::as_label(jo::self<jo::CampScreen>(this)->reward_frame)->create(i32{0x186}, i32{0x1e2}, img::obj(d::g_bmp_objmenu), img::str(d::s_SCR_FH), joan::orig::arg::i32(c), i32{3}, i32{0}, i32{2}, i32{0}, i32{-1}, i32{-1}, i32{-1}, i32{-1}, i32{0}, i32{-1}, i32{-1}, i32{0}, i32{1}, i32{1});  // 0x00629660 'SCR_FH'
        }
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->reward_frame)->scale_to(0, 1.0f, 0.0f, 1, 0u);  // 0x0061f1b0
        jo::self<jo::CampScreen>(this)->reward_up_button = static_cast<jo::Widget*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0)));  // 0x00620ee0
        joan::orig::as_menu_button(jo::self<jo::CampScreen>(this)->reward_up_button)->setup(joan::orig::arg::i16(i32{0xa9}), joan::orig::arg::i16(i32{0xc2}), img::obj(d::g_bmp_commele2), img::str(d::s_aw_u_ot), img::str(d::s_aw_u_dw), img::str(d::s_aw_u_ov), joan::orig::arg::str(i32{0}), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})));  // 0x006211d0 aw_u_*
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->reward_up_button)->scale_to(0, 1.0f, 0.0f, 1, 0u);
        jo::self<jo::CampScreen>(this)->reward_down_button = static_cast<jo::Button*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0)));
        joan::orig::as_menu_button(jo::self<jo::CampScreen>(this)->reward_down_button)->setup(joan::orig::arg::i16(i32{0xa9}), joan::orig::arg::i16(i32{0x26c}), img::obj(d::g_bmp_commele2), img::str(d::s_aw_d_ot), img::str(d::s_aw_d_dw), img::str(d::s_aw_d_ov), joan::orig::arg::str(i32{0}), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{0})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})));  // aw_d_*
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->reward_down_button)->scale_to(0, 1.0f, 0.0f, 1, 0u);
        jo::self<jo::CampScreen>(this)->reward_camera = static_cast<jo::Camera*>(new_obj(static_cast<std::uint32_t>(sizeof(jo::Camera)), fn::code_value(fn::Obj_ctor_613650)));  // 0x00613650
        joan::orig::as_sys_en(jo::self<jo::CampScreen>(this)->reward_camera)->init_613730();     // 0x00613730
        joan::orig::as_camera(jo::self<jo::CampScreen>(this)->reward_camera)->set_viewport(joan::orig::as_surface(img::obj(d::g_vga_back)), img::obj(d::g_a56ef0), joan::orig::arg::u32(i32{0x30}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0x3ff}), joan::orig::arg::u32(i32{0x2ff}));  // 0x00613bf0
        joan::orig::as_camera(jo::self<jo::CampScreen>(this)->reward_camera)->set_cf(joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(global<u32>(d::g_init_black)));  // 0x00613d30
        joan::orig::as_camera(jo::self<jo::CampScreen>(this)->reward_camera)->set_ranges(joan::orig::arg::f32(u32{0x3f800000}), joan::orig::arg::f32(u32{0x45fd0800}), joan::orig::arg::f32(u32{0x45fd0800}));  // 0x00613ca0
        joan::orig::as_camera(jo::self<jo::CampScreen>(this)->reward_camera)->set_scene(jo::self<jo::CampScreen>(this)->map_scene);  // 0x00613db0
        return 0;  // 0x00613db0 returns nothing; the only caller discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Map::create_widgets_732580", 0x00732580, &H::create_widgets_732580);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {

using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

struct CamParams {
    u32 v[10];
};

static_assert(sizeof(CamParams) == 0x28, "CamParams");

JOAN_COUNTER(c_release_732940, "Obj::release_732940");
JOAN_COUNTER(c_set_camera, "Obj::set_camera_732af0");

struct H {
    // Field release: optional pre-call, scalar deleting dtor(1) when non-null, field = 0.
    template <class T>
    static void release(T*& field, std::uintptr_t pre, std::uintptr_t del) {
        if (field == nullptr) return;
        if (pre != 0) thiscall<u32>(pre, field);
        T* const p = field;
        if (p != nullptr) thiscall<u32>(del, p, 1u);
        field = nullptr;
    }

    // ------------------------------------------------------------------ 0x00732940
    void release_732940() {
        c_release_732940.hit();
        release(jo::self<jo::CampScreen>(this)->reward_frame, fn::code_value(fn::Image2d_release), fn::code_value(fn::Sprite_scalar_dtor));  // 0x00629830, 0x006bd440
        release(jo::self<jo::CampScreen>(this)->reward_up_button, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));  // 0x006217e0, 0x006b33a0
        release(jo::self<jo::CampScreen>(this)->reward_down_button, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        release(jo::self<jo::CampScreen>(this)->reward_camera, 0, fn::code_value(fn::Obj562910_scalar_deleting_dtor));           // 0x00562910
    }

    // ------------------------------------------------------------------ 0x00732af0
    u32 set_camera_732af0() {
        c_set_camera.hit();
        CamParams p;
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_camera))->frame(&p);  // 0x006140a0
        p.v[0] = 0;
        const float cx = (jo::self<jo::CampScreen>(this)->map_min_x + jo::self<jo::CampScreen>(this)->map_max_x) / global<float>(d::flt_2);
        const float cy = (jo::self<jo::CampScreen>(this)->map_min_y + jo::self<jo::CampScreen>(this)->map_max_y) / global<float>(d::flt_2);
        std::memcpy(&p.v[1], &cx, 4);
        std::memcpy(&p.v[2], &cy, 4);
        p.v[3] = 0;
        p.v[7] = 0x44160000u;  // 600.0f
        const Fpu fpu{true};
        double a = fpu.mul(static_cast<double>(global<i16>(d::g_rotation)), global<float>(d::flt_90));  // fild * 90
        a = fpu.sub(global<float>(d::flt_90), a);                                                   // 90 - a
        a = fpu.mul(a, global<float>(d::flt_0_017453292));                                                   // * deg->rad
        const float angle = static_cast<float>(a);
        std::memcpy(&p.v[4], &angle, 4);
        p.v[5] = 0x3fc90fdbu;  // pi / 2
        p.v[9] = 0x3f000000u;  // 0.5f
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_camera))->set_frame(&p);  // 0x00614040
        return 0;  // 0x00614040 returns nothing; the only caller discards the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Obj::release_732940", 0x00732940, &H::release_732940);
B("Obj::set_camera_732af0", 0x00732af0, &H::set_camera_732af0);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Screen::nop_732bd0", 0x00732bd0, &H::nop);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b108_08.cpp (game/b108_08)
namespace {
namespace game_b108_08 {

using u32 = std::uint32_t;

using joan::call::thiscall;

// 0x0042a260 colour(r, g, b, a) on a local, packed dword read back.
u32 make_colour(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
}

// ---------------------------------------------------------------- thiscall functions
struct H {
    // 0x00732bf0
    u32 draw_black_732bf0(u32 /*unused*/) {
        const u32 colour = make_colour(0, 0, 0, 0xff);
        return joan::orig::arg::as<u32>(joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue(0, 0, 0x400, 0x300, 9u, joan::orig::arg::u32(colour), joan::orig::arg::mptr(0), 3, 0, 0, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(// 0x00632a70
                             -1), joan::orig::arg::i32(-1), 0));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_08", name, addr, fn)
B("Screen::draw_black_732bf0", 0x00732bf0, &H::draw_black_732bf0);
#undef B

}  // namespace game_b108_08
}  // namespace

// ---------------------------------------- from modules/game_b109_07.cpp (game/b109_07)
namespace {
namespace game_b109_07 {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_reward_draw, "RewardScreen::draw_732c70");

void* const kText = obj(d::g_sprites_a46128);
void* const kRes = obj(d::g_misc);
void* const kStrings = obj(d::g_strings);

// 0x0042a260 colour (r, g, b, a) into a 4-byte local, dword read back.
u32 colour(u8 r, u8 g, u8 b, u8 a) {
    alignas(4) u8 slot[4];
    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
    return jo::view<u32>(*jo::as_colour(p));
}
// fld 255.0f; fmul [p+0x11]; _ftol
u8 alpha_of(const void* gadget) {
    const float prod = global<float>(d::flt_255) * jo::as_widget(gadget)->scale;
    return static_cast<u8>(joan::hook::x87::ftol(static_cast<double>(prod)));
}
// 0x00633500 (text / image draw, 17 args) with the tail 3, 0, k, 0, -1, -1, -1, -1, 0, -1, -1.
void text_draw(i32 x, i32 y, u32 target, const char* name, img::Addr style, u32 color, u32 k = 2) {
    joan::orig::as_sprite_queue(kText)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(target), joan::orig::arg::str(name), img::obj(style), joan::orig::arg::u32(color), 3, 0, joan::orig::arg::i16(k), 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// 0x00626130 (text in a box, 13 args) with the tail 0, 1, 2, 0, 1.0f, 1.0f.
void box_text(void* font, i32 x, i32 y, i32 w, i32 h, u32 target, const void* text, u32 color) {
    joan::orig::as_text(font)->draw_3(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h), joan::orig::arg::u32(target), joan::orig::arg::str(text), joan::orig::arg::u32(color), 0, 1, 2, 0, 1.0f, 1.0f);  // 0x00626130
}
// 0x006255d0 (aligned text, 12 args) with target 2 and the tail -1, 0, 2, 2, 0, 1.0f, 1.0f.
void aligned_text(void* font, i32 x, i32 y, const void* text, u32 color) {
    joan::orig::as_text(font)->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), 2u, joan::orig::arg::str(text), joan::orig::arg::u32(color), 0xffffffff, 0, 2, 2, 0, 1.0f, 1.0f);  // 0x006255d0
}

struct H {
    // ------------------------------------------------------------------ 0x00732c70
    u8* row_gadget(i32 i) { return jo::view<u8*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]); }
    jo::EndCon& reward_rec(i32 j) {  // Game 0x00a58f90 EndCon q (0x00a5929b + q * 0x1a), q = (short)[this + 0xa7 + j*2] (re-read)
        return jo::as_game(obj(d::g_game_state))->end_conditions[static_cast<i32>(jo::self<jo::CampScreen>(this)->reward_condition_ids[j])];
    }
    void fancy_row(i32 i, std::uintptr_t text_fn) {
        text_draw(0x67, i * 0x28 + 0xdc, 2u, str(d::s_fancybar), d::g_bmp_objmenu,
                  colour(0xff, 0xff, 0xff, alpha_of(row_gadget(i))));  // fancybar
        const u32 c = colour(0, 0, 0, alpha_of(row_gadget(i)));
        void* const t = thiscall<void*>(text_fn, kStrings);
        box_text(obj(d::g_font_goudy12_bold_black), 0x67, i * 0x28 + 0xdc, 0x12f, i * 0x28 + 0x100, 2u, t, c);
    }
    void box_row(i32 i, img::Addr name) {
        text_draw(0x1f, i * 0x28 + 0xd9, 2u, str(name), d::g_bmp_objmenu, colour(0xff, 0xff, 0xff, alpha_of(row_gadget(i))));
        text_draw(0x1f, i * 0x28 + 0xd9, 2u, str(d::s_box_bg), d::g_bmp_objmenu,
                  colour(0xff, 0xff, 0xff, alpha_of(row_gadget(i))));  // box_bg
    }
    u32 white_d3() { return colour(0xff, 0xff, 0xff, alpha_of(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_frame))); }
    void reward_value(i32 j, u32 jo::EndCon::*field, i32 row) {
        if (static_cast<i8>(reward_rec(j).rewarded) != 0) {
            const u32 c = white_d3();
            const u32 v = reward_rec(j).*field;
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_menu(kStrings)->objmenu_28(joan::orig::arg::u32(v)));  // 0x004dc1f0
            aligned_text(obj(d::g_font_goudy_bold_italic_24), 0x2e6, global<i32>(d::k_9df47c + static_cast<u32>(row) * 4u) + 0xa, t, c);
        } else {
            const u32 c = white_d3();
            const u32 v = reward_rec(j).*field;
            void* const t = joan::orig::arg::as<void*>(joan::orig::as_misc(kRes)->format_int_58fca0(joan::orig::arg::i32(v), 4));  // 0x0058fca0
            aligned_text(obj(d::g_font_goudy_bold_italic_24), 0x2e6, global<i32>(d::k_9df47c + static_cast<u32>(row) * 4u) + 0xa, t, c);
        }
    }
    void reward_draw() {
        c_reward_draw.hit();
        const float zero_k = global<float>(d::k_9cf5c4);
        if (jo::self<jo::CampScreen>(this)->reward_up_button->scale > zero_k) {
            if (jo::self<jo::CampScreen>(this)->reward_scroll > 0) joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_up_button))->draw(2u, 7);  // 0x00622cc0
            if (jo::self<jo::CampScreen>(this)->reward_scroll < static_cast<i32>(jo::self<jo::CampScreen>(this)->reward_count - 0xau))
                joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_down_button))->draw(2u, 7);  // 0x00622cc0
            for (i32 i = 0; i < 10; ++i) {
                joan::orig::as_button(row_gadget(i))->draw(2u, 7);  // 0x00622cc0
                switch (static_cast<i32>(jo::view<i8>(jo::self<jo::CampScreen>(this)->reward_slot_kinds[i]))) {
                    case 1: fancy_row(i, fn::code_value(fn::ObjMenu_open_11)); break;  // 0x004dbd90
                    case 2: fancy_row(i, fn::code_value(fn::TxtRes_objmenu_12)); break;  // 0x004dbdc0
                    case 3: fancy_row(i, fn::code_value(fn::Menu_objmenu_13)); break;  // 0x004dbdf0
                    case 4: box_row(i, d::s_box_p); break;    // box_p
                    case 5: box_row(i, d::s_box_c); break;    // box_c
                    case 6: box_row(i, d::s_box_f); break;    // box_f
                    default: break;
                }
            }
        }
        if (static_cast<i32>(jo::view<i8>(jo::self<jo::CampScreen>(this)->visible)) != 2) return;
        if (jo::self<jo::CampScreen>(this)->reward_frame->scale > global<float>(d::k_9cf5c4)) {
            const u32 c = white_d3();
            text_draw(jo::view<u16>(jo::self<jo::CampScreen>(this)->shop_frame->x), jo::view<u16>(jo::self<jo::CampScreen>(this)->shop_frame->y), 2u, str(d::s_SCR_FHBG), d::g_bmp_objmenu,
                      c);  // SCR_FHBG
            joan::orig::as_label(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_frame))->draw(2u, 7u);  // 0x00629bd0
        }
        for (i32 j = 0; j < 10; ++j) {
            if (!(static_cast<i32>(jo::view<i8>(jo::self<jo::CampScreen>(this)->reward_slot_kinds[j])) > 2)) continue;
            {
                u8* const g = row_gadget(j);
                const i32 h = jo::as_widget(g)->y2;
                const i32 w = jo::as_widget(row_gadget(j))->x2;
                const i32 y = jo::as_widget(row_gadget(j))->y;
                const i32 x = jo::as_widget(row_gadget(j))->x;
                if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h))) == 0) continue;  // 0x00593980
            }
            i32 n = 0;
            if (!(static_cast<i32>(jo::self<jo::CampScreen>(this)->reward_condition_ids[j]) >= 0)) continue;
            if (jo::self<jo::CampScreen>(this)->reward_frame->scale > global<float>(d::k_9cf5c4)) {
                i32 row = 0;
                {
                    const u32 c = white_d3();
                    void* const t = joan::orig::arg::as<void*>(joan::orig::as_obj_menu(kStrings)->open_27());  // 0x004dc1c0
                    aligned_text(obj(d::g_font_goudy14_22_white), 0x268, 0x1f0, t, c);
                }
                text_draw(0x268, 0x20a, 2u, str(d::s_goldbar), d::g_bmp_objmenu, white_d3());  // goldbar
                if (reward_rec(j).reward_gold != 0) {
                    text_draw(0x25a, global<u16>(d::k_9df47c + static_cast<u32>(row) * 4u), 2u, str(d::s_reward_b), d::g_bmp_objmenu,
                              white_d3());  // reward_b
                    text_draw(0x266, global<i32>(d::k_9df47c + static_cast<u32>(row) * 4u) + 4, 2u, str(d::s_goldicon),
                              d::g_bmp_objmenu, white_d3());  // goldicon
                    reward_value(j, &jo::EndCon::reward_gold, row);
                    row = row + 1;
                }
                if (reward_rec(j).reward_exp != 0) {
                    text_draw(0x25a, global<u16>(d::k_9df47c + static_cast<u32>(row) * 4u), 2u, str(d::s_reward_b), d::g_bmp_objmenu,
                              white_d3());  // reward_b
                    text_draw(0x25e, global<i32>(d::k_9df47c + static_cast<u32>(row) * 4u) + 0xa, 2u,
                              str(d::s_expicon), d::g_bmp_objmenu, white_d3());  // expicon
                    reward_value(j, &jo::EndCon::reward_exp, row);
                }
            }
            if (reward_rec(j).text2 != nullptr)
                n = joan::orig::arg::as<i32>(joan::orig::as_table(obj(d::g_territory_array))->find_by_name(joan::orig::arg::str(reward_rec(j).text2)));  // 0x004e4b20
            if (!(n > 0)) continue;
            u8* const objective = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_territory_array))->get_420ff0(joan::orig::arg::i32(n)));  // 0x00420ff0
            const u8 state = jo::self<jo::CampScreen>(this)->reward_slot_kinds[j];
            if (state == 4)
                text_draw(0x1cc, 0x134, 2u, str(d::s_fc_pend), d::g_bmp_objmenu, colour(0xff, 0xff, 0xff, 0xff));  // fc_pend
            else if (state == 5)
                text_draw(0x1cc, 0x134, 2u, str(d::s_fc_pass), d::g_bmp_objmenu, colour(0xff, 0xff, 0xff, 0xff));  // fc_pass
            else if (state == 6)
                text_draw(0x1cc, 0x134, 2u, str(d::s_fc_fail), d::g_bmp_objmenu, colour(0xff, 0xff, 0xff, 0xff));  // fc_fail
            if (joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(jo::as_zone_def(objective)->script))) == 0) continue;  // strlen (ZoneDef +0x3c script)
            text_draw(0x18f, 0x1eb, 2u, jo::as_zone_def(objective)->script, d::g_bmp_objmenu, white_d3(), 1u);
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_07", name, addr, fn)
B("RewardScreen::draw", 0x00732c70, &H::reward_draw);
#undef B

}  // namespace game_b109_07
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_scroll_734120, "List::scroll_734120");

// 0x0042a260: 4-byte colour (r, g, b, a) built in place; the original reads the dword back.
u32 color_ffffffff() {
    alignas(4) u8 c[4] = {0, 0, 0, 0};
    const u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));  // 0x0042a260
    return jo::view<u32>(*jo::as_colour(p));
}

struct H {
    // --------------------------------------------------------------------- 0x00734120
    void scroll_refresh() {
        for (i32 i = 0; i < 10; ++i) {
            joan::orig::as_widget(jo::view<void**>(jo::self<jo::CampScreen>(this)->slot_buttons)[i])->scale_to(1, joan::orig::arg::f32(u32{0x40800000}), 0.0f, 0, 0xc8u);  // 0x0061f1b0
            const u32 color = color_ffffffff();
            joan::orig::as_button(jo::view<void**>(jo::self<jo::CampScreen>(this)->slot_buttons)[i])->set_text(str(d::kEmpty), obj(d::g_font_goudy12_bold_black), joan::orig::arg::i32(color), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
            jo::self<jo::CampScreen>(this)->reward_slot_kinds[i] = 0;
        }
    }
    u8 scroll_734120() {
        c_scroll_734120.hit();
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_up_button))->mouse(i32{0}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
            if (jo::self<jo::CampScreen>(this)->reward_scroll > 0) {
                scroll_refresh();
                joan::orig::as_panel(this)->build_list_734a30(joan::orig::arg::i32(jo::self<jo::CampScreen>(this)->reward_scroll - 1));  // 0x00734a30
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_down_button))->mouse(i32{0}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
            if (jo::self<jo::CampScreen>(this)->reward_scroll < static_cast<i32>(jo::self<jo::CampScreen>(this)->reward_count - 0xau)) {
                scroll_refresh();
                joan::orig::as_panel(this)->build_list_734a30(joan::orig::arg::i32(jo::self<jo::CampScreen>(this)->reward_scroll + 1));  // 0x00734a30
            }
            return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]))->mouse(i32{0x1b}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
            joan::orig::as_camp_screen(this)->set_state_721a60(joan::orig::arg::u32(i32{0}));  // 0x00721a60
        }
        return 1;
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("List::scroll_734120", 0x00734120, &H::scroll_734120);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using joan::hook::x87::ftol;

using joan::orig::helpers::at;

JOAN_COUNTER(c_panel_7343b0, "Panel::update_7343b0");

// ------------------------------------------------------------------ easing (0x007343b0)
// Moves `cur` towards `target` by 15 when the truncated values differ by more than 15, else
// snaps it (only when they differ at all).
void ease(float& cur, float target) {
    if (ftol(cur) < ftol(target)) {
        if (ftol(cur) < static_cast<i32>(static_cast<u32>(ftol(target)) - 0xfu))
            cur = cur + global<float>(d::flt_15);
        else
            cur = target;
    } else if (ftol(cur) > ftol(target)) {
        if (ftol(cur) > static_cast<i32>(static_cast<u32>(ftol(target)) + 0xfu))
            cur = cur - global<float>(d::flt_15);
        else
            cur = target;
    }
}
struct View {  // 40-byte view copied by 0x006140a0 / 0x00614040 (+0xa0 of the camera)
    float f[10];
};
static_assert(sizeof(View) == 40, "view");

// 0x0061f1b0 (thunk 0x004034e0), ret 0x14 / 0x0061e020 (thunk 0x00403454), ret 0x18.
void anim(void* button, u32 on, u32 speed, u32 a, u32 b, u32 c) {
    joan::orig::as_widget(button)->scale_to(joan::orig::arg::i16(on), joan::orig::arg::f32(speed), joan::orig::arg::f32(a), static_cast<char>(b), joan::orig::arg::u32(c));
}

struct H {
    // ------------------------------------------------------------------ 0x007343b0
    void update_7343b0() {
        c_panel_7343b0.hit();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::RewardScreen>(this)->reward_frame))->update();  // 0x0061f3b0
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::RewardScreen>(this)->reward_up_button))->update();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::RewardScreen>(this)->reward_down_button))->update();
        bool highlighted = false;
        if (jo::view<i8>(jo::self<jo::RewardScreen>(this)->visible) != 2) return;
        for (i32 i = 0; i < 10; ++i) {
            if (!(jo::view<i8>(jo::self<jo::CampScreen>(this)->reward_slot_kinds[i]) > 2)) continue;
            u8* const r = jo::view<u8*>(jo::self<jo::RewardScreen>(this)->slot_buttons->buttons[i]);
            if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(jo::as_widget(r)->x), static_cast<i32>(jo::self<jo::RewardScreen>(this)->slot_buttons->buttons[i]->y), static_cast<i32>(jo::self<jo::RewardScreen>(this)->slot_buttons->buttons[i]->x2), static_cast<i32>(jo::self<jo::RewardScreen>(this)->slot_buttons->buttons[i]->y2))) == 0)  // 0x00593980
                continue;
            i32 item = 0;
            if (jo::self<jo::CampScreen>(this)->reward_condition_ids[i] == -1) break;
            const jo::EndCon& entry = jo::as_game(obj(d::g_game_state))->end_conditions[static_cast<i32>(jo::self<jo::CampScreen>(this)->reward_condition_ids[i])];
            if (entry.reward_exp != 0 ||
                entry.reward_gold != 0) {
                const float scale = jo::self<jo::RewardScreen>(this)->reward_frame->scale_target;
                if (scale < global<float>(d::flt_1) || scale > global<float>(d::flt_1))
                    anim(jo::view<void*>(jo::self<jo::RewardScreen>(this)->reward_frame), 1, 0x40c00000u, 0, 0, 0);
                highlighted = true;
            }
            const jo::EndCon& entry2 = jo::as_game(obj(d::g_game_state))->end_conditions[static_cast<i32>(jo::self<jo::CampScreen>(this)->reward_condition_ids[i])];
            if (entry2.text2 != nullptr) {
                const jo::EndCon& entry3 = jo::as_game(obj(d::g_game_state))->end_conditions[static_cast<i32>(jo::self<jo::CampScreen>(this)->reward_condition_ids[i])];
                item = joan::orig::arg::as<i32>(joan::orig::as_table(obj(d::g_territory_array))->find_by_name(joan::orig::arg::str(entry3.text2)));  // 0x004e4b20
            }
            if (item <= 0) break;
            u8* const pos = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_territory_array))->get_420ff0(joan::orig::arg::i32(item)));  // 0x00420ff0
            const Fpu fpu{true};
            const float tx = static_cast<float>(
                fpu.mul(fpu.div(static_cast<double>(jo::as_zone_def(pos)->values[2]), global<float>(d::flt_16384)), global<float>(d::flt_200)));
            const float ty = static_cast<float>(
                fpu.mul(fpu.div(static_cast<double>(jo::as_zone_def(pos)->values[3]), global<float>(d::flt_16384)), global<float>(d::flt_200)));
            View view{};
            joan::orig::as_camera(jo::view<void*>(jo::self<jo::RewardScreen>(this)->reward_camera))->frame(&view);  // 0x006140a0
            ease(view.f[1], tx);
            ease(view.f[2], ty);
            joan::orig::as_camera(jo::view<void*>(jo::self<jo::RewardScreen>(this)->reward_camera))->set_frame(&view);  // 0x00614040
            return;
        }
        if (!highlighted) anim(jo::view<void*>(jo::self<jo::RewardScreen>(this)->reward_frame), 0, 0x40c00000u, 0, 0, 0);
        const Fpu fpu{true};
        const float cx = static_cast<float>(fpu.div(fpu.add(jo::self<jo::RewardScreen>(this)->map_min_x, jo::self<jo::RewardScreen>(this)->map_max_x), global<float>(d::flt_2)));
        const float cy = static_cast<float>(fpu.div(fpu.add(jo::self<jo::RewardScreen>(this)->map_min_y, jo::self<jo::RewardScreen>(this)->map_max_y), global<float>(d::flt_2)));
        View view{};
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::RewardScreen>(this)->reward_camera))->frame(&view);
        ease(view.f[1], cx);
        ease(view.f[2], cy);
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::RewardScreen>(this)->reward_camera))->set_frame(&view);
    }
};

JOAN_REPLACE("game/b108_07", "Panel::update_7343b0", 0x007343b0, &H::update_7343b0);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // ------------------------------------------------------------------ 0x00724670 / 0x007349b0
    static u32 button_reset(void* b) { joan::orig::as_widget(b)->scale_to_61f1b0(0, joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0u), 1, 0u); return 0; }  // 0x0061f1b0 (returns nothing; 0x007349b0's only caller discards)
    u32 reset_buttons_7349b0() {
        button_reset(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_frame));
        button_reset(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_up_button));
        return button_reset(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_down_button));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::reset_buttons_7349b0", 0x007349b0, &H::reset_buttons_7349b0);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b106_02.cpp (game/b106_02)
namespace {
namespace game_b106_02 {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

#if defined(__GNUC__) && !defined(__clang__)
// GCC inlines the callers and cannot prove the destination and source name different buffers of the same panel
// struct; the original (CRT strcat) never overlaps them.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wrestrict"
#endif
void crt_strcat(char* d, const char* s) { std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

JOAN_COUNTER(c_list, "Panel::build_list_734a30");

// ---------------------------------------------------------------------------------------
// 0x004cba40: 55 slots (1..0x37) of 0x30 bytes at this+8 (defaults at this+0xc), a key list
// of *this entries (0x28 bytes) at this+4.
using joan::orig::helpers::add32;

// ---------------------------------------------------------------------------------------
// 0x00734a30 helpers
struct Entry {  // the 0x1a-byte EndCon records of the Game at 0x00a58f90 (+0x30b = 0x00a5929b)
    static jo::EndCon& rec(i32 i) { return jo::as_game(obj(d::g_game_state))->end_conditions[i]; }
    static u8 type(i32 i) { return static_cast<u8>(rec(i).state); }
    static char* name(i32 i) { return rec(i).text; }
    static i16 w4(i32 i) { return rec(i).result; }
    static i16 w6(i32 i) { return rec(i).i16_b; }
    static i16 wa(i32 i) { return rec(i).used; }
};
u32 white() {
    u32 tmp = 0;
    u32* const c = joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(0xffu, 0xffu, 0xffu, 0xffu));  // 0x0042a260
    return *c;
}
void row_text(void* self, i32 k, const char* text) {
    const u32 colour = white();
    joan::orig::as_button(jo::as_camp_screen(self)->slot_buttons->buttons[k])->set_text(joan::orig::arg::str(text), obj(d::g_font_goudy12_bold_black), joan::orig::arg::i32(colour), 1, 2, 0, 2, 2);  // 0x00623ce0
}

struct H {
    std::uintptr_t build_list_734a30(i32 first) {  // 0x00734a30 (EAX = this / +0xcb, pointer-wide on x64)
        if (first < 0) return reinterpret_cast<std::uintptr_t>(this);
        c_list.hit();
        jo::self<jo::RewardScreen>(this)->reward_scroll = first;
        i32 k = 0;
        jo::self<jo::RewardScreen>(this)->u32_bb = -1;
        jo::self<jo::RewardScreen>(this)->u32_bf = -1;
        jo::self<jo::RewardScreen>(this)->u32_c3 = -1;
        i32 n0 = 0, n2 = 0, n3 = 0;
        // group 0
        for (i32 i = 0; i < global<i16>(d::g_a59299); ++i) {
            if (Entry::wa(i) == 0) continue;
            if (Entry::name(i) == nullptr) continue;
            if (Entry::type(i) != 0) continue;
            ++n0;
            if (jo::self<jo::RewardScreen>(this)->u32_bb == -1) {
                jo::self<jo::RewardScreen>(this)->u32_bb = 0;
                if (!(jo::self<jo::RewardScreen>(this)->reward_scroll > jo::self<jo::RewardScreen>(this)->u32_bb) && k < 10) {
                    jo::self<jo::CampScreen>(this)->reward_slot_kinds[k] = 1;
                    jo::view<u16>(jo::self<jo::CampScreen>(this)->reward_condition_ids[k]) = 0xffff;
                    ++k;
                }
            }
            if (!(jo::self<jo::RewardScreen>(this)->reward_scroll > add32(jo::self<jo::RewardScreen>(this)->u32_bb, n0)) && k < 10) {
                row_text(this, k, Entry::name(i));
                jo::self<jo::CampScreen>(this)->reward_slot_kinds[k] = Entry::w4(i) == 1 ? 5 : 4;
                jo::view<u16>(jo::self<jo::CampScreen>(this)->reward_condition_ids[k]) = static_cast<u16>(i);
                ++k;
            }
        }
        // group 2 (with a time / text suffix)
        for (i32 i = 0; i < global<i16>(d::g_a59299); ++i) {
            if (Entry::wa(i) == 0) continue;
            if (Entry::name(i) == nullptr) continue;
            if (Entry::type(i) != 2) continue;
            ++n2;
            if (jo::self<jo::RewardScreen>(this)->u32_bf == -1) {
                jo::self<jo::RewardScreen>(this)->u32_bf = n0 + 1;
                if (!(jo::self<jo::RewardScreen>(this)->reward_scroll > jo::self<jo::RewardScreen>(this)->u32_bf) && k < 10) {
                    jo::self<jo::CampScreen>(this)->reward_slot_kinds[k] = 2;
                    jo::view<u16>(jo::self<jo::CampScreen>(this)->reward_condition_ids[k]) = 0xffff;
                    ++k;
                }
            }
            if (!(jo::self<jo::RewardScreen>(this)->reward_scroll > add32(jo::self<jo::RewardScreen>(this)->u32_bf, n2)) && k < 10) {
                // One array for ebp-0x198 .. ebp-0x18 so that a sprintf result longer than its
                // 0x20-byte buffer spills into the next buffers / text exactly like the original.
                char frame[0x180];
                char* const text = frame + 0x80;  // ebp-0x118, 0x100 bytes
                std::strcpy(static_cast<char*>(joan::orig::arg::mptr(text)), joan::orig::arg::str(Entry::name(i)));  // strcpy
                if (Entry::w4(i) == 0) {
                    if (Entry::w6(i) == 1) {
                        const u32 secs = global<u32>(d::g_get_a59171) / 1000u;
                        const u32 mins = (global<u32>(d::g_get_a591cc) - secs) / 60u;
                        const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(d::g_strings))->open_5(joan::orig::arg::u32(mins)));  // 0x004dcba0
                        char* const t = frame + 0x60;  // ebp-0x138
                        joan::orig::libc::sprintf(joan::orig::arg::str(t), joan::orig::arg::str(s));  // sprintf
                        crt_strcat(text, t);
                    } else if (Entry::w6(i) == 2) {
                        const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(d::g_strings))->open_6(joan::orig::arg::u32(global<u32>(d::g_scenario_rescue_count)), joan::orig::arg::u32(global<u32>(d::g_scenario_rescue_limit))));  // 0x004dcbe0
                        char* const t = frame + 0x40;  // ebp-0x158
                        joan::orig::libc::sprintf(joan::orig::arg::str(t), joan::orig::arg::str(s));
                        crt_strcat(text, t);
                    }
                } else if (Entry::w4(i) == 2) {
                    if (Entry::w6(i) == 1) {
                        const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(d::g_strings))->open_7());  // 0x004dcc20
                        char* const t = frame + 0x20;  // ebp-0x178
                        joan::orig::libc::sprintf(joan::orig::arg::str(t), joan::orig::arg::str(s));
                        crt_strcat(text, t);
                    } else if (Entry::w6(i) == 2) {
                        const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(d::g_strings))->open_6(joan::orig::arg::u32(global<u32>(d::g_scenario_rescue_count)), joan::orig::arg::u32(global<u32>(d::g_scenario_rescue_limit))));  // 0x004dcbe0
                        char* const t = frame;  // ebp-0x198
                        joan::orig::libc::sprintf(joan::orig::arg::str(t), joan::orig::arg::str(s));
                        crt_strcat(text, t);
                    }
                }
                row_text(this, k, text);
                jo::self<jo::CampScreen>(this)->reward_slot_kinds[k] = Entry::w4(i) == 1 ? 5 : (Entry::w4(i) == 2 ? 6 : 4);
                jo::view<u16>(jo::self<jo::CampScreen>(this)->reward_condition_ids[k]) = static_cast<u16>(i);
                ++k;
            }
        }
        // group 3
        for (i32 i = 0; i < global<i16>(d::g_a59299); ++i) {
            if (Entry::wa(i) == 0) continue;
            if (Entry::name(i) == nullptr) continue;
            if (Entry::type(i) != 3) continue;
            ++n3;
            if (jo::self<jo::RewardScreen>(this)->u32_c3 == -1) {
                jo::self<jo::RewardScreen>(this)->u32_c3 = n0 + 1;
                if (n2 > 0) jo::self<jo::RewardScreen>(this)->u32_c3 = add32(add32(jo::self<jo::RewardScreen>(this)->u32_c3, n2), 1);
                if (!(jo::self<jo::RewardScreen>(this)->reward_scroll > jo::self<jo::RewardScreen>(this)->u32_c3) && k < 10) {
                    jo::self<jo::CampScreen>(this)->reward_slot_kinds[k] = 3;
                    jo::view<u16>(jo::self<jo::CampScreen>(this)->reward_condition_ids[k]) = 0xffff;
                    ++k;
                }
            }
            if (!(jo::self<jo::RewardScreen>(this)->reward_scroll > add32(jo::self<jo::RewardScreen>(this)->u32_c3, n3)) && k < 10) {
                row_text(this, k, Entry::name(i));
                jo::self<jo::CampScreen>(this)->reward_slot_kinds[k] = Entry::w4(i) == 1 ? 5 : (Entry::w4(i) == 2 ? 6 : 4);
                jo::view<u16>(jo::self<jo::CampScreen>(this)->reward_condition_ids[k]) = static_cast<u16>(i);
                ++k;
            }
        }
        jo::view<i32>(jo::self<jo::RewardScreen>(this)->reward_count) = n0 + n2 + n3;
        if (n0 != 0) jo::view<i32>(jo::self<jo::RewardScreen>(this)->reward_count) = jo::view<i32>(jo::self<jo::RewardScreen>(this)->reward_count) + 1;
        if (n2 != 0) jo::view<i32>(jo::self<jo::RewardScreen>(this)->reward_count) = jo::view<i32>(jo::self<jo::RewardScreen>(this)->reward_count) + 1;
        if (n3 != 0) jo::view<i32>(jo::self<jo::RewardScreen>(this)->reward_count) = jo::view<i32>(jo::self<jo::RewardScreen>(this)->reward_count) + 1;
        return jo::self<jo::RewardScreen>(this)->reward_count;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_02", name, addr, fn)
B("Panel::build_list_734a30", 0x00734a30, &H::build_list_734a30);
#undef B

}  // namespace game_b106_02
}  // namespace

// ---------------------------------------- from modules/game_b107_15.cpp (game/b107_15)
namespace {
namespace game_b107_15 {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
JOAN_COUNTER(c_dialog_reset, "Dialog::reset_7353a0");

struct H {
    // ------------------------------------------------------------------------ 0x007353a0
    u32 reset_7353a0() {
        c_dialog_reset.hit();
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]))->scale_to(1, 6.0f, 0.0f, 0, 200u);  // 0x0061f1b0
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->side_bar))->scale_to(1, 6.0f, 0.0f, 0, 0u);    // 0x0061f1b0
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->side_bar))->move_to(0x19, 0xb8, 3500.0f, -1500.0f, 0, 0u);  // 0x0061e020
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_up_button))->scale_to(1, 4.0f, 0.0f, 0, 200u);  // 0x0061f1b0
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->reward_down_button))->scale_to(1, 4.0f, 0.0f, 0, 200u);  // 0x0061f1b0
        for (i32 i = 0; i < 10; ++i) {
            joan::orig::as_widget(jo::view<void**>(jo::self<jo::CampScreen>(this)->slot_buttons)[i])->scale_to(1, 4.0f, 0.0f, 0, 200u);  // 0x0061f1b0
            u32 colour_obj = 0;
            const u32 colour = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&colour_obj)->construct(0xffu, 0xffu, 0xffu, 0xffu));  // 0x0042a260
            joan::orig::as_button(jo::view<void**>(jo::self<jo::CampScreen>(this)->slot_buttons)[i])->set_text(img::str(d::kEmpty), img::obj(d::g_font_goudy12_bold_black), joan::orig::arg::i32(colour), joan::orig::arg::i32(// 0x00623ce0
                          1), 2, 0, 2, 2);
            jo::self<jo::CampScreen>(this)->reward_slot_kinds[i] = 0;
            jo::view<u16>(jo::self<jo::CampScreen>(this)->reward_condition_ids[i]) = 0xffff;
        }
        return joan::orig::arg::as<u32>(joan::orig::as_panel(this)->build_list_734a30(0));  // 0x00734a30
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_15", name, addr, fn)
B("Dialog::reset_7353a0", 0x007353a0, &H::reset_7353a0);
#undef B

}  // namespace game_b107_15
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

using u32 = std::uint32_t;

// 0x0061f1b0 (ret 0x14), raw dwords as pushed by the original.
using joan::orig::helpers::anim_set;
using joan::orig::helpers::anim_set6;  // 6.0f
// 0x0061e020 (ret 0x18)
void anim_move(void* o, std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t e,
               std::uint32_t f) {
    joan::orig::as_widget(o)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(f));
}

struct H {
    template <class A, class B, class C>
    void reset_common(A*& first, B*& o2, C*& o3) {
        anim_set6(first);
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]));
        anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->side_bar));
        anim_move(jo::view<void*>(jo::self<jo::CampScreen>(this)->side_bar), 0xfffffe9cu, 0xb8u, 0x455ac000u, 0xc4bb8000u, 0, 0);
        anim_set6(o2);
        anim_set6(o3);
        for (std::int32_t i = 0; i < 10; ++i) anim_set6(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]));
    }

    // 0x00735550
    void reset_735550() { reset_common(jo::self<jo::CampScreen>(this)->reward_frame, jo::self<jo::CampScreen>(this)->reward_up_button, jo::self<jo::CampScreen>(this)->reward_down_button); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Screen::reset_735550", 0x00735550, &H::reset_735550);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Map::create_widgets_732580() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_04::H::create_widgets_732580>(this)); }  // 0x732580
auto joan::orig::ObjMenu::free_reward() -> void { joan::bind::member<&::game_b100_16::H::release_732940>(this); }  // 0x732940
auto joan::orig::ObjMenu::init_reward_camera() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_16::H::set_camera_732af0>(this)); }  // 0x732af0
auto joan::orig::Screen::nop_732bd0() -> void { joan::bind::member<&::game_b103_16::H::nop>(this); }  // 0x732bd0
auto joan::orig::Screen::draw_black_732bf0(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_08::H::draw_black_732bf0>(this, a0)); }  // 0x732bf0
auto joan::orig::ObjMenu::draw_reward() -> void { joan::bind::member<&::game_b109_07::H::reward_draw>(this); }  // 0x732c70
auto joan::orig::List::scroll_734120() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_16::H::scroll_734120>(this)); }  // 0x734120
auto joan::orig::Panel::update_7343b0() -> void { joan::bind::member<&::game_b108_07::H::update_7343b0>(this); }  // 0x7343b0
auto joan::orig::ObjMenu::reset_reward_widgets() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::reset_buttons_7349b0>(this)); }  // 0x7349b0
auto joan::orig::Panel::build_list_734a30(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b106_02::H::build_list_734a30>(this, a0)); }  // 0x734a30
auto joan::orig::Dialog::reset_7353a0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_15::H::reset_7353a0>(this)); }  // 0x7353a0
auto joan::orig::Screen::reset_735550() -> void { joan::bind::member<&::game_b92_06::H::reset_735550>(this); }  // 0x735550
