// game/unknown_i_attcmb.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_attcmb.cpp;
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
using i32 = std::int32_t;

using joan::call::thiscall;

using u16 = std::uint16_t;

using joan::call::cdecl_call;

using joan::img::obj;

using joan::hook::global;

using joan::img::str;

using i16 = std::int16_t;

using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b105_19.cpp (game/b105_19)
namespace {
namespace game_b105_19 {


struct ListLayout {  // 0x6c bytes passed by value to 0x00761990
    u32 d[27];
};
static_assert(sizeof(ListLayout) == 0x6c, "by-value 0x6c-byte argument");

JOAN_COUNTER(c_init_6ef3d0, "Panel::init_6ef3d0");

// Static initialiser: `call Init` (direct), eax as the callee leaves it.
template <fn::Code Init>
u32 JOAN_CDECL static_init() {
    return cdecl_call<u32>(Init);
}

struct H {
    // 0x006ef3d0
    u32 init_6ef3d0() {
        c_init_6ef3d0.hit();
        jo::self<jo::CombatScreen>(this)->u32_8 = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_iattcomb));
        jo::self<jo::CombatScreen>(this)->tex_iattcomb = nullptr;
        jo::self<jo::CombatScreen>(this)->u32_c = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_commele));
        jo::self<jo::CombatScreen>(this)->tex_commele = nullptr;
        for (i32 i = 0; i < 3; ++i) {
            button(&(&jo::self<jo::CombatScreen>(this)->buttons_0)[i], 0x2d6, static_cast<u32>(i * 0x28 + 0x1d7), jo::self<jo::CombatScreen>(this)->u32_8,
                   d::s_LEARN_OT, d::s_LEARN_DW, d::s_LEARN_OV);  // LEARN_OT / LEARN_DW / LEARN_OV
            button(&(&jo::self<jo::CombatScreen>(this)->buttons_13)[i], 0x15c, static_cast<u32>(i * 0x28 + 0x1d4), jo::self<jo::CombatScreen>(this)->u32_8,
                   d::s_ATT_OT, d::s_ATT_OV, d::s_ATT_OV);  // ATT_OT / ATT_OV / ATT_OV
        }
        for (i32 i = 0; i < 10; ++i)
            button(&(&jo::self<jo::CombatScreen>(this)->buttons_3)[i], 0x15d, static_cast<u32>(i * 0x1c + 0x69), jo::self<jo::CombatScreen>(this)->u32_8,
                   d::s_LIST_OT, d::s_LIST_OV, d::s_LIST_OV);  // LIST_OT / LIST_OV / LIST_OV
        button(&jo::self<jo::CombatScreen>(this)->buttons_17, 0x2b7, 0x6c, img::obj(d::g_bmp_commele), d::s_ADD_OT, d::s_ADD_DW, d::s_ADD_OV);   // ADD_*
        button(&jo::self<jo::CombatScreen>(this)->buttons_18, 0x1c0, 0x1a4, img::obj(d::g_bmp_commele), d::s_SBUP_OUT, d::s_SBUP_DW, d::s_SBUP_OV);  // SBUP_*
        button(&jo::self<jo::CombatScreen>(this)->buttons_19, 0x288, 0x1a4, img::obj(d::g_bmp_commele), d::s_SBDW_OUT, d::s_SBDW_DW, d::s_SBDW_OV);  // SBDW_*
        const ListLayout layout{{0x14, 0x14, 0x4, 0x4, 0x14, 0x14, 0x2, 0x4, 0x2, 0x1e, 0x0, 0x0, 0x0, 0xf7,
                                 0x0, 0x0, 0xe, 0x1f, 0xe, 0xda, 0x0, 0x4, 0x4, 0x0, 0x0, 0x21, 0xd5}};
        joan::orig::as_scroll_bar(&jo::self<jo::CombatScreen>(this)->scroll_bar)->set_layout(layout);  // 0x00761990
        const u32 r = joan::orig::arg::as<u32>(joan::orig::as_scroll_bar(&jo::self<jo::CombatScreen>(this)->scroll_bar)->build(jo::self<jo::CombatScreen>(this)->u32_c, jo::self<jo::CombatScreen>(this)->tex_commele, 1, 0x2e8, 0x69, 0xa, 0, 0, 0));  // 0x00761ae0
        jo::self<jo::CombatScreen>(this)->dirty = 0;
        jo::view<u16>(jo::self<jo::CombatScreen>(this)->scroll) = 0;
        return r;
    }

    static void button(void* b, u32 x, u32 y, void* surface, img::Addr ot, img::Addr dw, img::Addr ov) {
        joan::orig::as_menu_button(b)->setup(joan::orig::arg::i16(x), joan::orig::arg::i16(y), surface, joan::orig::arg::str(img::addr(ot)), joan::orig::arg::str(img::addr(dw)), joan::orig::arg::str(img::addr(ov)), joan::orig::arg::str(0u), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(0u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)), static_cast<std::uint8_t>(joan::orig::arg::u32(1u)));  // 0x006211d0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_19", name, addr, fn)
