// game/unknown_objm_shop.cpp: functions whose original source file is unknown (formerly game/_unplaced/objm_shop.cpp;
// re/attribution.csv: between ww/interfac/objmenub.cpp and ww/interfac/objmenuw.cpp). They move to their original file
// when evidence names it (Phase 3).
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
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;

using joan::hook::x87::Fpu;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using joan::call::cdecl_call;

// Colour(r, g, b, a) 0x0042a260 on a stack temporary, then its dword (mov eax,[eax]).
u32 make_colour(i32 r, i32 g, i32 b, i32 a) {
    u32 tmp = 0;
    const u32* const c = joan::orig::arg::as<const u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
    return *c;
}
// operator new (0x007b1d35) + default constructor through its thunk (0 when new fails).
void* new_obj(u32 size, std::uintptr_t ctor_thunk) {
    void* const mem = joan::crt::operator_new(joan::orig::arg::u32(size));
    return mem != nullptr ? thiscall<void*>(ctor_thunk, mem) : nullptr;
}

JOAN_COUNTER(c_7369c0, "ShopScreen::build_7369c0");

struct H {
    // 0x007369c0
    u32 shop_build() {
        c_7369c0.hit();
        jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_frame) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Image2D)), fn::code_value(fn::Widget629_ctor));  // 0x00629400
        const u32 c1 = make_colour(0, 0, 0, 0xff);
        joan::orig::as_label(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_frame))->create(0x186, 0x1e2, joan::orig::arg::mptr(obj(d::g_bmp_objmenu)), str(d::s_SCR_FH), joan::orig::arg::i32(c1), 3, 0, 2, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, 1, 1);                                   // 0x00629660 'SCR_FH'
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_frame))->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);  // 0x0061f1b0
        jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_up_button) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0));                     // 0x00620ee0
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_up_button))->setup(joan::orig::arg::i16(0xa9), joan::orig::arg::i16(0xc2), joan::orig::arg::mptr(obj(d::g_bmp_commele2)), str(d::s_aw_u_ot), str(d::s_aw_u_dw), str(d::s_aw_u_ov), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // 0x006211d0 aw_u_*
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_up_button))->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);
        jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_down_button) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0));
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_down_button))->setup(joan::orig::arg::i16(0xa9), joan::orig::arg::i16(0x26c), joan::orig::arg::mptr(obj(d::g_bmp_commele2)), str(d::s_aw_d_ot), str(d::s_aw_d_dw), str(d::s_aw_d_ov), joan::orig::arg::str(0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // aw_d_*
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_down_button))->scale_to(0, joan::orig::arg::f32(0x3f800000u), 0.0f, 1, 0u);
        jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_camera) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Camera)), fn::code_value(fn::Obj_ctor_613650));  // 0x00613650
        joan::orig::as_sys_en(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_camera))->init_613730();          // 0x00613730
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_camera))->set_viewport(static_cast<jo::Surface*>(obj(d::g_vga_back)), obj(d::g_a56ef0), 0x30u, 0u, 0x3ffu, 0x2ffu);  // 0x00613bf0
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_camera))->set_cf(0u, 1u, joan::orig::arg::u32(global<u32>(d::g_init_black)));                           // 0x00613d30
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_camera))->set_ranges(joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x45fd0800u), joan::orig::arg::f32(0x45fd0800u));                   // 0x00613ca0
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_camera))->set_scene(jo::self<jo::CampScreen>(this)->map_scene);                        // 0x00613db0
        return 0;  // 0x00613db0 returns nothing; the caller (objmenu.cpp) ignores the result
    }

    // 0x00737030 (argument unused)
    u32 shop_draw_bg(u32 unused) {
        static_cast<void>(unused);
        const u32 c = make_colour(0, 0, 0, 0xff);
        return joan::orig::arg::as<u32>(joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue(0, 0, 0x400, 0x300, 9u, joan::orig::arg::u32(c), joan::orig::arg::mptr(0), 3, 0, 0, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0));  // 0x00632a70
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("ShopScreen::build_7369c0", 0x007369c0, &H::shop_build);
B("ShopScreen::draw_bg_737030", 0x00737030, &H::shop_draw_bg);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {

// 0x0061f1b0 (5 args) / 0x0061e020 (6 args) on a button.
u32 button_anim(void* b, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    joan::orig::as_widget(b)->scale_to_61f1b0(joan::orig::arg::i16(a0), joan::orig::arg::f32(a1), joan::orig::arg::f32(a2), static_cast<char>(a3), joan::orig::arg::u32(a4));  // 0x0061f1b0
    return 0;  // 0x0061f1b0 returns nothing; no caller of reset_buttons_7388a0 reads the result
}
u32 button_reset(void* b) { return button_anim(b, 0u, 0x3f800000u, 0u, 1u, 0u); }

JOAN_COUNTER(c_release_736d80, "Obj::release_736d80");

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

    // ------------------------------------------------------------------ 0x00736d80
    void release_736d80() {
        c_release_736d80.hit();
        release(jo::self<jo::CampScreen>(this)->shop_frame, fn::code_value(fn::Image2d_release), fn::code_value(fn::Sprite_scalar_dtor));  // 0x00629830, 0x006bd440
        release(jo::self<jo::CampScreen>(this)->shop_up_button, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));  // 0x006217e0, 0x006b33a0
        release(jo::self<jo::CampScreen>(this)->shop_down_button, fn::code_value(fn::MenuButton_clear), fn::code_value(fn::ComboBox_scalar_dtor_6b33a0));
        release(jo::self<jo::CampScreen>(this)->shop_camera, 0, fn::code_value(fn::Obj562910_scalar_deleting_dtor));          // 0x00562910
    }

    // ------------------------------------------------------------------ 0x007388a0
    u32 reset_buttons_7388a0() {
        button_reset(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_frame));
        button_reset(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_up_button));
        return button_reset(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_down_button));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Obj::release_736d80", 0x00736d80, &H::release_736d80);
