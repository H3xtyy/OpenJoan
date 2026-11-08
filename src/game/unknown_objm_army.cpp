// game/unknown_objm_army.cpp: functions whose original source file is unknown (formerly game/_unplaced/objm_army.cpp;
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
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using u16 = std::uint16_t;
using i16 = std::int16_t;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b109_07.cpp (game/b109_07)
namespace {
namespace game_b109_07 {

JOAN_COUNTER(c_panel_init, "Panel724e00::init_724e00");

// 0x0042a260 colour (r, g, b, a) into a 4-byte local, dword read back.
u32 colour(u8 r, u8 g, u8 b, u8 a) {
    alignas(4) u8 slot[4];
    u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
    return jo::view<u32>(*jo::as_colour(p));
}
// 0x0061f1b0(0, 1.0f, 0, 1, 0)
void gadget_enable(void* g) { joan::orig::as_widget(g)->scale_to(0, 1.0f, 0.0f, 1, 0u); }
// 0x00629660 (image gadget, 19 args): x, y, style, name, colour, 3, 0, 2, 0, -1 x4, 0, -1, -1, 0, 1, 1.
void image_init(void* g, i32 x, i32 y, std::uintptr_t style, const char* name, u32 color) {
    joan::orig::as_label(g)->create(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::mptr(style), joan::orig::arg::str(name), joan::orig::arg::i32(color), 3, 0, 2, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0, 0xffffffff, 0xffffffff, 0, 1, 1);  // 0x00629660
}

struct H {
    // ------------------------------------------------------------------ 0x00724e00
    static void* new_obj(u32 size, std::uintptr_t ctor) {
        void* const mem = joan::crt::operator_new(joan::orig::arg::u32(size));  // operator new
        return mem != nullptr ? thiscall<void*>(ctor, mem) : nullptr;
    }
    void panel_init() {
        c_panel_init.hit();
        jo::view<void*>(jo::self<jo::CampScreen>(this)->army_image_frame) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Image2D)), fn::code_value(fn::Widget629_ctor));  // 0x00629400
        image_init(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_image_frame), 0x400, 0xb6, img::addr(d::g_bmp_objmenu), str(d::s_IMG_F), colour(0, 0, 0, 0xff));  // IMG_F
        gadget_enable(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_image_frame));
        jo::view<void*>(jo::self<jo::CampScreen>(this)->army_text_frame) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Image2D)), fn::code_value(fn::Widget629_ctor));  // 0x00629400
        image_init(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_text_frame), 0x400, 0x1e2, img::addr(d::g_bmp_objmenu), str(d::s_TXT_FH), colour(0, 0, 0, 0xff));  // TXT_FH
        gadget_enable(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_text_frame));
        jo::view<void*>(jo::self<jo::CampScreen>(this)->army_up_button) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0));  // 0x00620ee0
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_up_button))->setup(joan::orig::arg::i16(0xa9), joan::orig::arg::i16(0xc2), joan::orig::arg::mptr(img::addr(d::g_bmp_commele2)), str(d::s_aw_u_ot), str(d::s_aw_u_dw), str(d::s_aw_u_ov), joan::orig::arg::str(0u), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)));  // 0x006211d0 aw_u_*
        gadget_enable(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_up_button));
        jo::view<void*>(jo::self<jo::CampScreen>(this)->army_down_button) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Button)), fn::code_value(fn::Widget_ctor_620ee0));  // 0x00620ee0
        joan::orig::as_menu_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_down_button))->setup(joan::orig::arg::i16(0xa9), joan::orig::arg::i16(0x26c), joan::orig::arg::mptr(img::addr(d::g_bmp_commele2)), str(d::s_aw_d_ot), str(d::s_aw_d_dw), str(d::s_aw_d_ov), joan::orig::arg::str(0u), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)));  // 0x006211d0 aw_d_*
        gadget_enable(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_down_button));
        alignas(4) u32 box[6] = {0xc1200000u, 0xc1200000u, 0xc47a0000u, 0x47435000u, 0x47435000u, 0x453b8000u};
        jo::self<jo::CampScreen>(this)->army_model = nullptr;
        jo::view<void*>(jo::self<jo::CampScreen>(this)->army_scene) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Scene)), fn::code_value(fn::Scene_Scene));  // 0x005e9a00
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_scene))->reset();                          // 0x005e9ac0
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_scene))->setup(joan::orig::arg::mptr(static_cast<void*>(box)));  // 0x005e9e50
        u8* const light = joan::orig::arg::as<u8*>(joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_scene))->add_light());         // 0x005eb5d0
        joan::orig::as_camera(light)->init(0u);                                 // 0x005c58a0
        jo::view<u32>(jo::as_light(light)->f32_30) = 0x3f800000u;
        jo::view<u32>(jo::as_light(light)->colour) = colour(0xcf, 0xcf, 0xcf, 0xcf);
        joan::orig::as_light(light)->invalidate();  // 0x005c5c00
        jo::view<void*>(jo::self<jo::CampScreen>(this)->army_camera) = new_obj(static_cast<std::uint32_t>(sizeof(jo::Camera)), fn::code_value(fn::Obj_ctor_613650));  // 0x00613650
        if (jo::view<void*>(jo::self<jo::CampScreen>(this)->army_camera) == nullptr) joan::orig::as_error_log(obj(d::g_err))->mem_error();  // 0x00586af0
        joan::orig::as_sys_en(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_camera))->init_613730();  // 0x00613730
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_camera))->set_viewport(static_cast<jo::Surface*>(img::obj(d::g_vga_back)), img::obj(d::g_a56ef0), 0x23bu, 0xcbu, 0x376u, 0x1c1u);  // 0x00613bf0
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_camera))->set_cf(0u, 1u, joan::orig::arg::u32(global<u32>(d::g_init_black)));  // 0x00613d30
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_camera))->set_ranges(8.0f, 520.0f, 520.0f);           // 0x00613ca0
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_camera))->set_scene(jo::self<jo::CampScreen>(this)->army_scene);               // 0x00613db0
        jo::self<jo::CampScreen>(this)->army_model_id = 0;
        jo::self<jo::CampScreen>(this)->army_model_flags = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_07", name, addr, fn)
B("Panel724e00::init", 0x00724e00, &H::panel_init);
#undef B

}  // namespace game_b109_07
}  // namespace

// ---------------------------------------- from modules/game_b104_15.cpp (game/b104_15)
namespace {
namespace game_b104_15 {


JOAN_COUNTER(c_free_725440, "Screen::free_725440");
JOAN_COUNTER(c_layout_728a90, "Screen::layout_728a90");

struct H {
    // 0x0061e020 / 0x0061f1b0 on a widget member of the screen.
    static u32 m6(const void* w, u32 a, u32 b, u32 c, u32 d, u32 e, u32 g) {
        joan::orig::as_widget(w)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(g));  // 0x0061e020
        return 0;  // void callee; results dropped
    }
    static u32 f5(const void* w, u32 a, u32 b, u32 c, u32 d, u32 e) {
        joan::orig::as_widget(w)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
        return 0;  // void callee; results dropped
    }

    // ------------------------------------------------------------------------ 0x00725440
    void free_725440() {
        c_free_725440.hit();
        if (jo::self<jo::CampScreen>(this)->army_image_frame != nullptr) {
            joan::orig::as_image2d(jo::self<jo::CampScreen>(this)->army_image_frame)->release();  // 0x00629830
            auto* const p = jo::self<jo::CampScreen>(this)->army_image_frame;
            if (p != nullptr) joan::orig::as_image2d(p)->destroy(1u);  // 0x006bd440 scalar dtor
            jo::self<jo::CampScreen>(this)->army_image_frame = nullptr;
        }
        if (jo::self<jo::CampScreen>(this)->army_text_frame != nullptr) {
            joan::orig::as_image2d(jo::self<jo::CampScreen>(this)->army_text_frame)->release();  // 0x00629830
            auto* const p = jo::self<jo::CampScreen>(this)->army_text_frame;
            if (p != nullptr) joan::orig::as_image2d(p)->destroy(1u);  // 0x006bd440
            jo::self<jo::CampScreen>(this)->army_text_frame = nullptr;
        }
        if (jo::self<jo::CampScreen>(this)->army_up_button != nullptr) {
            joan::orig::as_menu_button(jo::self<jo::CampScreen>(this)->army_up_button)->clear();  // 0x006217e0
            auto* const p = jo::self<jo::CampScreen>(this)->army_up_button;
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b33a0(1u);  // 0x006b33a0
            jo::self<jo::CampScreen>(this)->army_up_button = nullptr;
        }
        if (jo::self<jo::CampScreen>(this)->army_down_button != nullptr) {
            joan::orig::as_menu_button(jo::self<jo::CampScreen>(this)->army_down_button)->clear();  // 0x006217e0
            auto* const p = jo::self<jo::CampScreen>(this)->army_down_button;
            if (p != nullptr) joan::orig::as_combo_box(p)->scalar_dtor_6b33a0(1u);  // 0x006b33a0
            jo::self<jo::CampScreen>(this)->army_down_button = nullptr;
        }
        if (jo::self<jo::CampScreen>(this)->army_item != nullptr) {
            joan::orig::as_list(jo::self<jo::CampScreen>(this)->army_scene)->remove_5ea2f0(joan::orig::arg::mptr(jo::self<jo::CampScreen>(this)->army_item));  // 0x005ea2f0
            jo::ActorDef* const inner = jo::self<jo::CampScreen>(this)->army_item->def;
            if (inner != nullptr) joan::orig::as_actor_def(inner)->destroy(1u);  // 0x004b0c20
            auto* const p = jo::self<jo::CampScreen>(this)->army_item;
            if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840
            jo::self<jo::CampScreen>(this)->army_item = nullptr;
        }
        if (jo::self<jo::CampScreen>(this)->army_model != nullptr) {
            joan::orig::as_list(jo::self<jo::CampScreen>(this)->army_scene)->remove_5ea2f0(joan::orig::arg::mptr(jo::self<jo::CampScreen>(this)->army_model));  // 0x005ea2f0
            auto* const p = jo::self<jo::CampScreen>(this)->army_model;
            if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840
            jo::self<jo::CampScreen>(this)->army_model = nullptr;
            jo::self<jo::CampScreen>(this)->army_actor_def = nullptr;
            if (jo::view<i32>(jo::self<jo::CampScreen>(this)->army_model_id) > 0) {
                const u32 id = static_cast<u32>(static_cast<i32>(static_cast<i16>(jo::self<jo::CampScreen>(this)->army_model_id)));
                joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(id));  // 0x004b16a0 (reads a word)
            }
            jo::self<jo::CampScreen>(this)->army_model_id = 0;
            jo::self<jo::CampScreen>(this)->army_model_flags = 0;
        }
        if (jo::self<jo::CampScreen>(this)->army_camera != nullptr) {
            joan::orig::as_model(jo::self<jo::CampScreen>(this)->army_camera)->release_parts();  // 0x00613a10
            auto* const p = jo::self<jo::CampScreen>(this)->army_camera;
            if (p != nullptr) joan::orig::as_camera(p)->destroy(1u);  // 0x00562910
            jo::self<jo::CampScreen>(this)->army_camera = nullptr;
        }
        if (jo::self<jo::CampScreen>(this)->army_scene != nullptr) {
            joan::orig::as_scene(jo::self<jo::CampScreen>(this)->army_scene)->clear();  // 0x005e9b10
            auto* const p = jo::self<jo::CampScreen>(this)->army_scene;
            if (p != nullptr) joan::orig::as_scene(p)->destroy(1u);  // 0x004201f0
            jo::self<jo::CampScreen>(this)->army_scene = nullptr;
        }
    }

