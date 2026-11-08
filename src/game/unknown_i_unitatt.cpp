// game/unknown_i_unitatt.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_unitatt.cpp;
// re/attribution.csv: between ww/interfac/i_work.cpp and ww/interfac/list.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

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

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::img::obj;

using u8 = std::uint8_t;

using joan::call::thiscall;

using joan::img::str;

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;

using i8 = std::int8_t;

using u16 = std::uint16_t;

}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

// ---- static initialiser wrappers (direct calls, no thunks) ----
template <fn::Code Body>
u32 JOAN_CDECL init_fwd() {
    return cdecl_call<u32>(Body);
}

struct H {
    // ------------------------------------------------------------------ empty stubs
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("init_6f5f00", 0x006f5f00, &init_fwd<fn::init_colour_a6a56c>);
B("Obj::nop_6f7d60", 0x006f7d60, &H::nop);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

// 0x006f5f20
void* JOAN_CDECL init_colour_a6a56c() {
    return joan::orig::as_colour(obj(d::g_a6a56c))->construct(0xffu, 0xffu, 0xffu, 0x40u);  // 0x0042a260
}

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("init_colour_a6a56c", 0x006f5f20, &init_colour_a6a56c);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {

using joan::orig::helpers::widget_ctor;  // 0x00620ee0

struct H {
    // ------------------------------------------------------------------ 0x006f5f60
    void* ctor_6f5f60() {
        widget_ctor(&jo::self<jo::UnitAttScreen>(this)->add_button);
        widget_ctor(&jo::self<jo::UnitAttScreen>(this)->cb_button);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Screen::ctor_6f5f60", 0x006f5f60, &H::ctor_6f5f60);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {


// 0x006211d0 (13 args): button ctor.
u32 button_ctor(void* b, u32 x, u32 y, void* owner, const char* ot, const char* dw, const char* ov) {
    return joan::orig::arg::as<u32>(joan::orig::as_menu_button(b)->setup(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::mptr(owner), joan::orig::arg::str(ot), joan::orig::arg::str(dw), joan::orig::arg::str(ov), joan::orig::arg::str(0u), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u))));
}

JOAN_COUNTER(c_add_ctor, "AddScreen::ctor");

struct H {
    // ------------------------------------------------------------------ 0x006f5fe0
    void* add_ctor() {
        c_add_ctor.hit();
        jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_iunitatt));
        jo::self<jo::UnitAttScreen>(this)->tex_iunitatt = nullptr;
        jo::self<jo::UnitAttScreen>(this)->button_sprites = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_commele));
        jo::self<jo::UnitAttScreen>(this)->tex_commele = nullptr;
        button_ctor(&jo::self<jo::UnitAttScreen>(this)->add_button, 0, 0, img::obj(d::g_bmp_commele), str(d::s_ADD_OT), str(d::s_ADD_DW), str(d::s_ADD_OV));  // ADD
        joan::orig::as_widget(&jo::self<jo::UnitAttScreen>(this)->add_button)->reset_child();  // 0x0062b660
        button_ctor(&jo::self<jo::UnitAttScreen>(this)->cb_button, 0xf6, 0x13, jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt, str(d::s_CB_OT), str(d::s_CB_DW), str(d::s_CB_OV));  // CB
        jo::view<u32>(jo::self<jo::UnitAttScreen>(this)->hover_slot) = 0xffffffffu;
        jo::self<jo::UnitAttScreen>(this)->show_inventory = 1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("AddScreen::ctor", 0x006f5fe0, &H::add_ctor);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {


JOAN_COUNTER(c_panel_close, "Panel::close");

struct H {
    // ------------------------------------------------------------------ 0x006f60d0
    i32 panel_reset() {
        joan::orig::as_hud(this)->release();                         // 0x006f61e0
        joan::orig::as_menu_button(&jo::self<jo::UnitAttScreen>(this)->add_button)->clear();           // 0x006217e0
        return joan::orig::arg::as<i32>(joan::orig::as_menu_button(&jo::self<jo::UnitAttScreen>(this)->cb_button)->clear());   // 0x006217e0
    }

    // ------------------------------------------------------------------ 0x006f7e90
    void panel_close() {
        c_panel_close.hit();
        jo::self<jo::UnitAttScreen>(this)->is_open = 0;
        joan::orig::as_hud(this)->release();              // 0x006f61e0
        joan::orig::as_unit(obj(d::g_player_unit_recno))->state_sound();   // 0x0042fef0
        if (global<u8>(d::g_game_running) == 0) {
            global<u8>(d::g_game_running) = 1;
            const float v = static_cast<float>(static_cast<i32>(global<i16>(d::g_saved_game_speed)));  // exact
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(v));  // 0x0042f930
        }
        jo::view<i32>(jo::self<jo::UnitAttScreen>(this)->unit_id) = 0;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Panel::reset", 0x006f60d0, &H::panel_reset);
B95("Panel::close", 0x006f7e90, &H::panel_close);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b86_01.cpp (game/b86_01)
namespace {
namespace game_b86_01 {


constexpr img::Addr kTextures(d::g_texture_array);

JOAN_COUNTER(c_set_mode, "Panel::set_mode");
JOAN_COUNTER(c_shared_tex, "Ui::load_shared_textures");

struct H {
    // 0x006f7f20 Panel::set_mode(char on). ret 4. eax = 0x006211d0's.
    std::uint32_t set_mode(char on) {
        c_set_mode.hit();
        jo::view<char>(jo::self<jo::UnitAttScreen>(this)->show_inventory) = on;
        void* const button = &jo::self<jo::UnitAttScreen>(this)->cb_button;
        if (static_cast<std::int32_t>(jo::view<std::int8_t>(jo::self<jo::UnitAttScreen>(this)->show_inventory)) != 0) {
            joan::orig::as_menu_button(button)->clear();  // 0x006217e0
            return joan::orig::arg::as<std::uint32_t>(joan::orig::as_menu_button(button)->setup(joan::orig::arg::i16(0xf6), joan::orig::arg::i16(0x13), joan::orig::arg::mptr(jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt), joan::orig::arg::str(// 0x006211d0
                                           str(d::s_CB_OT)), str(d::s_CB_DW), str(d::s_CB_OV), joan::orig::arg::str(// CB_OT / CB_DW / CB_OV
                                           0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1))));
        }
        joan::orig::as_menu_button(button)->clear();  // 0x006217e0
        return joan::orig::arg::as<std::uint32_t>(joan::orig::as_menu_button(button)->setup(joan::orig::arg::i16(0xf6), joan::orig::arg::i16(0x13), joan::orig::arg::mptr(jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt), joan::orig::arg::str(// 0x006211d0
                                       str(d::s_WP_OT)), str(d::s_WP_DW), str(d::s_WP_OV), joan::orig::arg::str(// WP_OT / WP_DW / WP_OV
                                       0), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1))));
    }

    // 0x006f6120 Ui::load_shared_textures(). ret 0. eax = 0x006b9420's.
    std::uint32_t load_shared_textures() {
        c_shared_tex.hit();
        const std::uint32_t saved = joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(img::obj(kTextures))->mode());  // 0x0060a720
        joan::orig::as_texture_manager(img::obj(kTextures))->set_mode(1u);                                            // 0x0060a740
        if (jo::self<jo::UnitAttScreen>(this)->tex_iunitatt == nullptr)
            jo::self<jo::UnitAttScreen>(this)->tex_iunitatt = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(str(d::s_IUNITATT), 1, 0u));  // 0x006099c0 IUNITATT
        if (jo::self<jo::UnitAttScreen>(this)->tex_commele == nullptr)
            jo::self<jo::UnitAttScreen>(this)->tex_commele = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(img::obj(kTextures))->load(str(d::s_COMMELE), 1, 0u));  // COMMELE
        joan::orig::as_texture_manager(img::obj(kTextures))->set_mode(joan::orig::arg::u32(saved));                                        // 0x0060a740
        joan::orig::as_inventory(img::obj(d::g_a488e0))->load_sprite();                     // 0x006d6680
        joan::orig::as_combat_screen(img::obj(d::g_combat_screen))->load_sprites();                     // 0x006ef8d0
        joan::orig::as_help_box(img::obj(d::g_tooltip))->load_sprite();  // 0x006b9420
        return 0;
    }
};

