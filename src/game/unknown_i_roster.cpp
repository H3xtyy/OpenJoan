// game/unknown_i_roster.cpp: functions whose original source file is unknown (formerly game/_unplaced/i_roster.cpp;
// re/attribution.csv: between ww/interfac/i_sms.cpp and ww/interfac/i_work.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstdarg>
#include <cmath>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using u8 = std::uint8_t;

using joan::orig::helpers::at;

using i8 = std::int8_t;

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using joan::call::cdecl_call;

}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using joan::orig::helpers::memset_;  // memset

struct H {
    // ------------------------------------------------------------------ 0x006e6050 etc.
    void* clear_1() { memset_(this, 0, 1); return this; }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj6e6050::Obj", 0x006e6050, &H::clear_1);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

void* crt_memset(void* d, i32 c, u32 n) { return std::memset(joan::orig::arg::mptr(d), joan::orig::arg::i32(c), joan::orig::arg::u32(n)); }

struct H {

    // ------------------------------------------------------------------------ 0x006b1f10 etc.
    void nop() {}

    // ------------------------------------------------------------------------ 0x006e60c0
    void* clear_6e60c0() { return crt_memset(this, 0, 1); }

    // ------------------------------------------------------------------------ 0x006e67d0
    i32 set_6e67d0(i8 v) {
        const i32 x = static_cast<i32>(v);
        jo::self<jo::Roster>(this)->u8_0 = x > 0 ? 1 : 0;
        return x;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::nop_6e6080", 0x006e6080, &H::nop);
B("Obj::nop_6e60a0", 0x006e60a0, &H::nop);
B("Obj::clear_6e60c0", 0x006e60c0, &H::clear_6e60c0);
B("Obj::nop_6e67b0", 0x006e67b0, &H::nop);
B("Obj::set_6e67d0", 0x006e67d0, &H::set_6e67d0);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_ui6.cpp (game/ui6)
namespace {
namespace game_ui6 {

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite queue
constexpr std::int32_t M1 = -1;

using joan::orig::helpers::screen_w;
using joan::orig::helpers::screen_h;

// 0x006255d0 Text::draw_line(x, y, 5, text, colour, -1, 0, 1, 2, 0, 1.0f, 1.0f) on 0x00a5d140. ret 0x30.
void text_line(std::int32_t x, std::int32_t y, const char* s, std::uint32_t c) {
    joan::orig::as_text(obj(d::g_text_a5d140))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), 5u, s, joan::orig::arg::u32(c), joan::orig::arg::i32(M1), 0, 1, 2, 0, 1.0f, 1.0f);  // 0x006255d0
}

JOAN_COUNTER(c_roster, "Roster::draw");

struct H {
    // 0x006e60f0 Roster::draw. ret 0.
    void roster_draw() {
        c_roster.hit();
        if (static_cast<std::int8_t>(jo::self<jo::Roster>(this)->u8_0) == 0) return;
        const std::uint32_t shade = 0x80c86400u;  // bytes 00 64 c8 80
        joan::orig::as_sprite2d(obj(kSprites))->queue(joan::orig::arg::i32(screen_w() / 2), 0, joan::orig::arg::i32(screen_w() - 1), joan::orig::arg::i32(screen_h() - 1), 5u, joan::orig::arg::u32(shade), joan::orig::arg::mptr(0), 3, 0, 0, 0, joan::orig::arg::i32(M1), joan::orig::arg::i32(M1), joan::orig::arg::i32(M1), joan::orig::arg::i32(M1), 0);  // 0x00632a70
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<std::int32_t>(d::g_player_unit_recno))) != 0) return;  // 0x00428b70
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<std::int32_t>(d::g_player_unit_recno))));  // 0x0041b3c0
        char buf[256];
        std::int32_t x = screen_w() / 2 + 0x14;
        std::int32_t y = 0x14;
        const char* s = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->upgrade_text_1(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_unit(unit)->attr_points))));  // 0x004daaf0
        joan::orig::libc::sprintf(joan::orig::arg::str(buf), joan::orig::arg::str(s));  // sprintf(buf, name)
        std::uint32_t colour = 0x9696ffffu;   // bytes ff ff 96 96
        text_line(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), buf, colour);
        x = screen_w() / 2 + 0x28;
        y += 0x14;
        s = joan::orig::arg::as<const char*>(joan::orig::as_txt_res(obj(d::g_strings))->upgrade_text_2());  // 0x004dab30
        joan::orig::libc::sprintf(joan::orig::arg::str(buf), joan::orig::arg::str(s));
        text_line(x + 0x12c, static_cast<std::int16_t>(y), buf, colour);
        colour = 0x96ffffffu;
        void* const groups = &jo::as_unit(unit)->upgrades;
        for (std::int32_t i = 1; i <= joan::orig::arg::as<std::int32_t>(joan::orig::as_upgrade_list(groups)->count()); ++i) {  // 0x00531360
            std::uint8_t* const g = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_upgrade_list(groups)->get(joan::orig::arg::i32(i)));      // 0x005313a0
            for (std::int32_t j = 0;
                 j < static_cast<std::int32_t>(jo::as_upgrade_list_entry(g)->max) - static_cast<std::int32_t>(jo::as_upgrade_list_entry(g)->cur); ++j) {
                y += 0x14;
                const std::int32_t level = jo::as_unit(unit)->stat_points;
                std::uint8_t* const m1 = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_upgrade_list_entry(g)->id))));  // 0x0050d240
                if (level < static_cast<std::int32_t>(jo::as_upgrade_item(m1)->cost)) colour &= 0xff0000ffu;
                const std::uint32_t c1 = colour;
                std::uint8_t* const m2 = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_upgrade_list_entry(g)->id))));
                text_line(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), jo::as_upgrade_item(m2)->description, c1);
                std::uint8_t* const m3 = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<std::int32_t>(jo::as_upgrade_list_entry(g)->id))));
                joan::orig::libc::sprintf(joan::orig::arg::str(buf), str(d::s_d), static_cast<std::int32_t>(jo::as_upgrade_item(m3)->cost));  // '%d'
                text_line(x + 0x12c, static_cast<std::int16_t>(y), buf, colour);
                colour |= 0x00ffff00u;
            }
        }
    }
};