    // ------------------------------------------------------------------------ 0x00728a90
    u32 layout_728a90() {
        c_layout_728a90.hit();
        f5(jo::self<jo::CampScreen>(this)->menu_buttons[7], 1, 0x40c00000u, 0, 0, 0xc8);
        f5(jo::self<jo::CampScreen>(this)->side_bar, 1, 0x40c00000u, 0, 0, 0);
        m6(jo::self<jo::CampScreen>(this)->side_bar, 0x19, 0xb8, 0x455ac000u, 0xc4bb8000u, 0, 0);
        m6(jo::self<jo::CampScreen>(this)->army_image_frame, 0x226, 0xb6, 0x455ac000u, 0xc4bb8000u, 0, 0);
        m6(jo::self<jo::CampScreen>(this)->army_text_frame, 0x226, 0x1e2, 0x455ac000u, 0xc4bb8000u, 0, 0);
        f5(jo::self<jo::CampScreen>(this)->army_up_button, 1, 0x40800000u, 0, 0, 0xc8);
        f5(jo::self<jo::CampScreen>(this)->army_down_button, 1, 0x40800000u, 0, 0, 0xc8);
        for (u32 i = 0; i < 10; ++i) {
            joan::orig::as_widget(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i])->scale_to(1, joan::orig::arg::f32(0x40800000u), 0.0f, 0, 0xc8u);  // 0x0061f1b0
            u32 colour_obj = 0;
            const u32* const c = joan::orig::arg::as<const u32*>(joan::orig::as_colour(&colour_obj)->construct(0xffu, 0xffu, 0xffu, 0xffu));  // 0x0042a260
            const u32 colour = *c;
            joan::orig::as_button(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i])->set_text(str(d::kEmpty), img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(colour), 1, 2, 0, 2, 2);  // 0x00623ce0
            jo::view<u16>(jo::self<jo::CampScreen>(this)->roster_unit_ids[i]) = 0;
            jo::self<jo::CampScreen>(this)->slot_kinds[i] = 0;
        }
        joan::orig::as_scenario(obj(d::g_mission))->clear();  // 0x004cddc0
        for (i32 i = joan::orig::as_dynarray(obj(d::g_units))->size(); i >= 1; --i) {  // 0x00417ad0
            if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00428b70
            u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3c0
            if (jo::as_unit(unit)->rank >= 4)
                joan::orig::as_scenario(obj(d::g_mission))->push_word_4cdd40(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(i))));  // 0x004cdd40 (word)
        }
        joan::orig::as_obj_menu(this)->build_roster(0);  // 0x00728390
        return 0;  // void callee; CampScreen::set_state_721a60 (case 4) callers drop the eax
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_15", name, addr, fn)
B("Screen::free_725440", 0x00725440, &H::free_725440);
B("Screen::layout_728a90", 0x00728a90, &H::layout_728a90);
#undef B

}  // namespace game_b104_15
}  // namespace