B("Obj::reset_buttons_7388a0", 0x007388a0, &H::reset_buttons_7388a0);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

struct CamParams {
    u32 v[10];
};

static_assert(sizeof(CamParams) == 0x28, "CamParams");

JOAN_COUNTER(c_set_camera, "Obj::set_camera_736f30");

struct H {
    // ------------------------------------------------------------------ 0x00736f30
    u32 set_camera_736f30() {
        c_set_camera.hit();
        CamParams p;
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_camera))->frame(&p);  // 0x006140a0
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
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_camera))->set_frame(joan::orig::arg::mptr(static_cast<void*>(&p)));  // 0x00614040
        return 0;  // 0x00614040 returns nothing; the caller (objmenu.cpp) ignores the result
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::set_camera_736f30", 0x00736f30, &H::set_camera_736f30);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Screen::nop_737010", 0x00737010, &H::nop);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b109_20.cpp (game/b109_20)
namespace {
namespace game_b109_20 {

using joan::hook::x87::ftol;

using joan::orig::helpers::at;

i32 isub(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(b)); }
float zero_c() { return global<float>(d::k_9cf5c4); }  // 0.0f

JOAN_COUNTER(c_panel_draw, "MapPanel::draw_7370b0");

using joan::orig::helpers::strcpy_;  // strcpy

// 0x0042a260 colour ctor (r, g, b, a bytes) on a 4-byte local; the dword is passed on.
u32 color4(i32 a) {
    u8 c[4];
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(c)->construct(i32{0xff}, i32{0xff}, i32{0xff}, a));  // 0x0042a260
}
// 0x00633500 sprite draw on 0xa46128 (17 args, ret 0x44).
void sprite(i32 x, i32 y, u32 a3, const char* name, img::Addr font, u32 color, i32 a9) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a3), joan::orig::arg::str(name), img::obj(font), joan::orig::arg::u32(color), 3, 0, joan::orig::arg::i16(a9), 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// 0x00593980 point-in-rect style test on 0xa516b0 (ret 0x10).
i32 hover(i32 x, i32 y, i32 w, i32 h) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(w), joan::orig::arg::i32(h)));  // 0x00593980
}

