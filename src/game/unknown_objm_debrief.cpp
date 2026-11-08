// game/unknown_objm_debrief.cpp: functions whose original source file is unknown (formerly
// game/_unplaced/objm_debrief.cpp; re/attribution.csv: between ww/interfac/objmenub.cpp and ww/interfac/objmenuw.cpp).
// They move to their original file when evidence names it (Phase 3).
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
#include "mission_count.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i8 = std::int8_t;

}  // namespace

// ---------------------------------------- from modules/game_b102_16.cpp (game/b102_16)
namespace {
namespace game_b102_16 {

using joan::orig::helpers::op_new;  // operator new

// new(n) + default ctor, null when new fails (SEH guard of the original not needed).
void* new_620ee0() {
    void* const m = op_new(static_cast<std::uint32_t>(sizeof(jo::Button)));  // 0x346 on x86
    return m != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_widget(m)->ctor_620ee0()) : nullptr;  // 0x00620ee0
}
void* new_629400() {
    void* const m = op_new(static_cast<std::uint32_t>(sizeof(jo::Image2d)));  // 0xb7 on x86
    return m != nullptr ? joan::orig::as_image2d(m)->construct() : nullptr;  // 0x00629400
}

JOAN_COUNTER(c_build_72a180, "Panel::build_72a180");

struct H {
    // ------------------------------------------------------------------------ 0x0072a180
    i32 build_72a180() {
        c_build_72a180.hit();
        jo::view<void*>(jo::self<jo::Debrief>(this)->u32_229) = new_629400();
        {
            u32 rgba = 0;
            void* const r = joan::orig::as_colour(&rgba)->construct(0, 0, 0, 0xff);  // 0x0042a260
            const u32 colour = *static_cast<u32*>(r);
            joan::orig::as_label(jo::view<void*>(jo::self<jo::Debrief>(this)->u32_229))->create(0x1bc, 0xb4, joan::orig::arg::mptr(img::addr(d::g_bmp_debrief)), str(d::s_frame), joan::orig::arg::i32(// 0x00629660
                          colour), 3, 0, 2, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, 1, 1);
        }
        jo::view<void*>(jo::self<jo::Debrief>(this)->p_231) = new_629400();
        {
            u32 rgba = 0;
            void* const r = joan::orig::as_colour(&rgba)->construct(0, 0, 0, 0xff);  // 0x0042a260
            const u32 colour = *static_cast<u32*>(r);
            joan::orig::as_label(jo::view<void*>(jo::self<jo::Debrief>(this)->p_231))->create(0, 0xa0, joan::orig::arg::mptr(img::addr(d::g_bmp_debrief)), str(d::s_joan_1), joan::orig::arg::i32(// 0x00629660
                          colour), 3, 0, 2, 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, joan::orig::arg::i32(-1), joan::orig::arg::i32(-1), 0, 1, 1);
        }
        void* const s = joan::orig::arg::as<void*>(joan::orig::as_bmp_res(obj(d::g_bmp_debrief))->find(str(d::s_joan_1sm)));  // 0x00620b40
        i32 i = 0;
        for (; i < 4; ++i) {
            jo::self<jo::CampScreen>(this)->hero_buttons[i] = new_620ee0();
            const i32 a4 = static_cast<i32>(static_cast<i16>(jo::as_bmp_entry(s)->height)) + 0x217;
            const u32 t = global<u32>(d::k_9df184 + i * 4);
            const u32 a3 = t + static_cast<u32>(static_cast<i32>(static_cast<i16>(jo::as_bmp_entry(s)->width))) - 1u;
            joan::orig::as_button(jo::self<jo::CampScreen>(this)->hero_buttons[i])->create(joan::orig::arg::i32(t), 0x218, joan::orig::arg::i32(a3), joan::orig::arg::i32(a4), fn::code_value(fn::Widget_draw_smith_frame), joan::orig::arg::i32(// 0x00621540
                          1), 0, 0, 1, 1, 1);
        }
        return i;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_16", name, addr, fn)
B("Panel::build_72a180", 0x0072a180, &H::build_72a180);
#undef B

}  // namespace game_b102_16
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

JOAN_COUNTER(c_free_panels, "Obj::free_panels");

struct H {
    // ------------------------------------------------------------------ 0x0072a470
    void free_panels() {
        c_free_panels.hit();
        for (jo::Image2D** const slot : {&jo::self<jo::CampScreen>(this)->u32_229, &jo::self<jo::CampScreen>(this)->u32_22d, &jo::self<jo::CampScreen>(this)->p_231}) {  // +0x229 / +0x22d / +0x231
            if (*slot != nullptr) {
                joan::orig::as_image2d(*slot)->destroy(1u);  // 0x006bd440 deleting dtor
                *slot = nullptr;
            }
        }
        for (i32 i = 0; i < 4; ++i) {
            if (jo::self<jo::CampScreen>(this)->hero_buttons[i] != nullptr) {
                joan::orig::as_combo_box(jo::self<jo::CampScreen>(this)->hero_buttons[i])->scalar_dtor_6b33a0(1u);  // 0x006b33a0 deleting dtor
                jo::self<jo::CampScreen>(this)->hero_buttons[i] = nullptr;
            }
        }
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::free_panels", 0x0072a470, &H::free_panels);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b110_12.cpp (game/b110_12)
namespace {
namespace game_b110_12 {

using joan::hook::x87::Fpu;


i16 to_i16(i32 v) { return static_cast<i16>(static_cast<u16>(static_cast<u32>(v) & 0xffffu)); }

JOAN_COUNTER(c_results, "ObjectiveMenu::draw_results_72a620");

// ---------------------------------------------------------------- 0x0072a620 helpers
constexpr img::Addr kFontBig = d::g_font_goudy12_bold_black, kFontNum = d::g_fontNum;
constexpr u32 kOne = 0x3f800000u;  // 1.0f

u32 rgba(i32 b0, i32 b1, i32 b2, i32 b3) {  // 0x0042a260
    u32 col = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&col)->construct(b0, b1, b2, b3));
}
u32 white() { return rgba(0xff, 0xff, 0xff, 0xff); }
u32 black() { return rgba(0, 0, 0, 0xff); }
u32 argb(i32 v) {  // 0x005e9640
    u32 col = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(&col)->Color_dword(joan::orig::arg::u32(v)));
}
// 0x00633500 (17 args).
void sprite(i32 x, i32 y, std::uintptr_t name, u32 col) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 7u, joan::orig::arg::str(str(name)), img::obj(d::g_bmp_debrief), joan::orig::arg::u32(col), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);
}
const char* menu_text(std::uintptr_t thunk) { return thiscall<const char*>(thunk, obj(d::g_strings)); }
i16 text_width(img::Addr font, const char* s) {  // 0x00624f00
    return joan::orig::as_font(obj(font))->measure(joan::orig::arg::str(s), -1, 0, 0, joan::orig::arg::f32(kOne));
}
void draw_line(img::Addr font, i32 x, i32 y, const char* s, u32 col) {  // 0x006255d0 (12 args)
    joan::orig::as_text(obj(font))->draw_line(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(i32{7}), s, joan::orig::arg::u32(col), i32{-1}, i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
}
void draw_2(i32 x, i32 y, const char* s, u32 col) {  // 0x00626080 on 0xa56e30 (11 args)
    joan::orig::as_text(obj(kFontNum))->draw_2(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(i32{7}), s, joan::orig::arg::u32(col), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
}
const char* number(i32 n) { return joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(joan::orig::arg::i32(n), i32{4})); }  // 0x0058fca0
const char* number_w(i16 n) { return joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_short(joan::orig::arg::i16(static_cast<i32>(n)), joan::orig::arg::u32(i32{4}))); }  // 0x004e1360

// The original frame from [ebp-0x64] up to `this` at [ebp-4].
struct Frame {
    char buf[0x60];
    u8* self;
};
static_assert(sizeof(Frame) == 0x60 + sizeof(void*), "Frame");  // x86: 0x64

struct Bars {
    i16 wc;   // largebar width  [ebp-0xc]
    i16 w10;  // large_la width  [ebp-0x10]
    i16 w18;  // [ebp-0x18] = 2
};
i16 centre(const Bars& b, i16 w, i32 x) { return to_i16((static_cast<i32>(b.wc) - static_cast<i32>(w)) / 2 + x); }

// Header bar with its centred caption (rows at y = 0xcc, 0xf4, 0x11c).
void header(const Bars& b, i32 x, i32 y, std::uintptr_t text_thunk) {
    {
        const u32 col = white();
        sprite(x, y, img::addr(d::s_largebar), col);  // largebar
    }
    const char* const t = menu_text(text_thunk);
    const i16 w1c = text_width(kFontBig, t);
    const i16 w20 = centre(b, w1c, x);
    {
        const u32 col = white();
        sprite(static_cast<i32>(w20) - static_cast<i32>(b.w10) - 4, y + static_cast<i32>(b.w18) + 2, img::addr(d::s_large_la), col);
    }
    {
        const u32 col = black();
        const char* const t2 = menu_text(text_thunk);
        draw_line(kFontBig, w20, y + 2, t2, col);
    }
    {
        const u32 col = white();
        sprite(static_cast<i32>(w20) + static_cast<i32>(w1c) + 4, y + static_cast<i32>(b.w18) + 2, img::addr(d::s_large_ra), col);
    }
}

// Counter value centred under a header: width from one 0x0058fca0 call, drawn with a second one.
void count_value(const Bars& b, i32 n, i32 y) {
    const i16 w1c = text_width(kFontNum, number(n));
    const i16 w20 = centre(b, w1c, 0x1e4);
    const u32 col = argb(-1);
    const char* const s = number(n);
    draw_line(kFontNum, w20, y + 0xb, s, col);
}

// Small row: bar + bullet at (0x1e4, y).
void small_row(i32 x, i32 y) {
    {
        const u32 col = white();
        sprite(x, y, img::addr(d::s_smallbar), col);  // smallbar
    }
}
void small_bullet(i32 x, i32 y) {
    const u32 col = white();
    sprite(x + 4, y + 0xc, img::addr(d::s_small_a), col);  // small_a
}

double table_u(img::Addr base, u32 d) { return static_cast<double>(global<u32>(base + d * 4u)); }
u32 table_raw(img::Addr base, u32 d) { return global<u32>(base + d * 4u); }

// a4 += part * 100 * 0.25
float add_part(float sum, float part) {
    const Fpu fpu{true};
    return static_cast<float>(
        fpu.add(fpu.mul(fpu.mul(part, global<float>(d::flt_100)), global<float>(d::flt_0_25)), sum));
}
// (r - lo) / span * (100 - [0x9df198]) * 0.01 + [0x9df198] * 0.01
double scaled_part(double x) {
    const Fpu fpu{true};
    const double g = static_cast<double>(global<i32>(d::s_Z));
    const double k100 = global<float>(d::flt_100), k001 = global<float>(d::flt_0_01);
    x = fpu.mul(x, fpu.sub(k100, g));
    x = fpu.mul(x, k001);
    return fpu.add(x, fpu.mul(g, k001));
}
// r / lo * [0x9df198] * 0.01
double low_part(double x) {
    const Fpu fpu{true};
    x = fpu.mul(x, static_cast<double>(global<i32>(d::s_Z)));
    return fpu.mul(x, global<float>(d::flt_0_01));
}

void results_body(Frame& f) {
    u8* const self = f.self;
    joan::orig::as_label(jo::as_camp_screen(self)->p_231)->draw(7u, 7u);  // 0x00629bd0
    Bars b{};
    {
        u8* const img = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(obj(d::g_bmp_debrief))->find(str(d::s_largebar)));  // 0x00620b40 largebar
        b.wc = static_cast<i16>(jo::as_bmp_entry(img)->u16_32) != 0 ? static_cast<i16>(jo::as_bmp_entry(img)->height) : static_cast<i16>(jo::as_bmp_entry(img)->width);
    }
    {
        u8* const img = joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(obj(d::g_bmp_debrief))->find(str(d::s_large_la)));  // large_la
        b.w10 = static_cast<i16>(jo::as_bmp_entry(img)->u16_32) != 0 ? static_cast<i16>(jo::as_bmp_entry(img)->height) : static_cast<i16>(jo::as_bmp_entry(img)->width);
    }
    b.w18 = 2;
    u8* const p74 = reinterpret_cast<u8*>(jo::as_camp_screen(self)->heroes[jo::as_camp_screen(self)->selected_hero]);
    const i32 x = 0x1e4;

    // Time.
    header(b, x, 0xcc, fn::code_value(fn::ObjMenu_open_23));  // 0x004dc100
    {
        const u32 t = global<u32>(d::g_mission_ms);
        const u32 h = t / 3600000u, m = (t % 3600000u) / 60000u, s = ((t % 3600000u) % 60000u) / 1000u;
        joan::orig::libc::sprintf(joan::orig::arg::str(f.buf), str(d::s_d_02d_02d), h, m, s);  // sprintf "%d:%02d:%02d"
        const i16 w1c = text_width(kFontNum, f.buf);
        const i16 w20 = centre(b, w1c, x);
        const u32 col = argb(-1);
        draw_line(kFontNum, w20, 0xcc + 0xd, f.buf, col);
    }

    // Kills.
    header(b, x, 0xf4, fn::code_value(fn::ObjMenu_open_24));  // 0x004dc130
    const i32 kills = jo::as_unit(p74)->kills;
    i32 total = 0;
    {
        u8* const q = joan::orig::arg::as<u8*>(joan::orig::as_unit(p74)->nation());  // 0x00509400
        if (q != nullptr) {
            void* const units = obj(d::g_nations);
            for (i32 i = joan::orig::as_dynarray(units)->size(); i > 0; --i) {  // 0x00417ad0
                if (joan::orig::arg::as<i32>(joan::orig::as_list(units)->is_null(joan::orig::arg::i32(i))) != 0) continue;    // 0x00484870
                u8* const r = joan::orig::arg::as<u8*>(joan::orig::as_player(q)->relation(joan::orig::arg::i32(i)));             // 0x004b42e0
                if (r != nullptr && jo::as_relation(r)->attitude >= 2) continue;
                u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(i)));  // 0x0047b600
                total = static_cast<i32>(static_cast<u32>(total) + jo::as_nation(u)->unit_count);
            }
        }
    }
    count_value(b, kills, 0xf4);

    // Objectives.
    i32 obj_all = 0, obj_done = 0;
    for (i32 i = 0; i < static_cast<i32>(global<i16>(d::g_a59299)); ++i) {
        const u32 o = static_cast<u32>(i) * 0x1au;
        if (global<i16>(d::g_a592aa + o) == 0) continue;
        if (global<char*>(d::g_a5929c + o) == nullptr) continue;
        if (global<i8>(d::g_a5929b + o) == 2) {
            ++obj_all;
            if (global<i16>(d::g_a592a4 + o) == 1) ++obj_done;
        }
    }
    if (obj_all != 0) {
        header(b, x, 0x11c, fn::code_value(fn::Menu_objmenu_25));  // 0x004dc160
        count_value(b, obj_done, 0x11c);
    }

    // Hero name.
    {
        const u32 col = white();
        sprite(0x210, 0xa4, img::addr(d::s_fancybar), col);  // fancybar
    }
    std::strcpy(static_cast<char*>(joan::orig::arg::mptr(f.buf)), joan::orig::arg::str(jo::as_unit(p74)->name_));  // strcpy
    joan::orig::libc::_strupr(joan::orig::arg::str(f.buf));                                             // _strupr
    {
        // [ebp-0x1c] = width: inside the original's frame at buf + 0x48, so a name of 0x48+ chars
        // is altered before it is drawn.
        const i16 w = text_width(kFontBig, f.buf);
        std::memcpy(f.buf + 0x48, &w, sizeof w);
    }
    {
        const u32 col = black();
        joan::orig::as_text(obj(kFontBig))->draw_3(i32{0x210}, i32{0xa4}, i32{0x330}, i32{0xcc}, joan::orig::arg::u32(i32{7}), f.buf, joan::orig::arg::u32(col), i32{0}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x00626130
    }
    {
        const u32 col = white();
        sprite(0x1e8, 0x147, img::addr(d::s_goldbar), col);  // goldbar
    }
    {
        const u32 col = white();
        sprite(0x1e8, 0x1b4, img::addr(d::s_goldbar), col);
    }

    // Row 1 (y 0x150): caption box + value +0x1bb.
    {
        const i32 y = 0x150;
        small_row(x, y);
        small_bullet(x, y);
        {
            const u32 col = black();
            const char* const t = menu_text(fn::code_value(fn::TxtRes_objmenu_29));  // 0x004dc230
            joan::orig::as_text(obj(kFontBig))->draw(joan::orig::arg::i32(x + 0x14), joan::orig::arg::i32(y), joan::orig::arg::i32(x + 0xc8), joan::orig::arg::i32(y + 0x20), joan::orig::arg::u32(i32{7}), t, joan::orig::arg::u32(col), i32{2}, i32{1}, i32{1}, i32{1}, i32{2}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne), i32{0}, i32{-1});  // 0x00626320
        }
        const u32 col = argb(-1);
        const char* const s = number_w(jo::as_unit(p74)->squad_kills);
        draw_2(x + 0x172, y + 2, s, col);
    }
    // Row 2 (y 0x170): level text, value = byte +2 of +0x33.
    {
        const i32 y = 0x170;
        small_row(x, y);
        i32 a0 = 0;
        if (jo::as_unit(p74)->group != nullptr) a0 = jo::as_group(jo::as_unit(p74)->group)->count;
        small_bullet(x, y);
        {
            const u32 col = black();
            const char* const t = menu_text(fn::code_value(fn::ObjMenu_open_2a));  // 0x004dc260
            draw_line(kFontBig, x + 0x14, y + 9, t, col);
        }
        const u32 col = argb(-1);
        const char* const s = number(a0);
        draw_2(x + 0x172, y + 2, s, col);
    }
    // Row 3 (y 0x190): value +0x1b9.
    {
        const i32 y = 0x190;
        small_row(x, y);
        small_bullet(x, y);
        {
            const u32 col = black();
            const char* const t = menu_text(fn::code_value(fn::Menu_objmenu_2b));  // 0x004dc290
            draw_line(kFontBig, x + 0x14, y + 9, t, col);
        }
        const u32 col = argb(-1);
        const char* const s = number_w(jo::as_unit(p74)->squad_losses);
        draw_2(x + 0x172, y + 2, s, col);
    }

    // Grade.
    const Fpu fpu{true};
    const double k100 = global<float>(d::flt_100);
    float score = 0.0f;
    {  // time
        float part;
        const u32 t = global<u32>(d::g_mission_ms);
        const u32 d = joan::mission_count::table_mission(global<u32>(d::g_mission));  // [Fixes] MissionCount: rows 1..8
        if (t < table_raw(d::k_9df1c4, d)) {
            part = 1.0f;
        } else if (t < table_raw(d::k_9df1a4, d)) {
            const u32 hi = table_raw(d::k_9df1a4, d), lo = table_raw(d::k_9df1c4, d);
            const u32 q = (hi - t) / (hi - lo);
            part = static_cast<float>(scaled_part(static_cast<double>(q)));
        } else {
            double v = fpu.div(static_cast<double>(t), static_cast<double>(static_cast<i32>(table_raw(d::k_9df1a4, d))));
            v = fpu.sub(global<float>(d::flt_2), v);
            part = static_cast<float>(low_part(v));
            if (!(part >= global<float>(d::k_9cf5c4))) part = 0.0f;
        }
        score = add_part(score, part);
    }
    {  // kills
        const float ratio = total != 0 ? static_cast<float>(fpu.div(fpu.mul(static_cast<double>(kills), k100),
                                                                     static_cast<double>(total)))
                                       : 0.0f;
        float part;
        const u32 d = joan::mission_count::table_mission(global<u32>(d::g_mission));  // [Fixes] MissionCount: rows 1..8
        if (!(table_u(d::k_9df204, d) >= ratio)) {
            part = 1.0f;
        } else if (!(table_u(d::k_9df1e4, d) >= ratio)) {
            double v = fpu.sub(ratio, table_u(d::k_9df1e4, d));
            v = fpu.div(v, static_cast<double>(static_cast<i32>(table_raw(d::k_9df204, d) - table_raw(d::k_9df1e4, d))));
            part = static_cast<float>(scaled_part(v));
        } else {
            part = static_cast<float>(low_part(fpu.div(ratio, table_u(d::k_9df1e4, d))));
        }
        score = add_part(score, part);
    }
    {  // objectives
        float part;
        if (obj_done == obj_all) {
            part = 1.0f;
        } else {
            const double kh = global<float>(d::flt_0_5);
            part = static_cast<float>(fpu.add(
                fpu.mul(fpu.div(static_cast<double>(obj_done), static_cast<double>(obj_all)), kh), kh));
        }
        score = add_part(score, part);
    }
    {  // level / experience
        float lv = static_cast<float>(static_cast<i32>(jo::as_unit(p74)->level));
        i32 base = 0;
        if (jo::as_unit(p74)->level != 1)
            base = jo::as_level_rec(joan::orig::as_level_res(obj(d::g_level_res))->find(joan::orig::arg::i32(static_cast<i32>(jo::as_unit(p74)->level) - 1)))->experience;  // 0x004a4e60
        const i32 next = jo::as_level_rec(joan::orig::as_level_res(obj(d::g_level_res))->find(static_cast<i32>(jo::as_unit(p74)->level)))->experience;
        double v = static_cast<double>(static_cast<i32>(jo::view<u32>(jo::as_unit(p74)->experience) - static_cast<u32>(base)));
        v = fpu.div(v, static_cast<double>(static_cast<i32>(static_cast<u32>(next) - static_cast<u32>(base))));
        lv = static_cast<float>(fpu.add(v, lv));
        float part;
        const u32 d = joan::mission_count::table_mission(global<u32>(d::g_mission));  // [Fixes] MissionCount: rows 1..8
        const double hi = static_cast<double>(static_cast<i32>(table_raw(d::k_9df244, d)));
        if (!(hi >= lv)) {
            part = 1.0f;
        } else {
            const double lo = static_cast<double>(static_cast<i32>(table_raw(d::k_9df224, d)));
            if (!(lo >= lv)) {
                double w = fpu.sub(lv, static_cast<double>(static_cast<i32>(table_raw(d::k_9df224, d))));
                w = fpu.div(w, static_cast<double>(static_cast<i32>(table_raw(d::k_9df244, d) - table_raw(d::k_9df224, d))));
                part = static_cast<float>(scaled_part(w));
            } else {
                part = static_cast<float>(low_part(fpu.div(lv, static_cast<double>(static_cast<i32>(table_raw(d::k_9df224, d))))));
            }
        }
        score = add_part(score, part);
    }
    {
        const u32 col = black();
        const char* const t = menu_text(fn::code_value(fn::ObjMenu_open_26));  // 0x004dc190
        draw_line(d::g_font_goudy_bold_14, 0x238, 0x1d4, t, col);
    }
    const double s = score;
    std::uintptr_t grade;
    if (!(static_cast<double>(global<i32>(d::s_us_9df194)) >= s))
        grade = img::addr(d::s_grade_ap);  // grade_ap
    else if (!(static_cast<double>(global<i32>(d::s_Z)) >= s))
        grade = img::addr(d::s_grade_a);  // grade_a
    else if (!(static_cast<double>(global<i32>(d::s_P)) >= s))
        grade = img::addr(d::s_grade_b);  // grade_b
    else if (!(static_cast<double>(global<i32>(d::s_F_9df1a0)) >= s))
        grade = img::addr(d::s_grade_c);  // grade_c
    else if (!(static_cast<double>(global<i32>(d::k_9df1a4)) >= s))
        grade = img::addr(d::s_grade_d);  // grade_d
    else
        grade = img::addr(d::s_grade_f);  // grade_f
    const u32 col = white();
    sprite(0x2a0, 0x1c0, grade, col);
}