B("init_6ef230", 0x006ef230, &static_init<fn::init_a6a568>);
B("Panel::init_6ef3d0", 0x006ef3d0, &H::init_6ef3d0);
#undef B

}  // namespace game_b105_19
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {

static void JOAN_CDECL init_a6a568() { joan::orig::as_colour(obj(d::g_a6a568))->construct(0xffu, 0xffu, 0xffu, 0x40u); }  // 0x006ef250

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("init_a6a568", 0x006ef250, &init_a6a568);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {

using joan::call::stdcall_call;

using joan::orig::helpers::widget_ctor;  // 0x00620ee0

JOAN_COUNTER(c_ctor_6ef290, "Screen::ctor_6ef290");

struct H {
    // ------------------------------------------------------------------ 0x006ef290
    void* ctor_6ef290() {
        c_ctor_6ef290.hit();
        const std::uintptr_t ctor = fn::code_value(fn::Widget_ctor_620ee0);  // 0x00620ee0
        const std::uintptr_t dtor = fn::code_value(fn::Widget_dtor_6211a0);  // 0x006211a0
        constexpr u32 kButton = sizeof(jo::Button);  // 0x346 on i686
        joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::CombatScreen>(this)->buttons_0)), kButton, 3, ctor, dtor);
        joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::CombatScreen>(this)->buttons_3)), kButton, 0xa, ctor, dtor);
        joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::CombatScreen>(this)->buttons_13)), kButton, 3, ctor, dtor);
        widget_ctor(&jo::self<jo::CombatScreen>(this)->buttons_16);
        widget_ctor(&jo::self<jo::CombatScreen>(this)->buttons_17);
        widget_ctor(&jo::self<jo::CombatScreen>(this)->buttons_18);
        widget_ctor(&jo::self<jo::CombatScreen>(this)->buttons_19);
        joan::orig::as_scroll_bar(&jo::self<jo::CombatScreen>(this)->scroll_bar)->construct();  // 0x00761720
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Screen::ctor_6ef290", 0x006ef290, &H::ctor_6ef290);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b101_15.cpp (game/b101_15)
namespace {
namespace game_b101_15 {

u32 button_free(void* b) { return joan::orig::arg::as<u32>(joan::orig::as_menu_button(b)->clear()); }  // 0x006217e0

JOAN_COUNTER(c_free_buttons, "Screen4188::free_buttons");

struct H {
    // 0x006ef7c0 (ret)
    u32 free_buttons() {
        c_free_buttons.hit();
        joan::orig::as_combat_screen(this)->free_sprites();  // 0x006ef970
        for (i32 i = 0; i < 3; ++i) {
            button_free(&(&jo::self<jo::CombatScreen>(this)->buttons_0)[i]);   // buttons_0 .. 2
            button_free(&(&jo::self<jo::CombatScreen>(this)->buttons_13)[i]);  // buttons_13 .. 15
        }
        for (i32 i = 0; i < 10; ++i) button_free(&(&jo::self<jo::CombatScreen>(this)->buttons_3)[i]);  // buttons_3 .. 12
        joan::orig::as_scroll_bar(&jo::self<jo::CombatScreen>(this)->scroll_bar)->reset();  // 0x00762880
        button_free(&jo::self<jo::CombatScreen>(this)->buttons_16);
        button_free(&jo::self<jo::CombatScreen>(this)->buttons_17);
        button_free(&jo::self<jo::CombatScreen>(this)->buttons_18);
        return button_free(&jo::self<jo::CombatScreen>(this)->buttons_19);
    }
};

#define B15(name, addr, fn) JOAN_REPLACE("game/b101_15", name, addr, fn)
B15("Screen4188::free_buttons", 0x006ef7c0, &H::free_buttons);
#undef B15

}  // namespace game_b101_15
}  // namespace

// ---------------------------------------- from modules/game_b87_06.cpp (game/b87_06)
namespace {
namespace game_b87_06 {


constexpr img::Addr kResources = d::g_texture_array;

JOAN_COUNTER(c_combat_load, "CombatPanel::load_sprites");

std::int32_t res_mode() { return joan::orig::arg::as<std::int32_t>(joan::orig::as_texture_manager(obj(kResources))->mode()); }                       // 0x0060a720
void res_set_mode(std::int32_t m) { joan::orig::as_texture_manager(obj(kResources))->set_mode(joan::orig::arg::u32(m)); }                               // 0x0060a740
void* res_load(const char* name) { return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(kResources))->load(joan::orig::arg::str(name), 1, 0u)); }          // 0x006099c0

struct H {
    // ------------------------------------------------------------ 0x006ef8d0 (ret 0)
    void combat_load_sprites() {
        c_combat_load.hit();
        const std::int32_t mode = res_mode();
        res_set_mode(1);
        if (jo::view<void*>(jo::self<jo::CombatScreen>(this)->tex_iattcomb) == nullptr) jo::view<void*>(jo::self<jo::CombatScreen>(this)->tex_iattcomb) = res_load(&img::global<const char>(d::s_IATTCOMB));  // 'IATTCOMB'
        if (jo::view<void*>(jo::self<jo::CombatScreen>(this)->tex_commele) == nullptr) jo::view<void*>(jo::self<jo::CombatScreen>(this)->tex_commele) = res_load(&img::global<const char>(d::s_COMMELE));  // 'COMMELE'
        res_set_mode(mode);
    }
};

