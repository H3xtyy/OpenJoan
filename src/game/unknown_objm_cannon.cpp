// game/unknown_objm_cannon.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/objm_cannon.cpp; re/attribution.csv: between ww/interfac/objmenub.cpp and ww/interfac/objmenuw.cpp).
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

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;


}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

using joan::hook::global;

using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;

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

JOAN_COUNTER(c_738e30, "CannonScreen::build_738e30");

struct H {
    // 0x00738e30
    std::uintptr_t cannon_build() {  // EAX = this (pointer-wide on x64)
        c_738e30.hit();
        jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Scene)), fn::code_value(fn::Scene_Scene));              // 0x005e9a00
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene))->reset();                    // 0x005e9ac0
        u8* const mat = joan::orig::arg::as<u8*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene))->add_light());  // 0x005eb5d0
        joan::orig::as_camera(mat)->init(0u);                                    // 0x005c58a0
        jo::view<u32>(jo::as_light(mat)->f32_30) = 0x3f800000u;
        jo::view<u32>(jo::as_light(mat)->colour) = make_colour(0xcf, 0xcf, 0xcf, 0xcf);
        joan::orig::as_light(mat)->invalidate();                                       // 0x005c5c00
        jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Camera)), fn::code_value(fn::Obj_ctor_613650));             // 0x00613650
        joan::orig::as_sys_en(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera))->init_613730();                    // 0x00613730
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera))->set_viewport(static_cast<jo::Surface*>(obj(d::g_vga_back)), obj(d::g_a56ef0), 0x68u, 0xc5u, 0x1bbu, 0x22cu);  // 0x00613bf0
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera))->set_cf(0u, 1u, joan::orig::arg::u32(global<u32>(d::g_init_black)));                              // 0x00613d30
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera))->set_ranges(joan::orig::arg::f32(0x3f800000u), joan::orig::arg::f32(0x46000400u), joan::orig::arg::f32(0x46000400u));                      // 0x00613ca0
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera))->set_scene(jo::self<jo::CampScreen>(this)->siege_scene);                                       // 0x00613db0
        jo::self<jo::CampScreen>(this)->siege_model_id = 0;
        jo::self<jo::CampScreen>(this)->siege_model_flags = 0;
        jo::self<jo::CampScreen>(this)->siege_model = nullptr;
        jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_frame) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Image2D)), fn::code_value(fn::Widget629_ctor));  // 0x00629400
        const u32 c = make_colour(0, 0, 0, 0xff);
        joan::orig::as_label(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_frame))->create(0x1bc, 0xb4, joan::orig::arg::mptr(obj(d::g_bmp_debrief)), str(d::s_frame), joan::orig::arg::i32(c), 3, 0, 2, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, 1, 1);                                            // 0x00629660 'frame'
        u8* const t18 = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(obj(d::g_bmp_objmenu))->find(str(d::s_cann)));  // 0x00620b40 'cann'
        for (i32 i = 0; i < 6; ++i) {
            jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_buttons[i]) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0));  // 0x00620ee0
            const i32 tx = global<i32>(d::k_9df5a4 + static_cast<std::uintptr_t>(i) * 4);
            joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_buttons[i]))->create(joan::orig::arg::i32(static_cast<i16>(tx)), 0x218, joan::orig::arg::i32(wsub(wadd(tx, static_cast<i16>(jo::as_bmp_entry(t18)->width)), 1)), joan::orig::arg::i32(wadd(static_cast<i16>(jo::as_bmp_entry(t18)->height), 0x217)), fn::code_value(fn::Button_draw_73b770), 1, 0, 0, 1, 1, 1);  // 0x00621540 (callback 0x0073b770)
            jo::self<jo::CampScreen>(this)->siege_flags[i] = 0;
        }
        jo::self<jo::CampScreen>(this)->siege_desc_text = 0;
        jo::self<jo::CampScreen>(this)->help_open = 0;
        return reinterpret_cast<std::uintptr_t>(this);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("CannonScreen::build_738e30", 0x00738e30, &H::cannon_build);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

JOAN_COUNTER(c_free_scene, "Obj::free_scene");