#define GAME86(name, addr, fn) JOAN_REPLACE("game/b86_01", name, addr, fn)
GAME86("Panel::set_mode", 0x006f7f20, &H::set_mode);
GAME86("Ui::load_shared_textures", 0x006f6120, &H::load_shared_textures);
#undef GAME86

}  // namespace game_b86_01
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

void* const kImages = obj(d::g_texture_array);


JOAN_COUNTER(c_hud_release, "Hud::release");

void free_image(void* h) { joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(h)); }                   // 0x0060a550

struct H {
    void hud_release() {  // 0x006f61e0
        c_hud_release.hit();
        if (jo::self<jo::UnitAttScreen>(this)->tex_iunitatt != nullptr) {
            free_image(jo::self<jo::UnitAttScreen>(this)->tex_iunitatt);
            jo::self<jo::UnitAttScreen>(this)->tex_iunitatt = nullptr;
        }
        if (jo::self<jo::UnitAttScreen>(this)->tex_commele != nullptr) {
            free_image(jo::self<jo::UnitAttScreen>(this)->tex_commele);
            jo::self<jo::UnitAttScreen>(this)->tex_commele = nullptr;
        }
        joan::orig::as_inventory(obj(d::g_a488e0))->free_sprite();  // 0x006d66f0
        joan::orig::as_combat_screen(obj(d::g_combat_screen))->free_sprites();  // 0x006ef970
        joan::orig::as_help_box(obj(d::g_tooltip))->free_sprite();  // 0x006b9490
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Hud::release", 0x006f61e0, &H::hud_release);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b116_07.cpp (game/b116_07)
namespace {
namespace game_b116_07 {

constexpr u32 kM1 = 0xffffffffu;
constexpr u32 kOne = 0x3f800000u;  // 1.0f

i32 gsx(img::Addr a) { return static_cast<i32>(global<i16>(a)); }
u32 w16(i32 v) { return static_cast<u32>(static_cast<u16>(v)); }  // low word of a 16-bit push

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite list
constexpr img::Addr kNumFmt = d::g_misc;   // number formatter
constexpr img::Addr kStrings = d::g_strings;  // UI string table
constexpr img::Addr kMouse = d::g_mouse;
constexpr img::Addr kSlots = d::g_slots_a277d8;    // 6 inventory slot rects, 16 bytes each
constexpr img::Addr kHighlight(d::s_highligh);  // "highligh"
constexpr img::Addr kExpbar(d::s_EXPBAR);     // "EXPBAR"
constexpr img::Addr kRatio(d::s_d_d_9de5e4);      // "%d/%d"

u32 argb(u32 c) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(&tmp)->Color_dword(joan::orig::arg::u32(c)));  // 0x005e9640
}
// 0x00633500 (thunk 0x00401bbd, 17 args, ret 0x44) on the sprite list.
void sprite(u32 x, u32 y, u32 a3, const char* name, void* holder, u32 colour, u32 a7, u32 a8, u32 a9,
            u32 a10, u32 a11, u32 a12, u32 a13, u32 a14, u32 a15, u32 a16, u32 a17) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(a3), joan::orig::arg::str(name), joan::orig::arg::mptr(holder), joan::orig::arg::u32(colour), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), joan::orig::arg::i16(a15), joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));  // 0x00633500
}
// Text::draw_2 0x00626080 (thunk 0x00405ee3, 11 args, ret 0x2c).
void text2(img::Addr font, u32 x, u32 y, const char* text, u32 colour, u32 a6, u32 a7, u32 a8, u32 a9) {
    joan::orig::as_text(obj(font))->draw_2(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(u32{1}), text, joan::orig::arg::u32(colour), joan::orig::arg::i32(a6), joan::orig::arg::i32(a7), joan::orig::arg::i32(a8), joan::orig::arg::i32(a9), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626080
}
// Text::draw_3 0x00626130 (thunk 0x00402e0f, 13 args, ret 0x34).
void text3(img::Addr font, u32 x1, u32 y1, u32 x2, u32 y2, const char* text, u32 colour, u32 a8, u32 a9,
           u32 a10, u32 a11) {
    joan::orig::as_text(obj(font))->draw_3(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(u32{1}), text, joan::orig::arg::u32(colour), joan::orig::arg::i32(a8), joan::orig::arg::i32(a9), joan::orig::arg::i32(a10), joan::orig::arg::i32(a11), joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626130
}
// 0x0058fca0 (thunk 0x0040ab0f): number -> text.
const char* num(i32 v, u32 digits) { return joan::orig::as_misc(obj(kNumFmt))->format_int_58fca0(joan::orig::arg::i32(v), joan::orig::arg::i32(digits)); }

JOAN_COUNTER(c_inventory, "Inventory::draw_6f6280");

struct H {
    // 0x006f6280
    void inventory_draw() {
        joan::canvas::ScreenScope canvas_scope(0x006f6280);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_inventory.hit();
        if (joan::orig::arg::as<u8>(joan::orig::as_panel(this)->is_open()) == 0) return;  // 0x004d8bb0
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(jo::self<jo::UnitAttScreen>(this)->unit_id)) != 0) return;  // 0x00428b70
        void* const unit = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::self<jo::UnitAttScreen>(this)->unit_id));  // 0x0041b3c0
        jo::self<jo::UnitAttScreen>(this)->hover_slot = -1;
        // Strings (0x005a1170 ctor: buf[0] = 0; 0x190 chars + NUL, slot 0x194 bytes).
        u8 s_sel[0x194];  // [ebp-0x19c]
        u8 s_tmp[0x194];  // [ebp-0x330]
        joan::orig::as_string(s_sel)->construct();  // 0x005a1170
        joan::orig::as_string(s_tmp)->construct();
        jo::BmpRes* const holder = jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt;
        const i16 hl_w = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<void*>(joan::orig::as_bmp_res(holder)->find(joan::orig::arg::str(img::str(kHighlight)))))->width);  // 0x00620b40
        const i16 hl_h = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<void*>(joan::orig::as_bmp_res(jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt)->find(joan::orig::arg::str(img::str(kHighlight)))))->height);
        const i32 mx = global<i32>(d::g_mouse_x) - 8;
        const i32 my = global<i32>(d::g_mouse_y) - 8;
        for (i32 i = 0; i < 6; ++i) {
            const img::Addr r = kSlots + (static_cast<u32>(i) << 4);
            if (gsx(r) > mx) continue;
            if (mx > gsx(r) + static_cast<i32>(hl_w)) continue;
            if (gsx(r + 2) > my) continue;
            if (my > gsx(r + 2) + static_cast<i32>(hl_h)) continue;
            jo::self<jo::UnitAttScreen>(this)->hover_slot = i;
            break;
        }
        if (jo::self<jo::UnitAttScreen>(this)->hover_slot != static_cast<std::uint32_t>(-1)) {
            const img::Addr r = kSlots + (static_cast<u32>(jo::self<jo::UnitAttScreen>(this)->hover_slot) << 4);
            const u32 c = argb(kM1);
            sprite(static_cast<u32>(gsx(r) + 8), static_cast<u32>(gsx(r + 2) + 8), 1, img::str(kHighlight),
                   jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt, c, 3, 0, 0, 1, kM1, kM1, kM1, kM1, 0, kM1, kM1);
        }
        {
            const u32 c = argb(kM1);
            sprite(8, 8, 1, &img::global<const char>(d::s_BG), jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt, c, 3, 0, 0, 1, kM1, kM1,
                   kM1, kM1, 0, kM1, kM1);
        }
        if (static_cast<i32>(jo::as_unit(unit)->level) != 0x1e) {
            const i32 bar_w = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<void*>(joan::orig::as_bmp_res(jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt)->find(joan::orig::arg::str(img::str(kExpbar)))))->width));
            i32 lo;
            if (static_cast<i32>(jo::as_unit(unit)->level) == 1) {
                lo = 0;
            } else {
                lo = jo::as_level_rec(joan::orig::arg::as<void*>(joan::orig::as_level_res(obj(d::g_level_res))->find(joan::orig::arg::i32(static_cast<i32>(jo::as_unit(unit)->level) - 1))))->experience;  // 0x004a4e60
            }
            const i32 hi = jo::as_level_rec(joan::orig::arg::as<void*>(joan::orig::as_level_res(obj(d::g_level_res))->find(joan::orig::arg::i32(static_cast<i32>(jo::as_unit(unit)->level)))))->experience;
            const u32 xp = static_cast<u32>(jo::as_unit(unit)->experience) - static_cast<u32>(lo);
            const i32 prod = static_cast<i32>(static_cast<u32>(bar_w) * xp);
            const i32 fill = prod / static_cast<i32>(static_cast<u32>(hi) - static_cast<u32>(lo));  // idiv
            const i32 bar_h = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<void*>(joan::orig::as_bmp_res(jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt)->find(joan::orig::arg::str(img::str(kExpbar)))))->height));
            const u32 c = argb(kM1);
            sprite(0x4a, 0xa8, 1, img::str(kExpbar), jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt, c, 3, 0, 0, 1, 0, 0, w16(fill), w16(bar_h), 0, kM1,
                   kM1);
        }
        u8 s_coins[0x194];  // [ebp-0x4f0]
        joan::orig::as_string(s_coins)->construct();
        const i32 gold = global<i32>(d::g_get_a596c4);
        std::uintptr_t coins;
        if (gold >= 6000) {
            coins = img::addr(d::s_coins4);  // "coins4"
        } else if (gold >= 4000) {
            coins = img::addr(d::s_coins3);  // "coins3"
        } else if (gold >= 2000) {
            coins = img::addr(d::s_coins2);  // "coins2"
        } else {
            coins = img::addr(d::s_coins1);  // "coins1"
        }
        joan::orig::as_string(s_coins)->assign(joan::orig::arg::str(reinterpret_cast<const char*>(coins)));  // 0x005a1440
        {
            const u32 c = argb(kM1);
            void* const h = jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt;
            const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_string(s_coins)->c_str());  // 0x00417d90
            sprite(0x18, 0xcc, 1, name, h, c, 3, 0, 0, 1, kM1, kM1, kM1, kM1, 0, kM1, kM1);
        }
        // Portrait.
        if (jo::self<jo::UnitAttScreen>(this)->hover_slot != static_cast<std::uint32_t>(-1)) {
            const img::Addr r = kSlots + (static_cast<u32>(jo::self<jo::UnitAttScreen>(this)->hover_slot) << 4);
            if (static_cast<i32>(jo::as_unit(unit)->attr_points) != 0) {
                const i32 px = gsx(r + 4) + 0xc;
                const i32 py = gsx(r + 0xa) + 8;
                joan::orig::as_button(&jo::self<jo::UnitAttScreen>(this)->add_button)->set_rect(joan::orig::arg::u32(px), joan::orig::arg::u32(py), joan::orig::arg::u32(kM1), joan::orig::arg::u32(kM1), joan::orig::arg::u32(u32{0}));  // 0x00623580
                joan::orig::as_panel(&jo::self<jo::UnitAttScreen>(this)->add_button)->reset_6b7cf0();                            // 0x006b7cf0
                joan::orig::as_button(&jo::self<jo::UnitAttScreen>(this)->add_button)->draw(1u, 7);                      // 0x00622cc0
            } else {
                joan::orig::as_widget(&jo::self<jo::UnitAttScreen>(this)->add_button)->reset_child();  // 0x0062b660
            }
        } else {
            joan::orig::as_widget(&jo::self<jo::UnitAttScreen>(this)->add_button)->reset_child();
        }
        joan::orig::as_button(&jo::self<jo::UnitAttScreen>(this)->cb_button)->draw(1u, 7);
        u32 white_obj = 0;  // [ebp-0x500]
        joan::orig::as_colour(&white_obj)->construct(0xffu, 0xffu, 0xffu, 0xffu);  // 0x0042a260
        i16 lines = 0;   // [ebp-0x504] (0x00627d80 writes 0 first)
        i16 height = 0;  // [ebp-0x508]
        const char* const name_text = jo::as_unit(unit)->name_;
        joan::orig::as_text_box(obj(d::g_font_goudy_bold_gold_24))->measure_lines(0x24, 0x1f, 0x108, 0x66, joan::orig::arg::str(name_text), 5, 0, &lines, &height, joan::orig::arg::u32(kOne), joan::orig::arg::u32(kOne));  // 0x00627d80
        if (static_cast<i32>(lines) <= 2) {
            joan::orig::as_text_renderer(obj(d::g_font_goudy_bold_gold_24))->draw_wrapped(0x24u, 0x1fu, 0x108u, 0x66u, 1u, joan::orig::arg::str(name_text), joan::orig::arg::u32(white_obj), 5u, 0u, 0u, 0u, 0u, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), 1u);  // 0x00626e00
        }
        {
            const char* const s = joan::orig::as_misc(obj(kNumFmt))->format_int_short(joan::orig::arg::i16(w16(static_cast<i32>(jo::as_unit(unit)->level))), 1u);  // 0x004e1360
            text3(d::g_font_goudy_bold_gold_24, 0x1a, 0x7b, 0x44, 0xad, s, white_obj, 0, 0, 0, 0);
        }
        {
            const char* const s = joan::orig::as_misc(obj(kNumFmt))->format_int(joan::orig::arg::u32(jo::as_unit(unit)->experience), 4u);  // 0x004d69a0
            text2(d::g_font_18pt_dark_numbers, 0x108, 0x7a, s, white_obj, 0, 0, 0, 0);
        }
        if (static_cast<i32>(jo::as_unit(unit)->level) == 0x1e) {
            const char* const s = joan::orig::as_txt_res(obj(kStrings))->unitatt_text_15();  // 0x004dd520
            text2(d::g_font_18pt_dark_numbers, 0x108, 0x8f, s, white_obj, 0, 0, 0, 0);
        } else {
            void* const lvl = joan::orig::arg::as<void*>(joan::orig::as_level_res(obj(d::g_level_res))->find(joan::orig::arg::i32(static_cast<i32>(jo::as_unit(unit)->level))));
            const char* const s = joan::orig::as_misc(obj(kNumFmt))->format_int(joan::orig::arg::u32(jo::as_level_rec(lvl)->experience), 4u);
            text2(d::g_font_18pt_dark_numbers, 0x108, 0x8f, s, white_obj, 0, 0, 0, 0);
        }
        {
            const char* const s = joan::orig::as_misc(obj(kNumFmt))->format_int(joan::orig::arg::u32(global<i32>(d::g_get_a596c4)), 4u);
            text2(d::g_font_24pt_large_numbers_gold, 0x108, 0xd2, s, white_obj, 0, 0, 0, 0);
        }
        // Slot values: the hovered slot's text goes to s_sel (drawn at the slot below), the others
        // are drawn in place.
        if (jo::self<jo::UnitAttScreen>(this)->hover_slot != 0) {
            const i32 v = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->hit_points_max());  // 0x00523320
            joan::orig::libc::sprintf(joan::orig::arg::str(joan::orig::as_string(s_tmp)->c_str()), joan::orig::arg::str(img::str(kRatio)), jo::as_unit(unit)->hit_points, v);
            const char* const s = joan::orig::as_string(s_tmp)->c_str();
            text2(d::g_font_18pt_attribute_numbers_out, static_cast<u32>(gsx(d::g_a277dc) + 8), static_cast<u32>(gsx(d::g_a277de) + 8), s,
                  white_obj, 0, 0, 0, 0);
        } else {
            const i32 v = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->hit_points_max());
            joan::orig::libc::sprintf(joan::orig::arg::str(joan::orig::as_string(s_sel)->c_str()), joan::orig::arg::str(img::str(kRatio)), jo::as_unit(unit)->hit_points, v);
        }
        if (jo::self<jo::UnitAttScreen>(this)->hover_slot != 1) {
            const i32 v = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->value_149());  // 0x00523700
            joan::orig::libc::sprintf(joan::orig::arg::str(joan::orig::as_string(s_tmp)->c_str()), joan::orig::arg::str(img::str(kRatio)), jo::as_unit(unit)->mana, v);
            const char* const s = joan::orig::as_string(s_tmp)->c_str();
            text2(d::g_font_18pt_attribute_numbers_out, static_cast<u32>(gsx(d::g_a277ec) + 8), static_cast<u32>(gsx(d::g_a277ee) + 8), s,
                  white_obj, 0, 0, 0, 0);
        } else {
            const i32 v = joan::orig::arg::as<i32>(joan::orig::as_unit(unit)->value_149());
            joan::orig::libc::sprintf(joan::orig::arg::str(joan::orig::as_string(s_sel)->c_str()), joan::orig::arg::str(img::str(kRatio)), jo::as_unit(unit)->mana, v);
        }
        struct Stat {
            i32 slot;
            std::uintptr_t getter;  // thunk
            img::Addr pos;     // x at pos, y at pos + 2
        };
        const Stat stats[4] = {{2, fn::code_value(fn::Unit_stat_3), d::g_a277fc},   // 0x005237e0
                               {3, fn::code_value(fn::Unit_level_sum), d::g_a2780c},   // 0x005238c0
                               {4, fn::code_value(fn::Unit_stat_4), d::g_a2781c},   // 0x00523860
                               {5, fn::code_value(fn::Unit_stat_6), d::g_a2782c}};  // 0x00523920
        for (const Stat& st : stats) {
            if (jo::self<jo::UnitAttScreen>(this)->hover_slot != static_cast<std::uint32_t>(st.slot)) {
                const char* const s = num(thiscall<i32>(st.getter, unit), 4);
                text2(d::g_font_18pt_attribute_numbers_out, static_cast<u32>(gsx(st.pos) + 8), static_cast<u32>(gsx(st.pos + 2) + 8), s,
                      white_obj, 0, 0, 0, 0);
            } else {
                const char* const s = num(thiscall<i32>(st.getter, unit), 4);
                joan::orig::as_string(s_sel)->assign(joan::orig::arg::str(s));  // 0x005a1440
            }
        }
        if (jo::self<jo::UnitAttScreen>(this)->hover_slot != static_cast<std::uint32_t>(-1)) {
            const char* const s = joan::orig::as_string(s_sel)->c_str();
            const img::Addr r = kSlots + (static_cast<u32>(jo::self<jo::UnitAttScreen>(this)->hover_slot) << 4);
            const i32 y = gsx(r + 6) + 8;
            const i32 x = gsx(r + 4) + 8;
            text2(d::g_font_18pt_attribute_numbers_over, static_cast<u32>(x), static_cast<u32>(y), s, white_obj, 0, 0, 0, 0);
        }
        text3(d::g_font_18pt_dark_numbers, 0xd8, 0x1a2, 0x11b, 0x1b7, joan::orig::as_misc(obj(kNumFmt))->format_int_short(joan::orig::arg::i16(w16(static_cast<i32>(jo::as_unit(unit)->kills))), 4u),
              white_obj, 0, 0, 0, 1);
        text3(d::g_font_18pt_dark_numbers, 0xd8, 0x1be, 0x11b, 0x1d5, joan::orig::as_misc(obj(kNumFmt))->format_int_short(joan::orig::arg::i16(w16(static_cast<i32>(jo::as_unit(unit)->kills_high_rank))), 4u),
              white_obj, 0, 0, 0, 0);
        text3(d::g_font_18pt_dark_numbers, 0xd8, 0x1da, 0x11b, 0x1f1, joan::orig::as_misc(obj(kNumFmt))->format_int_short(joan::orig::arg::i16(w16(static_cast<i32>(jo::view<i16>(jo::as_unit(unit)->crit_hits)))), 4u),
              white_obj, 0, 0, 0, 0);
        text3(d::g_font_18pt_dark_numbers, 0xd8, 0x1f5, 0x11b, 0x20c, joan::orig::as_misc(obj(kNumFmt))->format_int_short(joan::orig::arg::i16(w16(static_cast<i32>(jo::view<i16>(jo::as_unit(unit)->crit_kills)))), 4u),
              white_obj, 0, 0, 0, 0);
        text2(d::g_font_18pt_light_numbers, 0x108, 0x21c, joan::orig::as_misc(obj(kNumFmt))->format_int_short(joan::orig::arg::i16(w16(static_cast<i32>(jo::as_unit(unit)->attr_points))), 1u), white_obj,
              0, 0, 0, 0);
        text2(d::g_font_18pt_light_numbers, 0x108, 0x230, joan::orig::as_misc(obj(kNumFmt))->format_int_short(joan::orig::arg::i16(w16(static_cast<i32>(jo::as_unit(unit)->stat_points))), 1u), white_obj,
              0, 0, 0, 0);
        if (static_cast<i32>(jo::self<jo::UnitAttScreen>(this)->show_inventory) != 0) {
            joan::orig::as_inventory(obj(d::g_a488e0))->draw_items_6d6740(joan::orig::arg::u32(jo::self<jo::UnitAttScreen>(this)->unit_id));  // 0x006d6740
        } else {
            joan::orig::as_panel(obj(d::g_combat_screen))->draw_6ef9e0(joan::orig::arg::u32(jo::self<jo::UnitAttScreen>(this)->unit_id));  // 0x006ef9e0
        }
        // Tooltip after 250 ms.
        if (global<u32>(d::g_game_time_ms) - jo::self<jo::UnitAttScreen>(this)->mouse_moved_ms <= 0xfa) return;
        const char* help_a = nullptr;  // [ebp-0x50c]
        const char* help_b = nullptr;  // [ebp-0x510]
        if (jo::self<jo::UnitAttScreen>(this)->hover_slot != static_cast<std::uint32_t>(-1)) {
            switch (static_cast<u32>(jo::self<jo::UnitAttScreen>(this)->hover_slot)) {
                case 0:
                    help_a = joan::orig::as_menu(obj(kStrings))->unitatt_01();  // 0x004dd160
                    help_b = joan::orig::as_txt_res(obj(kStrings))->unitatt_text_2();  // 0x004dd190
                    break;
                case 1:
                    help_a = joan::orig::as_txt_res(obj(kStrings))->unitatt_text_03();  // 0x004dd1c0
                    help_b = joan::orig::as_menu(obj(kStrings))->unitatt_4();  // 0x004dd1f0
                    break;
                case 2:
                    help_a = joan::orig::as_txt_res(obj(kStrings))->unitatt_text_5();  // 0x004dd220
                    help_b = joan::orig::as_menu(obj(kStrings))->unitatt_06();  // 0x004dd250
                    break;
                case 3:
                    help_a = joan::orig::as_txt_res(obj(kStrings))->unitatt_text_7();  // 0x004dd280
                    help_b = joan::orig::as_txt_res(obj(kStrings))->unitatt_text_08();  // 0x004dd2b0
                    break;
                case 4:
                    help_a = joan::orig::as_txt_res(obj(kStrings))->unitatt_text_09();  // 0x004dd2e0
                    help_b = joan::orig::as_txt_res(obj(kStrings))->unitatt_text_10();  // 0x004dd310
                    break;
                case 5:
                    help_a = joan::orig::as_txt_res(obj(kStrings))->unitatt_text_0b();  // 0x004dd340
                    help_b = joan::orig::as_menu(obj(kStrings))->unitatt_0c();  // 0x004dd370
                    break;
                default:  // unreachable (see the header): uninitialised slots -> 0
                    break;
            }
            tooltip(help_a, help_b);
            return;
        }
        struct Area {
            i32 x1, y1, x2, y2;
            std::uintptr_t a, b;  // thunks
        };
        const Area areas[4] = {{0x49, 0x7e, 0xa8, 0x89, fn::code_value(fn::Menu_unitatt_0d), fn::code_value(fn::UnitAtt_open_0e)},    // 0x004dd3a0 / 0x004dd3d0
                               {0x49, 0x96, 0xa8, 0xa1, fn::code_value(fn::UnitAtt_open_15_4dd400), fn::code_value(fn::UnitAtt_open_10_4dd430)},    // 0x004dd400 / 0x004dd430
                               {0x18, 0x222, 0xdd, 0x22c, fn::code_value(fn::Menu_unitatt_11), fn::code_value(fn::UnitAtt_open_18)},  // 0x004dd460 / 0x004dd490
                               {0x18, 0x236, 0xdd, 0x240, fn::code_value(fn::UnitAtt_open_13), fn::code_value(fn::UnitAtt_open_14)}}; // 0x004dd4c0 / 0x004dd4f0
        for (const Area& ar : areas) {
            if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(kMouse))->inside(joan::orig::arg::i32(ar.x1), joan::orig::arg::i32(ar.y1), joan::orig::arg::i32(ar.x2), joan::orig::arg::i32(ar.y2))) != 0) {  // 0x00593980
                help_a = thiscall<const char*>(ar.a, obj(kStrings));
                help_b = thiscall<const char*>(ar.b, obj(kStrings));
                tooltip(help_a, help_b);
                return;
            }
        }
    }
    // 0x006b8db0 (thunk 0x004027e3, ret 0x14) at the mouse (words of 0xa516c4 / 0xa516c8).
    static void tooltip(const char* a, const char* b) {
        joan::orig::as_help_box(obj(d::g_tooltip))->draw_box(joan::orig::arg::u32(w16(global<i32>(d::g_mouse_x))), joan::orig::arg::u32(w16(global<i32>(d::g_mouse_y))), a, b, joan::orig::arg::u32(u32{1}));  // 0x006b8db0
    }
};