#define U6(name, addr, fn) JOAN_REPLACE("game/ui6", name, addr, fn)
U6("Roster::draw", 0x006e60f0, &H::roster_draw);
#undef U6

}  // namespace game_ui6
}  // namespace

// ---------------------------------------- from modules/game_b111_11.cpp (fix/uninit)
namespace {
namespace game_b111_11 {

using i16 = std::int16_t;

constexpr u32 kOne = 0x3f800000;  // 1.0f bits

JOAN_COUNTER(c_hover, "Shop::hover_6e6490");

struct H {
    // ------------------------------------------------------------------ 0x006e6490 (fix/uninit)
    u8 shop_hover() {
        c_hover.hit();
        const i32 x = global<i32>(d::g_screen_w) / 2 + 0x28;
        i32 y = 0x28;
        // fix/uninit: the original never writes the alpha byte [ebp-0xd]; 0xff here.
        const u8 col[4] = {0x0a, 0x0a, 0xff, 0xff};
        u32 colour;
        std::memcpy(&colour, col, 4);
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))) != 0) return 0;  // 0x00428b70
        u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno))));  // 0x0041b3c0
        i8 hit = 0;
        auto info = [](const u8* e) {
            return joan::orig::arg::as<u8*>(joan::orig::as_table(obj(d::g_upgrade_res))->entry(joan::orig::arg::u32(static_cast<i32>(jo::as_upgrade_list_entry(e)->id))));  // 0x0050d240
        };
        for (i32 i = 1; i <= joan::orig::arg::as<i32>(joan::orig::as_upgrade_list(&jo::as_unit(p)->upgrades)->count()); ++i) {  // 0x00531360 count
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_upgrade_list(&jo::as_unit(p)->upgrades)->get(joan::orig::arg::i32(i)));          // 0x005313a0
            for (i32 j = 0; j < static_cast<i32>(jo::as_upgrade_list_entry(e)->max) - static_cast<i32>(jo::as_upgrade_list_entry(e)->cur); ++j) {
                y += 0x14;
                const i32 level = jo::as_unit(p)->stat_points;
                if (level < static_cast<i32>(jo::as_upgrade_item(info(e))->cost)) continue;
                {
                    u8* const d = info(e);
                    const i32 tw = joan::orig::as_font(obj(d::g_text_a5d140))->measure(joan::orig::arg::str(jo::as_upgrade_item(d)->description), -1, 0, 0, joan::orig::arg::f32(kOne));  // 0x00624f00
                    if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(x + tw), joan::orig::arg::i32(y + 0x14))) != 0) {  // 0x00593980
                        u8* const d2 = info(e);
                        joan::orig::as_text(obj(d::g_text_a5d140))->draw_line(static_cast<i32>(static_cast<i16>(x)), static_cast<i32>(static_cast<i16>(y)), joan::orig::arg::u32(i32{5}), joan::orig::arg::str(jo::as_upgrade_item(d2)->description), joan::orig::arg::u32(colour), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x006255d0
                        char buf[0x40];                                              // [ebp-0x54]
                        const i32 cost = jo::as_upgrade_item(info(e))->cost;
                        joan::orig::libc::sprintf(joan::orig::arg::str(buf), str(d::s_d), cost);  // sprintf "%d"
                        joan::orig::as_text(obj(d::g_text_a5d140))->draw_line(joan::orig::arg::i32(x + 0x12c), static_cast<i32>(static_cast<i16>(y)), joan::orig::arg::u32(i32{5}), buf, joan::orig::arg::u32(colour), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
                        hit = 1;
                    }
                }
                {
                    u8* const d = info(e);
                    const i32 tw = joan::orig::as_font(obj(d::g_text_a5d140))->measure(joan::orig::arg::str(jo::as_upgrade_item(d)->description), -1, 0, 0, joan::orig::arg::f32(kOne));
                    if (joan::orig::arg::as<i32>(joan::orig::as_parser(obj(d::g_mouse))->rect_mode(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::i32(x + tw), joan::orig::arg::i32(y + 0x14), i32{0})) != 0) {  // 0x005940f0
                        joan::orig::as_unit(p)->use_item(joan::orig::arg::mptr(e));  // 0x004949f0
                        hit = 1;
                    }
                }
                if (hit == 1) return 1;
            }
        }
        return 0;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// fix/uninit: hover text colour alpha 0xff instead of the never-written byte [ebp-0xd].
FIX("Shop::hover_6e6490", 0x006e6490, &H::shop_hover);
#undef FIX

}  // namespace game_b111_11
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

struct H {

    u8 byte_0() { return jo::self<jo::Roster>(this)->u8_0; }      // 0x006ba890 / 0x006e6810
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::byte_0_6e6810", 0x006e6810, &H::byte_0);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::RosterScreen::shutdown() -> void { joan::bind::member<&::game_b103_15::H::nop>(this); }  // 0x6e60a0
auto joan::orig::RosterScreen::draw() -> void { joan::bind::member<&::game_ui6::H::roster_draw>(this); }  // 0x6e60f0
auto joan::orig::Shop::hover_6e6490() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b111_11::H::shop_hover>(this)); }  // 0x6e6490