// Owner classification shared by the two loops of 0x007370b0 (0 French, 1 English/mercenary,
// 2 other).
i8 owner_side(i16 id, u8** unit_out) {
    u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_territory_array))->get_420ff0(static_cast<i32>(id)));  // 0x00420ff0
    const u32 fr = joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(str(d::s_FRENCH))))));  // FRENCH
    const u32 en = joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(str(d::s_ENGLISH))))));  // ENGLISH
    const u32 me = joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(str(d::s_MERCEN))))));  // MERCEN
    *unit_out = u;
    if (jo::view<u32>(jo::as_zone_def(u)->owner) == fr) return 0;
    if (jo::view<u32>(jo::as_zone_def(u)->owner) == en || jo::view<u32>(jo::as_zone_def(u)->owner) == me) return 1;
    return 2;
}
// alpha = _ftol(255.0f * obj->+0x11)
i32 alpha_of(const void* o) {
    const float v = global<float>(d::flt_255) * jo::as_widget(o)->scale;
    return ftol(static_cast<double>(v));
}

struct H {
    // ---------------------------------------------------------------------------- 0x007370b0
    void row_sprite(i32 i, const char* name) {
        const u32 color = color4(alpha_of(jo::view<void**>(jo::self<jo::CampScreen>(this)->slot_buttons)[i]));
        sprite(0x1f, i * 0x28 + 0xd9, 2, name, d::g_bmp_objmenu, color, 2);
    }
    void row_icon(i32 i, i8 side, img::Addr fr, img::Addr en, img::Addr other) {
        if (side == 0) {
            row_sprite(i, str(fr));
        } else if (side == 1) {
            row_sprite(i, str(en));
        } else {
            row_sprite(i, str(other));
        }
        row_sprite(i, str(d::s_box_bg));  // "box_bg"
    }
    void hover_sprite(i32 x, i32 y, const char* name, i32 a9) {
        const u32 color = color4(alpha_of(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_frame)));
        sprite(x, y, 2, name, d::g_bmp_objmenu, color, a9);
    }
    void panel_draw() {
        c_panel_draw.hit();
        if (jo::self<jo::CampScreen>(this)->shop_up_button->scale > zero_c()) {
            if (jo::self<jo::CampScreen>(this)->shop_scroll > 0) joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_up_button))->draw(2u, 7);  // 0x00622cc0
            if (jo::self<jo::CampScreen>(this)->shop_scroll < isub(jo::view<i32>(jo::self<jo::CampScreen>(this)->shop_count), 0xa)) {
                joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_down_button))->draw(2u, 7);  // 0x00622cc0
            }
            for (i32 i = 0; i < 10; ++i) {
                joan::orig::as_button(jo::view<void**>(jo::self<jo::CampScreen>(this)->slot_buttons)[i])->draw(2u, 7);  // 0x00622cc0
                i8 side = 0;
                if (jo::self<jo::CampScreen>(this)->shop_territory_ids[i] > 0) {
                    u8* u = nullptr;
                    side = owner_side(jo::self<jo::CampScreen>(this)->shop_territory_ids[i], &u);
                }
                const u32 type = static_cast<u32>(static_cast<i32>(jo::view<i8>(jo::self<jo::CampScreen>(this)->shop_slot_kinds[i])) - 1);
                switch (type) {
                    case 0:  // camp
                        row_icon(i, side, d::s_f_camp, d::s_e_camp, d::s_i_camp);
                        break;
                    case 1:  // town
                        row_icon(i, side, d::s_f_town, d::s_e_town, d::s_i_town);
                        break;
                    case 2:
                    case 3:  // castle
                        row_icon(i, side, d::s_f_castle, d::s_e_castle, d::s_i_castle);
                        break;
                    case 4:
                    case 5:  // other
                        row_sprite(i, str(d::s_i_other));  // "i_other"
                        row_sprite(i, str(d::s_box_bg));  // "box_bg"
                        break;
                    default:
                        break;
                }
            }
        }
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->visible) != 3) return;
        char buf[0x100];
        strcpy_(buf, str(d::kEmpty));  // ""
        for (i32 j = 0; j < 10; ++j) {
            if (jo::view<i8>(jo::self<jo::CampScreen>(this)->shop_slot_kinds[j]) < 1) continue;
            const u8* const v = jo::view<u8**>(jo::self<jo::CampScreen>(this)->slot_buttons)[j];
            if (hover(jo::as_widget(v)->x, jo::as_widget(v)->y, jo::as_widget(v)->x2, jo::as_widget(v)->y2) == 0) continue;
            if (!(jo::self<jo::CampScreen>(this)->shop_territory_ids[j] > 0)) continue;
            u8* u = nullptr;
            const i8 side = owner_side(jo::self<jo::CampScreen>(this)->shop_territory_ids[j], &u);
            if (side == 0) {
                hover_sprite(0x1cc, 0x134, str(d::s_fc_fre), 2);  // "fc_fre"
            } else if (side == 1) {
                hover_sprite(0x1cc, 0x134, str(d::s_fc_eng), 2);  // "fc_eng"
            } else {
                hover_sprite(0x1cc, 0x134, str(d::s_fc_neu), 2);  // "fc_neu"
            }
            strcpy_(buf, jo::as_zone_def(u)->script);  // the territory records are ZoneDef (+0x3c script)
        }
        if (!(jo::self<jo::CampScreen>(this)->shop_frame->scale > zero_c())) return;
        {
            const u32 color = color4(alpha_of(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_frame)));
            sprite(jo::self<jo::CampScreen>(this)->shop_frame->x, jo::self<jo::CampScreen>(this)->shop_frame->y, 2, str(d::s_SCR_FHBG), d::g_bmp_objmenu,
                   color, 2);  // "SCR_FHBG"
        }
        if (buf[0] != 0) hover_sprite(0x18f, 0x1eb, buf, 1);
        joan::orig::as_label(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_frame))->draw(joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{7}));  // 0x00629bd0
    }
};