#define B06(name, addr, fn) JOAN_REPLACE("game/b87_06", name, addr, fn)
B06("CombatPanel::load_sprites", 0x006ef8d0, &H::combat_load_sprites);
#undef B06

}  // namespace game_b87_06
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {


void* const kRes = obj(d::g_texture_array);

void res_free(void* handle) { joan::orig::as_texture_manager(kRes)->release(joan::orig::arg::mptr(handle)); }  // 0x0060a550

struct H {
    void free2() {  // 0x006ef970
        if (jo::self<jo::CombatScreen>(this)->tex_iattcomb != nullptr) {
            res_free(jo::self<jo::CombatScreen>(this)->tex_iattcomb);
            jo::self<jo::CombatScreen>(this)->tex_iattcomb = nullptr;
        }
        if (jo::self<jo::CombatScreen>(this)->tex_commele != nullptr) {
            res_free(jo::self<jo::CombatScreen>(this)->tex_commele);
            jo::self<jo::CombatScreen>(this)->tex_commele = nullptr;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Res2::free", 0x006ef970, &H::free2);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b109_17.cpp (game/b109_17)
namespace {
namespace game_b109_17 {

JOAN_COUNTER(c_panel, "Panel::draw_6ef9e0");

// ------------------------------------------------------------------ shared draw helpers
// Colour object 0x0042a260 (r, g, b, a) through thunk 0x004098b8; returns the packed dword.
u32 rgba(i32 r, i32 g, i32 b, i32 a) {
    u32 slot = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&slot)->construct(r, g, b, a));  // 0x0042a260
}
// Colour object 0x005e9640 (packed argb) through thunk 0x00407577.
u32 argb(u32 v) {
    u32 slot = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(&slot)->Color_dword(joan::orig::arg::u32(v)));  // 0x005e9640
}
// Sprite 0x00633500 on 0xa46128 (17 args, ret 0x44).
void sprite(i32 x, i32 y, u32 ctx, const char* name, void* tex, u32 color, i32 e7, i32 e8, i32 e9, i32 e10) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(ctx), joan::orig::arg::str(name), tex, joan::orig::arg::u32(color), joan::orig::arg::i16(e7), joan::orig::arg::i16(e8), joan::orig::arg::i16(e9), joan::orig::arg::i16(e10), -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}
// Text 0x006255d0 (12 args, ret 0x30).
void text(img::Addr font, i32 x, i32 y, u32 ctx, const char* s, u32 color, i32 e6, i32 e7, i32 e8, i32 e9,
          i32 e10) {
    joan::orig::as_text(obj(font))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(ctx), s, joan::orig::arg::u32(color), joan::orig::arg::i32(e6), joan::orig::arg::i32(e7), joan::orig::arg::i32(e8), joan::orig::arg::i32(e9), joan::orig::arg::i32(e10), 1.0f, 1.0f);  // 0x006255d0
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

// ------------------------------------------------------------------ 0x006ef9e0 helpers
constexpr img::Addr kFont = d::g_font_a489c8;
const char* text_at(img::Addr a) { return str(a); }
u32 red() { return rgba(0xff, 0x33, 0x33, 0xff); }
// Cost texts 0x004cb5b0 on 0xa48820 (mode, id), ret 8.
const char* cost_text(i32 mode, i32 id) {
    return joan::orig::arg::as<const char*>(joan::orig::as_table(obj(d::g_keys_a48820))->lookup(joan::orig::arg::i32(mode), joan::orig::arg::i32(id)));  // 0x004cb5b0
}
void add16(i16& v, u16 d) { v = static_cast<i16>(static_cast<u16>(static_cast<u16>(v) + d)); }

struct H {
    void draw_panel(u32 id) {
        c_panel.hit();
        u8* const self = reinterpret_cast<u8*>(this);
        joan::orig::as_combat_screen(self)->build_roster(joan::orig::arg::i32(id));  // 0x006f0fe0
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(id)) != 0) return;  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0, ebp-0x8
        {
            const u32 c = argb(0xffffffff);
            sprite(0x144, 8, 2, str(d::s_PANEL), global<jo::BmpRes*>(d::g_a563a0), c, 3, 0, 0, 1);  // PANEL
        }
        {
            const u32 c = argb(0xffffffff);
            sprite(0x2e8, 0x69, 2, str(d::s_SRBAR), jo::self<jo::CombatScreen>(self)->u32_8, c, 3, 0, 0, 1);  // SRBAR
        }
        for (i32 i = 0; i < 3; ++i) {
            if (static_cast<i32>(jo::self<jo::CombatScreen>(self)->item_ids[i]) == -1) continue;
            joan::orig::as_button(&(&jo::self<jo::CombatScreen>(self)->buttons_0)[i])->draw(2u, 7);   // 0x00622cc0
            joan::orig::as_button(&(&jo::self<jo::CombatScreen>(self)->buttons_13)[i])->draw(2u, 7);  // 0x00622cc0
        }
        if (jo::self<jo::CombatScreen>(self)->i16_44e4 != 0) {
            if (jo::self<jo::CombatScreen>(self)->scroll != 0) joan::orig::as_button(&jo::self<jo::CombatScreen>(self)->buttons_18)->draw(2u, 7);
            if (static_cast<i32>(jo::self<jo::CombatScreen>(self)->row_count) - static_cast<i32>(jo::self<jo::CombatScreen>(self)->scroll) > 3)
                joan::orig::as_button(&jo::self<jo::CombatScreen>(self)->buttons_19)->draw(2u, 7);
        }
        jo::self<jo::CombatScreen>(self)->sel = -1;
        const i16 mx = global<i16>(d::g_mouse_x);
        const i16 my = global<i16>(d::g_mouse_y);
        for (i32 i = 0; i < 10; ++i) {
            jo::Button* const w = &(&jo::self<jo::CombatScreen>(self)->buttons_3)[i];  // buttons_3 .. 12
            joan::orig::as_button(w)->draw(2u, 7);  // 0x00622cc0
            if (static_cast<i32>(jo::self<jo::CombatScreen>(self)->stat_ids[i]) == -1) continue;
            if (w->x > mx) continue;
            if (w->x2 < mx) continue;
            if (w->y > my) continue;
            if (w->y2 < my) continue;
            jo::self<jo::CombatScreen>(self)->sel = static_cast<i16>(i);
        }
        if (jo::self<jo::CombatScreen>(self)->sel >= 0) {
            const i32 idx = jo::self<jo::CombatScreen>(self)->stat_ids[jo::self<jo::CombatScreen>(self)->sel];
            u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_upgrade_list(&jo::as_unit(unit)->upgrades)->get(joan::orig::arg::i32(idx)));  // 0x005313a0
            joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(jo::as_upgrade_list_entry(rec)->id)));  // 0x0050d240 (dead)
            if (jo::view<i16>(jo::as_upgrade_list_entry(rec)->level) < 10 && jo::as_unit(unit)->stat_points != 0) {
                joan::orig::as_button(&jo::self<jo::CombatScreen>(self)->buttons_17)->set_rect(joan::orig::arg::u32(i32{0x2b7}), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::CombatScreen>(self)->sel) * 0x1c + 0x6c), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{-1}), joan::orig::arg::u32(i32{0}));                                                 // 0x00623580
                joan::orig::as_panel(&jo::self<jo::CombatScreen>(self)->buttons_17)->reset_6b7cf0();                         // 0x006b7cf0
                joan::orig::as_button(&jo::self<jo::CombatScreen>(self)->buttons_17)->draw(2u, 7);         // 0x00622cc0
            } else {
                joan::orig::as_widget(&jo::self<jo::CombatScreen>(self)->buttons_17)->reset_child();  // 0x0062b660
            }
        } else {
            joan::orig::as_widget(&jo::self<jo::CombatScreen>(self)->buttons_17)->reset_child();  // 0x0062b660
            joan::orig::as_widget(&jo::self<jo::CombatScreen>(self)->buttons_16)->reset_child();  // 0x0062b660
        }
        joan::orig::as_scroll_bar(&jo::self<jo::CombatScreen>(self)->scroll_bar)->draw(i32{2}, i32{7});  // 0x00763530
        if (joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(&jo::as_unit(unit)->upgrades)->count()) != 0) {    // 0x00531360
            for (i32 i = 0; i < 10; ++i) {
                if (static_cast<i32>(jo::self<jo::CombatScreen>(self)->stat_ids[i]) == -1) continue;
                u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_upgrade_list(&jo::as_unit(unit)->upgrades)->get(static_cast<i32>(jo::self<jo::CombatScreen>(self)->stat_ids[i])));  // 0x005313a0
                u8* const info = joan::orig::arg::as<u8*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(jo::as_upgrade_list_entry(rec)->id))));  // 0x0050d240
                ability_row(i, rec, info);
            }
            for (i32 i = 0; i < 3; ++i) {
                if (static_cast<i32>(jo::self<jo::CombatScreen>(self)->item_ids[i]) == -1) continue;
                u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_upgrade_list(&jo::as_unit(unit)->upgrades)->get(static_cast<i32>(jo::self<jo::CombatScreen>(self)->item_ids[i])));  // 0x005313a0
                u8* const info = joan::orig::arg::as<u8*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(jo::as_upgrade_list_entry(rec)->id))));  // 0x0050d240
                {
                    const u32 c = argb(0xffffffff);
                    text(d::g_font_celtic_18pt_black, 0x160, i * 0x28 + 0x1d6, 2, jo::as_upgrade_item(info)->description, c, -1,
                         0, 0, 0, 0);
                }
                const char* const fmt = joan::orig::arg::as<const char*>(joan::orig::as_menu(obj(d::g_strings))->attcomb_1(joan::orig::arg::u32(static_cast<i32>(jo::as_upgrade_item(info)->cost))));  // 0x004dab60
                char buf[0x200];  // original: 0x100 bytes at ebp-0x158
                joan::orig::libc::sprintf(joan::orig::arg::str(buf), joan::orig::arg::str(fmt));  // sprintf
                const u32 c = argb(0xffffffff);
                joan::orig::as_text(obj(kFont))->draw_2(i32{0x2c2}, joan::orig::arg::i32(i * 0x28 + 0x1d9), joan::orig::arg::u32(u32{2}), static_cast<const char*>(buf), joan::orig::arg::u32(c), i32{-1}, i32{0}, i32{0}, i32{0}, 1.0f, 1.0f);  // 0x00626080
            }
        }
        const u32 c = argb(0xffffffff);
        sprite(0x158, 0x20, 2, str(d::s_BG), jo::self<jo::CombatScreen>(self)->u32_8, c, 3, 0, 0, 1);  // BG
    }

    // One row of the ability list (0x6efddf .. 0x6f058e).
    void ability_row(i32 i, u8* rec, u8* info) {
        const i32 y = i * 0x1c + 0x6e;
        const char* const name = jo::as_upgrade_item(info)->description;
        {
            const u32 c = argb(0xffffffff);
            text(kFont, 0x160, y, 2, name, c, -1, 0, 0, 0, 0);
        }
        const i16 kind = jo::as_upgrade_item(info)->type;
        if (kind == 0xb || jo::as_upgrade_item(info)->type == 0xc || jo::as_upgrade_item(info)->type == 0xd || jo::as_upgrade_item(info)->type == 0xe) {
            u32 civ = global<u32>(d::g_control_mode);  // ebp-0x2c
            if (civ == 4) civ = global<u32>(d::g_control_base_mode);
            i16 x = static_cast<i16>(static_cast<u16>(text_width(kFont, name)) + 0x160);  // ebp-0x30
            {
                const u32 c = argb(0xffffffff);
                text(kFont, x, y, 2, text_at(d::s_sp_lparen), c, -1, 0, 0, 0, 0);  // " ("
            }
            add16(x, text_width(kFont, text_at(d::s_sp_lparen)));
            const i32 sel = static_cast<i32>(jo::as_upgrade_item(info)->type) - 0xb;
            if (sel == 0) {
                const char* const s = cost_text(static_cast<i32>(civ), 0xc);
                const u32 c = red();
                text(kFont, x, y, 2, s, c, -1, 0, 1, 0, 0);
                add16(x, text_width(kFont, s));
            } else if (sel == 1) {
                if (civ == 1) {
                    const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->attcomb_text_2());  // 0x004daba0
                    const u32 c = red();
                    text(kFont, x, y, 2, s, c, -1, 0, 1, 0, 0);
                    add16(x, text_width(kFont, s));
                } else {
                    const char* const s = cost_text(static_cast<i32>(civ), 4);
                    const u16 sw = text_width(kFont, s);  // ebp-0x40
                    {
                        const u32 c = red();
                        text(kFont, x, y, 2, s, c, -1, 0, 1, 0, 0);
                    }
                    add16(x, sw);
                    {
                        const u32 c = argb(0xffffffff);
                        text(kFont, x, y, 2, text_at(d::s_plus), c, -1, 0, 0, 0, 0);  // "+"
                    }
                    add16(x, text_width(kFont, text_at(d::s_plus)));
                    {
                        const u32 c = red();
                        text(kFont, x, y, 2, s, c, -1, 0, 1, 0, 0);
                    }
                    add16(x, sw);
                }
            } else if (sel == 2) {
                if (civ == 1) {
                    const char* s = cost_text(static_cast<i32>(civ), 0xd);  // ebp-0x44
                    u16 sw = text_width(kFont, s);                          // ebp-0x48
                    {
                        const u32 c = red();
                        text(kFont, x, y, 2, s, c, -1, 0, 1, 0, 0);
                    }
                    add16(x, sw);
                    {
                        const u32 c = argb(0xffffffff);
                        text(kFont, x, y, 2, text_at(d::s_plus), c, -1, 0, 0, 0, 0);  // "+"
                    }
                    add16(x, text_width(kFont, text_at(d::s_plus)));
                    s = cost_text(static_cast<i32>(civ), 0xe);
                    sw = text_width(kFont, s);
                    {
                        const u32 c = red();
                        text(kFont, x, y, 2, s, c, -1, 0, 1, 0, 0);
                    }
                    add16(x, sw);
                } else {
                    const char* const s = cost_text(static_cast<i32>(civ), 0xf);
                    const u32 c = red();
                    text(kFont, x, y, 2, s, c, -1, 0, 1, 0, 0);
                    add16(x, text_width(kFont, s));
                }
            } else if (sel == 3) {
                const char* const s = cost_text(static_cast<i32>(civ), 0x12);
                const u32 c = red();
                text(kFont, x, y, 2, s, c, -1, 0, 1, 0, 0);
                add16(x, text_width(kFont, s));
            }
            const u32 c = argb(0xffffffff);
            text(kFont, x, y, 2, text_at(d::s_rparen), c, -1, 0, 0, 0, 0);  // ")"
        }
        const u32 c = argb(0xffffffff);
        const char* const level = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_short(joan::orig::arg::i16(static_cast<i32>(jo::view<i16>(jo::as_upgrade_list_entry(rec)->level))), joan::orig::arg::u32(i32{1})));  // 0x004e1360
        text_box(kFont, 0x269, i * 0x1c + 0x69, 0x2e4, i * 0x1c + 0x7f, 2, level, c, -1, 0, 0, 0);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b109_17", name, addr, fn)