struct H {
    // 0x0072a620
    void draw_results(u32) {
        u8* const self = reinterpret_cast<u8*>(this);
        if (jo::view<i8>(jo::as_camp_screen(self)->visible) != 7) return;
        c_results.hit();
        joan::orig::as_label(jo::as_camp_screen(self)->u32_229)->draw(7u, 7u);  // 0x00629bd0
        Frame f;
        f.buf[0] = 0;
        f.self = self;
        if (jo::as_camp_screen(self)->selected_hero != -1) results_body(f);
        for (i32 i = 0; i < 4; ++i) {
            if (jo::as_camp_screen(f.self)->heroes[i] != nullptr)
                joan::orig::as_button(jo::view<u8*>(jo::as_camp_screen(f.self)->hero_buttons[i]))->draw(7u, 7);  // 0x00622cc0
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b110_12", name, addr, fn)
B("ObjectiveMenu::draw_results_72a620", 0x0072a620, &H::draw_results);
#undef B

}  // namespace game_b110_12
}  // namespace

// ---------------------------------------- from modules/game_b108_17.cpp (game/b108_17)
namespace {
namespace game_b108_17 {

using joan::orig::helpers::stricmp_;  // _stricmp

using joan::orig::helpers::strcpy_;

i32 btn_clicked(void* b, i32 a, i32 c, i32 d, i32 e, i32 f, i32 g) {
    return joan::orig::arg::as<i32>(joan::orig::as_ui_button(b)->mouse(joan::orig::arg::i32(a), joan::orig::arg::i32(c), joan::orig::arg::i32(d), joan::orig::arg::i32(e), joan::orig::arg::i32(f), joan::orig::arg::i32(g)));  // 0x006218c0
}

JOAN_COUNTER(c_portrait, "Hero::pick_portrait");

struct H {
    // ------------------------------------------------------------------------ 0x0072c740
    u8 pick_portrait() {
        c_portrait.hit();
        if (jo::view<i8>(jo::self<jo::Debrief>(this)->visible) != 7) return 0;
        if (btn_clicked(jo::view<void*>(jo::self<jo::Debrief>(this)->menu_buttons[7]), 0x1b, 0, -1, 0, 0, 0) != 0) {
            joan::orig::as_camp_screen(this)->set_state_721a60(0u);  // 0x00721a60
            return 1;
        }
        for (i32 i = 0; i < 4; ++i) {
            u8* const hero = reinterpret_cast<u8*>(jo::self<jo::CampScreen>(this)->heroes[i]);
            if (hero == nullptr) return 0;
            if (btn_clicked(jo::self<jo::CampScreen>(this)->hero_buttons[i], 0, 0, -1, 0, 0, 0) == 0) continue;
            const i32 n = jo::as_unit(hero)->collision_id;
            static const img::Addr kNames[4] = {d::s_JOAN_OF_ARC, d::s_JEAN_DE_METZ, d::s_LAHIRE_9cfb3c, d::s_ALENCON_9cfb30};
            static const img::Addr kFmts[4] = {d::s_joan_1d, d::s_metz_1d, d::s_hire_1d, d::s_alen_1d};
            for (i32 k = 0; k < 4; ++k) {  // JOAN_OF_ARC / JEAN_DE_METZ / LAHIRE / ALENCON
                const char* const nm = jo::as_unit_type(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::as_unit(hero)->type_id)))->name;  // 0x0042d0a0
                if (stricmp_(nm, str(kNames[k])) == 0) {
                    const char* const s = joan::orig::arg::as<const char*>(joan::orig::format_positional(joan::orig::arg::str(str(kFmts[k])), n + 1));  // 0x005a6660
                    strcpy_(jo::self<jo::Debrief>(this)->p_231->sprite_name, s);
                    break;
                }
            }
            jo::self<jo::Debrief>(this)->selected_hero = i;
            return 1;
        }
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_17", name, addr, fn)
B("Hero::pick_portrait", 0x0072c740, &H::pick_portrait);
#undef B

}  // namespace game_b108_17
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

struct H {
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Screen::nop_72c9f0", 0x0072c9f0, &H::nop);
B("Screen::nop_72ca10", 0x0072ca10, &H::nop);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x0072ca30
    std::uintptr_t reset_72ca30() {
        joan::orig::as_obj_menu(this)->collect_debrief_heroes();  // 0x0072cad0
        const std::uintptr_t r = joan::orig::arg::as<u32>(joan::orig::as_widget(jo::self<jo::CampScreen>(this)->menu_buttons[7])->scale_to_61f1b0(joan::orig::arg::i16(i32{1}), joan::orig::arg::f32(u32{0x40c00000}), joan::orig::arg::f32(i32{0}), static_cast<char>(joan::orig::arg::i32(i32{0})), joan::orig::arg::u32(i32{0xc8})));  // 0x0061f1b0
        jo::self<jo::CampScreen>(this)->selected_hero = -1;
        return r;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Obj::reset_72ca30", 0x0072ca30, &H::reset_72ca30);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

struct H {
    void reset_72ca90() {  // 0x0072ca90
        joan::orig::as_widget(jo::view<void*>(jo::self<jo::CampScreen>(this)->menu_buttons[7]))->scale_to(0, joan::orig::arg::f32(0x40c00000u), 0.0f, 0, 0u);  // 0x0061f1b0 (0, 6.0f, 0, 0, 0)
        joan::orig::as_obj_menu(this)->free_debrief_images();  // 0x0072d060
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj72::reset", 0x0072ca90, &H::reset_72ca90);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b108_20.cpp (game/b108_20)
namespace {
namespace game_b108_20 {

JOAN_COUNTER(c_debrief_72cad0, "Debrief::setup_72cad0");

constexpr img::Addr kUnits = d::g_units;  // unit table (0x00428b70 check, 0x0041b3c0 get)

struct H {
    // -------------------------------------------------------------- 0x0072cad0
    void store_hero(i32 slot, void* hero) { jo::self<jo::CampScreen>(this)->heroes[slot] = static_cast<jo::Unit*>(hero); }
    u32 debrief_setup() {
        c_debrief_72cad0.hit();
        i16 ids[4] = {0, 0, 0, 0};
        for (i32 i = 0; i < 4; ++i)
            ids[i] = static_cast<i16>(joan::orig::arg::as<u16>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(str(d::g_a29820 + static_cast<u32>(i) * 0x104)))));  // 0x005270b0
        void* const me = joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x0041b3c0
        const i16 side = jo::as_unit(me)->side;
        std::memset(joan::orig::arg::mptr(jo::self<jo::CampScreen>(this)->heroes), i32{0}, joan::orig::arg::u32(static_cast<u32>(sizeof jo::self<jo::CampScreen>(this)->heroes)));  // memset
        for (i32 i = 0; i < static_cast<i32>(global<i16>(d::g_a5922e)); ++i) {
            void* const e = global<void**>(d::g_a5922a)[i];
            for (i32 j = 0; j < 4; ++j) {
                if (jo::as_unit(e)->type_id == ids[j]) {
                    store_hero(j, e);
                    break;
                }
            }
        }
        for (i32 i = joan::orig::as_dynarray(obj(kUnits))->size(); i > 0; --i) {  // 0x00417ad0
            void* const u = joan::orig::as_dynarray(obj(kUnits))->ptr_at(joan::orig::arg::i32(i));  // 0x0041b3f0
            if (u == nullptr || jo::as_unit(u)->side != side || jo::as_unit(u)->rank < 4) continue;
            for (i32 j = 0; j < 4; ++j) {
                if (jo::as_unit(u)->type_id == ids[j]) {
                    store_hero(j, u);
                    break;
                }
            }
        }
        for (i32 i = 2; i > 0; --i) {
            jo::Unit** const heroes = jo::self<jo::CampScreen>(this)->heroes;
            if (heroes[i] == nullptr) {
                // TODO(fix): the ranges overlap (shift down by one slot); the original calls its memmove-equivalent here, memcpy forward copy happens to work
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wrestrict"
#endif
                std::memcpy(joan::orig::arg::mptr(&heroes[i]), joan::orig::arg::ptr(&heroes[i + 1]), joan::orig::arg::u32(static_cast<u32>(3 - i) * static_cast<u32>(sizeof(jo::Unit*))));  // memmove
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
                heroes[3] = nullptr;
            }
        }
        const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(obj(d::g_texture_array))->mode());  // 0x0060a720
        joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode(joan::orig::arg::u32(i32{1}));             // 0x0060a740
        for (i32 i = 0; i < 4; ++i) {
            void* const hero = jo::self<jo::CampScreen>(this)->heroes[i];
            if (hero == nullptr) break;
            const i32 k = jo::as_unit(hero)->collision_id;
            // the original tests the dword at +0x259 + k * 4 for any k and loads only for k 0..2
            if (k >= 0 && k < 3 && jo::self<jo::CampScreen>(this)->tex_heroes[k] == nullptr) {
                if (k == 0) jo::self<jo::CampScreen>(this)->tex_heroes[0] = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(str(d::s_debrief_1), i32{1}, joan::orig::arg::u32(i32{0})));  // 0x006099c0
                else if (k == 1) jo::self<jo::CampScreen>(this)->tex_heroes[1] = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(str(d::s_debrief_2), i32{1}, joan::orig::arg::u32(i32{0})));
                else if (k == 2) jo::self<jo::CampScreen>(this)->tex_heroes[2] = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(str(d::s_debrief_3), i32{1}, joan::orig::arg::u32(i32{0})));
            }
            const img::Addr names[4] = {d::s_JOAN_OF_ARC, d::s_JEAN_DE_METZ, d::s_LAHIRE_9cfb3c, d::s_ALENCON_9cfb30};  // JOAN_OF_ARC JEAN_DE_METZ LAHIRE ALENCON
            const img::Addr fmts[4] = {d::s_joan_1dsm, d::s_metz_1dsm, d::s_hire_1dsm, d::s_alen_1dsm};   // joan_ metz_ hire_ alen_ %1dsm
            for (i32 n = 0; n < 4; ++n) {
                const u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(jo::view<u16>(jo::as_unit(hero)->type_id))));  // 0x0042d0a0
                if (joan::crt::stricmp(jo::as_unit_type(rec)->name, joan::orig::arg::str(str(names[n]))) == 0) {
                    const char* const name = joan::orig::arg::as<const char*>(joan::orig::format_positional(joan::orig::arg::str(str(fmts[n])), k + 1));  // 0x005a6660
                    std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::as_button(jo::self<jo::CampScreen>(this)->hero_buttons[i])->states[0].sprite)), joan::orig::arg::str(name));  // strcpy (+0x97)
                    break;
                }
            }
        }
        if (jo::self<jo::CampScreen>(this)->tex_debrief == nullptr) jo::self<jo::CampScreen>(this)->tex_debrief = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(d::g_texture_array))->load(str(d::s_debrief), i32{1}, joan::orig::arg::u32(i32{0})));
        joan::orig::as_texture_manager(obj(d::g_texture_array))->set_mode(joan::orig::arg::u32(saved));  // 0x0060a740
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_20", name, addr, fn)
B("Debrief::setup_72cad0", 0x0072cad0, &H::debrief_setup);
#undef B

}  // namespace game_b108_20
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