// ---------------------------------------- from modules/game_b111_10.cpp (fix/uninit)
namespace {
namespace game_b111_10 {

using joan::orig::helpers::at;
i32 sx(u16 v) { return static_cast<i32>(static_cast<i16>(v)); }

constexpr u32 kOne = 0x3f800000u;      // 1.0f pushed as an immediate

i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }
void crt_strcpy(void* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }

JOAN_COUNTER(c_group_panel, "GroupPanel::draw_725870");

// ---------------------------------------------------------------- 0x00725870 helpers
// Colour ctor 0x0042a260 (thiscall on a 4-byte temporary, 4 byte args, ret 0x10); the original
// copies the dword through the returned pointer.
u32 colour(i32 r, i32 g, i32 b, i32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
}
// ftol(255 * fade->+0x11) (float * float at PC24).
i32 fade255(const void* fade_obj) { return joan::hook::x87::ftol(255.0f * jo::as_widget(fade_obj)->scale); }
i32 fade255s(const void* fade_obj, float scale) {
    const float t = 255.0f * jo::as_widget(fade_obj)->scale;
    return joan::hook::x87::ftol(t * scale);
}
// SpriteQueue::draw_named (0x00633500 through 0x00401bbd) on 0x00a46128 with the set 0x00a57f00
// and the trailing arguments (3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1) used by every plain sprite.
void sprite(i32 x, i32 y, const char* name, u32 col) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 2u, joan::orig::arg::str(name), obj(d::g_bmp_objmenu), joan::orig::arg::u32(col), 3, 0, 2, 0, -1, -1, -1, -1, 0, -1, -1);
}
// TextBox::draw 0x00626130 (thiscall, 13 args, ret 0x34) with the trailing (0, 1, 2, 0, 1.0, 1.0).
void text_box(img::Addr self, i32 x1, i32 y1, i32 x2, i32 y2, const char* text, u32 col) {
    joan::orig::as_text(obj(self))->draw_3(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(i32{2}), text, joan::orig::arg::u32(col), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
}
const char* menu_text(fn::Code thunk) { return thiscall<const char*>(thunk, obj(d::g_strings)); }

struct UnitKind {
    img::Addr name, alt, sprite;  // alt: second accepted type name (img::kNoAddr = none)
    fn::Code text;
};
constexpr UnitKind kKinds[18] = {
    {d::s_FRENCH_ARCHER, img::kNoAddr, d::s_f_arch, fn::Menu_objmenu_21},           // FRENCH_ARCHER          f_arch   0x004dc0a0
    {d::s_FRENCH_LIGHT_INFANTRY, img::kNoAddr, d::s_f_linf, fn::Menu_objmenu_1d},           // FRENCH_LIGHT_INFANTRY  f_linf   0x004dbfe0
    {d::s_FRENCH_HEAVY_INFANTRY, img::kNoAddr, d::s_f_hinf, fn::TxtRes_objmenu_1e},           // FRENCH_HEAVY_INFANTRY  f_hinf   0x004dc010
    {d::s_FRENCH_POLEMAN, img::kNoAddr, d::s_f_pole, fn::ObjMenu_open_1f},           // FRENCH_POLEMAN         f_pole   0x004dc040
    {d::s_FRENCH_MACEMAN, img::kNoAddr, d::s_f_mace, fn::ObjMenu_open_20},           // FRENCH_MACEMAN         f_mace   0x004dc070
    {d::s_FRENCH_CROSSBOW_MAN, img::kNoAddr, d::s_f_cross, fn::TxtRes_objmenu_22},           // FRENCH_CROSSBOW_MAN    f_cross  0x004dc0d0
    {d::s_ENGLISH_ARCHER, d::s_ENGLISH_EX_ARCHER, d::s_e_arch, fn::Menu_objmenu_21},  // ENGLISH_(EX_)ARCHER    e_arch
    {d::s_ENGLISH_LIGHT_INFANTRY, img::kNoAddr, d::s_e_linf, fn::Menu_objmenu_1d},           // ENGLISH_LIGHT_INFANTRY e_linf
    {d::s_ENGLISH_HEAVY_INFANTRY, img::kNoAddr, d::s_e_hinf, fn::TxtRes_objmenu_1e},           // ENGLISH_HEAVY_INFANTRY e_hinf
    {d::s_ENGLISH_POLEMAN, img::kNoAddr, d::s_e_pole, fn::ObjMenu_open_1f},           // ENGLISH_POLEMAN        e_pole
    {d::s_ENGLISH_MACEMAN, img::kNoAddr, d::s_e_mace, fn::ObjMenu_open_20},           // ENGLISH_MACEMAN        e_mace
    {d::s_ENGLISH_CROSSBOW_MAN, img::kNoAddr, d::s_e_cross, fn::TxtRes_objmenu_22},           // ENGLISH_CROSSBOW_MAN   e_cross
    {d::s_BUR_LIGHTINFANTRY, img::kNoAddr, d::s_b_linf, fn::Menu_objmenu_1d},           // BUR_LIGHTINFANTRY      b_linf
    {d::s_BUR_HEAVYINFANTRY, img::kNoAddr, d::s_b_hinf, fn::TxtRes_objmenu_1e},           // BUR_HEAVYINFANTRY      b_hinf
    {d::s_BUR_ARCHER, img::kNoAddr, d::s_b_arch, fn::Menu_objmenu_21},           // BUR_ARCHER             b_arch
    {d::s_MER_LIGHT_INFANTRY, img::kNoAddr, d::s_m_linf, fn::Menu_objmenu_1d},           // MER_LIGHT_INFANTRY     m_linf
    {d::s_MER_POLEMAN, img::kNoAddr, d::s_m_pole, fn::ObjMenu_open_1f},           // MER_POLEMAN            m_pole
    {d::s_MER_ARCHER, d::s_MER_EX_ARCHER, d::s_m_arch, fn::Menu_objmenu_21},  // MER_(EX_)ARCHER        m_arch
};

// Frame of 0x00725870 from ebp-0x9c to ebp, addressed ebp-relative, so that an over-long caption
// (sprintf / strcpy into the 0x40-byte buffer) or more than 20 members spill into the neighbouring
// locals exactly like in the original.
#if UINTPTR_MAX == 0xffffffffu
constexpr i32 kFrameSize = 0x9c;
constexpr i32 kU2 = -0x9c, kJ = -0x98, kSprite = -0x94, kCaption = -0x84, kScale = -0x44, kType = -0x40,
              kUnit = -0x3c, kFlags = -0x38, kCount = -0x24, kY = -0x20, kX = -0x1c, kMercen = -0x18, kEnglish = -0x14,
              kFrench = -0x10, kGrp = -0xc, kI = -0x8, kThis = -0x4;
#else
// x64: the same locals in the same order, the pointer slots (kU2, kType, kUnit, kGrp, kThis) 8 bytes and aligned.
constexpr i32 kFrameSize = 0xb8;
constexpr i32 kU2 = -0xb8, kJ = -0xb0, kSprite = -0xac, kCaption = -0x9c, kScale = -0x5c, kType = -0x58,
              kUnit = -0x50, kFlags = -0x44, kCount = -0x30, kY = -0x2c, kX = -0x28, kMercen = -0x24, kEnglish = -0x20,
              kFrench = -0x1c, kGrp = -0x18, kI = -0xc, kThis = -0x8;
#endif
struct PanelFrame {
    alignas(alignof(void*)) u8 b[kFrameSize];
    template <class T>
    T& v(i32 ebp_off) {
        return at<T>(b, kFrameSize + ebp_off);
    }
    u8* p(i32 ebp_off) { return b + kFrameSize + ebp_off; }
};

struct H {
    // ------------------------------------------------------------------ 0x00725870
    // fix/uninit: in the member loop the sprite / caption buffers [ebp-0x94] / [ebp-0x84] are
    // only strcpy'd for the 20 known unit type names; a member of any other type (the *_CAPTAIN
    // types, ENGLISH_SPY, ...) is drawn with whatever the buffers hold - the previous member's
    // sprite / caption (reproduced: the frame is kept), but never-written stack bytes when no
    // known type came before it in this call. The buffers start empty here (draw nothing:
    // 0x00620b40 stricmp-searches the set and no entry is named ""; the caption is empty). The
    // rest of the frame starts zeroed, but every other local is written before it is read.
    // Risk kept as in the original: the hovered caption is rebuilt with sprintf(caption, text of
    // 0x004dbfa0) - a '%' conversion in that (already formatted) text would read varargs.
    void group_panel_draw(u8 /*unused, ret 4*/) {
        c_group_panel.hit();
        PanelFrame fr;
        std::memset(fr.b, 0, sizeof fr.b);
        fr.v<H*>(kThis) = this;
        if (jo::self<jo::CampScreen>(this)->army_image_frame->scale > 0.0f) {
            joan::orig::as_label(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_text_frame))->draw(joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{7}));  // 0x00629bd0
            const u32 col = colour(0xff, 0xff, 0xff, fade255(jo::self<jo::CampScreen>(this)->army_text_frame));
            const char* const text = menu_text(fn::Menu_objmenu_15);  // 0x004dbe50
            u8* const t = jo::view<u8*>(jo::self<jo::CampScreen>(this)->army_text_frame);
            text_box(d::g_font_goudy12_bold_black, sx(jo::view<u16>(jo::as_widget(t)->x)), sx(jo::view<u16>(jo::as_widget(t)->y)), sx(jo::view<u16>(jo::as_widget(t)->x2)), sx(jo::view<u16>(jo::as_widget(t)->y)) + 0x28,
                     text, col);
        }
        if (!(jo::self<jo::CampScreen>(this)->army_up_button->scale > 0.0f)) return;
        if (jo::self<jo::CampScreen>(this)->roster_scroll > 0) joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_up_button))->draw(2u, 7);  // 0x00622cc0
        if (jo::self<jo::CampScreen>(this)->roster_scroll < jo::self<jo::CampScreen>(this)->roster_count - 10) joan::orig::as_button(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_down_button))->draw(2u, 7);
        for (fr.v<i32>(kI) = 0; fr.v<i32>(kI) < 10; fr.v<i32>(kI) = fr.v<i32>(kI) + 1) {
            const i32 i = fr.v<i32>(kI);
            joan::orig::as_button(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i])->draw(2u, 7);
            const jo::Button* const b = jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i];
            const i32 kind = jo::view<i8>(jo::self<jo::CampScreen>(this)->slot_kinds[i]);
            if (kind >= 1 && kind <= 3) {
                const u32 c1 = colour(0xff, 0xff, 0xff, fade255(b));
                sprite(0x67, i * 0x28 + 0xdc, str(d::s_fancybar), c1);  // "fancybar"
                const u32 c2 = colour(0, 0, 0, fade255(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]));
                const fn::Code thunk = kind == 1 ? fn::ObjMenu_open_17 : kind == 2 ? fn::TxtRes_objmenu_16 : fn::ObjMenu_open_18;  // 0x004dbeb0 / 0x004dbe80 / 0x004dbee0
                const char* const text = menu_text(thunk);
                text_box(d::g_font_goudy12_bold_black, 0x67, i * 0x28 + 0xdc, 0x12f, i * 0x28 + 0x100, text, c2);
            } else if (kind >= 4 && kind <= 6) {
                const img::Addr icon = kind == 4 ? img::Addr{d::s_icon_f} : kind == 5 ? img::Addr{d::s_icon_e} : img::Addr{d::s_icon_m};  // icon_f / icon_e / icon_m
                const u32 c1 = colour(0xff, 0xff, 0xff, fade255(b));
                sprite(0x1f, i * 0x28 + 0xd9, str(icon), c1);
                const u32 c2 = colour(0xff, 0xff, 0xff, fade255(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i]));
                sprite(0x1f, i * 0x28 + 0xd9, str(d::s_box_bg), c2);  // "box_bg"
            }
        }
        if (sx(jo::self<jo::CampScreen>(this)->roster_selected) < 0) return;
        fr.v<u8*>(kGrp) = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(sx(jo::view<u16>(jo::self<jo::CampScreen>(this)->roster_unit_ids[sx(jo::self<jo::CampScreen>(this)->roster_selected)])))));  // 0x0041b3c0
        const u16 french = static_cast<u16>(thiscall<u32>(
            fn::List_find_by_word2, obj(d::g_nations), joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(str(d::s_FRENCH)))));  // 0x004b67e0(0x004b91a0("FRENCH"))
        fr.v<u16>(kFrench) = french;
        const u16 english = static_cast<u16>(
            joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(str(d::s_ENGLISH)))))));
        fr.v<u16>(kEnglish) = english;
        const u16 mercen = static_cast<u16>(
            joan::orig::arg::as<u32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(str(d::s_MERCEN)))))));
        fr.v<u16>(kMercen) = mercen;
        {
            const i32 nation = sx(jo::view<u16>(jo::as_unit(fr.v<u8*>(kGrp))->side));
            std::uintptr_t bg = 0;
            if (nation == sx(fr.v<u16>(kFrench)))
                bg = img::addr(d::s_BG_F);  // BG_F
            else if (nation == sx(fr.v<u16>(kEnglish)))
                bg = img::addr(d::s_BG_E);  // BG_E
            else if (nation == sx(fr.v<u16>(kMercen)))
                bg = img::addr(d::s_BG_M);  // BG_M
            if (bg != 0) {
                const u32 col = colour(0xff, 0xff, 0xff, fade255(jo::self<jo::CampScreen>(this)->army_image_frame));
                sprite(0x23a, 0xca, str(bg), col);
            }
        }
        if (jo::as_unit(fr.v<u8*>(kGrp))->group == nullptr) return;
        fr.v<i32>(kX) = 0x238;
        fr.v<i32>(kY) = 0x200;
        if (jo::as_group(jo::as_unit(fr.v<u8*>(kGrp))->group)->count == 0) {
            const i32 hit = joan::orig::arg::as<i32>(joan::orig::as_unit(fr.v<u8*>(kGrp))->hostile_to(joan::orig::arg::i16(sx(global<u16>(d::g_player_unit_recno)))));  // 0x005094d0
            const u32 col = colour(0xff, 0xff, 0xff, fade255(jo::self<jo::CampScreen>(this)->army_text_frame));
            const char* const text = menu_text(hit != 0 ? fn::ObjMenu_open_19 : fn::ObjMenu_open_14);  // 0x004dbf10 / 0x004dbe20
            text_box(d::g_font_goudy14_22_white, 0x226, 0x1f6, 0x38a, 0x228, text, col);
        }
        fr.v<i32>(kCount) = 0;
        std::memset(fr.p(kFlags), 0, 0x14);
        for (fr.v<i32>(kI) = 0;; fr.v<i32>(kI) = fr.v<i32>(kI) + 1) {
            const i32 i = fr.v<i32>(kI);
            if (!(i < jo::as_group(jo::as_unit(fr.v<u8*>(kGrp))->group)->count)) break;
            if (!member_drawable(fr, i)) continue;
            fr.v<u8*>(kUnit) = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(member_id(fr, i))));  // 0x0041b3c0
            fr.v<u8*>(kType) = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(sx(jo::view<u16>(jo::as_unit(fr.v<u8*>(kUnit))->type_id)))));  // 0x0042d0a0
            hover_scale(fr);
            const char* const type_name = jo::as_unit_type(fr.v<u8*>(kType))->name;
            for (const UnitKind& k : kKinds) {
                if (crt_stricmp(type_name, str(k.name)) == 0 || (k.alt != img::kNoAddr && crt_stricmp(type_name, str(k.alt)) == 0)) {
                    crt_strcpy(fr.p(kSprite), str(k.sprite));
                    crt_strcpy(fr.p(kCaption), menu_text(k.text));
                    break;
                }
            }
            draw_member(fr, fr.v<u8*>(kUnit));
            fr.v<i32>(kX) = fr.v<i32>(kX) + 0x1e;
            next_slot(fr);
            fr.p(kFlags)[fr.v<i32>(kI)] = 1;
            for (fr.v<i32>(kJ) = fr.v<i32>(kI);; fr.v<i32>(kJ) = fr.v<i32>(kJ) + 1) {
                const i32 j = fr.v<i32>(kJ);
                if (!(j < jo::as_group(jo::as_unit(fr.v<u8*>(kGrp))->group)->count)) break;
                if (!member_drawable(fr, j)) continue;
                fr.v<u8*>(kU2) = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(member_id(fr, j))));
                u8* const t2 = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(sx(jo::view<u16>(jo::as_unit(fr.v<u8*>(kU2))->type_id)))));
                if (crt_stricmp(jo::as_unit_type(fr.v<u8*>(kType))->name, jo::as_unit_type(t2)->name) != 0)
                    continue;
                hover_scale(fr);
                draw_member(fr, fr.v<u8*>(kU2));
                fr.v<i32>(kX) = fr.v<i32>(kX) + 0x1e;
                next_slot(fr);
                fr.p(kFlags)[fr.v<i32>(kJ)] = 1;
            }
        }
    }
    static i32 member_id(PanelFrame& fr, i32 i) { return sx(jo::view<u16>(jo::as_group(jo::as_unit(fr.v<u8*>(kGrp))->group)->members[i])); }
    // Not drawn yet, alive (0x00428b70 == 0) and not the selected unit.
    static bool member_drawable(PanelFrame& fr, i32 i) {
        if (fr.p(kFlags)[i] == 1) return false;
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(member_id(fr, i))) != 0) return false;  // 0x00428b70
        H* const self = fr.v<H*>(kThis);
        return member_id(fr, i) != sx(jo::view<u16>(jo::self<jo::CampScreen>(self)->roster_unit_ids[sx(jo::self<jo::CampScreen>(self)->roster_selected)]));
    }
    static void hover_scale(PanelFrame& fr) {
        fr.v<float>(kScale) = 0.5f;
        if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(fr.v<i32>(kX)), joan::orig::arg::i32(fr.v<i32>(kY)), joan::orig::arg::i32(fr.v<i32>(kX) + 0x1d), joan::orig::arg::i32(fr.v<i32>(kY) + 0x31))) != 0)  // 0x00593980
            fr.v<float>(kScale) = 1.0f;
    }
    static void next_slot(PanelFrame& fr) {
        if (fr.v<i32>(kCount) == 9) {
            fr.v<i32>(kX) = 0x238;
            fr.v<i32>(kY) = 0x232;
        }
        fr.v<i32>(kCount) = fr.v<i32>(kCount) + 1;
    }
    // Sprite, health bar background, health bar and (hovered) caption + health text of one member.
    static void draw_member(PanelFrame& fr, u8* unit) {
        H* const self = fr.v<H*>(kThis);
        const u32 c1 = colour(0xff, 0xff, 0xff, fade255s(jo::self<jo::CampScreen>(self)->army_text_frame, fr.v<float>(kScale)));
        sprite(sx(fr.v<u16>(kX)), sx(fr.v<u16>(kY)), reinterpret_cast<const char*>(fr.p(kSprite)), c1);
        const u32 c2 = colour(0xff, 0xff, 0xff, fade255s(jo::self<jo::CampScreen>(self)->army_text_frame, fr.v<float>(kScale)));
        sprite(fr.v<i32>(kX) + 0xd, fr.v<i32>(kY) + 0x2c, str(d::s_hp_bg), c2);  // "hp_bg"
        const Fpu fpu{true};
        const i32 width = joan::hook::x87::ftol(
            fpu.mul(fpu.div(static_cast<double>(jo::as_unit(unit)->hit_points), static_cast<double>(jo::as_unit(unit)->hit_points_base)), 20.0));
        const u32 c3 = colour(0xff, 0xff, 0xff, fade255s(jo::self<jo::CampScreen>(self)->army_text_frame, fr.v<float>(kScale)));
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(fr.v<i32>(kX) + 0xf), joan::orig::arg::i16(fr.v<i32>(kY) + 0x2e), 2u, str(d::s_hp), obj(d::g_bmp_objmenu), joan::orig::arg::u32(c3), 3, 0, 2, 0, 0, 0, joan::orig::arg::i16(width), 4, 0, -1, -1);  // "hp"
        if (fr.v<u32>(kScale) != kOne) return;
        const u32 c4 = colour(0xff, 0xff, 0xff, fade255(jo::self<jo::CampScreen>(self)->army_text_frame));
        joan::orig::as_text(obj(d::g_font_goudy12_bold_black))->draw_line(i32{0x276}, i32{0x26e}, joan::orig::arg::u32(i32{2}), reinterpret_cast<char*>(fr.p(kCaption)), joan::orig::arg::u32(c4), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x006255d0
        const char* const hp = joan::orig::arg::as<const char*>(joan::orig::as_obj_menu(obj(d::g_strings))->open_1c(joan::orig::arg::u32(sx(jo::view<u16>(jo::as_unit(unit)->level)))));  // 0x004dbfa0
        joan::orig::libc::sprintf(joan::orig::arg::str(reinterpret_cast<char*>(fr.p(kCaption))), joan::orig::arg::str(hp));  // sprintf(caption, hp)
        // The original re-reads [ebp-0x4] (kThis) here (726c5e), after the sprintf that may overflow the caption.
        const u32 c5 = colour(0xff, 0xff, 0xff, fade255(jo::self<jo::CampScreen>(fr.v<H*>(kThis))->army_text_frame));
        joan::orig::as_text(obj(d::g_font_goudy12_bold_black))->draw_2(i32{0x33a}, i32{0x26e}, joan::orig::arg::u32(i32{2}), reinterpret_cast<char*>(fr.p(kCaption)), joan::orig::arg::u32(c5), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626080
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// [ebp-0x94] / [ebp-0x84] sprite / caption before the first known unit type -> "" / "".
FIX("GroupPanel::draw_725870", 0x00725870, &H::group_panel_draw);
#undef FIX

}  // namespace game_b111_10
}  // namespace