struct H {
    // ------------------------------------------------------------------ 0x00739290
    void free_scene() {
        c_free_scene.hit();
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model) != nullptr) {
            joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene))->remove_5ea2f0(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model)));  // 0x005ea2f0
            if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model) != nullptr) joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_model))->destroy(1u);  // 0x00420840
            jo::self<jo::CampScreen>(this)->siege_model = nullptr;
            jo::self<jo::CampScreen>(this)->siege_actor_def = 0;
            if (jo::view<i32>(jo::self<jo::CampScreen>(this)->siege_model_id) > 0) joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(static_cast<i16>(jo::self<jo::CampScreen>(this)->siege_model_id)));  // 0x004b16a0
            jo::self<jo::CampScreen>(this)->siege_model_id = 0;
            jo::self<jo::CampScreen>(this)->siege_model_flags = 0;
        }
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera) != nullptr) {
            joan::orig::as_model(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera))->release_parts();  // 0x00613a10
            if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera) != nullptr) joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_camera))->destroy(1u);  // 0x00562910
            jo::self<jo::CampScreen>(this)->siege_camera = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene) != nullptr) {
            joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene))->clear();  // 0x005e9b10
            if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene) != nullptr) joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_scene))->destroy(1u);  // 0x004201f0
            jo::self<jo::CampScreen>(this)->siege_scene = nullptr;
        }
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_frame) != nullptr) {
            joan::orig::as_image2d(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_frame))->destroy(1u);  // 0x006bd440
            jo::self<jo::CampScreen>(this)->siege_frame = nullptr;
        }
        for (i32 i = 0; i < 6; ++i) {
            if (jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_buttons[i]) != nullptr) {
                joan::orig::as_combo_box(jo::view<void*>(jo::self<jo::CampScreen>(this)->siege_buttons[i]))->scalar_dtor_6b33a0(1u);  // 0x006b33a0
                jo::self<jo::CampScreen>(this)->siege_buttons[i] = nullptr;
            }
        }
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::free_scene", 0x00739290, &H::free_scene);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b109_12.cpp (game/b109_12)
namespace {
namespace game_b109_12 {

using i8 = std::int8_t;

constexpr u32 kOne = 0x3f800000;  // 1.0f bits

// 0x0042a260 (thunk 0x004098b8): 4-byte colour from r, g, b, a; returns its address.
u32 make_color(u32 r, u32 g, u32 b, i32 a) {
    u8 c[4] = {0, 0, 0, 0};
    u8* p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(r, g, b, a));
    return jo::view<u32>(*jo::as_colour(p));
}
// 0x00633500 (thunk 0x00401bbd) on 0xa46128, ret 0x44.
void draw_sprite(i32 x, i32 y, u32 surf, img::Addr name, img::Addr res, u32 color, i32 a7, i32 a8, i32 a9, i32 a10, i32 a11,
                 i32 a12, i32 a13, i32 a14, i32 a15, i32 a16, i32 a17) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(surf), joan::orig::arg::str(img::addr(name)), img::obj(res), joan::orig::arg::u32(color), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), joan::orig::arg::i16(a15), joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));
}
void strcpy_(void* d, const void* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }  // strcpy
void sprintf0(void* d, const char* fmt) { joan::orig::libc::sprintf(joan::orig::arg::str(d), fmt); }       // sprintf(d, fmt)

JOAN_COUNTER(c_p9, "Panel9::draw_739540");