void* const kImages = obj(d::g_texture_array);

void image_release(void* h) { joan::orig::as_texture_manager(kImages)->release(joan::orig::arg::mptr(h)); }                    // 0x0060a550

struct H {
    static void release_at(void*& tex) {
        if (tex != nullptr) {
            image_release(tex);
            tex = nullptr;
        }
    }

    // 0x0072d060: CampScreen +0x259 tex_heroes[3], then +0x255 tex_debrief
    void release_4() {
        for (std::int32_t i = 0; i < 3; ++i) release_at(jo::self<jo::CampScreen>(this)->tex_heroes[i]);
        release_at(jo::self<jo::CampScreen>(this)->tex_debrief);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Images4::release", 0x0072d060, &H::release_4);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b108_09.cpp (game/b108_09)
namespace {
namespace game_b108_09 {

// D3DCOLOR -> Colour 0x005e9640 on a stack temporary, then its dword.
u32 colour_from(u32 argb) {
    u32 tmp = 0;
    const u32* const c = joan::orig::arg::as<const u32*>(joan::orig::as_color(&tmp)->Color_dword(joan::orig::arg::u32(argb)));  // 0x005e9640
    return *c;
}

JOAN_COUNTER(c_72d120, "Widget::draw_smith_frame");

// 0x0072d120: widget draw callback (cdecl widget, ctx, flags).
i32 JOAN_CDECL draw_smith_frame(u8* w, u32 ctx, u32 flags) {  // ctx: the layer id (Button::draw's `arg`), not a pointer
    c_72d120.hit();
    const u32 f = flags & 0xff;
    if (f != 7 && (f & 1) != 0) return 0;
    i16 s4 = jo::as_widget(w)->x, s8 = jo::as_widget(w)->y, sc = jo::as_widget(w)->x2, s10 = jo::as_widget(w)->y2;
    float f14 = 1.0f, f18 = 1.0f;
    void* const canvas = obj(d::g_sprites_a46128);
    if (jo::as_widget(w)->fit != 0)
        joan::orig::as_sprite2d(canvas)->fit_rect(joan::orig::arg::i16(jo::as_widget(w)->x), joan::orig::arg::i16(jo::as_widget(w)->y), joan::orig::arg::i16(jo::as_widget(w)->x2), joan::orig::arg::i16(jo::as_widget(w)->y2), &f14, &f18, &s4, &s8, &sc, &s10, joan::orig::arg::i16(0x280), joan::orig::arg::i16(0x1e0));  // 0x00633e60
    const u32 c1 = colour_from(0xffffffffu);
    joan::orig::as_sprite_queue(canvas)->draw_named(joan::orig::arg::i16(s4), joan::orig::arg::i16(s8), ctx, joan::orig::arg::str(jo::as_button(w)->states[0].sprite), obj(d::g_bmp_debrief), joan::orig::arg::u32(c1), 3, 0, 0, 0, -1, -1, -1, -1, 0, joan::orig::arg::i16(sc), joan::orig::arg::i16(s10));  // 0x00633500
    if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(s4), static_cast<i32>(s8), static_cast<i32>(sc), static_cast<i32>(s10))) != 0) {  // 0x00593980
        const u32 c2 = colour_from(0xffffffffu);
        joan::orig::as_sprite_queue(canvas)->draw_named(joan::orig::arg::i16(s4), joan::orig::arg::i16(s8), ctx, str(d::s_sm_fh), obj(d::g_bmp_debrief), joan::orig::arg::u32(c2), 3, 0, 0, 0, -1, -1, -1, -1, 0, joan::orig::arg::i16(sc), joan::orig::arg::i16(s10));  // 0x00633500 'sm_fh'
    } else {
        const u32 c3 = colour_from(0xffffffffu);
        joan::orig::as_sprite_queue(canvas)->draw_named(joan::orig::arg::i16(s4), joan::orig::arg::i16(s8), ctx, str(d::s_sm_f), obj(d::g_bmp_debrief), joan::orig::arg::u32(c3), 3, 0, 0, 0, -1, -1, -1, -1, 0, joan::orig::arg::i16(sc), joan::orig::arg::i16(s10));  // 0x00633500 'sm_f'
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_09", name, addr, fn)
B("Widget::draw_smith_frame", 0x0072d120, &draw_smith_frame);
#undef B

}  // namespace game_b108_09
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Panel::build_72a180() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b102_16::H::build_72a180>(this)); }  // 0x72a180
auto joan::orig::ObjMenu::free_debrief() -> void { joan::bind::member<&::game_b97_08::H::free_panels>(this); }  // 0x72a470
auto joan::orig::ObjMenu::draw_debrief(std::uint32_t a0) -> void { joan::bind::member<&::game_b110_12::H::draw_results>(this, a0); }  // 0x72a620
auto joan::orig::ObjMenu::update_debrief() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b108_17::H::pick_portrait>(this)); }  // 0x72c740
auto joan::orig::Screen::nop_72c9f0() -> void { joan::bind::member<&::game_b103_16::H::nop>(this); }  // 0x72c9f0
auto joan::orig::Screen::nop_72ca10() -> void { joan::bind::member<&::game_b103_16::H::nop>(this); }  // 0x72ca10
auto joan::orig::ObjMenu::enter_debrief() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_19::H::reset_72ca30>(this)); }  // 0x72ca30
auto joan::orig::ObjMenu::leave_debrief() -> void { joan::bind::member<&::game_b93_06::H::reset_72ca90>(this); }  // 0x72ca90
auto joan::orig::ObjMenu::collect_debrief_heroes() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_20::H::debrief_setup>(this)); }  // 0x72cad0
auto joan::orig::ObjMenu::free_debrief_images() -> void { joan::bind::member<&::game_b92_06::H::release_4>(this); }  // 0x72d060