// ---------------------------------------- from modules/game_ui8.cpp (game/ui8)
namespace {
namespace game_ui8 {

struct H {
    // 0x00727740. ret 0.
    void draw_extra() {
        if (static_cast<std::int8_t>(jo::self<jo::CampScreen>(this)->state[0]) == 0) return;
        // fcomp ; test ah,0x41 ; jne skip : call only when above (not unordered).
        if (jo::self<jo::CampScreen>(this)->army_image_frame->scale > global<float>(d::k_9cf5c4)) {
            joan::orig::as_label(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_image_frame))->draw(2u, 7u);  // 0x00629bd0
        }
    }
};

#define U8(name, addr, fn) JOAN_REPLACE("game/ui8", name, addr, fn)
U8("ObjectiveMenu::draw_extra", 0x00727740, &H::draw_extra);
#undef U8

}  // namespace game_ui8
}  // namespace

// ---------------------------------------- from modules/game_b109_10.cpp (game/b109_10)
namespace {
namespace game_b109_10 {

using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;

// x87 fcomp + `test ah,0x40`: equal or unordered.
bool eq_or_nan(float a, float b) { return !(a < b) && !(a > b); }

// Colour(r, g, b, a) 0x0042a260 on a stack temporary, then its dword (bytes only are read).
u32 make_color(i32 r, i32 g, i32 b, i32 a) {
    u32 tmp = 0;
    const u32* const c = joan::orig::arg::as<const u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
    return *c;
}

JOAN_COUNTER(c_bar, "Bar::update_7277a0");

struct H {
    // 0x007277a0
    u8 bar_update() {
        c_bar.hit();
        void* const self = this;
        auto hit = [](void* button, i32 a1) -> bool {
            return joan::orig::arg::as<i32>(joan::orig::as_ui_button(button)->mouse(joan::orig::arg::i32(a1), i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0;  // 0x006218c0
        };
        auto slot = [&](i32 n) { return jo::view<void*>(jo::as_camp_screen(self)->slot_buttons->buttons[n]); };
        auto reset_slots = [&]() {
            for (i32 n = 0; n < 10; ++n) {
                joan::orig::as_widget(slot(n))->scale_to(1, 4.0f, 0.0f, 0, 0xc8u);  // 0x0061f1b0
                const u32 col = make_color(0xff, 0xff, 0xff, 0xff);
                joan::orig::as_button(slot(n))->set_text(str(d::kEmpty), obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(col), i32{1}, i32{2}, i32{0}, i32{2}, i32{2});  // 0x00623ce0
                jo::view<i16>(jo::as_camp_screen(self)->roster_unit_ids[n]) = 0;
                jo::as_camp_screen(self)->slot_kinds[n] = 0;
            }
        };
        auto anim = [&](i32 on) {
            joan::orig::as_widget(jo::view<void*>(jo::as_camp_screen(self)->army_image_frame))->scale_to(joan::orig::arg::i16(on), 6.0f, 0.0f, 0, 0u);  // 0x0061f1b0
            joan::orig::as_widget(jo::view<void*>(jo::as_camp_screen(self)->army_text_frame))->scale_to(joan::orig::arg::i16(on), 6.0f, 0.0f, 0, 0u);  // 0x0061f1b0
        };
        auto cleanup = [&]() {
            if (!eq_or_nan(jo::as_camp_screen(self)->army_image_frame->scale_target, 0.0f)) anim(0);
            if (jo::as_camp_screen(self)->army_item != nullptr) {
                joan::orig::as_list(jo::view<void*>(jo::as_camp_screen(self)->army_scene))->remove_5ea2f0(joan::orig::arg::mptr(jo::as_camp_screen(self)->army_item));  // 0x005ea2f0
                void* const inner = jo::view<void*>(jo::as_camp_screen(self)->army_item->def);
                if (inner != nullptr) joan::orig::as_actor_def(inner)->destroy(joan::orig::arg::u32(i32{1}));  // 0x004b0c20
                void* const p = jo::view<void*>(jo::as_camp_screen(self)->army_item);
                if (p != nullptr) joan::orig::as_scene_object(p)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00420840
                jo::as_camp_screen(self)->army_item = nullptr;
            }
            if (jo::as_camp_screen(self)->army_model != nullptr) {
                joan::orig::as_list(jo::view<void*>(jo::as_camp_screen(self)->army_scene))->remove_5ea2f0(joan::orig::arg::mptr(jo::as_camp_screen(self)->army_model));  // 0x005ea2f0
                void* const p = jo::view<void*>(jo::as_camp_screen(self)->army_model);
                if (p != nullptr) joan::orig::as_scene_object(p)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00420840
                jo::as_camp_screen(self)->army_model = nullptr;
                jo::as_camp_screen(self)->army_actor_def = nullptr;
                if (jo::view<i32>(jo::as_camp_screen(self)->army_model_id) > 0) {
                    joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(i32{static_cast<i16>(jo::as_camp_screen(self)->army_model_id)}));  // 0x004b16a0
                }
                jo::as_camp_screen(self)->army_model_id = 0;
                jo::as_camp_screen(self)->army_model_flags = 0;
            }
            jo::view<i16>(jo::as_camp_screen(self)->roster_selected) = -1;
        };
        if (jo::as_camp_screen(self)->roster_scroll > 0 && hit(jo::view<void*>(jo::as_camp_screen(self)->army_up_button), 0)) {
            if (jo::as_camp_screen(self)->roster_scroll > 0) {
                reset_slots();
                joan::orig::as_obj_menu(self)->build_roster(joan::orig::arg::i32(wsub(jo::as_camp_screen(self)->roster_scroll, 1)));  // 0x00728390
                cleanup();
            }
            return 1;
        }
        if (jo::as_camp_screen(self)->roster_scroll < wsub(jo::as_camp_screen(self)->roster_count, 10) && hit(jo::view<void*>(jo::as_camp_screen(self)->army_down_button), 0)) {
            if (jo::as_camp_screen(self)->roster_scroll < wsub(jo::as_camp_screen(self)->roster_count, 10)) {
                reset_slots();
                joan::orig::as_obj_menu(self)->build_roster(joan::orig::arg::i32(wadd(jo::as_camp_screen(self)->roster_scroll, 1)));  // 0x00728390
                cleanup();
            }
            return 1;
        }
        if (hit(jo::view<void*>(jo::as_camp_screen(self)->menu_buttons[7]), 0x1b)) {
            joan::orig::as_camp_screen(self)->set_state_721a60(joan::orig::arg::u32(i32{0}));  // 0x00721a60
            return 1;
        }
        if (jo::view<i8>(jo::as_camp_screen(self)->visible) == 4) {
            if (eq_or_nan(jo::as_widget(slot(0))->scale, 1.0f)) {
                for (i32 n = 0; n < 10; ++n) {
                    if (!(jo::view<i8>(jo::as_camp_screen(self)->slot_kinds[n]) > 2)) continue;
                    if (!hit(slot(n), 0)) continue;
                    if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(i32{jo::view<i16>(jo::as_camp_screen(self)->roster_unit_ids[n])}) == 0) {  // 0x00428b70
                        if (i32{jo::view<i16>(jo::as_camp_screen(self)->roster_selected)} != n) {
                            jo::view<i16>(jo::as_camp_screen(self)->roster_selected) = static_cast<i16>(n);
                            joan::orig::as_obj_menu(self)->play_idle_anim();  // 0x00729200
                        }
                        if (!eq_or_nan(jo::as_camp_screen(self)->army_image_frame->scale_target, 1.0f)) anim(1);
                    }
                    break;
                }
            }
            return 1;
        }
        cleanup();
        return 1;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_10", name, addr, fn)
B("Bar::update_7277a0", 0x007277a0, &H::bar_update);
#undef B

}  // namespace game_b109_10
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

struct H {
    // ------------------------------------------------------------------ 0x00728230
    u32 reset_buttons() {
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::GroupPanel>(this)->army_up_button))->update();         // 0x0061f3b0
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::GroupPanel>(this)->army_down_button))->update();         // 0x0061f3b0
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::GroupPanel>(this)->army_image_frame))->update();         // 0x0061f3b0
        return joan::orig::arg::as<u32>(joan::orig::as_widget(jo::view<void*>(jo::self<jo::GroupPanel>(this)->army_text_frame))->update());  // 0x0061f3b0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("Panel::reset_buttons", 0x00728230, &H::reset_buttons);