B("Panel::draw_6ef9e0", 0x006ef9e0, &H::draw_panel);
#undef B

}  // namespace game_b109_17
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

void* const kUnits = obj(d::g_units);
void* const kScreen = obj(d::g_mouse);

void* unit(u32 id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0
i32 unit_missing(u32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
// 0x006218c0 Button::clicked(a, 0, -1, 0, 0, 0)
i32 clicked(void* button, i32 a = 0) { return joan::orig::arg::as<i32>(joan::orig::as_ui_button(button)->mouse(joan::orig::arg::i32(a), 0, joan::orig::arg::i32(-1), 0, 0, 0)); }
i32 box_visible(i32 x0, i32 y0, i32 x1, i32 y1) {
    return joan::orig::arg::as<i32>(joan::orig::as_point(kScreen)->inside(joan::orig::arg::i32(x0), joan::orig::arg::i32(y0), joan::orig::arg::i32(x1), joan::orig::arg::i32(y1)));  // 0x00593980
}

JOAN_COUNTER(c_panel_buttons, "Panel::handle_buttons");

struct H {

    // ------------------------------------------------------------------ 0x006f0a90
    u8 handle_buttons(u32 unit_id) {
        c_panel_buttons.hit();
        if (unit_missing(unit_id) != 0) return 0;
        void* const u = unit(unit_id);
        if (jo::view<i8>(jo::self<jo::CombatScreen>(this)->buttons_17.drawn) != 0 && clicked(&jo::self<jo::CombatScreen>(this)->buttons_17) != 0) {
            const i32 sel = jo::self<jo::CombatScreen>(this)->sel;
            joan::orig::as_unit(u)->buy_stat(8, 0, joan::orig::arg::u32(static_cast<i32>(jo::self<jo::CombatScreen>(this)->stat_ids[sel])));  // 0x0050d640
            jo::self<jo::CombatScreen>(this)->dirty = 1;
            return 1;
        }
        if (jo::view<i8>(jo::self<jo::CombatScreen>(this)->buttons_16.drawn) != 0 && clicked(&jo::self<jo::CombatScreen>(this)->buttons_16) != 0) return 1;
        if (jo::self<jo::CombatScreen>(this)->i16_44e4 != 0) {
            if (jo::self<jo::CombatScreen>(this)->scroll != 0 && clicked(&jo::self<jo::CombatScreen>(this)->buttons_18) != 0) {
                jo::view<u16>(jo::self<jo::CombatScreen>(this)->scroll) = static_cast<u16>(jo::view<u16>(jo::self<jo::CombatScreen>(this)->scroll) - 1u);
                return 1;
            }
            if (clicked(&jo::self<jo::CombatScreen>(this)->buttons_19) != 0 && static_cast<i32>(jo::self<jo::CombatScreen>(this)->row_count) - static_cast<i32>(jo::self<jo::CombatScreen>(this)->scroll) > 3) {
                jo::view<u16>(jo::self<jo::CombatScreen>(this)->scroll) = static_cast<u16>(jo::view<u16>(jo::self<jo::CombatScreen>(this)->scroll) + 1u);
                return 1;
            }
        }
        for (i32 i = 0; i < 3; ++i) {
            if (static_cast<i32>(jo::self<jo::CombatScreen>(this)->item_ids[i]) == -1) continue;  // buttons_13 .. 15: tooltips, buttons_0 .. 2: items
            if ((&jo::self<jo::CombatScreen>(this)->buttons_13)[i].pressed != 0 &&
                box_visible(static_cast<i32>((&jo::self<jo::CombatScreen>(this)->buttons_13)[i].x), static_cast<i32>((&jo::self<jo::CombatScreen>(this)->buttons_13)[i].y), static_cast<i32>((&jo::self<jo::CombatScreen>(this)->buttons_13)[i].x2),
                            static_cast<i32>((&jo::self<jo::CombatScreen>(this)->buttons_13)[i].y2)) == 0)
                jo::view<u8>((&jo::self<jo::CombatScreen>(this)->buttons_13)[i].pressed) = 0;
            if (clicked(&(&jo::self<jo::CombatScreen>(this)->buttons_0)[i]) != 0) {  // buttons_0 .. 2
                void* const item = joan::orig::arg::as<void*>(joan::orig::as_upgrade_list(&jo::as_unit(u)->upgrades)->get(joan::orig::arg::i32(static_cast<i32>(jo::self<jo::CombatScreen>(this)->item_ids[i]))));  // 0x005313a0
                joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(jo::as_upgrade_list_entry(item)->id)));  // 0x0050d240 (result unused)
                joan::orig::as_unit(u)->use_item(joan::orig::arg::mptr(item));                         // 0x004949f0
                jo::self<jo::CombatScreen>(this)->dirty = 1;
                jo::view<u16>(jo::self<jo::CombatScreen>(this)->scroll) = 0;
                return 1;
            }
            if (clicked(&(&jo::self<jo::CombatScreen>(this)->buttons_13)[i]) != 0) return 1;
        }
        for (i32 i = 0; i < 10; ++i) {
            if (static_cast<i32>(jo::self<jo::CombatScreen>(this)->stat_ids[i]) == -1) continue;  // buttons_3 .. 12
            if ((&jo::self<jo::CombatScreen>(this)->buttons_3)[i].pressed != 0 &&
                box_visible(static_cast<i32>((&jo::self<jo::CombatScreen>(this)->buttons_3)[i].x), static_cast<i32>((&jo::self<jo::CombatScreen>(this)->buttons_3)[i].y), static_cast<i32>((&jo::self<jo::CombatScreen>(this)->buttons_3)[i].x2), static_cast<i32>((&jo::self<jo::CombatScreen>(this)->buttons_3)[i].y2)) ==
                    0)
                jo::view<u8>((&jo::self<jo::CombatScreen>(this)->buttons_3)[i].pressed) = 0;
            if (clicked(&(&jo::self<jo::CombatScreen>(this)->buttons_3)[i]) != 0) return 1;
        }
        if (joan::orig::arg::as<i32>(joan::orig::as_scroll_bar(&jo::self<jo::CombatScreen>(this)->scroll_bar)->update()) != 0) return 1;  // 0x00762a00
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Panel::handle_buttons", 0x006f0a90, &H::handle_buttons);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

struct H {
    // ------------------------------------------------------------------ empty stubs
    void nop() {}
    void nop1(u32) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Obj::nop_6f0fa0", 0x006f0fa0, &H::nop1);
B("Obj::nop_6f0fc0", 0x006f0fc0, &H::nop);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- from modules/game_b104_14.cpp (game/b104_14)
namespace {
namespace game_b104_14 {


constexpr img::Addr kUnits = d::g_units;

JOAN_COUNTER(c_roster, "Roster::build_6f0fe0");

// The unit-name order sort_unit_names fills: 1-based i16s at +0x4472 of the CombatScreen (the Roster view's u16_4472).
i16* order_of(void* screen) { return reinterpret_cast<i16*>(jo::self<jo::CombatScreen>(screen)->gap_4474) - 1; }

struct H {
    // ------------------------------------------------------------------ 0x006f0fe0
    void build_6f0fe0(i32 id) {
        c_roster.hit();
        for (i32 i = 0; i < 10; ++i) jo::view<u16>(jo::self<jo::CombatScreen>(this)->stat_ids[i]) = 0xffff;  // the same object as CombatScreen
        for (i32 i = 0; i < 3; ++i) jo::view<u16>(jo::self<jo::CombatScreen>(this)->item_ids[i]) = 0xffff;
        if (joan::orig::as_dynarray(obj(kUnits))->empty_at(joan::orig::arg::i32(id)) != 0) return;  // 0x00428b70
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(id)));   // 0x0041b3c0
        i16 a = 0, b = 0, c = 0;
        jo::self<jo::CombatScreen>(this)->row_count = 0;  // the object is the CombatScreen (Roster is a partial view)
        joan::orig::as_list(this)->sort_unit_names(joan::orig::arg::i32(id));  // 0x006f1460
        void* const list = &jo::as_unit(rec)->upgrades;
        for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(list)->count()); ++i) {  // 0x00531360
            i16* const e = joan::orig::arg::as<i16*>(joan::orig::as_upgrade_list(list)->get(static_cast<i32>(order_of(this)[i])));  // 0x005313a0
            u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(e[0]))));     // 0x0050d240
            const i32 ty = jo::as_upgrade_item(t)->type;
            if (ty != 0xb && ty != 9 && ty != 0xd && ty != 0xe && ty != 0xa && ty != 0xc && ty != 0xf &&
                ty != 0x10)
                continue;
            if (e[1] == e[2]) c = static_cast<i16>(c + 1);
            const i32 page = static_cast<i16>(jo::self<jo::CombatScreen>(this)->scroll_bar.vbar.pos);
            if (static_cast<i32>(b) != page + 10 && e[1] == e[2]) {
                const i32 k = static_cast<i32>(b) - page;
                if (k >= 0) jo::view<u16>(jo::self<jo::CombatScreen>(this)->stat_ids[k]) = jo::view<u16>(order_of(this)[i]);
                b = static_cast<i16>(b + 1);
                continue;
            }
            if (static_cast<i32>(e[1]) > 0 && e[1] != e[2]) {
                for (i32 j = 1; j <= joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(list)->count()); ++j) {
                    i16* const e2 = joan::orig::arg::as<i16*>(joan::orig::as_upgrade_list(list)->get(joan::orig::arg::i32(j)));
                    u8* const t2 = joan::orig::arg::as<u8*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(e2[0]))));
                    if (jo::view<i8>(jo::as_upgrade_item(t)->required[0]) != 0 &&
                        joan::crt::stricmp(joan::orig::arg::str(jo::as_upgrade_item(t2)->bonus_text), joan::orig::arg::str(jo::as_upgrade_item(t)->required)) != 0)  // _stricmp
                        continue;
                    if (e2[1] != e2[2]) continue;
                    const i32 first = static_cast<i32>(jo::self<jo::CombatScreen>(this)->scroll);
                    if (static_cast<i32>(a) - first >= 3) {
                        jo::self<jo::CombatScreen>(this)->row_count = static_cast<i16>(jo::self<jo::CombatScreen>(this)->row_count + 1);
                    } else {
                        if (!(first > static_cast<i32>(a)))
                            jo::view<u16>(jo::self<jo::CombatScreen>(this)->item_ids[static_cast<i32>(a) - static_cast<i32>(jo::self<jo::CombatScreen>(this)->scroll)]) =
                                jo::view<u16>(order_of(this)[i]);
                        a = static_cast<i16>(a + 1);
                        jo::self<jo::CombatScreen>(this)->row_count = static_cast<i16>(jo::self<jo::CombatScreen>(this)->row_count + 1);
                    }
                    break;
                }
            }
        }
        jo::self<jo::CombatScreen>(this)->i16_44e4 = jo::self<jo::CombatScreen>(this)->row_count > 3 ? 1 : 0;
        joan::orig::as_scroll(&jo::self<jo::CombatScreen>(this)->scroll_bar.vbar)->set_end(
            joan::orig::arg::u32(static_cast<i32>(c) - 1));  // 0x0062ced0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_14", name, addr, fn)