JOAN_REPLACE("game/b116_07", "Inventory::draw", 0x006f6280, &H::inventory_draw);
JOAN_CANVAS_SELF(0x006f6280);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_07
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {


constexpr u32 M1 = 0xffffffffu;
using joan::orig::helpers::units;

JOAN_COUNTER(c_panel_6f7780, "Panel::update_6f7780");

struct H {
    // ------------------------------------------------------------------ 0x006f7780
    void set_button(img::Addr a, img::Addr b, img::Addr c) {
        joan::orig::as_menu_button(&jo::self<jo::UnitAttScreen>(this)->cb_button)->setup(joan::orig::arg::i16(0xf6u), joan::orig::arg::i16(0x13u), joan::orig::arg::mptr(jo::self<jo::UnitAttScreen>(this)->bmp_iunitatt), joan::orig::arg::str(str(a)), joan::orig::arg::str(str(b)), joan::orig::arg::str(str(c)), joan::orig::arg::str(0u), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)));  // 0x006211d0
    }
    u8 panel_update() {
        c_panel_6f7780.hit();
        if (static_cast<i8>(joan::orig::arg::as<u8>(joan::orig::as_panel(this)->is_open())) == 0) return 0;  // 0x004d8bb0
        if (global<i32>(d::g_mouse_x) != jo::self<jo::UnitAttScreen>(this)->last_mouse_x || global<i32>(d::g_mouse_y) != jo::self<jo::UnitAttScreen>(this)->last_mouse_y) {
            jo::view<u16>(jo::self<jo::UnitAttScreen>(this)->last_mouse_x) = global<u16>(d::g_mouse_x);
            jo::view<u16>(jo::self<jo::UnitAttScreen>(this)->last_mouse_y) = global<u16>(d::g_mouse_y);
            jo::self<jo::UnitAttScreen>(this)->mouse_moved_ms = global<u32>(d::g_game_time_ms);
        }
        if ((joan::orig::arg::as<u32>(joan::orig::as_inventory(obj(d::g_a488e0))->is_closed()) & 0xffu) != 0) {  // 0x006de1d0
            if (global<i32>(d::g_key) == 0x1b || joan::orig::arg::as<i32>(joan::orig::as_mode(obj(d::g_screen_mgr))->check_6f5a70()) != 0) {  // 0x006f5a70
                joan::orig::as_panel(this)->close();  // 0x006f7e90
                if (global<u32>(d::g_screen_pending) != 0) {
                    joan::orig::as_menu(obj(d::g_screen_mgr))->set_state_6f5cc0();  // 0x006f5cc0
                } else {
                    joan::orig::as_triple(obj(d::g_screen_mgr))->clear();  // 0x006f5e90
                }
                return 1;
            }
            void* const unit = joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(jo::self<jo::UnitAttScreen>(this)->unit_id));
            if (jo::view<i32>(jo::self<jo::UnitAttScreen>(this)->hover_slot) != -1 &&
                joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::UnitAttScreen>(this)->add_button)->mouse(0, 0, joan::orig::arg::i32(M1), 0, 0, 0)) != 0) {  // 0x006218c0
                static const u32 kOrder[6] = {6, 7, 1, 3, 2, 5};                         // table 0x6f7bfc
                const u32 sel = jo::self<jo::UnitAttScreen>(this)->hover_slot;
                if (sel <= 5) joan::orig::as_unit(unit)->buy_stat(joan::orig::arg::i32(kOrder[sel]), 0, 0u);  // 0x0050d640
                return 1;
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(&jo::self<jo::UnitAttScreen>(this)->cb_button)->mouse(0, 0, joan::orig::arg::i32(M1), 0, 0, 0)) != 0) {
                if (jo::view<i8>(jo::self<jo::UnitAttScreen>(this)->show_inventory) != 0) {
                    joan::orig::as_menu_button(&jo::self<jo::UnitAttScreen>(this)->cb_button)->clear();               // 0x006217e0
                    set_button(d::s_WP_OT, d::s_WP_DW, d::s_WP_OV);       // WP_OT / WP_DW / WP_OV
                    jo::self<jo::UnitAttScreen>(this)->show_inventory = 0;
                } else {
                    joan::orig::as_menu_button(&jo::self<jo::UnitAttScreen>(this)->cb_button)->clear();
                    set_button(d::s_CB_OT, d::s_CB_DW, d::s_CB_OV);  // CB_OT / CB_DW / CB_OV
                    jo::self<jo::UnitAttScreen>(this)->show_inventory = 1;
                }
                return 1;
            }
            if (jo::view<i8>(jo::self<jo::UnitAttScreen>(this)->show_inventory) != 0 && jo::self<jo::UnitAttScreen>(this)->cb_button.i8_8 != 0  /* +0x370: the CB button's +8 */ && jo::as_unit(unit)->stat_points != 0 &&
                global<i8>(d::g_a4d4d6) == 0 && global<u32>(d::g_game_time_ms) - jo::self<jo::UnitAttScreen>(this)->blink_ms >= 0x1f4u) {
                joan::orig::as_menu_button(&jo::self<jo::UnitAttScreen>(this)->cb_button)->clear();
                if (jo::view<i8>(jo::self<jo::UnitAttScreen>(this)->blink_on) != 0) {
                    set_button(d::s_CB_OT, d::s_CB_DW, d::s_CB_OV);  // CB_OT / CB_DW / CB_OV
                    jo::self<jo::UnitAttScreen>(this)->blink_on = 0;
                } else {
                    jo::self<jo::UnitAttScreen>(this)->blink_on = 1;
                    set_button(d::s_CB_OTG, d::s_CB_DWG, d::s_CB_OVG);  // CB_OTG / CB_DWG / CB_OVG
                }
                jo::self<jo::UnitAttScreen>(this)->blink_ms = global<u32>(d::g_game_time_ms);
            }
            if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->rect_mode(joan::orig::arg::i32(global<u32>(d::g_mouse_limit_x1)), joan::orig::arg::i32(global<u32>(d::g_mouse_limit_y1)), joan::orig::arg::i32(global<u32>(d::g_mouse_limit_x2)), joan::orig::arg::i32(global<u32>(d::g_mouse_limit_y2)), 0)) != 0) {  // 0x005940f0
                const u32 my = joan::orig::arg::as<u32>(joan::orig::as_table(obj(d::g_mouse))->at_676(0));  // 0x0043c3f0
                const u32 mx = joan::orig::arg::as<u32>(joan::orig::as_table(obj(d::g_mouse))->at_672(0));  // 0x0043c3c0
                const u32 id = joan::orig::arg::as<u32>(joan::orig::as_hud(obj(d::g_player_hud))->hit_portrait(joan::orig::arg::i32(mx), joan::orig::arg::i32(my)));  // 0x006ffd90
                if (joan::orig::as_dynarray(units())->empty_at(joan::orig::arg::i32(id)) == 0 &&
                    jo::as_unit(joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(id)))->rank >= 4) {
                    (void)joan::orig::as_dynarray(units())->ptr_at(joan::orig::arg::i32(id));
                    jo::self<jo::UnitAttScreen>(this)->unit_id = id;
                    joan::orig::as_panel(&jo::self<jo::UnitAttScreen>(this)->cb_button)->reset_6b7cf0();  // 0x006b7cf0
                    if (joan::orig::arg::as<i32>(joan::orig::unit::is_hero(joan::orig::arg::i32(id))) == 0) {  // 0x0042ff90
                        joan::orig::as_panel(this)->set_mode(static_cast<char>(joan::orig::arg::i32(1u)));           // 0x006f7f20
                        joan::orig::as_widget(&jo::self<jo::UnitAttScreen>(this)->cb_button)->reset_child();  // 0x0062b660
                    }
                    global<u32>(d::g_a4d75f) = global<u32>(d::g_game_time_ms);
                    global<u16>(d::g_a4d763) = 0;
                    return 1;
                }
            }
        }
        if (jo::view<i8>(jo::self<jo::UnitAttScreen>(this)->show_inventory) != 0) {
            joan::orig::as_inventory(obj(d::g_a488e0))->update_6d8ac0(joan::orig::arg::u32(jo::self<jo::UnitAttScreen>(this)->unit_id));  // 0x006d8ac0
        } else {
            joan::orig::as_panel(obj(d::g_combat_screen))->handle_buttons(joan::orig::arg::u32(jo::self<jo::UnitAttScreen>(this)->unit_id));  // 0x006f0a90
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Panel::update_6f7780", 0x006f7780, &H::panel_update);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

struct H {
    // ------------------------------------------------------------------------ 0x006b1f10 etc.
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::nop_6f7d40", 0x006f7d40, &H::nop);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
namespace {
namespace game_b85_05 {


JOAN_COUNTER(c_open_level, "Screen::open_level");

struct H {

    // ------------------------------------------------------------------ 0x006f7d80
    void open_level(std::int32_t level) {
        c_open_level.hit();
        jo::self<jo::UnitAttScreen>(this)->is_open = 1;
        jo::self<jo::UnitAttScreen>(this)->unit_id = static_cast<std::uint32_t>(level);
        joan::orig::as_panel(&jo::self<jo::UnitAttScreen>(this)->cb_button)->reset_6b7cf0();  // 0x006b7cf0
        if (joan::orig::arg::as<std::int32_t>(joan::orig::unit::is_hero(joan::orig::arg::i32(level))) == 0) {  // 0x0042ff90
            joan::orig::as_panel(this)->set_mode(static_cast<char>(joan::orig::arg::i32(1)));             // 0x006f7f20
            joan::orig::as_widget(&jo::self<jo::UnitAttScreen>(this)->cb_button)->reset_child();  // 0x0062b660
        }
        joan::orig::as_unit_attr_screen(this)->load_sprites();  // 0x006f6120
        jo::self<jo::UnitAttScreen>(this)->blink_ms = global<std::uint32_t>(d::g_game_time_ms);
        global<std::uint32_t>(d::g_a4d75f) = global<std::uint32_t>(d::g_game_time_ms);
        global<std::uint16_t>(d::g_a4d763) = 0;
        joan::orig::as_cursor(obj(d::g_mouse))->reset_pos();  // 0x00592c30
        if (global<std::uint8_t>(d::g_game_running) != 0) {
            global<std::uint8_t>(d::g_game_running) = 0;
            global<std::uint16_t>(d::g_saved_game_speed) = static_cast<std::uint16_t>(
                joan::hook::x87::ftol(static_cast<double>(global<float>(d::g_game_speed))));
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(0u);  // 0x0042f930
        }
        jo::view<std::uint16_t>(jo::self<jo::UnitAttScreen>(this)->last_mouse_x) = 0xffff;
        jo::view<std::uint16_t>(jo::self<jo::UnitAttScreen>(this)->last_mouse_y) = 0xffff;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("Screen::open_level", 0x006f7d80, &H::open_level);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Panel::reset_6f60d0() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b95_08::H::panel_reset>(this)); }  // 0x6f60d0
auto joan::orig::UnitAttrScreen::load_sprites() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_01::H::load_shared_textures>(this)); }  // 0x6f6120
auto joan::orig::Hud::release() -> void { joan::bind::member<&::game_b94_08::H::hud_release>(this); }  // 0x6f61e0
auto joan::orig::Screen::open_level(std::int32_t a0) -> void { joan::bind::member<&::game_b85_05::H::open_level>(this, a0); }  // 0x6f7d80
auto joan::orig::Panel::close() -> void { joan::bind::member<&::game_b95_08::H::panel_close>(this); }  // 0x6f7e90
auto joan::orig::Panel::set_mode(char a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_01::H::set_mode>(this, a0)); }  // 0x6f7f20