#undef B

}  // namespace game_b108_18
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

JOAN_COUNTER(c_layout_4, "X::layout_4");

u32 anim_a(void* p, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f) {
    joan::orig::as_widget(p)->move_to(joan::orig::arg::i16(a), joan::orig::arg::i16(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d), static_cast<char>(e), joan::orig::arg::u32(f));  // 0x0061e020
    return 0;  // void callee; X::layout_4's caller drops the eax
}
void anim_b(void* p, u32 a, u32 b, u32 c, u32 d, u32 e) {
    joan::orig::as_widget(p)->scale_to(joan::orig::arg::i16(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), static_cast<char>(d), joan::orig::arg::u32(e));  // 0x0061f1b0
}

struct H {
    // ------------------------------------------------------------------------ 0x00728290
    u32 layout_4() {
        c_layout_4.hit();
        anim_b(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_up_button), 0, 0x3f800000, 0, 1, 0);
        anim_b(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_down_button), 0, 0x3f800000, 0, 1, 0);
        anim_b(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_image_frame), 0, 0x3f800000, 0, 1, 0);
        anim_a(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_image_frame), 0x400, 0xb6, 0x3f800000, 0, 1, 0);
        anim_b(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_text_frame), 0, 0x3f800000, 0, 1, 0);
        return anim_a(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_text_frame), 0x400, 0x1e2, 0x3f800000, 0, 1, 0);
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::layout_4", 0x00728290, &H::layout_4);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

JOAN_COUNTER(c_build_728390, "Roster::build_728390");
JOAN_COUNTER(c_hide_728d20, "Roster::hide_728d20");