B("Roster::build_6f0fe0", 0x006f0fe0, &H::build_6f0fe0);
#undef B

}  // namespace game_b104_14
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

using joan::orig::helpers::stricmp_;

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_sort_names, "List::sort_unit_names");

struct H {
    // ------------------------------------------------------------------ 0x006f1460
    void sort_unit_names(i32 id) {
        c_sort_names.hit();
        if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) != 0) return;  // 0x00428b70
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        void* const list = &jo::as_unit(unit)->upgrades;
        i16* const order = reinterpret_cast<i16*>(jo::self<jo::CombatScreen>(this)->gap_4474) - 1;  // +0x4472 of the CombatScreen (Roster is a partial view)
        for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(list)->count()); ++i) order[i] = static_cast<i16>(i);  // 0x00531360
        for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(list)->count()); ++i) {                                 // 0x00531360
            for (i32 j = i + 1; j <= joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(list)->count()); ++j) {                         // 0x00531360
                i16* const a = joan::orig::arg::as<i16*>(joan::orig::as_upgrade_list(list)->get(static_cast<i32>(order[i])));  // 0x005313a0
                i16* const b = joan::orig::arg::as<i16*>(joan::orig::as_upgrade_list(list)->get(static_cast<i32>(order[j])));  // 0x005313a0
                u8* const na = joan::orig::arg::as<u8*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(*a))));  // 0x0050d240
                u8* const nb = joan::orig::arg::as<u8*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(*b))));  // 0x0050d240
                if (stricmp_(jo::as_upgrade_item(na)->description, jo::as_upgrade_item(nb)->description) > 0) {
                    const i16 t = order[i];
                    order[i] = order[j];
                    order[j] = t;
                }
            }
        }
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("List::sort_unit_names", 0x006f1460, &H::sort_unit_names);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::CombatScreen::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b101_15::H::free_buttons>(this)); }  // 0x6ef7c0
auto joan::orig::CombatScreen::load_sprites() -> void { joan::bind::member<&::game_b87_06::H::combat_load_sprites>(this); }  // 0x6ef8d0
auto joan::orig::CombatScreen::free_sprites() -> void { joan::bind::member<&::game_b93_06::H::free2>(this); }  // 0x6ef970
auto joan::orig::Panel::draw_6ef9e0(std::uint32_t a0) -> void { joan::bind::member<&::game_b109_17::H::draw_panel>(this, a0); }  // 0x6ef9e0
auto joan::orig::Panel::handle_buttons(std::uint32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_13::H::handle_buttons>(this, a0)); }  // 0x6f0a90
auto joan::orig::CombatScreen::build_roster(std::int32_t a0) -> void { joan::bind::member<&::game_b104_14::H::build_6f0fe0>(this, a0); }  // 0x6f0fe0
auto joan::orig::List::sort_unit_names(std::int32_t a0) -> void { joan::bind::member<&::game_b97_08::H::sort_unit_names>(this, a0); }  // 0x6f1460