#define B109(name, addr, fn) JOAN_REPLACE("game/b109_20", name, addr, fn)
B109("MapPanel::draw_7370b0", 0x007370b0, &H::panel_draw);
#undef B109

}  // namespace game_b109_20
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

JOAN_COUNTER(c_scroller, "Scroller::handle_buttons");

struct H {
    // ------------------------------------------------------------------ 0x007380b0
    void scroller_reset(i32 k) {
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[k]))->scale_to(1, 4.0f, 0.0f, 0, 200u);  // 0x0061f1b0
        u32 colour;
        const u32* const c = joan::orig::arg::as<u32*>(joan::orig::as_colour(static_cast<void*>(&colour))->construct(0xff, 0xff, 0xff, 0xff));  // 0x0042a260
        const u32 value = *c;
        joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[k]))->set_text(str(d::kEmpty), obj(d::g_font_goudy12_bold_black), joan::orig::arg::i32(value), 1, 2, 0, 2, 2);  // 0x00623ce0
        jo::self<jo::CampScreen>(this)->shop_slot_kinds[k] = 0;
    }
    std::uintptr_t scroller_handle_buttons() {
        c_scroller.hit();
        std::uintptr_t eax = joan::orig::arg::as<u32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_up_button))->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0));  // 0x006218c0
        if (eax != 0) {
            if (jo::self<jo::CampScreen>(this)->shop_scroll > 0) {
                for (i32 k = 0; k < 10; ++k) scroller_reset(k);
                joan::orig::as_screen(this)->fill_list(joan::orig::arg::i32(jo::self<jo::CampScreen>(this)->shop_scroll - 1));  // 0x00738920 (returns nothing; this function's result is unused)
            }
            return (eax & ~std::uintptr_t{0xffu}) | 1u;
        }
        eax = joan::orig::arg::as<u32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_down_button))->mouse(0, 0, joan::orig::arg::i32(-1), 0, 0, 0));  // 0x006218c0
        if (eax != 0) {
            eax = reinterpret_cast<std::uintptr_t>(this);
            if (jo::self<jo::CampScreen>(this)->shop_scroll < static_cast<i32>(jo::self<jo::CampScreen>(this)->shop_count - 10u)) {
                for (i32 k = 0; k < 10; ++k) scroller_reset(k);
                joan::orig::as_screen(this)->fill_list(joan::orig::arg::i32(jo::self<jo::CampScreen>(this)->shop_scroll + 1));  // 0x00738920 (returns nothing; this function's result is unused)
            }
            return (eax & ~std::uintptr_t{0xffu}) | 1u;
        }
        eax = joan::orig::arg::as<u32>(joan::orig::as_ui_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]))->mouse(0x1b, 0, joan::orig::arg::i32(-1), 0, 0, 0));  // 0x006218c0
        if (eax != 0) {
            eax = joan::orig::arg::as<u32>(joan::orig::as_camp_screen(this)->set_state_721a60(0u));  // 0x00721a60
            return (eax & ~std::uintptr_t{0xffu}) | 1u;
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Scroller::handle_buttons", 0x007380b0, &H::scroller_handle_buttons);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b108_10.cpp (game/b108_10)
namespace {
namespace game_b108_10 {

template <class T>
T& at(const void* p, i32 offset) {
    return *reinterpret_cast<T*>(static_cast<u8*>(const_cast<void*>(p)) + offset);
}

JOAN_COUNTER(c_mapview_update, "MapView::update_738340");

// ---- MapView camera pan ----
void approach(float& cur, const float* target) {
    using joan::hook::x87::ftol;
    u32 tbits;
    std::memcpy(&tbits, target, 4);
    const float tgt = *target;
    if (ftol(cur) < ftol(tgt)) {
        if (ftol(cur) < static_cast<i32>(static_cast<u32>(ftol(tgt)) - 15u)) cur = cur + 15.0f;
        else jo::view<u32>(cur) = tbits;
    } else if (ftol(cur) > ftol(tgt)) {
        if (ftol(cur) > static_cast<i32>(static_cast<u32>(ftol(tgt)) + 15u)) cur = cur - 15.0f;
        else jo::view<u32>(cur) = tbits;
    }
}

struct H {
    // ------------------------------------------------------------------ 0x00738340
    void mapview_update() {
        c_mapview_update.hit();
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->shop_frame)->update();  // 0x0061f3b0
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->shop_up_button)->update();
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->shop_down_button)->update();
        if (jo::view<i8>(jo::self<jo::CampScreen>(this)->visible) != 3) return;
        alignas(4) u8 cam[40];
        for (i32 i = 0; i < 10; ++i) {
            if (jo::view<i8>(jo::self<jo::CampScreen>(this)->shop_slot_kinds[i]) < 1) continue;
            void* const r = jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]);
            if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(jo::as_widget(r)->x), static_cast<i32>(jo::as_widget(r)->y), static_cast<i32>(jo::as_widget(r)->x2), static_cast<i32>(jo::as_widget(r)->y2))) == 0)  // 0x00593980
                continue;
            if (jo::self<jo::CampScreen>(this)->shop_territory_ids[i] <= 0) continue;
            void* const e = joan::orig::arg::as<void*>(joan::orig::as_dynarray(obj(d::g_territory_array))->get_420ff0(static_cast<i32>(jo::self<jo::CampScreen>(this)->shop_territory_ids[i])));  // 0x00420ff0
            const Fpu fpu{true};
            float t[2];
            t[0] = static_cast<float>(fpu.mul(fpu.div(static_cast<double>(jo::as_zone_def(e)->values[2]), 16384.0), 200.0));
            t[1] = static_cast<float>(fpu.mul(fpu.div(static_cast<double>(jo::as_zone_def(e)->values[3]), 16384.0), 200.0));
            joan::orig::as_camera(jo::self<jo::CampScreen>(this)->shop_camera)->frame(joan::orig::arg::mptr(cam));  // 0x006140a0
            approach(jo::as_cam_params(cam)->position[0], &t[0]);
            approach(jo::as_cam_params(cam)->position[1], &t[1]);
            joan::orig::as_camera(jo::self<jo::CampScreen>(this)->shop_camera)->set_frame(joan::orig::arg::mptr(cam));  // 0x00614040
            const float z = jo::self<jo::CampScreen>(this)->shop_frame->scale_target;
            if (z < 1.0f || z > 1.0f)
                joan::orig::as_widget(jo::self<jo::CampScreen>(this)->shop_frame)->scale_to(1, joan::orig::arg::f32(u32{0x40c00000}), 0.0f, 0, 0u);  // 0x0061f1b0
            return;
        }
        const float z = jo::self<jo::CampScreen>(this)->shop_frame->scale_target;
        if (z < 0.0f || z > 0.0f)
            joan::orig::as_widget(jo::self<jo::CampScreen>(this)->shop_frame)->scale_to(0, joan::orig::arg::f32(u32{0x40c00000}), 0.0f, 0, 0u);  // 0x0061f1b0
        float t[2];
        t[0] = (jo::self<jo::CampScreen>(this)->map_min_x + jo::self<jo::CampScreen>(this)->map_max_x) / 2.0f;
        t[1] = (jo::self<jo::CampScreen>(this)->map_min_y + jo::self<jo::CampScreen>(this)->map_max_y) / 2.0f;
        joan::orig::as_camera(jo::self<jo::CampScreen>(this)->shop_camera)->frame(joan::orig::arg::mptr(cam));  // 0x006140a0
        approach(jo::as_cam_params(cam)->position[0], &t[0]);
        approach(jo::as_cam_params(cam)->position[1], &t[1]);
        joan::orig::as_camera(jo::self<jo::CampScreen>(this)->shop_camera)->set_frame(joan::orig::arg::mptr(cam));  // 0x00614040
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_10", name, addr, fn)
B("MapView::update_738340", 0x00738340, &H::mapview_update);
#undef B

}  // namespace game_b108_10
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