constexpr img::Addr kUnits = d::g_units;
i16 unit_id(i32 k) { return global<i16>(d::g_a591dc + static_cast<u32>(k) * 2); }
i32 unit_count() { return global<i16>(d::g_unit_count); }
i32 unit_dead(i32 id) { return joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit_rec(i32 id) { return joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
i32 side_id(img::Addr name) {
    const u32 key = joan::orig::arg::as<u32>(joan::orig::as_name_table(obj(d::g_nation_res))->find_4b91a0(joan::orig::arg::str(img::addr(name))));  // 0x004b91a0
    return joan::orig::arg::as<i32>(joan::orig::as_list(obj(d::g_nations))->find_by_word2(joan::orig::arg::i32(key)));            // 0x004b67e0
}

struct H {
    // one roster row: label + unit id + kind byte
    void roster_row(i32 row, i32 k, void* rec, u8 kind) {
        u32 rgba;
        const u32 col = *joan::orig::arg::as<u32*>(joan::orig::as_colour(&rgba)->construct(0xffu, 0xffu, 0xffu, 0xffu));  // 0x0042a260
        void* label = jo::self<jo::CampScreen>(this)->slot_buttons->buttons[row];
        joan::orig::as_button(label)->set_text(jo::as_unit(rec)->name_, img::obj(d::g_font_goudy14_22_white), joan::orig::arg::i32(col), 1, 2, 0, 2, 2);  // 0x00623ce0
        jo::view<u16>(jo::self<jo::CampScreen>(this)->roster_unit_ids[row]) = static_cast<u16>(unit_id(k));
        jo::self<jo::CampScreen>(this)->slot_kinds[row] = kind;
    }

    void build_728390(i32 min_count) {
        c_build_728390.hit();
        if (min_count < 0) return;
        jo::self<jo::CampScreen>(this)->roster_selected = 0xffff;
        jo::self<jo::CampScreen>(this)->roster_scroll = min_count;
        i32 row = 0;
        jo::self<jo::CampScreen>(this)->roster_french_row = -1;
        jo::self<jo::CampScreen>(this)->roster_english_row = -1;
        jo::self<jo::CampScreen>(this)->roster_mercenary_row = -1;
        i32 n_fr = 0, n_en = 0, n_mc = 0;
        const i32 fr = side_id(d::s_FRENCH);  // 'FRENCH'
        const i32 en = side_id(d::s_ENGLISH);  // 'ENGLISH'
        const i32 mc = side_id(d::s_MERCEN);  // 'MERCEN'

        for (i32 k = 0; k < unit_count(); ++k) {
            if (unit_dead(unit_id(k)) != 0) continue;
            void* rec = unit_rec(unit_id(k));
            if (jo::as_unit(rec)->excluded != 0) continue;
            if (jo::as_unit(rec)->side != fr) continue;
            ++n_fr;
            if (jo::self<jo::CampScreen>(this)->roster_french_row == -1) {
                jo::self<jo::CampScreen>(this)->roster_french_row = 0;
                if (!(jo::self<jo::CampScreen>(this)->roster_scroll > jo::self<jo::CampScreen>(this)->roster_french_row) && row < 10) {
                    jo::self<jo::CampScreen>(this)->slot_kinds[row] = 1;
                    ++row;
                }
            }
            if (!(jo::self<jo::CampScreen>(this)->roster_scroll > jo::self<jo::CampScreen>(this)->roster_french_row + n_fr)) {
                if (row < 10) roster_row(row, k, rec, 4);
                ++row;
            }
        }
        for (i32 k = 0; k < unit_count(); ++k) {
            if (unit_dead(unit_id(k)) != 0) continue;
            void* rec = unit_rec(unit_id(k));
            if (jo::as_unit(rec)->excluded != 0) continue;
            if (jo::as_unit(rec)->side != en) continue;
            ++n_en;
            if (jo::self<jo::CampScreen>(this)->roster_english_row == -1) {
                jo::self<jo::CampScreen>(this)->roster_english_row = n_fr + 1;
                if (!(jo::self<jo::CampScreen>(this)->roster_scroll > jo::self<jo::CampScreen>(this)->roster_english_row) && row < 10) {
                    jo::self<jo::CampScreen>(this)->slot_kinds[row] = 2;
                    ++row;
                }
            }
            if (!(jo::self<jo::CampScreen>(this)->roster_scroll > jo::self<jo::CampScreen>(this)->roster_english_row + n_en)) {
                if (row < 10) roster_row(row, k, rec, 5);
                ++row;
            }
        }
        for (i32 k = 0; k < unit_count(); ++k) {
            if (unit_dead(unit_id(k)) != 0) continue;
            void* rec = unit_rec(unit_id(k));
            if (jo::as_unit(rec)->excluded != 0) continue;
            if (jo::as_unit(rec)->side != mc) continue;
            ++n_mc;
            if (jo::self<jo::CampScreen>(this)->roster_mercenary_row == -1) {
                jo::self<jo::CampScreen>(this)->roster_mercenary_row = n_fr + 1;
                if (n_en > 0) jo::self<jo::CampScreen>(this)->roster_mercenary_row = jo::self<jo::CampScreen>(this)->roster_mercenary_row + n_en + 1;
                if (!(jo::self<jo::CampScreen>(this)->roster_scroll > jo::self<jo::CampScreen>(this)->roster_mercenary_row) && row < 10) {
                    jo::self<jo::CampScreen>(this)->slot_kinds[row] = 3;
                    ++row;
                }
            }
            if (!(jo::self<jo::CampScreen>(this)->roster_scroll > jo::self<jo::CampScreen>(this)->roster_mercenary_row + n_mc)) {
                if (row < 10) roster_row(row, k, rec, 6);
                ++row;
            }
        }
        jo::self<jo::CampScreen>(this)->roster_count = n_fr + n_en + n_mc;
        if (n_fr != 0) jo::self<jo::CampScreen>(this)->roster_count = jo::self<jo::CampScreen>(this)->roster_count + 1;
        if (n_en != 0) jo::self<jo::CampScreen>(this)->roster_count = jo::self<jo::CampScreen>(this)->roster_count + 1;
        if (n_mc != 0) jo::self<jo::CampScreen>(this)->roster_count = jo::self<jo::CampScreen>(this)->roster_count + 1;
    }

    // entry with an owned object in its first dword (+0x161 / +0x165)
    void free_entry_with_obj(jo::Actor*& entry) {
        if (entry == nullptr) return;
        joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_scene))->remove_5ea2f0(joan::orig::arg::mptr(entry));  // 0x005ea2f0
        void* inner = jo::view<void*>(entry->def);
        if (inner != nullptr) joan::orig::as_actor_def(inner)->destroy(1u);  // 0x004b0c20
        void* e = jo::view<void*>(entry);
        if (e != nullptr) joan::orig::as_scene_object(e)->destroy(1u);  // 0x00420840
        entry = nullptr;
    }

    void hide_728d20() {
        c_hide_728d20.hit();
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->menu_buttons[7])->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->side_bar)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->side_bar)->move_to(-356, 0xb8, joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->army_image_frame)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->army_image_frame)->move_to(0x400, 0xb6, joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->army_text_frame)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->army_text_frame)->move_to(0x400, 0x1e2, joan::orig::arg::f32(0x455ac000), joan::orig::arg::f32(0xc4bb8000), 0, 0x0u);  // 0x0061e020
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->army_up_button)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        joan::orig::as_widget(jo::self<jo::CampScreen>(this)->army_down_button)->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        for (i32 i = 0; i < 10; ++i)
            joan::orig::as_widget(jo::self<jo::CampScreen>(this)->slot_buttons->buttons[i])->scale_to(0x0, joan::orig::arg::f32(0x40c00000), 0.0f, 0, 0x0u);  // 0x0061f1b0
        free_entry_with_obj(jo::self<jo::CampScreen>(this)->army_item);
        free_entry_with_obj(jo::self<jo::CampScreen>(this)->army_item2);
        if (jo::self<jo::CampScreen>(this)->army_model != nullptr) {
            joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_scene))->remove_5ea2f0(joan::orig::arg::mptr(jo::self<jo::CampScreen>(this)->army_model));  // 0x005ea2f0
            void* e = jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model);
            if (e != nullptr) joan::orig::as_scene_object(e)->destroy(1u);  // 0x00420840
            jo::self<jo::CampScreen>(this)->army_model = nullptr;
            jo::self<jo::CampScreen>(this)->army_actor_def = nullptr;
            if (jo::view<i32>(jo::self<jo::CampScreen>(this)->army_model_id) > 0) {
                // mov ax, word [this+0x16d]; push eax (the upper half held `this`; release() reads the word only)
                const u32 arg = static_cast<u16>(jo::self<jo::CampScreen>(this)->army_model_id);
                joan::orig::as_model_res_mgr(obj(d::g_model_res_mgr))->release(joan::orig::arg::i16(arg));  // 0x004b16a0
            }
            jo::self<jo::CampScreen>(this)->army_model_id = 0;
            jo::self<jo::CampScreen>(this)->army_model_flags = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Roster::build_728390", 0x00728390, &H::build_728390);
B("Roster::hide_728d20", 0x00728d20, &H::hide_728d20);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

void* const kUnitTable = obj(d::g_units);
void* const kTypes = obj(d::g_types);

JOAN_COUNTER(c_free_slot, "Formation::free_slot");

u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* type_of(const void* unit) {
    return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
}