struct H {
    // ---- 0x00739540 -------------------------------------------------------------------------
    void panel9(u8) {
        void* const self = this;
        if (jo::view<i8>(jo::as_camp_screen(self)->visible) != 9) return;
        c_p9.hit();
        if (jo::view<i32>(jo::as_camp_screen(self)->siege_selected) != -1) {
            joan::orig::as_label(jo::as_camp_screen(self)->siege_frame)->draw(joan::orig::arg::u32(i32{3}), joan::orig::arg::u32(i32{7}));  // 0x00629bd0
            u32 id = 0;  // [+0x1bb] is always 0..5 here (see header)
            static const img::Addr kNames[6] = {d::s_BATTERRAM, d::s_CROSSBOW, d::s_BOMBARD,
                                                     d::s_CANNON, d::s_FAUCONNEAU, d::s_TREBUCHET};
            const u32 wsel = jo::as_camp_screen(self)->siege_selected;
            if (wsel <= 5) id = joan::orig::arg::as<u32>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(img::addr(kNames[wsel]))));  // 0x005270b0
            u8* const card = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i16>(id))));  // 0x0042d0a0
            {
                const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
                draw_sprite(0x210, 0xa4, 3, d::s_fancybar, d::g_bmp_debrief, color, 3, 0, 0, 0, -1, -1, -1, -1, 0, -1,
                            -1);  // "fancybar"
            }
            char buf[260];
            strcpy_(buf, jo::as_unit_type(card)->title_b4);
            joan::orig::libc::_strupr(joan::orig::arg::str(buf));  // _strupr
            joan::orig::as_font(obj(d::g_font_goudy12_bold_black))->measure(buf, -1, 0, 0, joan::orig::arg::f32(kOne));  // 0x00624f00
            {
                const u32 color = make_color(0, 0, 0, 0xff);
                joan::orig::as_text(obj(d::g_font_goudy12_bold_black))->draw_3(i32{0x210}, i32{0xa4}, i32{0x330}, i32{0xcc}, joan::orig::arg::u32(i32{3}), buf, joan::orig::arg::u32(color), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626130
            }
            {
                const u32 color = make_color(0x15, 0x15, 0x15, 0xff);
                joan::orig::as_text_renderer(obj(d::g_font_goudy12_bold_black))->draw_wrapped(joan::orig::arg::u32(i32{0x1d0}), joan::orig::arg::u32(i32{0xcc}), joan::orig::arg::u32(i32{0x356}), joan::orig::arg::u32(i32{0x186}), joan::orig::arg::u32(i32{3}), joan::orig::arg::str(jo::as_camp_screen(self)->siege_desc_text), joan::orig::arg::u32(color), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{1}), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), joan::orig::arg::u32(i32{2}));  // 0x00626e00
            }
            {
                const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
                draw_sprite(0x1e8, 0x188, 3, d::s_goldbar, d::g_bmp_debrief, color, 3, 0, 0, 0, -1, -1, -1, -1, 0, -1,
                            -1);  // "goldbar"
            }
            {
                const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
                draw_sprite(0x1e8, 0x1b4, 3, d::s_goldbar, d::g_bmp_debrief, color, 3, 0, 0, 0, -1, -1, -1, -1, 0, -1,
                            -1);  // "goldbar"
            }
            auto label = [](std::uintptr_t getter, i32 x) {
                const u32 color = make_color(0x15, 0x15, 0x15, 0xff);
                const char* const text = thiscall<const char*>(getter, obj(d::g_strings));
                joan::orig::as_text(obj(d::g_font_goudy14_22_white))->draw_line(joan::orig::arg::i32(x), i32{0x18d}, joan::orig::arg::u32(i32{3}), text, joan::orig::arg::u32(color), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x006255d0
            };
            label(fn::code_value(fn::Menu_objmenu_40), 0x1e8);  // 0x004dc360
            label(fn::code_value(fn::ObjMenu_open_41), 0x270);  // 0x004dc390
            label(fn::code_value(fn::ObjMenu_open_42), 0x308);  // 0x004dc3c0
            i32 x = 0x1e4;
            const i32 y = 0x1a1;
            auto pip = [](i32 px, i32 py, img::Addr name) {
                const u32 color = make_color(0xff, 0xff, 0xff, 0xff);
                draw_sprite(static_cast<i16>(px), static_cast<i16>(py), 3, name, d::g_bmp_objmenu, color, 3, 0, 2, 0, -1,
                            -1, -1, -1, 0, -1, -1);
            };
            if (jo::as_unit_type(card)->item_stack > 0) {
                for (i32 i = 1; i <= static_cast<i32>(jo::as_unit_type(card)->dexterity); i += 0x14) {
                    pip(x, y, d::s_att_r);  // "att_r"
                    x += 0x12;
                    if (i == 0x51) break;
                }
            } else {
                for (i32 i = 1; i <= static_cast<i32>(jo::as_unit_type(card)->dexterity); i += 5) {
                    pip(x, y, d::s_att_r);  // "att_r"
                    x += 0x12;
                    if (i == 0x15) break;
                }
            }
            x = 0x270;
            for (i32 i = 1; i <= static_cast<i32>(jo::as_unit_type(card)->defense); i += 5) {
                pip(x, y, d::s_att_y);  // "att_y"
                x += 0x12;
                if (i == 0x15) break;
            }
            x = 0x2fe;
            // fild i; fcomp [card+0xa0]; test ah,0x41; je exit -> leave when i > value (NaN stays).
            for (i32 i = 1; !(static_cast<double>(i) > static_cast<double>(jo::as_unit_type(card)->speed_run)); ++i) {
                pip(x, y, d::s_att_b);  // "att_b"
                x += 0x12;
                if (i == 5) break;
            }
            x = 0x1e8;
            for (i32 i = 0; i < jo::as_unit_type(card)->slot_count; ++i) {
                pip(x, 0x1b0, d::s_linf);  // "linf"
                x += 0x2d;
            }
            if (jo::as_unit_type(card)->slot_count == 1)
                sprintf0(buf, joan::orig::as_obj_menu(obj(d::g_strings))->open_43());  // 0x004dc3f0
            else
                sprintf0(buf, joan::orig::as_txt_res(obj(d::g_strings))->objmenu_44(joan::orig::arg::u32(jo::view<u32>(jo::as_unit_type(card)->slot_count))));  // 0x004dc420
            {
                const u32 color = make_color(0x15, 0x15, 0x15, 0xff);
                joan::orig::as_text(obj(d::g_font_goudy12_bold_black))->draw_line(i32{0x1d1}, i32{0x1e2}, joan::orig::arg::u32(i32{3}), buf, joan::orig::arg::u32(color), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x006255d0
            }
            switch (jo::as_camp_screen(self)->siege_selected) {
                case 0: sprintf0(buf, joan::orig::as_menu(obj(d::g_strings))->objmenu_45()); break;  // 0x004dc460
                case 1: sprintf0(buf, str(d::kEmpty)); break;                                 // ""
                case 2: sprintf0(buf, joan::orig::as_obj_menu(obj(d::g_strings))->open_46()); break;  // 0x004dc490
                case 3: sprintf0(buf, joan::orig::as_menu(obj(d::g_strings))->objmenu_47()); break;  // 0x004dc4c0
                case 4: sprintf0(buf, joan::orig::as_obj_menu(obj(d::g_strings))->open_48()); break;  // 0x004dc4f0
                case 5: sprintf0(buf, joan::orig::as_obj_menu(obj(d::g_strings))->open_49()); break;  // 0x004dc520
                default: break;
            }
            {
                const u32 color = make_color(0x15, 0x15, 0x15, 0xff);
                joan::orig::as_text(obj(d::g_font_goudy12_bold_black))->draw_3(i32{0x1d1}, i32{0x1f0}, i32{0x370}, i32{0x1fe}, joan::orig::arg::u32(i32{3}), buf, joan::orig::arg::u32(color), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626130
            }
        }
        for (i32 i = 0; i < 6; ++i) {
            if (jo::view<i8>(jo::as_camp_screen(self)->siege_flags[i]) != 1) continue;
            joan::orig::as_button(jo::view<u8*>(jo::as_camp_screen(self)->siege_buttons[i]))->draw(3u, 7);  // 0x00622cc0
            if (jo::view<i32>(jo::as_camp_screen(self)->siege_selected) != i)
                jo::view<u32>(jo::as_widget(jo::view<u8*>(jo::as_camp_screen(self)->siege_buttons[i]))->scale) = 0x3ecccccd;  // 0.4f
            else
                jo::view<u32>(jo::as_widget(jo::view<u8*>(jo::as_camp_screen(self)->siege_buttons[i]))->scale) = kOne;
        }
        if (jo::view<i8>(jo::as_camp_screen(self)->help_open) != 0)
            joan::orig::as_popup(obj(d::g_popup_a52df8))->draw(i32{2}, i32{-1}, i32{-1});  // 0x006d3cb0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_12", name, addr, fn)
B("Panel9::draw_739540", 0x00739540, &H::panel9);
#undef B

}  // namespace game_b109_12
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ObjMenu::build_siege() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_09::H::cannon_build>(this)); }  // 0x738e30
auto joan::orig::ObjMenu::free_siege() -> void { joan::bind::member<&::game_b97_08::H::free_scene>(this); }  // 0x739290
auto joan::orig::ObjMenu::draw_siege(std::uint8_t a0) -> void { joan::bind::member<&::game_b109_12::H::panel9>(this, a0); }  // 0x739540