using joan::orig::helpers::at;

// 0x0061f1b0 (ret 0x14), raw dwords as pushed by the original.
using joan::orig::helpers::anim_set;
using joan::orig::helpers::anim_set6;  // 6.0f
// 0x0061e020 (ret 0x18)
void anim_move(void* o, std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d, std::uint32_t e,
               std::uint32_t f) {
    joan::orig::as_widget(o)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(f));
}
// 0x0042a260 Colour(r, g, b, a) (ret 0x10), returns the 4-byte object.
std::uint32_t colour(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    alignas(4) std::uint8_t c[4];
    std::uint8_t* const p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<std::uint32_t>(*jo::as_colour(p));
}

std::uint32_t u(std::int32_t v) { return static_cast<std::uint32_t>(v); }
using joan::orig::helpers::s;

JOAN_COUNTER(c_fill, "Screen::fill_list");

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

    // 0x00738d00
    void reset_738d00() { reset_common(jo::self<jo::CampScreen>(this)->shop_frame, jo::self<jo::CampScreen>(this)->shop_up_button, jo::self<jo::CampScreen>(this)->shop_down_button); }

    // 0x00738920
    void fill_list(std::int32_t first) {
        c_fill.hit();
        if (first < 0) return;
        jo::self<jo::CampScreen>(this)->shop_scroll = first;
        jo::view<std::int32_t>(jo::self<jo::CampScreen>(this)->shop_count) = 0;
        std::int32_t k = 0;
        void* const list = obj(d::g_territory_array);
        for (std::int32_t i = 1; i <= joan::orig::as_dynarray(list)->size(); ++i) {  // 0x00417ad0
            if (joan::orig::arg::as<std::int32_t>(joan::orig::as_dynarray(list)->is_empty_at(joan::orig::arg::i32(i))) != 0) continue;            // 0x004217e0
            std::uint8_t* const p = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(list)->get_420ff0(joan::orig::arg::i32(i)));      // 0x00420ff0
            jo::view<std::int32_t>(jo::self<jo::CampScreen>(this)->shop_count) = s(u(jo::view<std::int32_t>(jo::self<jo::CampScreen>(this)->shop_count)) + 1u);
            if (!(jo::self<jo::CampScreen>(this)->shop_scroll < jo::view<std::int32_t>(jo::self<jo::CampScreen>(this)->shop_count)) || k >= 10) continue;
            const std::uint32_t c = colour(0xff, 0xff, 0xff, 0xff);
            joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[k]))->set_text(joan::orig::arg::str(static_cast<void*>(jo::as_zone_def(p)->name)), obj(d::g_font_goudy12_bold_black), joan::orig::arg::i32(c), 1, 2, 0, 2, 2);  // 0x00623ce0, ret 0x20
            const std::int32_t kind = jo::as_zone_def(p)->kind;
            if (u(kind) <= 5u) jo::self<jo::CampScreen>(this)->shop_slot_kinds[k] = static_cast<std::uint8_t>(kind + 1);
            jo::self<jo::CampScreen>(this)->shop_territory_ids[k] = static_cast<std::int16_t>(i);
            ++k;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Screen::fill_list", 0x00738920, &H::fill_list);