struct H {
    // ------------------------------------------------------------------ 0x00729120
    i32 free_slot() {
        c_free_slot.hit();
        const i32 idx = jo::view<i16>(jo::self<jo::CampScreen>(this)->roster_selected);
        u8* const unit = unit_get(jo::view<i16>(jo::self<jo::CampScreen>(this)->roster_unit_ids[idx]));
        u8* const type = type_of(unit);
        const i32 key = jo::as_unit_type(type)->model_flags[jo::as_unit(unit)->collision_id];
        void* const form = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->entry(joan::orig::arg::u32(key)));  // 0x004b2730
        for (i32 i = jo::as_anim_set(form)->group_count; i > 0; --i) {
            i32* const e = joan::orig::arg::as<i32*>(joan::orig::as_anim_set(form)->entry(joan::orig::arg::u32(i)));  // 0x004168d0
            if (e[0] == 0 && e[1] == 1) return i;
        }
        return 0;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Formation::free_slot", 0x00729120, &H::free_slot);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b107_11.cpp (game/b107_11)
namespace {
namespace game_b107_11 {

struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");

JOAN_COUNTER(c_idle_camera, "Cine::idle_camera_729200");

struct H {
    // ------------------------------------------------------------------ 0x00729200
    u32 idle_camera_729200() {
        c_idle_camera.hit();
        void* const units = obj(d::g_units);
        {
            const i32 idx = jo::view<i16>(jo::self<jo::CampScreen>(this)->roster_selected);
            const u32 busy = joan::orig::arg::as<u32>(joan::orig::as_dynarray(units)->empty_at(static_cast<i32>(jo::view<i16>(jo::self<jo::CampScreen>(this)->roster_unit_ids[idx]))));  // 0x00428b70
            if (busy != 0) return busy;
        }
        const i32 idx = jo::view<i16>(jo::self<jo::CampScreen>(this)->roster_selected);
        void* unit = joan::orig::as_dynarray(units)->ptr_at(static_cast<i32>(jo::view<i16>(jo::self<jo::CampScreen>(this)->roster_unit_ids[idx])));  // 0x0041b3c0
        joan::orig::as_obj_menu(this)->refresh_preview();                                                // 0x00729590
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model))->set_yaw(joan::orig::arg::u32(static_cast<u32>(0x40278d36)));  // 0x005ac6a0
        const u32 a = joan::orig::arg::as<u32>(joan::orig::as_formation(this)->free_slot());                                  // 0x00729120
        void* kind = joan::orig::arg::as<void*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
        const u32 v = jo::view<u32>(jo::as_unit_type(kind)->model_flags[jo::as_unit(unit)->collision_id]);
        void* q = joan::orig::arg::as<void*>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->entry(joan::orig::arg::u32(v)));  // 0x004b2730
        void* r = joan::orig::arg::as<void*>(joan::orig::as_anim_set(q)->entry(joan::orig::arg::u32(a)));                // 0x004168d0
        void* const anim = *joan::orig::arg::as<void**>(joan::orig::as_anim_res_mgr(obj(d::g_anim_res_mgr))->slot_4174f0(joan::orig::arg::u32(jo::view<u32>(jo::as_anim_group_rec(r)->anim_id))));  // 0x004174f0
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model))->start_anim(anim, str(d::s_F_IDLE), joan::orig::arg::u32(global<u32>(d::g_now)), joan::orig::arg::u32(static_cast<u32>(0)), joan::orig::arg::u32(static_cast<u32>(0)), joan::orig::arg::u32(static_cast<u32>(0x43fa0000)));  // 0x005abe30 "F_IDLE"
        void* out40 = nullptr;  // select_mesh writes pointers into both (the plane, the bone)
        void* out44 = nullptr;
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model))->select_mesh(joan::orig::arg::mptr(jo::self<jo::CampScreen>(this)->army_camera), joan::orig::arg::i32(static_cast<u32>(1)), joan::orig::arg::mptr(static_cast<void*>(&out40)), joan::orig::arg::mptr(static_cast<void*>(&out44)));  // 0x005ad060
        void* o44 = jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model->def->model->bones);
        const u32 e = joan::orig::arg::as<u32>(joan::orig::as_bone_table(o44)->find(i32{1}));  // 0x005ba160
        alignas(4) u8 cam[0x28];                                           // [ebp-0x30]
        std::memset(cam, 0, sizeof cam);
        joan::orig::as_camera_params(static_cast<void*>(cam))->construct();  // 0x00616d50
        Vec3 p{0.0f, 0.0f, 0.0f};                            // [ebp-0x54]
        if (e != 0) {
            void* node = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model->plane))->bone_5cf8f0(joan::orig::arg::i32(e)));  // 0x005cf8f0
            joan::orig::as_bone(node)->update();                                                    // 0x0060ee60
            void* m = joan::orig::arg::as<void*>(joan::orig::as_bone(node)->matrix());                                        // 0x0060ef80
            joan::orig::matrix_position(static_cast<void*>(&p), joan::orig::arg::ptr(m));                           // 0x0060c310
            p.z = p.z - 50.0f;   // [0x9cfc18]
            p.x = p.x + 155.0f;  // [0x9df180]
            jo::as_cam_params(cam)->orbit[1] =
                static_cast<float>(Fpu{true}.add(static_cast<double>(jo::as_cam_params(cam)->orbit[1]), 0.3));  // qword [0x9deca0]
            jo::view<u32>(jo::as_cam_params(cam)->orbit[3]) = 0x434f8000;
        } else {
            const u32 bits[3] = {0x42820000u, 0xc3610000u, 0x43640000u};  // 65, -225, 228
            std::memcpy(&p, bits, 12);
            jo::view<u32>(jo::as_cam_params(cam)->orbit[3]) = 0x43960000;
        }
        std::memcpy(cam + 4, &p, 12);
        joan::orig::as_camera(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_camera))->set_frame(joan::orig::arg::mptr(static_cast<void*>(cam)));  // 0x00614040
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model))->start_anim(joan::orig::arg::mptr(jo::self<jo::CampScreen>(this)->army_model->anim), str(d::s_F_IDLE),
                             joan::orig::arg::u32(global<u32>(d::g_game_time_ms)), joan::orig::arg::u32(static_cast<u32>(1)), joan::orig::arg::u32(static_cast<u32>(1)),
                             joan::orig::arg::u32(static_cast<u32>(0x43fa0000)));  // 0x005abe30 "F_IDLE"
        return 0;  // void callee; ObjMenu::play_idle_anim's caller drops the eax
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_11", name, addr, fn)
B("Cine::idle_camera_729200", 0x00729200, &H::idle_camera_729200);
#undef B

}  // namespace game_b107_11
}  // namespace

// ---------------------------------------- from modules/game_b102_16.cpp (game/b102_16)
namespace {
namespace game_b102_16 {

JOAN_COUNTER(c_remaining, "Timer::remaining");

struct H {
    // ------------------------------------------------------------------------ 0x007294b0
    double remaining() {
        c_remaining.hit();
        if (jo::self<jo::CampScreen>(this)->army_model == nullptr) return static_cast<double>(global<float>(d::k_9cf5c4));
        void* p = jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model);
        u32 d = global<u32>(d::g_game_time_ms) - jo::as_actor(p)->start_time;
        p = jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model);
        const u32 a44 = jo::view<u32>(jo::as_actor(p)->action);
        p = jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model);
        void* const q = joan::orig::arg::as<void*>(joan::orig::as_anim_set(jo::view<void*>(jo::as_actor(p)->anim))->frame(joan::orig::arg::i32(a44), 0u));  // 0x005b5dd0
        const float r = joan::orig::arg::as<float>(joan::orig::as_range(q)->span());                       // 0x0042d130
        const u32 c = static_cast<u32>(joan::hook::x87::ftol(static_cast<double>(r)));
        p = jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model);
        if (jo::as_actor(p)->anim_flags != 0) d = d % c;  // div (unsigned), #DE on c == 0 as in the original
        if (c > d) return static_cast<double>(c - d);  // fild qword (c - d, 0): exact
        return static_cast<double>(global<float>(d::k_9cf5c4));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_16", name, addr, fn)
B("Timer::remaining", 0x007294b0, &H::remaining);
#undef B

}  // namespace game_b102_16
}  // namespace

// ---------------------------------------- from modules/game_b106_19.cpp (game/b106_19)
namespace {
namespace game_b106_19 {

void* const kUnitTypes = obj(d::g_item_res);
void* const kSoldiers = obj(d::g_units);
void* const kItems = obj(d::g_model_res_mgr);
void* const kItemSets = obj(d::g_types);

JOAN_COUNTER(c_item_refresh, "ItemView::refresh");

struct H {
    // ------------------------------------------------------------------------ 0x00729590
    static void* game_new(u32 n) { return joan::crt::operator_new(joan::orig::arg::u32(n)); }  // operator new

    void release(jo::Actor*& slot, bool inner) {
        if (slot == nullptr) return;
        joan::orig::as_list(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_scene))->remove_5ea2f0(joan::orig::arg::mptr(slot));  // 0x005ea2f0
        if (inner) {
            void* const a = jo::view<void*>(slot->def);
            if (a != nullptr) joan::orig::as_actor_def(a)->destroy(joan::orig::arg::u32(i32{1}));  // 0x004b0c20 deleting dtor
        }
        void* const b = jo::view<void*>(slot);
        if (b != nullptr) joan::orig::as_scene_object(b)->destroy(joan::orig::arg::u32(i32{1}));  // 0x00420840 deleting dtor
        slot = nullptr;
    }

    void attach_item(jo::Actor*& slot, u8* type) {
        {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86
            jo::view<void*>(slot) = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        }
        {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::ActorDef)));  // 0x2d8 on x86
            void* const o = mem != nullptr ? joan::orig::as_actor_def(mem)->construct() : nullptr;  // 0x005af390
            jo::view<void*>(slot->def) = o;
        }
        char name[0x80];
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(name)), joan::orig::arg::str(jo::view<const char*>(jo::as_item_def(type)->model_names[0])));  // strcpy
        for (i32 i = static_cast<i32>(joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(name))) - 1u); i >= 0; --i)  // strlen
            if (static_cast<i32>(static_cast<i8>(name[i])) == 0x2e) name[i] = 0;
        joan::orig::as_actor_def(jo::view<void*>(slot->def))->load2(str(d::s_ITEM_9cf648), joan::orig::arg::str(static_cast<const char*>(name)), static_cast<std::uint8_t>(joan::orig::arg::u32(i32{1})), nullptr, nullptr);  // 0x005af3d0 "ITEM\\"
        slot->anim = nullptr;
        joan::orig::as_scene_object(jo::view<void*>(slot))->init();  // 0x005aab70
        i32 sub = 0;
        {
            void* const q = jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model->def->model->bones);
            if (q != nullptr) {
                void* const q2 = jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model->def->model->bones);
                sub = joan::orig::arg::as<i32>(joan::orig::as_bone_table(q2)->find(static_cast<i32>(jo::view<i16>(jo::as_item_def(type)->bone))));  // 0x005ba160
            }
        }
        void* const node = joan::orig::arg::as<void*>(joan::orig::as_plane(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model->plane))->bone_5cf8f0(joan::orig::arg::i32(sub)));  // 0x005cf8f0
        if (slot != nullptr) joan::orig::as_bone(jo::view<void*>(slot->bone))->set_parent(joan::orig::arg::mptr(node));  // 0x0060ec70
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_scene))->add_actor(joan::orig::arg::mptr(slot));  // 0x005ea1f0
    }

    void item_refresh() {
        c_item_refresh.hit();
        if (joan::orig::as_dynarray(kSoldiers)->empty_at(static_cast<i32>(jo::view<i16>(jo::self<jo::CampScreen>(this)->roster_unit_ids[jo::view<i16>(jo::self<jo::CampScreen>(this)->roster_selected)]))) != 0)
            return;  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(static_cast<i32>(jo::view<i16>(jo::self<jo::CampScreen>(this)->roster_unit_ids[jo::view<i16>(jo::self<jo::CampScreen>(this)->roster_selected)]))));
        release(jo::self<jo::CampScreen>(this)->army_item, true);
        release(jo::self<jo::CampScreen>(this)->army_item2, true);
        if (jo::self<jo::CampScreen>(this)->army_model != nullptr) {
            release(jo::self<jo::CampScreen>(this)->army_model, false);
            jo::self<jo::CampScreen>(this)->army_actor_def = nullptr;
            if (jo::view<i32>(jo::self<jo::CampScreen>(this)->army_model_id) > 0) joan::orig::as_model_res_mgr(kItems)->release(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(jo::self<jo::CampScreen>(this)->army_model_id))));  // 0x004b16a0
            jo::self<jo::CampScreen>(this)->army_model_id = 0;
            jo::self<jo::CampScreen>(this)->army_model_flags = 0;
        }
        {
            u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kItemSets)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
            jo::self<jo::CampScreen>(this)->army_model_id = jo::view<u32>(jo::as_unit_type(r)->model_ids[jo::as_unit(unit)->collision_id]);
        }
        {
            u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kItemSets)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));
            jo::self<jo::CampScreen>(this)->army_model_flags = jo::view<u32>(jo::as_unit_type(r)->model_flags[jo::as_unit(unit)->collision_id]);
        }
        joan::orig::as_model_res_mgr(kItems)->set_params(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(jo::self<jo::CampScreen>(this)->army_model_id))), static_cast<std::int8_t>(joan::orig::arg::i32(static_cast<i32>(jo::self<jo::CampScreen>(this)->army_model_flags != 0))), static_cast<std::int8_t>(joan::orig::arg::i32(i32{1})), joan::orig::arg::i16(i32{0}));  // 0x004b0ec0
        joan::orig::as_model_res(kItems)->acquire(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(jo::self<jo::CampScreen>(this)->army_model_id))));  // 0x004b0f40
        {
            void* const mem = game_new(static_cast<std::uint32_t>(sizeof(jo::SceneObject)));  // 0x10c on x86
            jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model) = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        }
        {
            u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_model_res_mgr(kItems)->entry(joan::orig::arg::u32(jo::self<jo::CampScreen>(this)->army_model_id)));  // 0x0041a180
            jo::self<jo::CampScreen>(this)->army_actor_def = jo::as_model_entry(r)->actor_def;
        }
        jo::self<jo::CampScreen>(this)->army_model->def = jo::self<jo::CampScreen>(this)->army_actor_def;
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model))->init();  // 0x005aab70
        jo::self<jo::CampScreen>(this)->army_model->kind = -1;
        jo::self<jo::CampScreen>(this)->army_model->owner = nullptr;
        u8* const mesh = jo::view<u8*>(jo::self<jo::CampScreen>(this)->army_model->plane);
        for (i32 i = 0; i < jo::as_plane(mesh)->material_count; ++i) {
            u32 black = 0;
            void* const c0 = joan::orig::as_colour(&black)->construct(i32{0}, i32{0}, i32{0}, i32{0});  // 0x0042a260
            joan::orig::as_colour(&jo::as_plane(mesh)->materials[i].specular)->from_rgba(joan::orig::arg::mptr(c0));  // 0x005e95b0
            jo::as_plane(mesh)->materials[i].power = 0;
            u32 white = 0;
            void* const c1 = joan::orig::as_colour(&white)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff});
            joan::orig::as_colour(&jo::as_plane(mesh)->materials[i].ambient)->from_rgba(joan::orig::arg::mptr(c1));
        }
        joan::orig::as_scene(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_scene))->add_actor(joan::orig::arg::mptr(jo::self<jo::CampScreen>(this)->army_model));  // 0x005ea1f0
        joan::orig::as_model_instance(jo::view<void*>(jo::self<jo::CampScreen>(this)->army_model))->set_10a(joan::orig::arg::u32(i32{1}));          // 0x0042cf20
        {
            const i32 n = jo::as_modifiers(jo::as_unit(unit)->modifiers)->hand_mode;
            if (n == 0) return;
            if (static_cast<i32>(jo::as_modifiers(jo::as_unit(unit)->modifiers)->hand_mode) > 1) return;
        }
        u8* t0 = nullptr;
        u8* t1 = nullptr;
        {
            i16* const c = joan::orig::arg::as<i16*>(joan::orig::as_modifiers(jo::as_unit(unit)->modifiers)->slot(i32{0}));  // 0x004974f0
            if (c != nullptr) t0 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kUnitTypes)->get(joan::orig::arg::i16(static_cast<i32>(*c))));  // 0x00425ce0
        }
        {
            i16* const c = joan::orig::arg::as<i16*>(joan::orig::as_modifiers(jo::as_unit(unit)->modifiers)->slot(i32{1}));
            if (c != nullptr) t1 = joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(kUnitTypes)->get(joan::orig::arg::i16(static_cast<i32>(*c))));
        }
        if (t0 != nullptr) attach_item(jo::self<jo::CampScreen>(this)->army_item, t0);
        if (t1 != nullptr) attach_item(jo::self<jo::CampScreen>(this)->army_item2, t1);
    }
};

#define B106(name, addr, fn) JOAN_REPLACE("game/b106_19", name, addr, fn)
B106("ItemView::refresh", 0x00729590, &H::item_refresh);
#undef B106

}  // namespace game_b106_19
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::ObjMenu::build_army() -> void { joan::bind::member<&::game_b109_07::H::panel_init>(this); }  // 0x724e00
auto joan::orig::Screen::free_725440() -> void { joan::bind::member<&::game_b104_15::H::free_725440>(this); }  // 0x725440
auto joan::orig::ObjMenu::draw_army(std::uint8_t a0) -> void { joan::bind::member<&::game_b111_10::H::group_panel_draw>(this, a0); }  // 0x725870
auto joan::orig::ObjMenu::draw_army_title() -> void { joan::bind::member<&::game_ui8::H::draw_extra>(this); }  // 0x727740
auto joan::orig::ObjMenu::update_army() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b109_10::H::bar_update>(this)); }  // 0x7277a0
auto joan::orig::Panel::reset_buttons() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_18::H::reset_buttons>(this)); }  // 0x728230
auto joan::orig::ObjMenu::reset_army_widgets() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::layout_4>(this)); }  // 0x728290
auto joan::orig::ObjMenu::build_roster(std::int32_t a0) -> void { joan::bind::member<&::game_b103_16::H::build_728390>(this, a0); }  // 0x728390
auto joan::orig::Screen::layout_728a90() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b104_15::H::layout_728a90>(this)); }  // 0x728a90
auto joan::orig::ObjMenu::leave_army() -> void { joan::bind::member<&::game_b103_16::H::hide_728d20>(this); }  // 0x728d20
auto joan::orig::Formation::free_slot() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_08::H::free_slot>(this)); }  // 0x729120
auto joan::orig::ObjMenu::play_idle_anim() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b107_11::H::idle_camera_729200>(this)); }  // 0x729200
auto joan::orig::ObjMenu::refresh_preview() -> void { joan::bind::member<&::game_b106_19::H::item_refresh>(this); }  // 0x729590