B("Screen::reset_738d00", 0x00738d00, &H::reset_738d00);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

using u16 = std::uint16_t;

constexpr u32 kFour = 0x40800000u;   // 4.0f
constexpr u32 kSix = 0x40c00000u;    // 6.0f
constexpr u32 kHi = 0x455ac000u;     // 3500.0f
constexpr u32 kLo = 0xc4bb8000u;     // -1500.0f

JOAN_COUNTER(c_panel2_layout, "Panel2::layout");

// 0x0042a260 Color::Color(b0, b1, b2, b3) on a 4-byte local; returns its address.
u32 make_color(i32 b0, i32 b1, i32 b2, i32 b3) {
    u32 c = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&c)->construct(b0, b1, b2, b3));
}
// 0x0061e020 Widget::place(x, y, a, b, c, d)
void place(void* w, i32 x, i32 y, u32 a, u32 b, i32 c, i32 d) {
    joan::orig::as_widget(w)->move_to(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::f32(a), joan::orig::arg::f32(b), static_cast<char>(c), joan::orig::arg::u32(d));
}
// 0x0061f1b0 Widget::set_anim(a, b, c, d, e)
void anim(void* w, i32 a, u32 b, i32 c, i32 d, i32 e) {
    joan::orig::as_widget(w)->scale_to_61f1b0(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
}

struct H {
    // ------------------------------------------------------------------ 0x00738b50
    i32 panel2_layout() {
        c_panel2_layout.hit();
        anim(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]), 1, kSix, 0, 0, 0xc8);
        anim(jo::view<void*>(jo::self<jo::CampScreen>(this)->side_bar), 1, kSix, 0, 0, 0);
        place(jo::view<void*>(jo::self<jo::CampScreen>(this)->side_bar), 0x19, 0xb8, kHi, kLo, 0, 0);
        anim(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_up_button), 1, kFour, 0, 0, 0xc8);
        anim(jo::view<void*>(jo::self<jo::CampScreen>(this)->shop_down_button), 1, kFour, 0, 0, 0xc8);
        for (i32 i = 0; i < 10; ++i) {
            anim(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]), 1, kFour, 0, 0, 0xc8);
            const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
            joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]))->set_text(joan::orig::arg::str(obj(d::kEmpty)), // 0x00623ce0
                          obj(d::g_font_goudy12_bold_black), joan::orig::arg::i32(color)
, 1, 2, 0, 2, 2);
            jo::self<jo::CampScreen>(this)->shop_slot_kinds[i] = 0;
            jo::view<u16>(jo::self<jo::CampScreen>(this)->shop_territory_ids[i]) = 0;
        }
        joan::orig::as_screen(this)->fill_list(0);  // 0x00738920
        return 0;  // 0x00738920 returns nothing; the result reaches only callers that ignore it
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Panel2::layout", 0x00738b50, &H::panel2_layout);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ObjMenu::build_shop() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_09::H::shop_build>(this)); }  // 0x7369c0
auto joan::orig::ObjMenu::free_shop() -> void { joan::bind::member<&::game_b100_16::H::release_736d80>(this); }  // 0x736d80
auto joan::orig::ObjMenu::init_shop_camera() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::set_camera_736f30>(this)); }  // 0x736f30
auto joan::orig::Screen::nop_737010() -> void { joan::bind::member<&::game_b103_16::H::nop>(this); }  // 0x737010
auto joan::orig::ObjMenu::draw_shop_bg(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_09::H::shop_draw_bg>(this, a0)); }  // 0x737030
auto joan::orig::ObjMenu::draw_shop() -> void { joan::bind::member<&::game_b109_20::H::panel_draw>(this); }  // 0x7370b0
auto joan::orig::ObjMenu::update_shop() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_18::H::scroller_handle_buttons>(this)); }  // 0x7380b0
auto joan::orig::ObjMenu::update_shop_widgets() -> void { joan::bind::member<&::game_b108_10::H::mapview_update>(this); }  // 0x738340
auto joan::orig::ObjMenu::reset_shop_widgets() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b100_16::H::reset_buttons_7388a0>(this)); }  // 0x7388a0
auto joan::orig::Screen::fill_list(std::int32_t a0) -> void { joan::bind::member<&::game_b92_06::H::fill_list>(this, a0); }  // 0x738920
auto joan::orig::ObjMenu::enter_shop() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_08::H::panel2_layout>(this)); }  // 0x738b50
auto joan::orig::Screen::reset_738d00() -> void { joan::bind::member<&::game_b92_06::H::reset_738d00>(this); }  // 0x738d00
