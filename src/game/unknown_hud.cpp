// game/unknown_hud.cpp: functions whose original source file is unknown (formerly game/_unplaced/hud.cpp;
// re/attribution.csv: between ww/interfac/list.cpp and ww/interfac/minires.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include "../compiler.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "pad_feedback.hpp"
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
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;

u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }

const Fpu fpu{true};

void* const kUnits = obj(d::g_units);

using joan::orig::helpers::wadd;
using joan::orig::helpers::wsub;

using joan::call::cdecl_call;

using joan::orig::helpers::imul;

void* const kSoldiers = obj(d::g_units);

}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16 {

using joan::call::thiscall;
using joan::call::stdcall_call;

using joan::orig::helpers::crt_memset;
i32 ftol(double v) { return joan::hook::x87::ftol(v); }

JOAN_COUNTER(c_group_avg, "Units::group_average");
JOAN_COUNTER(c_blink_all, "Hud::blink_all");
JOAN_COUNTER(c_blink_restart, "Hud::blink_restart");

struct H {
    // Blink timer restart (+0x14f state, +0x153 time), shared by 0x707b20 / 0x707ce0.
    void blink_timer() {
        if (jo::self<jo::Hud>(this)->blink_time == 0) {
            jo::view<u32>(jo::self<jo::Hud>(this)->blink_time) = 1;
            jo::self<jo::Hud>(this)->blink_on = 1;
            jo::self<jo::Hud>(this)->blink_start = global<u32>(d::g_now);
            jo::self<jo::Hud>(this)->blink_state = 0;
        }
        if (jo::self<jo::Hud>(this)->blink_time == 2) {
            jo::view<u32>(jo::self<jo::Hud>(this)->blink_time) = 1;
            const double now = static_cast<double>(global<u32>(d::g_now));                  // fild qword {now, 0}
            const double dt = static_cast<double>(global<u32>(d::g_now) - jo::self<jo::Hud>(this)->blink_start);  // fild qword {now - t0, 0}
            double v = fpu.div(dt, global<float>(d::flt_3000));  // / 3000
            v = fpu.mul(v, global<float>(d::flt_255));          // * 255
            v = fpu.sub(global<float>(d::flt_255), v);          // 255 - v
            v = fpu.mul(v, global<float>(d::flt_2000));          // * 2000
            v = fpu.div(v, global<float>(d::flt_255));          // / 255
            v = fpu.sub(now, v);                                // fsubp st(1),st
            jo::view<i32>(jo::self<jo::Hud>(this)->blink_start) = ftol(v);
            jo::self<jo::Hud>(this)->blink_on = 1;
            jo::self<jo::Hud>(this)->blink_state = 0;
        }
    }

    // ------------------------------------------------------------------ 0x006f9440
    void* ctor_6f9440() {
        joan::orig::libc::eh_vector_constructor_iterator(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Hud>(this)->squad_button0)), sizeof(jo::Button), 8, fn::code_value(fn::Widget_ctor_620ee0),  // 0x00620ee0, 0x346 bytes on i686
                     fn::code_value(fn::Widget_dtor_6211a0));  // 0x006211a0 (eh vector ctor)
        joan::orig::as_mini_map(&jo::self<jo::Hud>(this)->minimap)->zero();  // 0x0070e190
        // 0x7c1 bytes on i686: up to squad_button1 + 0x142 (inside its tooltip), as the original.
        crt_memset(this, 0, static_cast<u32>(offsetof(jo::Hud, squad_button1) + offsetof(jo::Button, tooltip) + 0x35));
        return this;
    }

    // ------------------------------------------------------------------ 0x006ffb40
    i16 group_average(u32 id) {
        c_group_avg.hit();
        u8* const rec = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        i32 sum = jo::as_unit(rec)->stats[7];
        i16 n = 1;
        if (jo::as_group(jo::as_unit(rec)->group)->count == 0) return 0;
        for (i32 i = 0; i < static_cast<i32>(jo::as_group(jo::as_unit(rec)->group)->count); ++i) {
            const i32 k = jo::as_group(jo::as_unit(rec)->group)->members[i];
            if (joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(k)) != 0) continue;  // 0x00428b70
            u8* const m = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(k)));  // 0x0041b3c0
            sum += jo::as_unit(m)->stats[7];
            n = static_cast<i16>(n + 1);
        }
        return static_cast<i16>(sum / static_cast<i32>(n));
    }

    // ------------------------------------------------------------------ 0x00707b20
    void blink_all(i32 k) {
        c_blink_all.hit();
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Hud>(this)->obj_count); ++i) {
            if (jo::self<jo::Hud>(this)->obj_id[i] - 1u != static_cast<u32>(k)) continue;
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(d::g_strings))->open_2());  // 0x004dcb00
            jo::self<jo::Hud>(this)->obj_alpha[i] = static_cast<u16>(crt_strlen(s) + 1);
            jo::self<jo::Hud>(this)->obj_1a7[i] = 1;
            jo::self<jo::Hud>(this)->obj_time[i] = global<u32>(d::g_now);
        }
        jo::self<jo::Hud>(this)->blink_start = global<u32>(d::g_now);
        blink_timer();
    }

    // ------------------------------------------------------------------ 0x00707ce0
    void blink_restart() {
        c_blink_restart.hit();
        blink_timer();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Obj::ctor_6f9440", 0x006f9440, &H::ctor_6f9440);
B("Units::group_average", 0x006ffb40, &H::group_average);
B("Hud::blink_all", 0x00707b20, &H::blink_all);
B("Hud::blink_restart", 0x00707ce0, &H::blink_restart);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
namespace {
namespace game_b108_02 {

using joan::call::thiscall;

JOAN_COUNTER(c_panel_init, "Panel::init_6f94f0");

struct H {
    // 0x006f94f0
    void init_6f94f0() {
        c_panel_init.hit();
        jo::view<u8>(jo::self<jo::Hud>(this)->target_is_unit) = 1;
        jo::self<jo::Hud>(this)->sprites = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_maingame));
        jo::self<jo::Hud>(this)->exp_sprites = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_mgexp));
        jo::self<jo::Hud>(this)->sprites_18 = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_hurt));
        jo::self<jo::Hud>(this)->target_sprites = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_tui));
        jo::self<jo::Hud>(this)->tex_maingame = nullptr;
        jo::self<jo::Hud>(this)->tex_mgexp = nullptr;
        jo::self<jo::Hud>(this)->tex_08 = nullptr;
        jo::self<jo::Hud>(this)->tex_tui = nullptr;
        joan::orig::as_hud(&jo::self<jo::Hud>(this)->minimap)->init_70e1d0();  // 0x0070e1d0
        jo::view<u8>(jo::self<jo::Hud>(this)->health_state) = 0;
        jo::view<u8>(jo::self<jo::Hud>(this)->level_on) = 0;
        jo::self<jo::Hud>(this)->level_state = 0;
        jo::view<u8>(jo::self<jo::Hud>(this)->combo_on) = 0;
        jo::view<u8>(jo::self<jo::Hud>(this)->combo_state) = 0;
        jo::view<u16>(jo::self<jo::Hud>(this)->exp_count) = 0;
        jo::view<u8>(jo::self<jo::Hud>(this)->exp_on) = 1;
        jo::view<u16>(jo::self<jo::Hud>(this)->arrow_count) = 0;
        jo::self<jo::Hud>(this)->u8_32 = 0;
        jo::view<u16>(jo::self<jo::Hud>(this)->obj_count) = 0;
        jo::Button* const squad = &jo::self<jo::Hud>(this)->squad_button0;  // squad_button0 .. squad_button7
        for (i32 i = 0; i < 8; ++i) {
            const i32 x = wadd(wadd(global<i32>(d::k_9de64c + i * 4), global<i32>(d::g_screen_w)), -0x320);
            const i32 y = wsub(global<i32>(d::g_screen_h), 0x5a);
            joan::orig::as_menu_button(&squad[i])->setup(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::mptr(jo::self<jo::Hud>(this)->sprites), str(d::s_RESUMEOT), str(d::s_RESUMEDW), str(d::s_RESUMEOV), str(d::s_RESUMEOT), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(0)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)), static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // 0x006211d0
            joan::orig::as_button(&squad[i])->set_hit_triangle(3, 3, 0x16, 3, 3, 0x1b, 1u);  // 0x00623380
        }
        for (i32 i = 0; i < 8; ++i) {
            const u16 id = joan::orig::arg::as<u16>(joan::orig::as_unit_types(obj(d::g_types))->find(joan::orig::arg::str(str(d::g_a27d48 + static_cast<u32>(i) * 0x21))));  // 0x005270b0
            global<u16>(d::g_a6a570 + i * 2) = id;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Panel::init_6f94f0", 0x006f94f0, &H::init_6f94f0);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

using joan::call::thiscall;

struct H {
    // 0x006f9720
    void screen_close() {
        joan::orig::as_hud(this)->free_resources();              // 0x006f98a0
        joan::orig::as_screen(&jo::self<jo::Hud>(this)->minimap)->release_70e3c0();  // 0x0070e3c0
        if (jo::view<void*>(jo::self<jo::Hud>(this)->portrait_actor) != nullptr) {
            joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::Hud>(this)->portrait_actor))->destroy(1u);  // 0x00420840
            jo::self<jo::Hud>(this)->portrait_actor = nullptr;
        }
        jo::self<jo::Hud>(this)->obj_count = 0;
        jo::self<jo::Hud>(this)->blink_on = 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Screen::close_6f9720", 0x006f9720, &H::screen_close);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_ui7.cpp (game/ui7)
namespace {
namespace game_ui7 {

using joan::call::thiscall;

constexpr img::Addr kResources = d::g_texture_array; // resource group manager

JOAN_COUNTER(c_hud_load, "Hud::load_resources");
JOAN_COUNTER(c_pause, "Hud::draw_pause_caption");

// 0x0042a260 Color(r, g, b, a) on a 4-byte local (ret 0x10), the dword read back.
std::uint32_t color(std::int32_t r, std::int32_t g, std::int32_t b, std::int32_t a) {
    std::uint32_t c = 0;
    std::uint32_t* const p = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
    return *p;
}
std::uint32_t white() { return color(0xff, 0xff, 0xff, 0xff); }

struct H {
    // 0x006f97d0 Hud::load_resources. ret 0.
    std::uint32_t load_resources() {
        c_hud_load.hit();
        const std::uint32_t saved = joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture_manager(obj(kResources))->mode());  // 0x0060a720
        joan::orig::as_texture_manager(obj(kResources))->set_mode(1u);                           // 0x0060a740
        if (jo::self<jo::Hud>(this)->tex_maingame == nullptr) jo::self<jo::Hud>(this)->tex_maingame = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(obj(kResources))->load(str(d::s_MAINGAME), 1, 0u));  // 0x006099c0 "MAINGAME"
        if (jo::self<jo::Hud>(this)->tex_mgexp == nullptr) jo::self<jo::Hud>(this)->tex_mgexp = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(obj(kResources))->load(str(d::s_MGEXP), 1, 0u));  // "MGEXP"
        if (jo::self<jo::Hud>(this)->tex_tui == nullptr) jo::self<jo::Hud>(this)->tex_tui = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(obj(kResources))->load(str(d::s_TUI), 1, 0u));  // "TUI"
        joan::orig::as_texture_manager(obj(kResources))->set_mode_60a740(saved);  // 0x0060a740
        return 0;  // 0x0060a740 returns nothing; the only caller discards the result
    }

    // 0x006f98a0 Hud::free_resources. ret 0.
    H* free_resources() {
        jo::Texture** const tex = &jo::self<jo::Hud>(this)->tex_maingame;  // tex_maingame, tex_mgexp, tex_08, tex_tui
        for (std::int32_t i = 0; i < 4; ++i) {
            if (tex[i] != nullptr) {
                joan::orig::as_texture_manager(obj(kResources))->release(joan::orig::arg::mptr(tex[i]));  // 0x0060a550
                tex[i] = nullptr;
            }
        }
        return this;
    }

    // 0x006f9a60 Hud::draw_pause_caption. ret 4.
    void draw_pause_caption(char flags) {
        joan::canvas::ScreenScope canvas_scope(0x006f9a60);  // canvas: a 1024x768-layout caption at the top (hud_top_right: under the minimap, UiScale)
        if (static_cast<std::int8_t>(joan::orig::arg::as<char>(joan::orig::as_game(obj(d::g_game_state))->flag_744())) != 0) return;  // 0x004301c0
        if (static_cast<std::int8_t>(joan::orig::arg::as<char>(joan::orig::as_shop(obj(d::g_game_a5d380))->active())) != 0) return;  // 0x0042ced0
        if (global<std::int8_t>(d::g_fade_title) != 0) return;
        if (global<std::int8_t>(d::g_a46048) != 0) return;
        if (global<std::int32_t>(d::g_mission) != 8) return;
        if ((static_cast<std::int32_t>(flags) & 1) == 0) return;
        c_pause.hit();
        const std::uint32_t c = white();
        const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(d::g_strings))->state4(joan::orig::arg::u32(global<std::uint32_t>(d::g_get_a591d8))));  // 0x004dcb60
        joan::orig::as_text(obj(d::g_font_goudy12_bold_black))->draw_line(0x337, 0xdc, 9u, text, joan::orig::arg::u32(c), joan::orig::arg::i32(-1), 0, 1, 2, 0, 1.0f, 1.0f);  // 0x006255d0
    }
};

#define U7(name, addr, fn) JOAN_REPLACE("game/ui7", name, addr, fn)
U7("Hud::load_resources", 0x006f97d0, &H::load_resources);
U7("Hud::free_resources", 0x006f98a0, &H::free_resources);
U7("Hud::draw_pause_caption", 0x006f9a60, &H::draw_pause_caption);
JOAN_CANVAS_SELF(0x006f9a60);  // ScreenScope in the body (no thunk wrapper)
#undef U7

}  // namespace game_ui7
}  // namespace

// ---------------------------------------- from modules/game_b116_03.cpp (fix/uninit)
namespace {
namespace game_b116_03 {

constexpr img::Addr kTextRes = d::g_strings;    // localised text resource

// Not joan::call::thiscall: a __thiscall function pointer, not a pointer to member (a class-type R is returned
// as a free function returns it); the target is an fn:: name or a computed address (see src/call.hpp).
template <class R = void, class T, class... A>
R thiscall(T address, const void* self, A... args) {
    return reinterpret_cast<R(JOAN_THISCALL*)(void*, A...)>(joan::fn::target(address))(const_cast<void*>(self), args...);
}

JOAN_COUNTER(c_objectives, "Hud::draw_objectives");
JOAN_COUNTER(c_corner, "Hud::draw_corner_icon");

constexpr img::Addr kTime = d::g_now;       // game time (ms)
constexpr img::Addr kSprites = d::g_sprites_a46128;    // 2D sprite drawer (0x00633500)
constexpr img::Addr kFont = d::g_font_goudy12_bold_black;       // Text (0x006255d0)
constexpr img::Addr kEmpty = d::kEmpty;      // ""
constexpr img::Addr kSpace = d::s_sp_9d05f0;      // " "

// The objective record of index `idx` (movsx word): Game (0x00a58f90) end_conditions[idx].
jo::EndCon* record(i16 idx) { return &jo::as_game(obj(d::g_game_state))->end_conditions[idx]; }

// 0x0042a260 colour ctor (thiscall (r, g, b, a) bytes, ret 0x10) into `slot`; returns the dword.
u32 make_colour(void* slot, u32 r, u32 g, u32 b, u32 a) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
}

// 0x00633500 (ret 0x44): sprite `name` at (x, y), 17 arguments.
void draw_sprite(i32 x, i32 y, const char* name, void* ctx, u32 colour, i32 a8, i32 a9, i32 a10) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 9u, joan::orig::arg::str(name), joan::orig::arg::mptr(ctx), joan::orig::arg::u32(colour), 3, joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
}

// The emulated 0x900-byte frame of 0x00704810: ref<T>(n) = [ebp - n].
struct Frame {
    alignas(16) u8 bytes[0x900];
    u8* ebp() { return bytes + sizeof bytes; }
    u8* p(i32 n) { return ebp() - n; }
    template <class T>
    T& ref(i32 n) {
        return *reinterpret_cast<T*>(p(n));
    }
};

struct Objectives {
    Frame f;
    u8* self_ = nullptr;  // [ebp-0x4] of the original frame (kept as a pointer: 8 bytes on x64)

    u8* self() { return self_; }
    i32 i() { return f.ref<i32>(0x18); }
    u32 time() { return global<u32>(kTime); }

    // fild qword (u32 delta) / fdiv div / fmul 255 [/ fsubr 255].
    static double fade_in(u32 delta, float div) {
        const Fpu x{true};
        return x.mul(x.div(static_cast<double>(delta), static_cast<double>(div)), 255.0);
    }
    static double fade_out(u32 delta, float div) {
        const Fpu x{true};
        return x.sub(255.0, fade_in(delta, div));
    }
    // fade_out clamped: > 0 keeps the value, else 0; fstp float, fld, _ftol.
    static i32 fade_out_alpha(u32 delta, float div) {
        const double v = fade_out(delta, div);
        const float stored = (v > 0.0) ? static_cast<float>(v) : 0.0f;
        return joan::hook::x87::ftol(static_cast<double>(stored));
    }

    // The +0x14f state machine (identical blocks at 0x007048e6 / 0x007055df). True = return.
    bool panel_state() {
        const i32 state = jo::as_hud(self())->blink_time;
        if (state == 1) {
            const double v = fade_in(time() - jo::as_hud(self())->blink_start, 2000.0f);
            const float stored = !(v >= 255.0) ? static_cast<float>(v) : 255.0f;
            f.ref<i16>(8) = static_cast<i16>(joan::hook::x87::ftol(static_cast<double>(stored)));
            if (static_cast<i32>(f.ref<i16>(8)) == 0xff) jo::view<u32>(jo::as_hud(self())->blink_time) = 3;
        } else if (state == 2) {
            f.ref<i16>(8) = static_cast<i16>(fade_out_alpha(time() - jo::as_hud(self())->blink_start, 3000.0f));
            if (f.ref<i16>(8) == 0) {
                jo::view<u32>(jo::as_hud(self())->blink_time) = 0;
                jo::as_hud(self())->blink_on = 0;
                return true;
            }
        } else if (state == 3) {
            f.ref<i16>(8) = 0xff;
            if (jo::view<i8>(jo::as_hud(self())->blink_state) == 0 && time() - jo::as_hud(self())->blink_start >= 0x2ee0u) {
                jo::view<u32>(jo::as_hud(self())->blink_time) = 2;
                jo::as_hud(self())->blink_start = time();
            }
        }
        return false;
    }

    // Label text by mode (+0x1a7[i]): strcpy / strcat " " into buf, length word, label colour
    // into [ebp-0xc] (blocks at 0x00704af8 / 0x007057ee). Mode 0 of pass 1 stores the length
    // before the strcat, pass 2 after it (`late_cat`).
    void label(i32 buf, i32 len, i32 slot0, i32 slot1, i32 slot2, bool late_cat) {
        const i32 mode = jo::view<i16>(jo::as_hud(self())->obj_1a7[i()]);
        char* const b = reinterpret_cast<char*>(f.p(buf));
        if (mode == 0 && late_cat) {
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(b)), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(kTextRes))->open_1())));  // 0x004dcad0, strcpy
            f.ref<u16>(len) = static_cast<u16>(
                joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(kTextRes))->open_1())))) + 1);  // strlen
            f.ref<u32>(0xc) = make_colour(f.p(slot0), 0xf2, 0xd5, 0x31, 0xff);
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(b)), joan::orig::arg::str(str(kSpace)));  // strcat
        } else if (mode == 0) {
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(b)), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(kTextRes))->open_1())));  // 0x004dcad0
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(b)), joan::orig::arg::str(str(kSpace)));
            f.ref<u32>(0xc) = make_colour(f.p(slot0), 0xf2, 0xd5, 0x31, 0xff);
            f.ref<u16>(len) = static_cast<u16>(
                joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(kTextRes))->open_1())))) + 1);
        } else if (mode == 1) {
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(b)), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(kTextRes))->open_2())));  // 0x004dcb00
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(b)), joan::orig::arg::str(str(kSpace)));
            f.ref<u32>(0xc) = make_colour(f.p(slot1), 0xff, 0, 0, 0xff);
            f.ref<u16>(len) = static_cast<u16>(
                joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(kTextRes))->open_2())))) + 1);
        } else if (mode == 2) {
            std::strcpy(static_cast<char*>(joan::orig::arg::mptr(b)), joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(kTextRes))->open_3())));  // 0x004dcb30
            std::strcat(static_cast<char*>(joan::orig::arg::mptr(b)), joan::orig::arg::str(str(kSpace)));
            f.ref<u32>(0xc) = make_colour(f.p(slot2), 0, 0xff, 0, 0xff);
            f.ref<u16>(len) = static_cast<u16>(
                joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(kTextRes))->open_3())))) + 1);
        }
    }

    // assign (0x005a1440, strncpy 0x190) of buf + count - 1: lea [ebp + count - (buf + 1)].
    void assign_suffix(i32 text, i32 buf, i32 cnt) {
        const std::uintptr_t a =
            reinterpret_cast<std::uintptr_t>(f.ebp()) + static_cast<std::uintptr_t>(static_cast<i32>(f.ref<i16>(cnt))) -
            static_cast<std::uintptr_t>(buf + 1);
        joan::orig::as_string(f.p(text))->assign(joan::orig::arg::str(reinterpret_cast<const char*>(a)));  // 0x005a1440
    }

    // One channel of the blend: word tmp = byte cur[k]; if (len) tmp += (tgt[k] - cur[k]) / len *
    // (len - cnt) (16-bit add).
    void blend_channel(i32 tmp, i32 cur, i32 tgt, i32 len, i32 cnt) {
        f.ref<u16>(tmp) = f.ref<u8>(cur);
        if (f.ref<i16>(len) != 0) {
            i32 a = static_cast<i32>(f.ref<u8>(tgt)) - static_cast<i32>(f.ref<u8>(cur));
            a /= static_cast<i32>(f.ref<i16>(len));
            const i32 m = static_cast<i32>(f.ref<i16>(len)) - static_cast<i32>(f.ref<i16>(cnt));
            a = static_cast<i32>(static_cast<u32>(a) * static_cast<u32>(m));
            f.ref<u16>(tmp) = static_cast<u16>(f.ref<u16>(tmp) + static_cast<u16>(a));
        }
    }
    // Blend of the row colour [ebp-cur] towards the label colour [ebp-0xc]; new colour -> [ebp-cur].
    void blend(i32 cur, i32 len, i32 cnt, i32 t0, i32 t1, i32 t2, i32 slot) {
        blend_channel(t0, cur, 0xc, len, cnt);
        blend_channel(t1, cur - 1, 0xb, len, cnt);
        blend_channel(t2, cur - 2, 0xa, len, cnt);
        f.ref<u32>(cur) = make_colour(f.p(slot), f.ref<u8>(t0), f.ref<u8>(t1), f.ref<u8>(t2), f.ref<u8>(8));
    }

    // Counter tick: every > 100 ms the typing counter loses one.
    void tick() {
        if (time() - jo::as_hud(self())->obj_time[i()] > 0x64u) {
            jo::as_hud(self())->obj_alpha[i()] = static_cast<u16>(jo::as_hud(self())->obj_alpha[i()] - 1);
            jo::as_hud(self())->obj_time[i()] = time();
        }
    }
    u32 row_delta() { return time() - jo::as_hud(self())->obj_time[i()]; }
    void reset_row() {
        jo::as_hud(self())->obj_1a7[i()] = 0xffff;
        jo::as_hud(self())->obj_alpha[i()] = 0;
        jo::as_hud(self())->obj_time[i()] = 0;
    }

    // LIGHTBAR (even row) / DARKBAR (odd row) at row * 0x14 + 0x94.
    void bar(i32 light_slot, i32 light_tmp, i32 dark_slot, i32 dark_tmp) {
        const i32 row = f.ref<i32>(0x10);
        const bool even = (row % 2) == 0;
        const i32 slot = even ? light_slot : dark_slot;
        const i32 tmp = even ? light_tmp : dark_tmp;
        f.ref<u32>(tmp) = make_colour(f.p(slot), 0xff, 0xff, 0xff, f.ref<u8>(8));
        draw_sprite(0, row * 0x14 + 0x94, str(even ? img::Addr{d::s_LIGHTBAR} : img::Addr{d::s_DARKBAR}), jo::as_hud(self())->sprites, f.ref<u32>(tmp),
                    0, 2, 1);
    }

    // Kind 1 / 2 suffix (sprintf of the localised result, appended with 0x005a1550) and the text.
    void row_text(i16 idx, i32 text, i32 buf1, i32 buf2, i32 colour, i32 colour_tmp) {
        const i32 kind = record(idx)->i16_b;
        if (kind == 1) {
            const u32 secs = global<u32>(d::g_get_a59171) / 1000u;
            const u32 minutes = (global<u32>(d::g_get_a591cc) - secs) / 60u;
            const char* fmt = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(kTextRes))->open_5(joan::orig::arg::u32(minutes)));  // 0x004dcba0
            joan::orig::libc::sprintf(joan::orig::arg::str(reinterpret_cast<char*>(f.p(buf1))), joan::orig::arg::str(fmt));         // sprintf
            joan::orig::as_string(f.p(text))->append(joan::orig::arg::str(reinterpret_cast<const char*>(f.p(buf1))));  // 0x005a1550
        } else if (kind == 2) {
            const u32 d4 = global<u32>(d::g_scenario_rescue_limit);
            const u32 d0 = global<u32>(d::g_scenario_rescue_count);
            const char* fmt = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(kTextRes))->open_6(joan::orig::arg::u32(d0), joan::orig::arg::u32(d4)));  // 0x004dcbe0
            joan::orig::libc::sprintf(joan::orig::arg::str(reinterpret_cast<char*>(f.p(buf2))), joan::orig::arg::str(fmt));
            joan::orig::as_string(f.p(text))->append(joan::orig::arg::str(reinterpret_cast<const char*>(f.p(buf2))));
        }
        f.ref<u32>(colour_tmp) = f.ref<u32>(colour);
        const char* s = joan::orig::arg::as<const char*>(joan::orig::as_string(f.p(text))->c_str());  // 0x00417d90
        joan::orig::as_text(obj(kFont))->draw_line(i32{5}, joan::orig::arg::i32(f.ref<i32>(0x10) * 0x14 + 0x97), joan::orig::arg::u32(i32{9}), s, joan::orig::arg::u32(// 0x006255d0
                      f.ref<u32>(colour_tmp)), i32{-1}, i32{0}, i32{1}, i32{2}, i32{1}, 1.0f, 1.0f);
        f.ref<i32>(0x10) = f.ref<i32>(0x10) + 1;
    }

    // Row body shared by both passes; offsets per pass. Returns true when the function returns.
    struct Offsets {
        i32 idx, colour, header, text, len, buf;      // locals
        i32 lslot0, lslot1, lslot2, white_slot;       // label colour slots
        i32 a_cnt, a_t0, a_t1, a_t2, a_slot;          // typing
        i32 b_cnt, b_t0, b_t1, b_t2, b_slot, b_fade;  // mode 2, count 1
        i32 c_cnt, c_t0, c_t1, c_t2, c_slot;          // other modes, count 1
        bool fade_first;                              // pass 2: fade before the assign, mode-0 strcat first
    };
    bool row(const Offsets& o) {
        if (panel_state()) return true;
        joan::orig::as_string(f.p(o.header))->construct(joan::orig::arg::str(record(f.ref<i16>(o.idx))->text));  // 0x005a11a0
        joan::orig::as_string(f.p(o.text))->construct();                                          // 0x005a1170
        label(o.buf, o.len, o.lslot0, o.lslot1, o.lslot2, !o.fade_first);
        f.ref<u32>(o.colour) = make_colour(f.p(o.white_slot), 0xff, 0xff, 0xff, f.ref<u8>(8));
        const i32 count = jo::view<i16>(jo::as_hud(self())->obj_alpha[i()]);
        if (count != 0 && count != 1 && jo::as_hud(self())->blink_time == 3) {
            f.ref<i16>(o.a_cnt) = jo::view<i16>(jo::as_hud(self())->obj_alpha[i()]);
            assign_suffix(o.text, o.buf, o.a_cnt);
            blend(o.colour, o.len, o.a_cnt, o.a_t0, o.a_t1, o.a_t2, o.a_slot);
            tick();
            return false;
        }
        joan::orig::as_string(f.p(o.text))->assign(joan::orig::arg::str(str(kEmpty)));  // 0x005a1440
        if (jo::view<i16>(jo::as_hud(self())->obj_alpha[i()]) != 1) return false;
        if (jo::view<i16>(jo::as_hud(self())->obj_1a7[i()]) == 2) {
            f.ref<i16>(8) = 0xff;
            f.ref<i16>(o.b_cnt) = jo::view<i16>(jo::as_hud(self())->obj_alpha[i()]);
            assign_suffix(o.text, o.buf, o.b_cnt);
            blend(o.colour, o.len, o.b_cnt, o.b_t0, o.b_t1, o.b_t2, o.b_slot);
            f.ref<i16>(o.b_fade) = static_cast<i16>(fade_out_alpha(row_delta(), 2000.0f));
            if (f.ref<i16>(o.b_fade) == 0) reset_row();
            return false;
        }
        if (o.fade_first) {
            f.ref<i16>(8) = static_cast<i16>(fade_out_alpha(row_delta(), 2000.0f));
            f.ref<i16>(o.c_cnt) = jo::view<i16>(jo::as_hud(self())->obj_alpha[i()]);
            assign_suffix(o.text, o.buf, o.c_cnt);
        } else {
            f.ref<i16>(o.c_cnt) = jo::view<i16>(jo::as_hud(self())->obj_alpha[i()]);
            assign_suffix(o.text, o.buf, o.c_cnt);
            f.ref<i16>(8) = static_cast<i16>(fade_out_alpha(row_delta(), 2000.0f));
        }
        blend(o.colour, o.len, o.c_cnt, o.c_t0, o.c_t1, o.c_t2, o.c_slot);
        if (f.ref<i16>(8) == 0) {
            joan::orig::as_hud(self())->remove_slot(joan::orig::arg::u32(jo::view<i32>(jo::as_hud(self())->obj_id[i()]) - 1));  // 0x007072c0
            return true;
        }
        return false;
    }

    void run(void* hud) {
        std::memset(f.bytes, 0, sizeof f.bytes);
        self_ = static_cast<u8*>(hud);
        if (jo::view<i8>(jo::as_hud(self())->blink_on) == 0) {
            jo::view<u32>(jo::as_hud(self())->blink_time) = 0;
            return;
        }
        f.ref<i16>(8) = 0;
        joan::orig::as_colour(f.p(0xc))->construct(u32{0xf2}, u32{0xd5}, u32{0x31}, u32{0xff});  // 0x0042a260
        f.ref<i32>(0x10) = 0;
        f.ref<u8>(0x14) = 0;

        static constexpr Offsets kPass1{0x20,  0x1c,  0x1b4, 0x348, 0x34c, 0x36c, 0x7ac, 0x7b0, 0x7b4,
                                        0x7b8, 0x370, 0x374, 0x378, 0x37c, 0x7bc, 0x380, 0x384, 0x388,
                                        0x38c, 0x7c0, 0x390, 0x394, 0x398, 0x39c, 0x3a0, 0x7c4, false};
        static constexpr Offsets kPass2{0x3e8, 0x3e4, 0x57c, 0x710, 0x714, 0x734, 0x7dc, 0x7e0, 0x7e4,
                                        0x7e8, 0x738, 0x73c, 0x740, 0x744, 0x7ec, 0x748, 0x74c, 0x750,
                                        0x754, 0x7f0, 0x758, 0x75c, 0x760, 0x764, 0x768, 0x7f4, true};

        // Pass 1: open objectives.
        for (f.ref<i32>(0x18) = 0; f.ref<i32>(0x18) < static_cast<i32>(jo::as_hud(self())->obj_count);
             f.ref<i32>(0x18) = f.ref<i32>(0x18) + 1) {
            joan::orig::as_color(f.p(0x1c))->Color_dword(joan::orig::arg::u32(i32{-1}));  // 0x005e9640
            f.ref<i16>(0x20) = static_cast<i16>(jo::view<i32>(jo::as_hud(self())->obj_id[i()]) - 1);
            if (f.ref<i16>(0x20) < 0) continue;
            if (record(f.ref<i16>(0x20))->used == 0) continue;
            if (f.ref<i32>(0x10) == 10) continue;
            if (record(f.ref<i16>(0x20))->state != 0) continue;
            if (row(kPass1)) return;
            joan::orig::as_string(f.p(0x348))->append_str(joan::orig::as_string(f.p(0x1b4)));  // 0x005a14f0
            if (jo::view<u8>(record(f.ref<i16>(0x20))->state) != 0) continue;
            bar(0x7cc, 0x7c8, 0x7d4, 0x7d0);
            row_text(f.ref<i16>(0x20), 0x348, 0x3c0, 0x3e0, 0x1c, 0x7d8);
            f.ref<u8>(0x14) = 1;
        }

        // Pass 2: completed objectives (state 2), after a separator line.
        for (f.ref<i32>(0x18) = 0; f.ref<i32>(0x18) < static_cast<i32>(jo::as_hud(self())->obj_count);
             f.ref<i32>(0x18) = f.ref<i32>(0x18) + 1) {
            joan::orig::as_color(f.p(0x3e4))->Color_dword(joan::orig::arg::u32(i32{-1}));  // 0x005e9640
            f.ref<i16>(0x3e8) = static_cast<i16>(jo::view<i32>(jo::as_hud(self())->obj_id[i()]) - 1);
            if (record(f.ref<i16>(0x3e8))->used == 0) continue;
            if (f.ref<i32>(0x10) == 10) continue;
            if (record(f.ref<i16>(0x3e8))->state != 2) continue;
            if (row(kPass2)) return;
            joan::orig::as_string(f.p(0x710))->append_str(joan::orig::as_string(f.p(0x57c)));  // 0x005a14f0
            if (jo::view<u8>(record(f.ref<i16>(0x3e8))->state) != 2) continue;
            if (f.ref<i8>(0x14) != 0) {
                f.ref<u32>(0x7f8) = make_colour(f.p(0x7fc), 0xff, 0xff, 0xff, f.ref<u8>(8));
                draw_sprite(0, f.ref<i32>(0x10) * 0x14 + 0x93, str(d::s_LINE), jo::as_hud(self())->sprites,  // "LINE"
                            f.ref<u32>(0x7f8), 0, 2, 1);
                f.ref<u8>(0x14) = 0;
            }
            bar(0x804, 0x800, 0x80c, 0x808);
            row_text(f.ref<i16>(0x3e8), 0x710, 0x788, 0x7a8, 0x3e4, 0x810);
        }
    }
};
static_assert(sizeof(Frame) == 0x900, "frame size");

struct H {
    // -------------------------------------------------------------- 0x00704810
    void draw_objectives() {
        joan::canvas::ScreenScope canvas_scope(0x00704810);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_objectives.hit();
        Objectives o;
        o.run(this);
    }

    // -------------------------------------------------------------- 0x006f9970
    void draw_corner_icon(u32 flag) {
        joan::canvas::ScreenScope canvas_scope(0x006f9970);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_corner.hit();
        if (joan::orig::arg::as<u8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) != 0) return;  // 0x004301c0
        if (joan::orig::arg::as<u8>(joan::orig::as_shop(obj(d::g_game_a5d380))->active()) != 0) return;  // 0x0042ced0
        if (global<i8>(d::g_fade_title) != 0) return;
        if (global<i8>(d::g_a46048) != 0) return;
        if ((static_cast<u8>(flag) & 1) == 0) return;
        u32 slot = 0;
        const u32 colour = make_colour(&slot, 0xff, 0xff, 0xff, 0xff);
        draw_sprite(0x12, global<i32>(d::g_screen_h) - 0x4d, str(d::s_BACK), jo::self<jo::Hud>(this)->sprites, colour, 0, 0,  // "BACK"
                    0);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b116_03", name, addr, fn)
// fix/uninit: stale label buffer / length read for a desynchronised mode -1 row (see "Notes").
JOAN_REPLACE("fix/uninit", "Hud::draw_objectives", 0x00704810, &H::draw_objectives);
JOAN_CANVAS_SELF(0x00704810);  // ScreenScope in the body (no thunk wrapper)
B("Hud::draw_corner_icon", 0x006f9970, &H::draw_corner_icon);
JOAN_CANVAS_SELF(0x006f9970);  // ScreenScope in the body (no thunk wrapper)
#undef B

}  // namespace game_b116_03
}  // namespace

// ---------------------------------------- from modules/game_b116_01.cpp (game/b116_01)
namespace {
namespace game_b116_01 {

constexpr u32 kOne = 0x3f800000u;  // 1.0f pushed as an immediate

using joan::hook::x87::ftol;

using joan::call::thiscall;
// 32-bit imul (wrapping).
i32 mul32(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) * static_cast<u32>(b)); }
// 32-bit sub (wrapping).
i32 sub32(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(b)); }

i32 scr_w() { return global<i32>(d::g_screen_w); }
i32 scr_h() { return global<i32>(d::g_screen_h); }

void crt_strcpy(void* d, const char* s) { std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void crt_strcat(void* d, const char* s) { std::strcat(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
i32 crt_stricmp(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }

// Colour ctor 0x0042a260 (thiscall on a 4-byte temporary, 4 byte args, ret 0x10): all 0xff.
u32 white() {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));
}
// Rgba(dword) 0x005e9640 (thiscall on a temporary, ret 4).
u32 rgba(u32 v) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_color(&tmp)->Color_dword(joan::orig::arg::u32(v)));
}
// SpriteQueue::draw_named 0x00633500 (thiscall on 0xa46128, 17 args, ret 0x44): x, y, 9, name,
// set, colour, 3, 0, a9, 0, a11, a12, a13, a14, 0, -1, -1.
void sprite(i32 x, i32 y, const char* name, jo::BmpRes* set, u32 col, i32 a9, i32 a11, i32 a12, i32 a13, i32 a14) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 9u, joan::orig::arg::str(name), joan::orig::arg::mptr(set), joan::orig::arg::u32(col), 3, 0, joan::orig::arg::i16(a9), 0, joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), 0, -1, -1);
}
void sprite_plain(i32 x, i32 y, const char* name, jo::BmpRes* set, u32 col) {
    sprite(x, y, name, set, col, 0, -1, -1, -1, -1);
}
void sprite_member(i32 x, i32 y, const char* name, jo::BmpRes* set, u32 col) {
    sprite(x, y, name, set, col, 2, -1, -1, -1, -1);
}
// Bar sprite: source width / height words (args 13 / 14).
void sprite_bar(i32 x, i32 y, const char* name, jo::BmpRes* set, u32 col, i32 w, i32 h) {
    sprite(x, y, name, set, col, 0, 0, 0, w, h);
}
// Text::draw_2 0x00626080 (thiscall, 11 args, ret 0x2c): x, y, 9, text, colour, -1, 0, 0, 0, 1, 1.
void text2(img::Addr font, i32 x, i32 y, const char* text, u32 col) {
    joan::orig::as_text(obj(font))->draw_2(joan::orig::arg::i32(x), joan::orig::arg::i32(y), joan::orig::arg::u32(i32{9}), text, joan::orig::arg::u32(col), i32{-1}, i32{0}, i32{0}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
}
// Text::draw_3 0x00626130 (thiscall, 13 args, ret 0x34): x1, y1, x2, y2, 9, text, colour, -1,
// 0, 0, 0, 1, 1.
void text3(img::Addr font, i32 x1, i32 y1, i32 x2, i32 y2, const char* text, u32 col) {
    joan::orig::as_text(obj(font))->draw_3(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), joan::orig::arg::u32(i32{9}), text, joan::orig::arg::u32(col), i32{-1}, i32{0}, i32{0}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));
}
// Number / level texts on 0xa45fa8.
const char* num_text(i32 v) { return joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int(joan::orig::arg::u32(v), joan::orig::arg::u32(i32{1}))); }  // 0x004d69a0
const char* word_text(i32 w) {
    return joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_short(joan::orig::arg::i16(w), joan::orig::arg::u32(i32{1})));  // 0x004e1360 (word)
}
const char* fmt_text(i32 v, i32 mode) {
    return joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(joan::orig::arg::i32(v), joan::orig::arg::i32(mode)));  // 0x0058fca0
}

u32 unit_gone(u32 id) { return joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(id))); }  // 0x00428b70
u8* unit_get(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* unit_type(i32 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(w))); }  // 0x0042d0a0 (word)
u8* image(jo::BmpRes* set, img::Addr name) { return joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(set)->find(joan::orig::arg::str(str(name)))); }  // 0x00620b40
u8* level_row(i32 level) { return joan::orig::arg::as<u8*>(joan::orig::as_level_res(obj(d::g_level_res))->find(joan::orig::arg::i32(level))); }  // 0x004a4e60
u8* item_type(i32 w) { return joan::orig::arg::as<u8*>(joan::orig::as_records_2bf(obj(d::g_item_res))->get(joan::orig::arg::i16(w))); }  // 0x00425ce0 (word)

// 0x194-byte fixed string (0x005a1170 ctor; set 0x005a1440, append 0x005a1550, append int
// 0x005a15d0, c_str 0x00417d90).
struct alignas(4) Str {
    u8 b[0x194];
};
void s_init(Str& s) { joan::orig::as_string(&s)->construct(); }
void s_init(Str& s, const char* t) { joan::orig::as_string(&s)->construct(joan::orig::arg::str(t)); }
void s_set(Str& s, const char* t) { joan::orig::as_string(&s)->assign(joan::orig::arg::str(t)); }
void s_add(Str& s, const char* t) { joan::orig::as_string(&s)->append(joan::orig::arg::str(t)); }
void s_add_int(Str& s, i32 v) { joan::orig::as_string(&s)->append(joan::orig::arg::i32(v)); }
const char* s_cstr(Str& s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str()); }

struct Vec12 {
    u32 a, b, c;
};

// Top of the original frame: [ebp-0x58] name buffer .. [ebp-0x4] this (see the header).
struct Top {
    char buf[0x40];  // [ebp-0x58]
    i32 w18;         // [ebp-0x18] (word)
    i32 w14;         // [ebp-0x14] (word)
    u8* p10;         // [ebp-0x10] unit type
    const char* pc;  // [ebp-0xc]  portrait name
    u8* p8;          // [ebp-0x8]  target unit
    jo::Hud* self;   // [ebp-0x4]  this
    char spill[0x800];
};
static_assert(sizeof(void*) != 4 || sizeof(Top) == 0x58 + 0x800, "frame layout (i686)");

JOAN_COUNTER(c_hud_draw, "Hud::draw");

// ftol(int a / int b * k) at PC24.
i32 ratio_ftol(i32 a, i32 b, float k) {
    const Fpu f{true};
    return ftol(f.mul(f.div(static_cast<double>(a), static_cast<double>(b)), static_cast<double>(k)));
}
// `if (hud->+0x2c) w = w > 1 ? w : 1;` on a word.
i16 clamp_bar(const jo::Hud* self, i16 w) {
    if (self->target_hp != 0) {
        const i32 t = static_cast<i32>(w) > 1 ? static_cast<i32>(w) : 1;
        w = static_cast<i16>(t);
    }
    return w;
}

// Alpha byte of a member's colour: 0xff when visible (not +0x2f, not 0x00432b90, 0x00717a90
// on hud + 0x1d69 with the unit position +0xe), else 0x80.
u8 member_alpha(jo::Hud* self, u8* unit) {
    if (jo::as_unit(unit)->speech_off == 0 && joan::orig::arg::as<u8>(joan::orig::as_game(unit)->byte_2e1()) == 0) {  // 0x00432b90
        Vec12 v;
        std::memcpy(&v, jo::as_unit(unit)->position_, sizeof v);
        if (joan::orig::as_mini_map(&self->minimap)->near_tracked_unit(joan::orig::arg::f32(v.a), joan::orig::arg::f32(v.b), v.c)) return 0xff;  // 0x00717a90
    }
    return 0x80;
}

// CHIT / MHIT pair (0x006fb533.. / 0x006fb9ee..): `n` = the hit frame word.
void draw_hits(Str& s, jo::Hud* self, u8* unit, i32 slot, i16 n, u32 col) {
    if (static_cast<i32>(n) <= 0 || static_cast<i32>(n) >= 4) return;
    s_set(s, str(d::s_CHIT));  // 'CHIT'
    s_add(s, word_text(n));
    {
        const char* c = s_cstr(s);
        sprite_member(global<i32>(d::k_9de64c + static_cast<u32>(slot) * 4) + scr_w() - 0x320, scr_h() - 0x64, c,
                      self->sprites, col);
    }
    if (jo::as_unit(unit)->group != nullptr && static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count) > 0) {
        s_set(s, str(d::s_MHIT));  // 'MHIT'
        s_add(s, word_text(n));
        const char* c = s_cstr(s);
        sprite_member(global<i32>(d::k_9de64c + static_cast<u32>(slot) * 4) + scr_w() - 0x31a, scr_h() - 0x15, c,
                      self->sprites, col);
    }
}

struct Hud {
    void draw(char flags);
};

void Hud::draw(char flags) {
    joan::canvas::ScreenScope canvas_scope(0x006f9b60);  // canvas: what the thunk wrapper did (canvas_api.hpp)
    c_hud_draw.hit();
    Top F;
    F.self = jo::self<jo::Hud>(this);
    const i32 fl = static_cast<i32>(flags);

    if (joan::orig::arg::as<u8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) != 0) return;  // 0x004301c0
    if (joan::orig::arg::as<u8>(joan::orig::as_shop(obj(d::g_game_a5d380))->active()) != 0) return;  // 0x0042ced0
    if (global<i8>(d::g_fade_title) != 0) return;
    if (global<i8>(d::g_a46048) != 0) return;

    // ---------------------------------------------------------------- 1. target panel
    if (unit_gone(F.self->target_id) == 0 && static_cast<i32>(F.self->target_is_unit) == 1) {
        F.p8 = unit_get(F.self->target_id);
        if (fl & 1) {
            F.pc = joan::orig::arg::as<const char*>(joan::orig::as_hud(F.self)->siege_model_prefix(joan::orig::arg::u32(F.self->target_id)));  // 0x00703880
            F.p10 = unit_type(static_cast<i32>(jo::as_unit(F.p8)->type_id));
            const i32 kind = static_cast<i32>(jo::as_unit_type(F.p10)->kind);
            if (kind != 0 && kind != 1) {
                i16 w = static_cast<i16>(ratio_ftol(F.self->target_hp, jo::as_unit(F.p8)->hit_points_base,
                                                    global<float>(d::flt_78)));
                w = clamp_bar(F.self, w);
                F.w14 = w;
                sprite_bar(0x43, 0x72, str(d::s_EHPBAR), F.self->target_sprites, rgba(0xffffffffu), w, 8);  // EHPBAR
                if (w != 0) {
                    const u32 c = white();
                    sprite_plain(static_cast<i32>(w) + 0x43, 0x72, str(d::s_EHPTIP), F.self->target_sprites, c);  // EHPTIP
                }
                {
                    const u32 c = white();
                    sprite_plain(0, 0x50, str(d::s_SBASE), F.self->target_sprites, c);  // SBASE
                }
                {
                    const u32 c = white();
                    sprite_plain(8, 0x3c, str(reinterpret_cast<std::uintptr_t>(F.pc)), F.self->target_sprites, c);
                }
            } else {
                if (F.pc != nullptr) {
                    const u32 c = white();
                    sprite_plain(0, 0x15, str(reinterpret_cast<std::uintptr_t>(F.pc)), F.self->target_sprites, c);
                }
                {
                    const u32 c = white();
                    sprite_plain(0, 0x68, str(d::s_SWORD), F.self->target_sprites, c);  // SWORD
                }
                {
                    const u32 c = white();
                    sprite_plain(0x2e, 0x72, str(d::s_SWORDBG), F.self->target_sprites, c);  // SWORDBG
                }
                i16 w = static_cast<i16>(ratio_ftol(F.self->target_hp, jo::as_unit(F.p8)->hit_points_base,
                                                    global<float>(d::flt_94)));
                w = clamp_bar(F.self, w);
                F.w18 = w;
                sprite_bar(0x2f, 0x72, str(d::s_EHPBAR), F.self->target_sprites, rgba(0xffffffffu), w, 8);  // EHPBAR
                if (w != 0) {
                    const u32 c = white();
                    sprite_plain(static_cast<i32>(w) + 0x2f, 0x72, str(d::s_EHPTIP), F.self->target_sprites, c);
                }
            }
        }
        // 0x006fa04d: name text
        if ((fl & 4) && jo::as_unit(F.p8)->draw != nullptr) {
            // The whole frame object is handed out, so the compiler re-reads F.p8 / F.self after it.
            char* const name = reinterpret_cast<char*>(&F);
            bool named = false;
            if (jo::as_unit(F.p8)->group != nullptr) {
                const u8* g = unit_get(static_cast<u32>(static_cast<i32>(jo::as_group(jo::as_unit(F.p8)->group)->leader)));
                named = jo::as_unit(g)->rank == 4;
            }
            if (named) {
                crt_strcpy(name, str(reinterpret_cast<std::uintptr_t>(jo::as_unit(F.p8)->draw->label_text)));  // UnitDraw+0x6: the label text
            } else {
                crt_strcpy(name, jo::as_unit_type(unit_type(static_cast<i32>(jo::as_unit(F.p8)->type_id)))->title_b4);
                crt_strcat(name, str(d::s_sp_lparen));  // " ("
                crt_strcat(name, word_text(static_cast<i32>(jo::as_unit(F.p8)->level)));  // [ebp-0x8] after the copy
                crt_strcat(name, str(d::s_rparen));  // ")"
            }
            const u32 c = rgba(0xffffffffu);
            joan::orig::as_text(obj(d::g_font_goudy12_18_target_unit))->draw_line(i32{8}, i32{0x82}, joan::orig::arg::u32(i32{9}), static_cast<const char*>(name), joan::orig::arg::u32(c), i32{-1}, i32{0}, i32{0}, i32{0}, i32{0}, joan::orig::arg::f32(kOne), joan::orig::arg::f32(kOne));  // 0x006255d0
        }
    } else if (joan::orig::arg::as<u32>(joan::orig::as_units(obj(d::g_items))->list.empty_at(joan::orig::arg::i32(F.self->target_id))) == 0 &&  // 0x00425d50
               F.self->target_is_unit == 0) {
        // ---------------------------------------------------------------- 1b. building target
        const u8* rec = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(F.self->target_id)));  // 0x00425d20
        const u8* bt = item_type(static_cast<i32>(jo::as_map_object(rec)->type_id));
        if (jo::view<i32>(jo::as_item_def(bt)->struction_type) > 0 && (fl & 1)) {
            i16 w = static_cast<i16>(ratio_ftol(F.self->target_hp, jo::as_item_def(bt)->i32_5e, global<float>(d::flt_78)));
            w = clamp_bar(F.self, w);
            if (jo::view<i32>(jo::as_item_def(bt)->struction_type) == 3) {
                const float hp = static_cast<float>(F.self->target_hp);
                u8* o = joan::orig::arg::as<u8*>(joan::orig::as_units(obj(d::g_items))->list.ptr_at(joan::orig::arg::i32(F.self->target_id)));  // 0x00425d20
                const std::uintptr_t slot = static_cast<const std::uintptr_t*>(jo::as_map_object(o)->vtable)[0x4c / 4];  // slot 0x4c returns the Gate
                const u8* r = thiscall<u8*>(slot, o);
                const Fpu f{true};
                w = static_cast<i16>(ftol(f.mul(f.div(static_cast<double>(hp), static_cast<double>(jo::as_gate(r)->max_hp)),
                                                static_cast<double>(global<float>(d::flt_78)))));
                w = clamp_bar(F.self, w);
            }
            sprite_bar(0x43, 0x72, str(d::s_EHPBAR), F.self->target_sprites, rgba(0xffffffffu), w, 8);  // EHPBAR
            if (w != 0) {
                const u32 c = white();
                sprite_plain(static_cast<i32>(w) + 0x43, 0x72, str(d::s_EHPTIP), F.self->target_sprites, c);
            }
            {
                const u32 c = white();
                sprite_plain(0, 0x50, str(d::s_SBASE), F.self->target_sprites, c);  // SBASE
            }
            static const img::Addr kBuilding[6] = {d::s_TOWER, d::s_WALL, d::s_GATE,
                                                         d::s_WOODGATE, d::s_DOOR, d::s_TRUNK};
            const u32 k = static_cast<u32>(jo::view<i32>(jo::as_item_def(bt)->struction_type) - 1);
            if (k <= 5) {
                const u32 c = white();
                sprite_plain(8, 0x3c, str(kBuilding[k]), F.self->target_sprites, c);
            }
        }
    }

    joan::padfx::frame();  // pad vibration from the hero's hits / knock-downs / landed blows (game/pad_feedback.hpp)

    // ---------------------------------------------------------------- 2. hero panel
    if (unit_gone(joan::orig::arg::as<u32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_id())) != 0) return;  // 0x004203c0
    u8* const hero = unit_get(global<u32>(d::g_player_unit_recno));
    if (fl & 1) {
        {
            const u32 c = white();
            sprite_plain(0, scr_h() - 0x6c, str(d::s_PANEL), F.self->sprites, c);  // PANEL
        }
        Str portrait;
        const i32 type_id = static_cast<i32>(jo::as_unit(hero)->type_id);
        s_init(portrait, jo::as_unit_type(unit_type(static_cast<i32>(static_cast<i16>(type_id))))->portrait);
        if (F.self->health_state == 0) {
            s_add(portrait, str(d::s_1_9d8de4));  // "1"
        } else if (static_cast<i32>(F.self->health_state) == 3) {
            s_add(portrait, str(d::s_1_9d8de4));
        } else {
            s_add(portrait, str(d::s_1_9d8de4));
        }
        {
            const u32 c = white();
            jo::BmpRes* const set = F.self->sprites;
            const char* name = s_cstr(portrait);
            i32 x = 0xf;
            // [Game] AllHeroes: MAINGAME has the panel portrait ("<portrait>1", 58x90) of the four playable heroes
            // only. The Bastard of Orleans, Richemont and Charles VII get their army-row portrait (70x88), centred.
            if (joan::hook::settings().all_heroes != 0 &&
                joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(set)->find(joan::orig::arg::str(name))) == nullptr) {  // 0x00620b40
                const char* const plain = jo::as_unit_type(unit_type(static_cast<i32>(static_cast<i16>(type_id))))->portrait;
                if (joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(set)->find(joan::orig::arg::str(plain))) != nullptr) {
                    name = plain;
                    x -= 6;
                }
            }
            sprite_plain(x, scr_h() - 0x6c, name, set, c);
        }
        // hp bar
        {
            const i32 full = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(image(F.self->sprites, d::s_PHPBAR))->width));  // PHPBAR
            const i32 num = mul32(full, F.self->shown_hp);
            const i32 den = joan::orig::arg::as<i32>(joan::orig::as_unit(hero)->hit_points_max());  // 0x00523320
            const i32 w = num / den;
            const i32 h = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(image(F.self->sprites, d::s_PHPBAR))->height));
            const u32 c = white();
            sprite_bar(0x8d, scr_h() - 0x3a, str(d::s_PHPBAR), F.self->sprites, c, static_cast<i16>(w),
                       static_cast<i16>(h));
        }
        // ep bar
        {
            const i32 full = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(image(F.self->sprites, d::s_PEPBAR))->width));  // PEPBAR
            const i32 num = mul32(full, F.self->shown_145);
            const i32 den = joan::orig::arg::as<i32>(joan::orig::as_unit(hero)->value_149());  // 0x00523700
            const i32 w = num / den;
            const i32 h = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(image(F.self->sprites, d::s_PEPBAR))->height));
            const u32 c = white();
            sprite_bar(0x8d, scr_h() - 0x2a, str(d::s_PEPBAR), F.self->sprites, c, static_cast<i16>(w),
                       static_cast<i16>(h));
        }
        // xp bar
        if (static_cast<i32>(jo::as_unit(hero)->level) != 0x1e) {
            const i32 full = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(image(F.self->sprites, d::s_PLVLBAR))->width));  // PLVLBAR
            i32 prev;
            if (static_cast<i32>(jo::as_unit(hero)->level) == 1) {
                prev = 0;
            } else {
                prev = jo::as_level_rec(level_row(static_cast<i32>(jo::as_unit(hero)->level) - 1))->experience;
            }
            const i32 next = jo::as_level_rec(level_row(static_cast<i32>(jo::as_unit(hero)->level)))->experience;
            const i32 w = mul32(full, sub32(F.self->shown_126, prev)) / sub32(next, prev);
            const i32 h = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(image(F.self->sprites, d::s_PLVLBAR))->height));
            const u32 c = white();
            sprite_bar(0x8d, scr_h() - 0x14, str(d::s_PLVLBAR), F.self->sprites, c, static_cast<i16>(w),
                       static_cast<i16>(h));
        }
        joan::orig::as_hud(F.self)->draw_exp_counter();  // 0x006fe660
        joan::orig::as_hud(F.self)->update_level_banner();  // 0x006ffed0
        joan::orig::as_hud(F.self)->draw_combo_counter();  // 0x00702740
    }
    if (fl & 4) {
        char nums[0x40];  // [ebp-0x26c]
        crt_strcpy(nums, num_text(jo::as_unit(hero)->hit_points));
        crt_strcat(nums, str(d::s_slash));  // "/"
        crt_strcat(nums, num_text(jo::as_unit(hero)->hit_points_base));
        {
            const u32 c = rgba(0xffffffffu);
            text2(d::g_font_verdana_12_regular_black_out, 0xf2, scr_h() - 0x3d, nums, c);
        }
        if (static_cast<i32>(jo::as_unit(hero)->level) != 0x1e) {
            i32 have;
            if (static_cast<i32>(jo::as_unit(hero)->level) == 1) {
                have = jo::as_unit(hero)->experience;
            } else {
                const u8* p = level_row(static_cast<i32>(jo::as_unit(hero)->level) - 1);
                have = sub32(jo::as_unit(hero)->experience, jo::as_level_rec(p)->experience);
            }
            i32 span;
            if (static_cast<i32>(jo::as_unit(hero)->level) == 1) {
                span = jo::as_level_rec(level_row(static_cast<i32>(jo::as_unit(hero)->level)))->experience;
            } else {
                const u8* a = level_row(static_cast<i32>(jo::as_unit(hero)->level));
                const u8* b = level_row(static_cast<i32>(jo::as_unit(hero)->level) - 1);
                span = sub32(jo::as_level_rec(a)->experience, jo::as_level_rec(b)->experience);
            }
            const i32 pct = ratio_ftol(have, span, global<float>(d::flt_100));
            crt_strcpy(nums, fmt_text(pct, 3));
            const u32 c = rgba(0xffffffffu);
            text2(d::g_font_verdana_12_regular_black_out, 0xf2, scr_h() - 0x17, nums, c);
        }
        crt_strcpy(nums, num_text(jo::as_unit(hero)->mana));
        crt_strcat(nums, str(d::s_slash));  // "/"
        crt_strcat(nums, num_text(jo::as_unit(hero)->base_max_ep));
        {
            const u32 c = rgba(0xffffffffu);
            text2(d::g_font_verdana_12_regular_black_out, 0xf2, scr_h() - 0x2d, nums, c);
        }
        if (joan::orig::as_unit_data(jo::as_unit(hero)->modifiers)->slot_e() != nullptr) {  // 0x00497580
            const u32 c = rgba(0xffffffffu);
            const u8* q = joan::orig::arg::as<u8*>(joan::orig::as_unit_data(jo::as_unit(hero)->modifiers)->slot_e());
            const char* t = word_text(static_cast<i32>(jo::view<i16>(jo::as_item_stack(q)->count)));
            text3(d::g_font_verdana_12_regular_black_out, 0x68, scr_h() - 0x18, 0x7b, scr_h() - 0xd, t, c);
        } else {
            const u32 c = rgba(0xffffffffu);
            text3(d::g_font_verdana_12_regular_black_out, 0x68, scr_h() - 0x18, 0x7b, scr_h() - 0xd, str(d::s_0), c);  // "0"
        }
        {
            const u32 c = rgba(0xffffffffu);
            const char* t = word_text(static_cast<i32>(jo::as_unit(hero)->level));
            text3(d::g_font_verdana_12_gold_black_outline, 0x69, scr_h() - 0x38, 0x7b, scr_h() - 0x2d, t, c);
        }
    }
    joan::orig::as_critical_hud(obj(d::g_critical_hud))->draw();  // 0x0045fae0

    // ---------------------------------------------------------------- 3. group members
    if (fl & 1) {
        joan::orig::as_squad(F.self)->collect_slots();  // 0x006ff950
        Str s;     // [ebp-0x418]
        Str blank; // [ebp-0x5b4]
        for (i32 i = 0; i < static_cast<i32>(F.self->arrow_count); ++i) {
            u32* const id_addr = &F.self->arrow_unit[i];
            if (unit_gone(*id_addr) != 0) continue;
            u8* const unit = unit_get(*id_addr);
            const u32 col = 0x00ffffffu | static_cast<u32>(member_alpha(F.self, unit)) << 24;
            auto slot = [&]() { return 8 - static_cast<i32>(F.self->arrow_count) + i; };
            auto x64c = [&](i32 k) { return global<i32>(d::k_9de64c + static_cast<u32>(slot()) * 4) + scr_w() - k; };
            auto anim = [&]() -> i8& { return jo::view<i8>(F.self->slot_flag[slot()]); };
            auto stamp = [&]() -> u32& { return F.self->slot_time[slot()]; };
            auto flash = [&]() -> i8& { return jo::view<i8>(F.self->slot_state[slot()]); };

            if (global<i32>(d::g_control_mode) == 4 || *id_addr != global<u32>(d::g_player_unit_recno)) {
                sprite_member(x64c(0x320), scr_h() - 0x5a, str(d::s_HR_FRAME), F.self->sprites, col);  // HR_FRAME
                jo::Button* const btn = &(&F.self->squad_button0)[slot()];
                if (jo::as_unit(unit)->task.kind == 0 && jo::view<i32>(jo::as_unit(unit)->task.target) != 0) {
                    crt_strcpy(btn->states[0].sprite, str(d::s_HOLD_OT));  // HOLD_OT
                    crt_strcpy((&F.self->squad_button0)[slot()].states[1].sprite, str(d::s_HOLD_DW));  // HOLD_DW
                    crt_strcpy((&F.self->squad_button0)[slot()].states[2].sprite, str(d::s_HOLD_OV));  // HOLD_OV
                    crt_strcpy((&F.self->squad_button0)[slot()].states[3].sprite, str(d::s_HOLD_OT));  // HOLD_OT
                } else {
                    crt_strcpy(btn->states[0].sprite, str(d::s_RESUMEOT));  // RESUMEOT
                    crt_strcpy((&F.self->squad_button0)[slot()].states[1].sprite, str(d::s_RESUMEDW));  // RESUMEDW
                    crt_strcpy((&F.self->squad_button0)[slot()].states[2].sprite, str(d::s_RESUMEOV));  // RESUMEOV
                    crt_strcpy((&F.self->squad_button0)[slot()].states[3].sprite, str(d::s_RESUMEOT));  // RESUMEOT
                }
                joan::orig::as_button(&(&F.self->squad_button0)[slot()])->draw(9u, 7);  // 0x00622cc0
            }
            // 0x006fb2b6
            s_init(s);
            const Fpu f{true};
            const double q = f.div(static_cast<double>(jo::as_unit(unit)->hit_points), static_cast<double>(jo::as_unit(unit)->hit_points_base));
            if (!(q >= static_cast<double>(global<float>(d::g_a3cd24)))) {
                s_set(s, str(d::s_CPTOP));  // CPTOP
                {
                    jo::BmpRes* const set = F.self->sprites;
                    const char* c = s_cstr(s);
                    sprite_member(x64c(0x320), scr_h() - 0x64, c, set, col);
                }
                if (static_cast<i32>(anim()) % 3 != 0) {
                    s_set(s, str(d::s_DICON));  // DICON
                    jo::BmpRes* const set = F.self->sprites;
                    const char* c = s_cstr(s);
                    sprite_member(x64c(0x30c), scr_h() - 0x8c, c, set, col);
                }
                s_set(s, str(d::s_DSWORD));  // DSWORD
                s_add_int(s, static_cast<i32>(anim()) % 9 + 1);
                {
                    jo::BmpRes* const set = F.self->sprites;
                    const char* c = s_cstr(s);
                    sprite_member(x64c(0x30c), scr_h() - 0x8c, c, set, col);
                }
                draw_hits(s, F.self, unit, slot(), static_cast<i16>(static_cast<i32>(anim()) % 9 + 1), col);
                // 0x006fb69c
                if (global<u32>(d::g_now) - (stamp() + static_cast<u32>(static_cast<i32>(anim()) * 100)) >= 100u) {
                    anim() = static_cast<i8>(anim() + 1);
                }
                if (static_cast<i32>(anim()) >= 9) {
                    anim() = static_cast<i8>(static_cast<i32>(anim()) % 9);
                    stamp() = global<u32>(d::g_now);
                }
            } else if (joan::orig::as_unit(unit)->find_message(i32{1}) != nullptr) {  // 0x00414fe0
                flash() = 1;
                stamp() = global<u32>(d::g_now);
            } else if (flash() != 0) {
                s_set(s, str(d::s_DICON));  // DICON
                {
                    jo::BmpRes* const set = F.self->sprites;
                    const char* c = s_cstr(s);
                    sprite_member(x64c(0x30c), scr_h() - 0x8c, c, set, col);
                }
                s_set(s, str(d::s_DSWORD));  // DSWORD
                s_add_int(s, static_cast<i32>(anim()) % 9 + 1);
                {
                    jo::BmpRes* const set = F.self->sprites;
                    const char* c = s_cstr(s);
                    sprite_member(x64c(0x30c), scr_h() - 0x8c, c, set, col);
                }
                draw_hits(s, F.self, unit, slot(), static_cast<i16>(static_cast<i32>(anim()) % 9 + 1), col);
                // 0x006fbb57
                anim() = static_cast<i8>((global<u32>(d::g_now) - stamp()) / 100u % 9u);
                if (global<u32>(d::g_now) - stamp() >= 1000u) flash() = 0;
            }
            // 0x006fbc02
            s_init(blank);
            s_set(blank, str(d::s_sp_9d05f0));  // " "
            bool selected = false;
            if (joan::orig::arg::as<u8>(joan::orig::as_inventory(obj(d::g_a56398))->is_open()) != 0 &&  // 0x00430340
                joan::orig::arg::as<i32>(joan::orig::as_unit_attr_screen(obj(d::g_a56398))->unit_6af()) == static_cast<i32>(jo::as_unit(unit)->id)) {  // 0x006e0df0
                selected = true;
            } else if (joan::orig::arg::as<u8>(joan::orig::as_inventory(obj(d::g_a56398))->is_open()) == 0 && global<i32>(d::g_control_mode) == 4 &&
                       joan::orig::arg::as<i32>(joan::orig::as_player(obj(d::g_player_unit_recno))->unit_1238()) == static_cast<i32>(jo::as_unit(unit)->id)) {  // 0x006f5ed0
                selected = true;
            }
            if (selected) {
                F.self->u16_14b =
                    static_cast<u16>((global<u32>(d::g_game_time_ms) - F.self->u32_147) / 100u % 8u);
                s_set(s, str(d::s_CSELECT));  // CSELECT
                const i32 n = static_cast<i32>(jo::view<i16>(F.self->u16_14b));
                if (n > 0 && n < 4) {
                    s_add(s, word_text(static_cast<i32>(jo::view<i16>(F.self->u16_14b))));
                    jo::BmpRes* const set = F.self->sprites;
                    const char* c = s_cstr(s);
                    sprite_member(x64c(0x320), scr_h() - 0x64, c, set, col);
                }
            }
            // 0x006fbe7b
            if (jo::as_unit(unit)->group != nullptr && static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count) > 0) {
                const i16 morale = joan::orig::arg::as<i16>(joan::orig::as_units(F.self)->group_average(joan::orig::arg::u32(static_cast<i32>(jo::as_unit(unit)->id))));  // 0x006ffb40
                if (joan::orig::arg::as<u8>(joan::orig::as_inventory(obj(d::g_a56398))->is_open()) != 0 &&
                    joan::orig::arg::as<i32>(joan::orig::as_unit_attr_screen(obj(d::g_a56398))->unit_6af()) == static_cast<i32>(jo::as_unit(unit)->id)) {
                    F.self->u16_14b =
                        static_cast<u16>((global<u32>(d::g_game_time_ms) - F.self->u32_147) / 100u % 8u);
                    s_set(s, str(d::s_MSELECT));  // MSELECT
                    const i32 n = static_cast<i32>(jo::view<i16>(F.self->u16_14b));
                    if (n > 0 && n < 4) {
                        s_add(s, word_text(static_cast<i32>(jo::view<i16>(F.self->u16_14b))));
                        jo::BmpRes* const set = F.self->sprites;
                        const char* c = s_cstr(s);
                        sprite_member(x64c(0x31a), scr_h() - 0x15, c, set, col);
                    }
                }
                sprite_member(x64c(0x31e), scr_h() - 0x19, str(d::s_CMBASE), F.self->sprites, col);  // CMBASE
                for (i32 k = 0; k < static_cast<i32>(morale) * 6 / 100; ++k) {
                    const i32 x = global<i32>(d::k_9de6cc + static_cast<u32>(k) * 4) +
                                  global<i32>(d::k_9de64c + static_cast<u32>(slot()) * 4) + scr_w() - 0x320;
                    sprite_member(x, scr_h() - 0xd, str(d::s_CMORAL), F.self->sprites, col);  // CMORAL
                }
            }
            // 0x006fc0fc
            sprite_member(x64c(0x320), scr_h() - 0x64, str(d::s_CPTOP), F.self->sprites, col);  // CPTOP
            {
                jo::BmpRes* const set = F.self->sprites;
                const i32 full = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(image(set, d::s_CHP))->width));  // CHP
                const i16 hp = static_cast<i16>(mul32(full, jo::as_unit(unit)->hit_points) / jo::as_unit(unit)->hit_points_base);
                const i16 h = jo::view<i16>(jo::as_bmp_entry(image(F.self->sprites, d::s_CHP))->height);
                const i16 skip = static_cast<i16>(static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(image(F.self->sprites, d::s_CHP))->width)) -
                                                  static_cast<i32>(hp));
                const i32 x = global<i32>(d::k_9de6ac + static_cast<u32>(slot()) * 4) + scr_w() - 0x320 + static_cast<i32>(skip);
                sprite(x, scr_h() - 0x1d, str(d::s_CHP), F.self->sprites, col, 2, skip, 0,
                       static_cast<i32>(skip) + static_cast<i32>(hp), h);
            }
            // hero portraits
            s_set(s, str(d::kEmpty));  // ""
            struct Hero {
                img::Addr name, lv[3], pic[3];
            };
            static const Hero kHeroes[4] = {
                {d::s_JOAN_9d99e0, {d::s_JOAN_ARMOR_LV1, d::s_JOAN_ARMOR_LV2, d::s_JOAN_ARMOR_LV3}, {d::s_JOANC2, d::s_JOANC1, d::s_JOANC3}},  // JOAN
                {d::s_JEAN, {d::s_JEAN_ARMOR_LV1, d::s_JEAN_ARMOR_LV2, d::s_JEAN_ARMOR_LV3}, {d::s_JEANC1, d::s_JEANC2, d::s_JEANC3}},  // JEAN
                {d::s_LAHIRE_9cfb3c, {d::s_LAHIRE_ARMOR_LV1, d::s_LAHIRE_ARMOR_LV2, d::s_LAHIRE_ARMOR_LV3}, {d::s_LAHIREC1, d::s_LAHIREC2, d::s_LAHIREC3}},  // LAHIRE
                {d::s_ALENCON_9cfb30, {d::s_ALENCON_ARMOR_LV1, d::s_ALENCON_ARMOR_LV2, d::s_ALENCON_ARMOR_LV3}, {d::s_ALENCOC1, d::s_ALENCOC2, d::s_ALENCOC3}},  // ALENCON
            };
            static const img::Addr kPlain[4] = {d::s_RICHEMON, d::s_CHARLES_9d4790, d::s_FLAVY, d::s_BASTARD};  // RICHEMON..BASTARD
            bool done = false;
            for (int h = 0; h < 4 && !done; ++h) {
                const char* tn = jo::as_unit_type(unit_type(static_cast<i32>(jo::as_unit(unit)->type_id)))->portrait;
                if (crt_stricmp(tn, str(kHeroes[h].name)) != 0) continue;
                done = true;
                const i16 armor = jo::as_unit(unit)->stats[5];
                for (int l = 0; l < 3; ++l) {
                    const u8* it = item_type(static_cast<i32>(armor));
                    if (crt_stricmp(str(reinterpret_cast<std::uintptr_t>(jo::as_item_def(it)->name)), str(kHeroes[h].lv[l])) == 0) {
                        s_set(s, str(kHeroes[h].pic[l]));
                        break;
                    }
                }
            }
            for (int h = 0; h < 4 && !done; ++h) {
                const char* tn = jo::as_unit_type(unit_type(static_cast<i32>(jo::as_unit(unit)->type_id)))->portrait;
                if (crt_stricmp(tn, str(kPlain[h])) != 0) continue;
                done = true;
                s_set(s, str(kPlain[h]));
            }
            {
                jo::BmpRes* const set = F.self->sprites;
                const char* c = s_cstr(s);
                sprite_member(global<i32>(d::k_9de68c + static_cast<u32>(slot()) * 4) + scr_w() - 0x320, scr_h() - 0x78,
                              c, set, col);
            }
            const i16 kind = joan::orig::arg::as<i16>(joan::orig::as_unit(F.self)->major_class(static_cast<i32>(jo::as_unit(unit)->id)));  // 0x006ffc40
            if (jo::as_unit(unit)->group != nullptr) {
                for (i32 j = 0; j < static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count); ++j) {
                    const i32 m = static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->members[j]);
                    if (unit_gone(static_cast<u32>(m)) != 0) continue;
                    const u8* mu = unit_get(static_cast<u32>(m));
                    const u8* mt = unit_type(static_cast<i32>(jo::as_unit(mu)->type_id));
                    if (static_cast<i32>(jo::as_unit_type(mt)->unit_class) == static_cast<i32>(kind)) break;
                }
            }
        }
        joan::orig::as_hud(F.self)->draw_arrows();  // 0x00704260
        joan::orig::as_hud(F.self)->draw_objectives();  // 0x00704810 Hud::draw_objectives
    }

    // ---------------------------------------------------------------- 4. squad counts
    if (fl & 4) {
        for (i32 i = 0; i < static_cast<i32>(F.self->arrow_count); ++i) {
            u32* const id_addr = &F.self->arrow_unit[i];
            if (unit_gone(*id_addr) != 0) continue;
            u8* const unit = unit_get(*id_addr);
            const u32 col = 0x00ffffffu | static_cast<u32>(member_alpha(F.self, unit)) << 24;
            joan::orig::as_unit(F.self)->major_class(static_cast<i32>(jo::as_unit(unit)->id));  // 0x006ffc40 (unused)
            if (jo::as_unit(unit)->group != nullptr && jo::as_group(jo::as_unit(unit)->group)->count != 0) {
                const char* t = fmt_text(static_cast<i32>(jo::as_group(jo::as_unit(unit)->group)->count), 1);
                const u32 x = static_cast<u32>(8 - static_cast<i32>(F.self->arrow_count) + i) * 4;
                const i32 x2 = global<i32>(d::k_9de6e4 + x) + scr_w() - 0x310;
                const i32 x1 = global<i32>(d::k_9de6e4 + static_cast<u32>(8 - static_cast<i32>(F.self->arrow_count) + i) * 4) +
                               scr_w() - 0x320;
                text3(d::g_font_verdana_12_gold_black_outline, x1, scr_h() - 0x23, x2, scr_h() - 0xc, t, col);
            }
        }
    }
    joan::orig::as_mini_map(&F.self->minimap)->draw(joan::orig::arg::u32(static_cast<u8>(flags)));  // 0x0070e500
}

JOAN_REPLACE("game/b116_01", "Hud::draw", 0x006f9b60, &Hud::draw);
JOAN_CANVAS_SELF(0x006f9b60);  // ScreenScope in the body (no thunk wrapper)

}  // namespace game_b116_01
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using joan::call::thiscall;

JOAN_COUNTER(c_panel, "Panel::update_6fd670");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x006fd670
    u8 panel_update() {
        c_panel.hit();
        joan::orig::as_mini_map(&jo::self<jo::Hud>(this)->minimap)->handle_zoom_key();  // 0x0070eb20
        joan::orig::as_news_list(obj(d::g_tasks))->show_recent();    // 0x004b93b0
        u8 flag = 0;
        if (joan::orig::arg::as<u8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) == 0 && joan::orig::arg::as<u8>(joan::orig::as_shop(obj(d::g_game_a5d380))->active()) == 0 &&
            global<i8>(d::g_fade_title) == 0 && global<i8>(d::g_a46048) == 0) {  // 0x004301c0, 0x0042ced0
            void* const units = obj(d::g_units);
            for (i32 i = 0; i < jo::self<jo::Hud>(this)->arrow_count; ++i) {
                if (joan::orig::as_dynarray(units)->empty_at(joan::orig::arg::i32(jo::self<jo::Hud>(this)->arrow_unit[i])) != 0) continue;  // 0x00428b70
                void* u = joan::orig::as_dynarray(units)->ptr_at(joan::orig::arg::i32(jo::self<jo::Hud>(this)->arrow_unit[i]));  // 0x0041b3c0
                auto slot = [&]() -> jo::Button& { return (&jo::self<jo::Hud>(this)->squad_button0)[8 - jo::self<jo::Hud>(this)->arrow_count + i]; };  // squad_button0 .. 7
                bool show = global<i32>(d::g_control_mode) == 4;
                if (!show && jo::self<jo::Hud>(this)->arrow_unit[i] != global<u32>(d::g_player_unit_recno) &&
                    joan::orig::arg::as<u8>(joan::orig::as_inventory(obj(d::g_a56398))->is_open()) != 0) {  // 0x00430340
                    show = true;
                }
                if (show) {
                    joan::orig::as_widget_child(&slot().child)->enable();  // 0x006ca000
                    if (joan::orig::arg::as<i32>(joan::orig::as_ui_button(&slot())->mouse(i32{0}, i32{0}, i32{-1}, i32{0}, i32{0}, i32{0})) != 0) {  // 0x006218c0
                        joan::orig::as_unit(u)->select_speech(joan::orig::arg::u32(i32{1}));  // 0x005121c0
                        flag = 1;
                    }
                } else {
                    joan::orig::as_widget_child(&slot().child)->disable();  // 0x006ca020
                }
            }
        }
        if (flag != 0) return 1;
        jo::self<jo::Hud>(this)->u8_32 = 0;
        return 0;
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Panel::update_6fd670", 0x006fd670, &H::panel_update);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b109_04.cpp (game/b109_04)
namespace {
namespace game_b109_04 {

using joan::call::thiscall;
// Virtual call without arguments (thiscall); `off` = the x86 byte offset in the vtable (slot off / 4).
template <class R>
R vcall(const void* o, std::int32_t off) {
    return thiscall<R>(static_cast<const std::uintptr_t*>(jo::as_map_object(o)->vtable)[joan::call::x86_vslot(off)], o);
}

void* const kHudObjs = obj(d::g_a50530);

JOAN_COUNTER(c_track, "Hud::track_6fd8b0");

u8* soldier(u32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 soldier_gone(u32 id) { return joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70

// ------------------------------------------------------------------------------------ H
struct H {
    // ---------------------------------------------------------------- 0x006fd8b0
    u8* sel() { return soldier(global<u32>(d::g_player_unit_recno)); }
    u8* hud_obj() { return joan::orig::arg::as<u8*>(joan::orig::as_list(kHudObjs)->at_plus4(joan::orig::arg::u32(jo::self<jo::Hud>(this)->target_id))); }  // 0x004853e0
    // eases +0x2c towards the virtual record's field (re-fetched as in the original): slot 0x4c returns the Gate
    // (Gate::hp), slot 0x58 the Wall (Wall::u32_c6)
    template <class Rec, auto Field>
    JOAN_NOINLINE void ease_virtual(i32 slot) {
        auto rec = [&]() -> u8* { return vcall<u8*>(hud_obj(), slot); };
        auto fld = [](u8* r) -> i32 { return jo::view<i32>(jo::self<Rec>(r)->*Field); };
        u8* r = rec();
        if (jo::self<jo::Hud>(this)->target_hp == fld(r)) return;
        r = rec();
        if (jo::self<jo::Hud>(this)->target_hp < fld(r)) {
            r = rec();
            i32 s;
            if (wsub(fld(r), jo::self<jo::Hud>(this)->target_hp) > 0x28) {
                s = 0x28;
            } else {
                r = rec();
                s = wsub(fld(r), jo::self<jo::Hud>(this)->target_hp);
            }
            jo::self<jo::Hud>(this)->target_hp = wadd(jo::self<jo::Hud>(this)->target_hp, s);
        } else {
            r = rec();
            if (fld(r) == 0) {
                jo::self<jo::Hud>(this)->target_hp = 0;
                return;
            }
            r = rec();
            i32 s;
            if (wsub(jo::self<jo::Hud>(this)->target_hp, fld(r)) > 0x28) {
                s = 0x28;
            } else {
                r = rec();
                s = wsub(jo::self<jo::Hud>(this)->target_hp, fld(r));
            }
            jo::self<jo::Hud>(this)->target_hp = wsub(jo::self<jo::Hud>(this)->target_hp, s);
        }
    }
    void track_6fd8b0() {
        c_track.hit();
        if (soldier_gone(global<u32>(d::g_player_unit_recno)) != 0) return;
        if (jo::self<jo::Hud>(this)->shown_hp != jo::as_unit(sel())->hit_points) {
            if (jo::self<jo::Hud>(this)->shown_hp < jo::as_unit(sel())->hit_points) {
                i32 s;
                if (wsub(jo::as_unit(sel())->hit_points, jo::self<jo::Hud>(this)->shown_hp) > 9) {
                    s = 9;
                } else {
                    s = wsub(jo::as_unit(sel())->hit_points, jo::self<jo::Hud>(this)->shown_hp);
                }
                jo::self<jo::Hud>(this)->shown_hp = wadd(jo::self<jo::Hud>(this)->shown_hp, s);
            } else {
                i32 s;
                if (wsub(jo::self<jo::Hud>(this)->shown_hp, jo::as_unit(sel())->hit_points) > 9) {
                    s = 9;
                } else {
                    s = wsub(jo::self<jo::Hud>(this)->shown_hp, jo::as_unit(sel())->hit_points);
                }
                jo::self<jo::Hud>(this)->shown_hp = wsub(jo::self<jo::Hud>(this)->shown_hp, s);
            }
        }
        {
            const i32 q = static_cast<i32>(static_cast<u32>(jo::as_unit(sel())->hit_points_base) << 1) / 3;
            if (!(jo::self<jo::Hud>(this)->shown_hp < q)) {
                if (jo::self<jo::Hud>(this)->health_state != 0) joan::orig::as_hud(this)->set_health_state(static_cast<std::int8_t>(joan::orig::arg::i32(i32{0})));  // 0x006fe4a0
            } else {
                const i32 q2 = jo::as_unit(sel())->hit_points_base / 3;
                if (!(jo::self<jo::Hud>(this)->shown_hp < q2)) {
                    if (jo::self<jo::Hud>(this)->health_state != 3) joan::orig::as_hud(this)->set_health_state(static_cast<std::int8_t>(joan::orig::arg::i32(i32{3})));
                } else {
                    if (jo::self<jo::Hud>(this)->health_state != 5) joan::orig::as_hud(this)->set_health_state(static_cast<std::int8_t>(joan::orig::arg::i32(i32{5})));
                }
            }
        }
        {
            const i32 v8 = jo::as_unit(sel())->mana;
            if (jo::self<jo::Hud>(this)->shown_145 != v8) {
                if (jo::self<jo::Hud>(this)->shown_145 < v8) {
                    const i32 d = wsub(v8, jo::self<jo::Hud>(this)->shown_145);
                    jo::self<jo::Hud>(this)->shown_145 = wadd(jo::self<jo::Hud>(this)->shown_145, d > 3 ? 3 : d);
                } else {
                    const i32 d = wsub(jo::self<jo::Hud>(this)->shown_145, v8);
                    jo::self<jo::Hud>(this)->shown_145 = wsub(jo::self<jo::Hud>(this)->shown_145, d > 3 ? 3 : d);
                }
            }
        }
        if (jo::self<jo::Hud>(this)->shown_126 != jo::as_unit(sel())->experience) {
            i32 s;
            if (wsub(jo::as_unit(sel())->experience, jo::self<jo::Hud>(this)->shown_126) > 0x1f4) {
                s = 0x1f4;
            } else {
                s = wsub(jo::as_unit(sel())->experience, jo::self<jo::Hud>(this)->shown_126);
            }
            jo::self<jo::Hud>(this)->shown_126 = wadd(jo::self<jo::Hud>(this)->shown_126, s);
        }
        if (jo::self<jo::Hud>(this)->target_id != 0 && global<u32>(d::g_now) - jo::self<jo::Hud>(this)->target_time > 0xbb8u)
            joan::orig::as_hud(this)->set_item_portrait(i32{0});  // 0x006fe280
        if (jo::self<jo::Hud>(this)->target_is_unit == 1) {
            if (soldier_gone(jo::self<jo::Hud>(this)->target_id) == 0 && jo::self<jo::Hud>(this)->target_hp != jo::as_unit(soldier(jo::self<jo::Hud>(this)->target_id))->hit_points) {
                if (jo::self<jo::Hud>(this)->target_hp < jo::as_unit(soldier(jo::self<jo::Hud>(this)->target_id))->hit_points) {
                    i32 s;
                    if (wsub(jo::as_unit(soldier(jo::self<jo::Hud>(this)->target_id))->hit_points, jo::self<jo::Hud>(this)->target_hp) > 0x28) {
                        s = 0x28;
                    } else {
                        s = wsub(jo::as_unit(soldier(jo::self<jo::Hud>(this)->target_id))->hit_points, jo::self<jo::Hud>(this)->target_hp);
                    }
                    jo::self<jo::Hud>(this)->target_hp = wadd(jo::self<jo::Hud>(this)->target_hp, s);
                } else if (jo::as_unit(soldier(jo::self<jo::Hud>(this)->target_id))->hit_points != 0) {
                    i32 s;
                    if (wsub(jo::self<jo::Hud>(this)->target_hp, jo::as_unit(soldier(jo::self<jo::Hud>(this)->target_id))->hit_points) > 0x28) {
                        s = 0x28;
                    } else {
                        s = wsub(jo::self<jo::Hud>(this)->target_hp, jo::as_unit(soldier(jo::self<jo::Hud>(this)->target_id))->hit_points);
                    }
                    jo::self<jo::Hud>(this)->target_hp = wsub(jo::self<jo::Hud>(this)->target_hp, s);
                } else {
                    jo::self<jo::Hud>(this)->target_hp = 0;
                }
            }
        } else if (joan::orig::arg::as<i32>(joan::orig::as_types(kHudObjs)->empty_at(joan::orig::arg::i32(jo::self<jo::Hud>(this)->target_id))) == 0) {  // 0x00485410
            const i32 k = static_cast<i32>(jo::as_map_object(hud_obj())->class_id);
            if (k == 1)
                ease_virtual<jo::Gate, &jo::Gate::hp>(0x4c);
            else if (k == 4)
                ease_virtual<jo::Wall, &jo::Wall::u32_c6>(0x58);
        }
        joan::orig::as_mini_map(&jo::self<jo::Hud>(this)->minimap)->update_70ec50();  // 0x0070ec50
        if (joan::orig::arg::as<u8>(joan::orig::as_game(obj(d::g_game_state))->flag_744()) != 0 && jo::view<i8>(jo::self<jo::Hud>(this)->blink_on) != 0)  // 0x004301c0
            jo::self<jo::Hud>(this)->blink_start = jo::self<jo::Hud>(this)->blink_start + global<u32>(d::g_frame_elapsed_ms);
    }
};

static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4 && sizeof(double) == 8, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_04", name, addr, fn)
B("Hud::track_6fd8b0", 0x006fd8b0, &H::track_6fd8b0);
#undef B

}  // namespace game_b109_04
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::call::thiscall;

i32 ftol(double v) { return joan::hook::x87::ftol(v); }

JOAN_COUNTER(c_blink, "Hud::blink");

struct H {
    // ------------------------------------------------------------------ 0x006fe240
    u32 reset_6fe240() {
        jo::self<jo::Hud>(this)->target_id = 0;
        joan::orig::as_panel(&jo::self<jo::Hud>(this)->minimap)->clear_unit();  // 0x0070f9b0
        return 0;  // 0x0070f9b0 returns nothing; the only caller discards the result
    }

    // ------------------------------------------------------------------ 0x007076c0
    void blink(i32 k) {
        c_blink.hit();
        i32 i = 0;
        for (; i < static_cast<i32>(jo::self<jo::Hud>(this)->obj_count); ++i) {
            if (jo::self<jo::Hud>(this)->obj_id[i] - 1u == static_cast<u32>(k)) break;
        }
        if (i == static_cast<i32>(jo::self<jo::Hud>(this)->obj_count)) return;
        bool skip = false;
        if (global<i32>(d::g_mission) == 6) {
            const jo::Scenario& sc = jo::as_game(obj(d::g_game_state))->scenario;  // 0x00a58fab index, 0x00a58fb7 entries
            if (jo::view<i32>(sc.entries[static_cast<i32>(sc.index) - 1].u32_19a) == 1) skip = true;
        }
        if (!skip) {
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(d::g_strings))->open_3());  // 0x004dcb30
            jo::self<jo::Hud>(this)->obj_alpha[i] = static_cast<u16>(crt_strlen(s) + 1);
            jo::self<jo::Hud>(this)->obj_time[i] = global<u32>(d::g_now);
            jo::self<jo::Hud>(this)->obj_1a7[i] = 2;
        }
        if (jo::self<jo::Hud>(this)->blink_time == 0) {
            jo::view<u32>(jo::self<jo::Hud>(this)->blink_time) = 1;
            jo::self<jo::Hud>(this)->blink_on = 1;
            jo::self<jo::Hud>(this)->blink_start = global<u32>(d::g_now);
            jo::self<jo::Hud>(this)->blink_state = 0;
        }
        if (jo::self<jo::Hud>(this)->blink_time == 2) {
            jo::view<u32>(jo::self<jo::Hud>(this)->blink_time) = 1;
            const double now = static_cast<double>(global<u32>(d::g_now));                      // fild qword {now, 0}
            const double dt = static_cast<double>(global<u32>(d::g_now) - jo::self<jo::Hud>(this)->blink_start);      // fild qword {now - t0, 0}
            double v = fpu.div(dt, global<float>(d::flt_3000));     // / 3000
            v = fpu.mul(v, global<float>(d::flt_255));             // * 255
            v = fpu.sub(global<float>(d::flt_255), v);             // 255 - v
            v = fpu.mul(v, global<float>(d::flt_2000));             // * 2000
            v = fpu.div(v, global<float>(d::flt_255));             // / 255
            v = fpu.sub(now, v);                                   // fsubp st(1),st
            jo::view<i32>(jo::self<jo::Hud>(this)->blink_start) = ftol(v);
            jo::self<jo::Hud>(this)->blink_on = 1;
            jo::self<jo::Hud>(this)->blink_state = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::reset_6fe240", 0x006fe240, &H::reset_6fe240);
B("Hud::blink", 0x007076c0, &H::blink);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b83_05.cpp (game/b83_05)
namespace {
namespace game_b83_05 {

struct Obj {};
using joan::call::thiscall;

JOAN_COUNTER(c_unit, "UnitPortrait::set_unit");
JOAN_COUNTER(c_item, "ItemPortrait::set_item");

struct H {
    // 0x006fe340. ret 4.
    void unit_set(std::int32_t id) {
        c_unit.hit();
        jo::self<jo::Hud>(this)->target_time = global<std::uint32_t>(d::g_now);
        if (id == 0) {
            jo::self<jo::Hud>(this)->target_id = 0;
            jo::view<std::uint32_t>(jo::self<jo::Hud>(this)->target_hp) = 0;
            return;
        }
        if (jo::view<std::int32_t>(jo::self<jo::Hud>(this)->target_id) == id && static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Hud>(this)->target_is_unit)) == 0) return;
        void* const list = obj(d::g_a50530);
        if (joan::orig::arg::as<std::int32_t>(joan::orig::as_types(list)->empty_at(joan::orig::arg::i32(id))) != 0) return;  // 0x00485410
        jo::view<std::int32_t>(jo::self<jo::Hud>(this)->target_id) = id;
        void* const u = joan::orig::arg::as<void*>(joan::orig::as_list(list)->at_plus4(joan::orig::arg::u32(id)));  // 0x004853e0
        const std::int32_t kind = jo::as_map_object(u)->class_id;
        using Virtual = std::uint8_t* (Obj::*)();
        JOAN_MEMBER_POINTER_CHECK(Virtual);
        if (kind == 1) {
            void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(list)->at_plus4(joan::orig::arg::u32(id)));
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(o)[0x4c / 4], sizeof(void*));
            std::uint8_t* const r = (reinterpret_cast<Obj*>(o)->*method)();
            jo::view<std::uint32_t>(jo::self<jo::Hud>(this)->target_hp) = jo::view<std::uint32_t>(jo::as_gate(r)->hp);  // slot 0x4c returns the Gate
        } else if (kind == 4) {
            void* const o = joan::orig::arg::as<void*>(joan::orig::as_list(list)->at_plus4(joan::orig::arg::u32(id)));
            Virtual method{};
            std::memcpy(&method, &joan::call::vtable_of(o)[0x58 / 4], sizeof(void*));
            std::uint8_t* const r = (reinterpret_cast<Obj*>(o)->*method)();
            jo::view<std::uint32_t>(jo::self<jo::Hud>(this)->target_hp) = jo::as_wall(r)->u32_c6;  // slot 0x58 returns the Wall
        }
        jo::view<std::uint8_t>(jo::self<jo::Hud>(this)->target_is_unit) = 0;
    }

    // 0x006fe280. ret 4.
    void item_set(std::int32_t id) {
        c_item.hit();
        jo::self<jo::Hud>(this)->target_time = global<std::uint32_t>(d::g_now);
        if (id == 0) {
            jo::self<jo::Hud>(this)->target_id = 0;
            jo::view<std::uint32_t>(jo::self<jo::Hud>(this)->target_hp) = 0;
            return;
        }
        if (jo::view<std::int32_t>(jo::self<jo::Hud>(this)->target_id) == id && static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Hud>(this)->target_is_unit)) != 0) return;
        void* const list = obj(d::g_units);
        if (joan::orig::as_dynarray(list)->empty_at(joan::orig::arg::i32(id)) != 0) return;  // 0x00428b70
        jo::view<std::int32_t>(jo::self<jo::Hud>(this)->target_id) = id;
        std::uint8_t* const item = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(list)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        jo::view<std::uint32_t>(jo::self<jo::Hud>(this)->target_hp) = jo::view<std::uint32_t>(jo::as_unit(item)->hit_points);
        jo::view<std::uint8_t>(jo::self<jo::Hud>(this)->target_is_unit) = 1;
    }

    // 0x006fe500 (empty). ret 8.
    void nothing(void*, std::uint32_t) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b83_05", name, addr, fn)
B("UnitPortrait::set_unit", 0x006fe340, &H::unit_set);
B("ItemPortrait::set_item", 0x006fe280, &H::item_set);
B("Portrait::nothing", 0x006fe500, &H::nothing);
#undef B

}  // namespace game_b83_05
}  // namespace

// ---------------------------------------- from modules/game_b88_06.cpp (game/b88_06)
namespace {
namespace game_b88_06 {


struct H {
    // 0x006fe4a0
    void set_30(std::int8_t b) {
        jo::view<std::int8_t>(jo::self<jo::Hud>(this)->health_state_31) = jo::self<jo::Hud>(this)->health_state;
        jo::self<jo::Hud>(this)->health_state = b;
    }

    // 0x006fe480
    void nop2(void*, void*) {}
};

#define M(name, addr, fn) JOAN_REPLACE("game/b88_06", name, addr, fn)
M("Obj6fe::set_30", 0x006fe4a0, &H::set_30);
M("Obj6fe::nop2", 0x006fe480, &H::nop2);
#undef M

}  // namespace game_b88_06
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

struct H {
    // ------------------------------------------------------------------ 0x006fe4e0
    float zero_6fe4e0() { return global<float>(d::k_9cf5c4); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Obj::zero_6fe4e0", 0x006fe4e0, &H::zero_6fe4e0);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- from modules/game_b89_05.cpp (game/b89_05)
namespace {
namespace game_b89_05 {

using joan::call::thiscall;
using joan::orig::helpers::s16;

void* const kArrays = obj(d::g_misc);  // 0x0058fca0

JOAN_COUNTER(c_list_add, "NameList::add");

struct H {
    // ------------------------------------------------------------ 0x006fe520 (ret 4)
    void list_add(std::int32_t value) {
        c_list_add.hit();
        if (s16(jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count)) >= 9) return;
        if (s16(jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count)) == 0) jo::self<jo::Hud>(this)->exp_start = global<std::uint32_t>(d::g_now);
        jo::self<jo::Hud>(this)->exp_target[s16(jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count))] = value;
        const char* const text = joan::orig::arg::as<const char*>(joan::orig::as_misc(kArrays)->format_int_58fca0(joan::orig::arg::i32(jo::self<jo::Hud>(this)->exp_target[s16(jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count))]), 4));  // 0x0058fca0
        const std::uint32_t len = joan::orig::arg::as<std::uint32_t>(std::strlen(joan::orig::arg::str(text)));  // strlen
        jo::self<jo::Hud>(this)->exp_len[s16(jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count))] = static_cast<std::uint8_t>(len);
        jo::view<std::uint32_t>(jo::self<jo::Hud>(this)->exp_shown[s16(jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count))]) = 0;
        jo::view<std::uint8_t>(jo::self<jo::Hud>(this)->exp_state[s16(jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count))]) = 0;
        jo::self<jo::Hud>(this)->exp_time[s16(jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count))] = 0;
        jo::view<std::uint8_t>(jo::self<jo::Hud>(this)->exp_on) = 1;
        jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count) = static_cast<std::uint16_t>(jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count) + 1);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b89_05", name, addr, fn)
B("NameList::add", 0x006fe520, &H::list_add);
#undef B

}  // namespace game_b89_05
}  // namespace

// ---------------------------------------- from modules/game_b109_10.cpp (game/b109_10)
namespace {
namespace game_b109_10 {

using joan::hook::x87::ftol;

using joan::call::thiscall;

// Colour(r, g, b, a) 0x0042a260 on a stack temporary, then its dword (bytes only are read).
u32 make_color(i32 r, i32 g, i32 b, i32 a) {
    u32 tmp = 0;
    const u32* const c = joan::orig::arg::as<const u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
    return *c;
}
// Sprite queue 0x00633500 (thunk 0x00401bbd) on 0xa46128, ret 0x44.
u32 sprite(i32 x, i32 y, u32 layer, const char* name, void* sprites, u32 color, i32 a7, i32 a8, i32 a9, i32 a10, i32 a16,
           i32 a17) {
    return joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(layer), name, joan::orig::arg::mptr(sprites), joan::orig::arg::u32(color), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));
}

// 0x0058fca0 (thunk 0x0040ab0f) number -> text on 0xa45fa8, ret 8.
const char* fmt_num(i32 v, i32 mode) { return joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(joan::orig::arg::i32(v), joan::orig::arg::i32(mode))); }

// 0x191-byte path string (0x005a11a0 ctor, 0x005a1550 append, 0x00417d90 c_str); the
// original's stack slot is 0x194 bytes.
struct Str {
    alignas(4) u8 b[0x194];
};
void str_init(Str& s, img::Addr text) { joan::orig::as_string(&s)->construct(joan::orig::arg::str(str(text))); }  // 0x005a11a0
void str_append(Str& s, const char* t) { joan::orig::as_string(&s)->append(joan::orig::arg::str(t)); }    // 0x005a1550
const char* c_str(Str& s) { return joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str()); }  // 0x00417d90

// p = 10 * 10^n with the original's wrapping imul.
i32 pow10w(i32 n) {
    i32 p = 10;
    for (i32 k = 0; k < n; ++k) p = imul(p, 10);
    return p;
}
// (v % p) / (p / 10), idiv semantics.
i32 digit(i32 v, i32 p) {
    const i32 r = v % p;
    return r / (p / 10);
}

JOAN_COUNTER(c_exp, "ExpCounter::draw_6fe660");

struct H {
    // 0x006fe660
    void exp_draw() {
        c_exp.hit();
        void* const self = this;
        const Fpu F{true};
        if (global<i32>(d::g_control_mode) == 4) return;
        if (!(jo::as_hud(self)->exp_count > 0)) return;
        if (jo::as_hud(self)->exp_on == 0) return;
        auto elapsed = [&](u32 since, u32 cap) -> double {
            const u32 d = global<u32>(d::g_now) - since;
            return static_cast<double>(d < cap ? d : cap);
        };
        float c = static_cast<float>(F.div(elapsed(jo::as_hud(self)->exp_start, 200), 200.0));
        auto banner = [&](i32 x, img::Addr name) {
            const u32 col = make_color(0xff, 0xff, 0xff, 0xff);
            sprite(x, global<i32>(d::g_screen_h) - 0x104, 9, str(name), jo::as_hud(self)->exp_sprites, col, 3, 0, 2, 1, -1,
                   -1);
        };
        if (jo::as_hud(self)->exp_state[0] == 0 || jo::as_hud(self)->exp_state[0] == 4) {
            crt_strlen(fmt_num(jo::as_hud(self)->exp_target[0], 4));  // [ebp-0x10], unused
            if (jo::as_hud(self)->exp_count == 1 && jo::as_hud(self)->exp_state[0] == 4) {
                c = static_cast<float>(F.sub(1.0, F.div(elapsed(jo::as_hud(self)->exp_start, 200), 200.0)));
            }
            // fild W - fild(W - (W - 0x7d)) * c; the integer part is 0x7d for every W (mod 2^32).
            const i32 W = global<i32>(d::g_screen_w);
            const i16 x = static_cast<i16>(ftol(F.sub(static_cast<double>(W), F.mul(125.0, c))));
            u32 bits = 0;
            std::memcpy(&bits, &c, 4);
            banner(x, bits != 0x3f800000u ? img::Addr{d::s_EXPTEXT} : img::Addr{d::s_EXPTEXT2});  // "EXPTEXT" / "EXPTEXT2"
        } else {
            const i16 x = static_cast<i16>(wsub(global<i32>(d::g_screen_w), 0x7d));
            banner(x, d::s_EXPTEXT2);  // "EXPTEXT2"
        }

        // Slot loop: for (i = 0; i < 1; ++i) - runs once with i = 0.
        const i32 i = 0;
        auto draw_digit = [&](Str& s, i32 x, i32 y, i32 alpha, i32 a16, i32 a17) {
            const u32 col = make_color(0xff, 0xff, 0xff, alpha);
            jo::BmpRes* const sprites = jo::as_hud(self)->exp_sprites;
            const char* const name = c_str(s);
            sprite(x, y, 9, name, sprites, col, 3, 0, 2, 1, a16, a17);
        };
        if (jo::as_hud(self)->exp_state[i] == 0) {
            i16 d = static_cast<i16>(crt_strlen(fmt_num(jo::as_hud(self)->exp_shown[i], 4)));
            if (jo::as_hud(self)->exp_target[i] % 10 == 0) {
                const i32 dd = d;
                if (static_cast<u32>(dd) != crt_strlen(fmt_num(jo::as_hud(self)->exp_target[i], 4))) d = 0;
            }
            i32 p = pow10w(d);
            for (;;) {
                const i32 a = digit(jo::as_hud(self)->exp_shown[i], p);
                const i32 b = digit(jo::as_hud(self)->exp_target[i], p);
                if (a != b) break;
                const i32 dd = d;
                if (static_cast<u32>(dd) == crt_strlen(fmt_num(jo::as_hud(self)->exp_target[i], 4))) break;
                d = static_cast<i16>(d + 1);
                p = pow10w(d);
            }
            const i16 alpha = static_cast<i16>(ftol(F.sub(255.0, F.mul(F.div(static_cast<double>(i), 10.0), 255.0))));
            for (i32 k = 0; k < jo::view<i8>(jo::as_hud(self)->exp_len[i]); ++k) {
                const i32 pk = pow10w(k);
                const i16 x = static_cast<i16>(wsub(wsub(global<i32>(d::g_screen_w), 0x50), imul(k, 0x50)));
                const i16 y = static_cast<i16>(global<i32>(d::g_screen_h) - 0x140);
                Str s{};
                str_init(s, d::s_E_9cfc2c);  // "E"
                str_append(s, fmt_num(digit(jo::as_hud(self)->exp_shown[i], pk), 1));
                str_append(s, str(d::s_us_9cf774));  // "_"
                str_append(s, str(d::s_1_9d8de4));  // "1"
                if (digit(jo::as_hud(self)->exp_shown[i], pk) != digit(jo::as_hud(self)->exp_target[i], pk)) {
                    const i32 d3 = digit(jo::as_hud(self)->exp_target[i], pk);
                    const i32 d6 = digit(jo::as_hud(self)->exp_shown[i], pk);
                    if (wsub(d3, d6) >= 2) {
                        jo::as_hud(self)->exp_shown[i] = wadd(jo::as_hud(self)->exp_shown[i], imul(pk, 2) / 10);
                    } else {
                        jo::as_hud(self)->exp_shown[i] = wadd(jo::as_hud(self)->exp_shown[i], pk / 10);
                    }
                    draw_digit(s, x, y, alpha & 0xff, -1, -1);
                    break;
                }
                draw_digit(s, x, y, alpha & 0xff, -1, -1);
                if (k + 1 == jo::view<i8>(jo::as_hud(self)->exp_len[i]) && i == 0) jo::view<u8>(jo::as_hud(self)->exp_state[i]) = 1;
            }
            return;
        }

        for (i32 k = 0; k < jo::view<i8>(jo::as_hud(self)->exp_len[i]); ++k) {
            const i32 pk = pow10w(k);
            const i16 x = static_cast<i16>(wsub(wsub(global<i32>(d::g_screen_w), 0x50), imul(k, 0x50)));
            const i16 y = static_cast<i16>(global<i32>(d::g_screen_h) - 0x140);
            i16 alpha = 0xff;
            Str s{};
            str_init(s, d::s_E_9cfc2c);  // "E"
            str_append(s, fmt_num(digit(jo::as_hud(self)->exp_shown[i], pk), 1));
            str_append(s, str(d::s_us_9cf774));  // "_"
            if (jo::as_hud(self)->exp_state[i] == 1 || jo::as_hud(self)->exp_state[i] == 3) {
                str_append(s, str(d::s_2_9d8de0));  // "2"
                if (k + 1 == jo::view<i8>(jo::as_hud(self)->exp_len[i])) {
                    if (jo::as_hud(self)->exp_count == 1 && jo::as_hud(self)->exp_state[i] == 3) {
                        jo::as_hud(self)->exp_start = global<u32>(d::g_now);
                    }
                    if (jo::as_hud(self)->exp_state[i] == 3) jo::as_hud(self)->exp_time[i] = global<u32>(d::g_now);
                    jo::view<u8>(jo::as_hud(self)->exp_state[i]) = static_cast<u8>(jo::view<u8>(jo::as_hud(self)->exp_state[i]) + 1);
                }
            } else if (jo::as_hud(self)->exp_state[i] == 2) {
                str_append(s, str(d::s_3_9d8dd0));  // "3"
                if (jo::as_hud(self)->exp_time[i] == 0) jo::as_hud(self)->exp_time[i] = global<u32>(d::g_now);
                if (!(global<u32>(d::g_now) - jo::as_hud(self)->exp_time[i] < 0x5dcu)) {
                    jo::view<u8>(jo::as_hud(self)->exp_state[i]) = static_cast<u8>(jo::view<u8>(jo::as_hud(self)->exp_state[i]) + 1);
                }
            } else if (jo::as_hud(self)->exp_state[i] == 4) {
                str_append(s, str(d::s_1_9d8de4));  // "1"
                const double t = elapsed(jo::as_hud(self)->exp_time[i], 300);
                alpha = static_cast<i16>(ftol(F.sub(255.0, F.mul(F.div(t, 300.0), 255.0))));
                c = static_cast<float>(F.div(static_cast<double>(alpha), 255.0));
            }
            if (alpha == 0) {
                for (i32 j = i; j < jo::as_hud(self)->exp_count - 1; ++j) {
                    jo::view<u32>(jo::as_hud(self)->exp_target[j]) = jo::view<u32>(jo::as_hud(self)->exp_target[j + 1]);
                    jo::as_hud(self)->exp_len[j] = jo::as_hud(self)->exp_len[j + 1];
                    jo::view<u32>(jo::as_hud(self)->exp_shown[j]) = jo::view<u32>(jo::as_hud(self)->exp_shown[j + 1]);
                    jo::view<u8>(jo::as_hud(self)->exp_state[j]) = jo::view<u8>(jo::as_hud(self)->exp_state[j + 1]);
                    jo::as_hud(self)->exp_time[j] = jo::as_hud(self)->exp_time[j + 1];
                }
                jo::as_hud(self)->exp_count = static_cast<i16>(jo::as_hud(self)->exp_count - 1);
                return;  // the original also decrements its (dead) slot index
            }
            if (jo::as_hud(self)->exp_state[i] == 4) {
                const i32 n = jo::view<i8>(jo::as_hud(self)->exp_len[i]);
                const i32 e = wsub(wsub(global<i32>(d::g_screen_w), 0x50), imul(n - 1, 0x50));
                const i32 half = wsub(0x400, e) / 2;
                const i32 p0 = wsub(0x400, half);
                const double inv = F.sub(1.0, c);
                const i32 px = ftol(F.add(static_cast<double>(p0), F.mul(static_cast<double>(wsub(0x3fc, p0)), inv)));
                const i32 q0 = global<i32>(d::g_screen_h) - 0x126;
                const i32 qy = ftol(F.add(static_cast<double>(q0), F.mul(static_cast<double>(wsub(0x212, q0)), inv)));
                const i16 sw = static_cast<i16>(ftol(F.mul(c, 80.0)));
                const i16 sh = static_cast<i16>(ftol(F.mul(c, 52.0)));
                double t = F.div(static_cast<double>(n), 2.0);
                t = F.add(t, static_cast<double>(k));
                t = F.sub(t, static_cast<double>(n));
                t = F.add(t, 1.0);
                t = F.mul(t, static_cast<double>(sw));
                const i16 xx = static_cast<i16>(ftol(F.sub(static_cast<double>(px), t)));
                const i16 yy = static_cast<i16>(wsub(qy, i32{sh} / 2));
                const i16 x2 = static_cast<i16>(i32{xx} + i32{sw});
                const i16 y2 = static_cast<i16>(i32{yy} + i32{sh});
                draw_digit(s, xx, yy, alpha & 0xff, x2, y2);
            } else {
                draw_digit(s, x, y, alpha & 0xff, -1, -1);
            }
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");
static_assert(sizeof(Str) == 0x194, "string slot");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_10", name, addr, fn)
B("ExpCounter::draw_6fe660", 0x006fe660, &H::exp_draw);
#undef B

}  // namespace game_b109_10
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using joan::call::thiscall;

void* const kUnitTable = obj(d::g_units);

JOAN_COUNTER(c_collect_slots, "Squad::collect_slots");
JOAN_COUNTER(c_fade_restart, "Fade::restart");

struct H {
    // ------------------------------------------------------------------ 0x006ff950
    void collect_slots() {
        c_collect_slots.hit();
        u8* const leader = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno))));  // 0x0041b3c0
        i16 slots[8];
        std::memset(slots, 0, sizeof slots);
        for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i > 0; --i) {  // 0x00417ad0
            u8* const e = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3f0
            if (e == nullptr) continue;
            if (static_cast<i32>(jo::as_unit(e)->side) != static_cast<i32>(jo::as_unit(leader)->side)) continue;
            if (jo::as_unit(e)->rank <= 3) continue;
            if (static_cast<i32>(jo::as_unit(e)->excluded) != 0) continue;
            i32 idx = -1;
            for (i32 k = 0; k < 8; ++k) {
                if (static_cast<i32>(jo::as_unit(e)->type_id) == static_cast<i32>(global<i16>(d::g_a6a570 + k * 2))) {
                    idx = k;
                    break;
                }
            }
            if (idx == -1) continue;
            if (static_cast<i32>(joan::orig::arg::as<i8>(joan::orig::as_unit(e)->dead())) != 0) continue;  // 0x0042d0e0
            slots[idx] = static_cast<i16>(i);
        }
        jo::self<jo::Hud>(this)->arrow_count = 0;
        for (i32 k = 0; k < 8; ++k) {
            if (static_cast<i32>(slots[k]) != 0) {
                jo::view<i32>(jo::self<jo::Hud>(this)->arrow_unit[static_cast<i32>(jo::self<jo::Hud>(this)->arrow_count)]) = static_cast<i32>(slots[k]);
                jo::self<jo::Hud>(this)->arrow_count = static_cast<i16>(jo::self<jo::Hud>(this)->arrow_count + 1);
            }
            if (static_cast<i32>(jo::self<jo::Hud>(this)->arrow_count) == 8) break;
        }
    }

    // ------------------------------------------------------------------ 0x00707960
    void fade_restart(i32 id) {
        c_fade_restart.hit();
        for (i32 k = 0; k < static_cast<i32>(jo::self<jo::Hud>(this)->obj_count); ++k) {
            if (jo::view<i32>(jo::self<jo::Hud>(this)->obj_id[k]) - 1 != id) continue;
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(d::g_strings))->open_1());  // 0x004dcad0
            const u32 len = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s)));  // strlen
            jo::self<jo::Hud>(this)->obj_alpha[k] = static_cast<u16>(len + 1);
            jo::view<i16>(jo::self<jo::Hud>(this)->obj_1a7[k]) = 0;
            jo::self<jo::Hud>(this)->obj_time[k] = global<u32>(d::g_now);
        }
        jo::self<jo::Hud>(this)->blink_start = global<u32>(d::g_now);
        if (jo::self<jo::Hud>(this)->blink_time == 0) {
            jo::self<jo::Hud>(this)->blink_time = 1;
            jo::self<jo::Hud>(this)->blink_on = 1;
            jo::self<jo::Hud>(this)->blink_start = global<u32>(d::g_now);
            jo::self<jo::Hud>(this)->blink_state = 0;
        }
        if (jo::self<jo::Hud>(this)->blink_time == 2) {
            jo::self<jo::Hud>(this)->blink_time = 1;
            const double t = static_cast<double>(global<u32>(d::g_now));                     // fild qword
            const double d = static_cast<double>(global<u32>(d::g_now) - jo::self<jo::Hud>(this)->blink_start);     // fild qword
            double x = fpu.div(d, global<float>(d::flt_3000));   // 3000
            x = fpu.mul(x, global<float>(d::flt_255));          // 255
            x = fpu.sub(global<float>(d::flt_255), x);          // fsubr 255
            x = fpu.mul(x, global<float>(d::flt_2000));          // 2000
            x = fpu.div(x, global<float>(d::flt_255));          // 255
            jo::view<i32>(jo::self<jo::Hud>(this)->blink_start) = joan::hook::x87::ftol(fpu.sub(t, x));
            jo::self<jo::Hud>(this)->blink_on = 1;
            jo::self<jo::Hud>(this)->blink_state = 0;
        }
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Squad::collect_slots", 0x006ff950, &H::collect_slots);
B96("Fade::restart", 0x00707960, &H::fade_restart);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {


void* const kTypes = obj(d::g_types);

JOAN_COUNTER(c_major_class, "Unit::major_class");

struct H {
    // ------------------------------------------------------------------------ 0x006ffc40
    i16 major_class(i32 id) {
        c_major_class.hit();
        u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        i32 best = 0;
        i32 count[10];
        for (i32 i = 0; i < 10; ++i) count[i] = 0;
        if (jo::as_unit(u)->group != nullptr) {
            for (i32 j = 0; j < static_cast<i32>(jo::as_group(jo::as_unit(u)->group)->count); ++j) {
                const i32 m = static_cast<i32>(jo::as_group(jo::as_unit(u)->group)->members[j]);
                if (joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(m)) != 0) continue;  // 0x00428b70
                u8* const v = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(m)));  // 0x0041b3c0
                u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(v)->type_id))));  // 0x0042d0a0
                const i32 c = static_cast<i32>(jo::as_unit_type(t)->unit_class);
                count[c] = count[c] + 1;
                if (count[c] > count[best]) best = c;
            }
        }
        return static_cast<i16>(best);
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("Unit::major_class", 0x006ffc40, &H::major_class);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {


std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit_get(std::int32_t id) { return joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0

JOAN_COUNTER(c_hit, "Obj6ff::hit");

struct H {
    // 0x006ffd90
    std::int32_t hit_6ffd90(std::int32_t x, std::int32_t y) {
        c_hit.hit();
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::Hud>(this)->arrow_count); ++i) {
            if (unit_missing(jo::view<std::int32_t>(jo::self<jo::Hud>(this)->arrow_unit[i])) != 0) continue;
            unit_get(jo::view<std::int32_t>(jo::self<jo::Hud>(this)->arrow_unit[i]));  // result unused ([ebp-0xc])
            const std::int32_t idx = i + 8 - static_cast<std::int32_t>(jo::self<jo::Hud>(this)->arrow_count);
            const std::uint32_t px = global<std::uint32_t>(d::k_9de66c + idx * 4) + global<std::uint32_t>(d::g_screen_w) - 0x320u;
            const std::uint32_t py = global<std::uint32_t>(d::g_screen_h) - 0x3au;
            const std::uint32_t dx = px - static_cast<std::uint32_t>(x);
            const std::uint32_t dy = py - static_cast<std::uint32_t>(y);
            const std::int32_t d2 = static_cast<std::int32_t>(dx * dx + dy * dy);
            const double dist = joan::orig::arg::as<double>(joan::orig::libc::sqrt_f(joan::orig::arg::f32(static_cast<float>(d2))));  // 0x004256d0
            if (!(dist > static_cast<double>(global<float>(d::flt_36)))) return jo::view<std::int32_t>(jo::self<jo::Hud>(this)->arrow_unit[i]);
        }
        return 0;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj6ff::hit", 0x006ffd90, &H::hit_6ffd90);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b110_06.cpp (game/b110_06)
namespace {
namespace game_b110_06 {

constexpr Fpu kFpu{true};

using joan::call::thiscall;

JOAN_COUNTER(c_update, "LevelBanner::update");

const char* const kLevelUp0 = str(d::s_LEVELUP0);  // "LEVELUP0"
const char* const kLevelUp1 = str(d::s_LEVELUP1);  // "LEVELUP1"
const char* const kLevelUp2 = str(d::s_LEVELUP2);  // "LEVELUP2"
const char* const kLevelUp3 = str(d::s_LEVELUP3);  // "LEVELUP3"
const char* const kLevel1 = str(d::s_LEVEL1);    // "LEVEL1"
const char* const kLevel2 = str(d::s_LEVEL2);    // "LEVEL2"
const char* const kLevel3 = str(d::s_LEVEL3);    // "LEVEL3"
const char* const kSuffix1 = str(d::s_1_9de91c);   // "_1"
const char* const kSuffix2 = str(d::s_2_9de910);   // "_2"
const char* const kSuffix3 = str(d::s_3_9de904);   // "_3"
const char* const kL = str(d::s_L);         // "L"

i16 to16(i32 v) { return static_cast<i16>(static_cast<u16>(static_cast<u32>(v))); }
i16 ftol16(double v) { return to16(joan::hook::x87::ftol(v)); }
u32 now() { return global<u32>(d::g_now); }
using joan::orig::helpers::strlen_;  // strlen

i8& state(void* self) { return jo::view<i8>(jo::as_hud(self)->level_state); }
void next_state(void* self) { state(self) = static_cast<i8>(state(self) + 1); }

// min(now - stamp, limit), unsigned; the original re-reads both operands for the result.
u32 elapsed_clamped(void* self, u32 limit) {
    if (now() - jo::as_hud(self)->level_time < limit) return now() - jo::as_hud(self)->level_time;
    return limit;
}

void* picture(void* self, const char* name) {
    return joan::orig::arg::as<void*>(joan::orig::as_bmp_res(jo::view<void*>(jo::as_hud(self)->exp_sprites))->find(joan::orig::arg::str(name)));  // 0x00620b40
}

u32 rgba(i32 alpha) {
    u32 tmp = 0;
    void* p = joan::orig::as_colour(&tmp)->construct(0xff, 0xff, 0xff, alpha);  // 0x0042a260
    return *static_cast<u32*>(p);
}

void blit(void* self, i32 x, i32 y, const char* name, u32 color, i32 x2, i32 y2) {
    joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 9u, joan::orig::arg::str(name), joan::orig::arg::mptr(jo::view<void*>(jo::as_hud(self)->exp_sprites)), joan::orig::arg::u32(color), 3, 0, 2, 1, -1, -1, -1, -1, 0, joan::orig::arg::i16(x2), joan::orig::arg::i16(y2));  // 0x00633500
}

float alpha_scale(i16 alpha) { return static_cast<float>(kFpu.div(static_cast<double>(alpha), 255.0)); }

// Fade-out alpha over 300 ticks: ftol(255 - min(t, 300) / 300 * 255).
i16 alpha_out300(void* self) {
    const u32 t = elapsed_clamped(self, 0x12c);
    return ftol16(kFpu.sub(255.0, kFpu.mul(kFpu.div(static_cast<double>(t), 300.0), 255.0)));
}
// Fade-in alpha over 300 ticks: ftol(min(t, 300) / 300 * 255).
i16 alpha_in300(void* self) {
    const u32 t = elapsed_clamped(self, 0x12c);
    return ftol16(kFpu.mul(kFpu.div(static_cast<double>(t), 300.0), 255.0));
}

i32 center_y(i16 h) {
    return static_cast<i32>(global<u32>(d::g_screen_h) + static_cast<u32>(h / 2) - 0x190u);
}

// "LEVELUPn" picture, scaled by f around (0x400 - w/2, g + h/2 - 0x190).
void draw_banner(void* self, const char* name, i16 alpha, float f) {
    const i16 w = jo::view<i16>(jo::as_bmp_entry(picture(self, name))->width);
    const i16 h = jo::view<i16>(jo::as_bmp_entry(picture(self, name))->height);
    const i16 sw = ftol16(kFpu.mul(w, f));
    const i16 sh = ftol16(kFpu.mul(h, f));
    const i16 cx = to16(0x400 - w / 2);
    const i16 cy = to16(center_y(h));
    const i16 x = to16(cx - sw / 2);
    const i16 y = to16(cy - sh / 2);
    const i16 x2 = to16(x + sw);
    const i16 y2 = to16(y + sh);
    blit(self, x, y, name, rgba(alpha), x2, y2);
}

// "LEVELn" picture shifted left by len*80*f, then one "L<digit>_n" picture per digit of +0xc3.
void draw_level(void* self, const char* name, const char* suffix, i16 alpha, float f) {
    const i16 w = jo::view<i16>(jo::as_bmp_entry(picture(self, name))->width);
    const i16 h = jo::view<i16>(jo::as_bmp_entry(picture(self, name))->height);
    const char* number = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(static_cast<i32>(jo::view<i8>(jo::as_hud(self)->level_c3)), 4));  // 0x0058fca0
    const i16 len = to16(static_cast<i32>(strlen_(number)));
    const i16 sh = ftol16(kFpu.mul(h, f));
    const i16 sw = ftol16(kFpu.mul(w, f));
    const i16 cx = to16(0x400 - w / 2);
    const i16 cy = to16(center_y(h));
    const i32 shift = static_cast<i32>(len) * 0x50;
    const i16 x = ftol16(kFpu.sub(static_cast<double>(cx - sw / 2), kFpu.mul(static_cast<double>(shift), f)));
    const i16 y = to16(cy - sh / 2);
    const i16 x2 = to16(x + sw);
    const i16 y2 = to16(y + sh);
    blit(self, x, y, name, rgba(alpha), x2, y2);

    alignas(4) char text[0x194];  // 0x191-byte fixed string (strncpy 0x190 + NUL at 0x190)
    joan::orig::as_string(text)->construct(joan::orig::arg::str(kL));  // 0x005a11a0 ctor
    for (i32 i = 0; i < static_cast<i32>(len); ++i) {
        joan::orig::as_string(text)->assign(joan::orig::arg::str(kL));  // 0x005a1440 assign
        const i16 dx = ftol16(kFpu.sub(1024.0, kFpu.mul(static_cast<double>((i + 1) * 0x50), f)));
        const i16 dx2 = ftol16(kFpu.add(static_cast<double>(dx), kFpu.mul(80.0, f)));
        i16 p = 10;
        for (i32 j = 0; j < i; ++j) p = to16(p * 10);
        const i32 rem = static_cast<i32>(jo::view<i8>(jo::as_hud(self)->level_c3)) % static_cast<i32>(p);
        const i32 digit = rem / (static_cast<i32>(p) / 10);
        joan::orig::as_string(text)->append(joan::orig::arg::str(joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(joan::orig::arg::i32(digit), 1))));  // 0x005a1550
        joan::orig::as_string(text)->append(joan::orig::arg::str(suffix));  // 0x005a1550
        const u32 color = rgba(alpha);
        const char* s = joan::orig::arg::as<const char*>(joan::orig::as_string(text)->c_str());  // 0x00417d90 c_str
        blit(self, dx, y, s, color, dx2, y2);
    }
}

struct H {
    // ------------------------------------------------------------------------ 0x006ffed0
    void update() {
        c_update.hit();
        void* self = this;
        if (jo::as_hud(self)->level_on == 0) return;
        if (global<i32>(d::g_control_mode) == 4) return;
        if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno))) != 0) return;  // 0x00428b70
        (void)joan::orig::arg::as<i32>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<i32>(d::g_player_unit_recno))));  // 0x0041b3c0

        const i8 s1 = state(self);
        if (s1 == 1) {
            const u32 t = elapsed_clamped(self, 0x1f4);
            const i16 alpha = ftol16(kFpu.mul(kFpu.div(static_cast<double>(t), 500.0), 255.0));
            draw_banner(self, kLevelUp0, alpha, alpha_scale(alpha));
            if (alpha == 0xff) next_state(self);
        } else if (s1 == 2) {
            draw_banner(self, kLevelUp1, 0xff, alpha_scale(0xff));
            next_state(self);
        } else if (s1 == 3) {
            draw_banner(self, kLevelUp2, 0xff, alpha_scale(0xff));
            jo::as_hud(self)->level_time = now();
            next_state(self);
        } else if (s1 == 4) {
            draw_banner(self, kLevelUp3, 0xff, alpha_scale(0xff));
            if (now() - jo::as_hud(self)->level_time >= 0x898) {
                next_state(self);
                jo::as_hud(self)->level_time = now();
            }
        } else if (s1 == 5) {
            const i16 out = alpha_out300(self);
            draw_banner(self, kLevelUp3, out, alpha_scale(out));
            const i16 in = alpha_in300(self);
            draw_level(self, kLevel1, kSuffix1, in, alpha_scale(in));
            if (in == 0xff) next_state(self);
        } else if (s1 == 6) {
            draw_level(self, kLevel2, kSuffix2, 0xff, alpha_scale(0xff));
            jo::as_hud(self)->level_time = now();
            next_state(self);
        }

        const i8 s2 = state(self);
        if (s2 == 7) {
            draw_level(self, kLevel3, kSuffix3, 0xff, alpha_scale(0xff));
            if (now() - jo::as_hud(self)->level_time >= 0x898) {
                next_state(self);
                jo::as_hud(self)->level_time = now();
            }
        } else if (s2 == 8) {
            draw_level(self, kLevel2, kSuffix2, 0xff, alpha_scale(0xff));
            jo::as_hud(self)->level_time = now();
            next_state(self);
        } else if (s2 == 9) {
            const i16 out = alpha_out300(self);
            draw_level(self, kLevel1, kSuffix1, out, alpha_scale(out));
            if (out == 0) {
                jo::as_hud(self)->level_on = 0;
                state(self) = 0;
            }
        }
    }
};

JOAN_REPLACE("game/b110_06", "LevelBanner::update", 0x006ffed0, &H::update);

}  // namespace game_b110_06
}  // namespace

// ---------------------------------------- from modules/game_b116_09.cpp (game/b116_09)
namespace {
namespace game_b116_09 {

using joan::call::thiscall;

JOAN_COUNTER(c_combo, "ComboCounter::draw_702740");

// 0x0042a260 (thunk 0x004098b8): Color(r, g, b, a) bytes into *slot; returns slot.
u32 color4(u32* slot, i32 r, i32 g, i32 b, i32 a) {
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(slot)->construct(r, g, b, a));  // 0x0042a260
}

struct H {
    // -------------------------------------------------------------- 0x00702740
    u8* self() { return reinterpret_cast<u8*>(this); }

    // One centred sprite (state 1/2/3/4/5 blocks): returns nothing, draws `name` at alpha `alpha`
    // scaled by `scale`; `imm` = the patched 0x400 immediate operand.
    void combo_sprite(const char* name, i16 alpha, float scale, std::uintptr_t imm, i16* out_x, i16* out_y) {
        const i16 w = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::as_hud(self())->exp_sprites)->find(joan::orig::arg::str(name))))->width);  // 0x00620b40
        const i16 h = jo::view<i16>(jo::as_bmp_entry(joan::orig::arg::as<u8*>(joan::orig::as_bmp_res(jo::as_hud(self())->exp_sprites)->find(joan::orig::arg::str(name))))->height);  // 0x00620b40
        const i16 sw = static_cast<i16>(joan::hook::x87::ftol(static_cast<double>(static_cast<float>(w) * scale)));
        const i16 sh = static_cast<i16>(joan::hook::x87::ftol(static_cast<double>(static_cast<float>(h) * scale)));
        const i16 cx = static_cast<i16>(static_cast<i32>(joan::canvas::resolution_imm(imm)) - static_cast<i32>(w) / 2);
        const i16 cy = static_cast<i16>(global<i32>(d::g_screen_h) + static_cast<i32>(h) / 2 - 0x1c7);
        const i16 x1 = static_cast<i16>(static_cast<i32>(cx) - static_cast<i32>(sw) / 2);
        const i16 y1 = static_cast<i16>(static_cast<i32>(cy) - static_cast<i32>(sh) / 2);
        const i16 x2 = static_cast<i16>(static_cast<i32>(x1) + static_cast<i32>(sw));
        const i16 y2 = static_cast<i16>(static_cast<i32>(y1) + static_cast<i32>(sh));
        u32 slot = 0;
        const u32 color = color4(&slot, i32{0xff}, i32{0xff}, i32{0xff}, static_cast<i32>(static_cast<u8>(alpha)));
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(static_cast<i32>(x1)), joan::orig::arg::i16(static_cast<i32>(y1)), 9u, joan::orig::arg::str(name), joan::orig::arg::mptr(jo::as_hud(self())->exp_sprites), joan::orig::arg::u32(color), 3, 0, 2, 1, -1, -1, -1, -1, 0, joan::orig::arg::i16(static_cast<i32>(x2)), joan::orig::arg::i16(static_cast<i32>(y2)));  // 0x00633500
        *out_x = x1;
        *out_y = y1;
    }
    static float alpha_scale(i16 alpha) { return static_cast<float>(static_cast<i32>(alpha)) / 255.0f; }
    void bump_state() { jo::view<u8>(jo::as_hud(self())->combo_state) = static_cast<u8>(jo::view<u8>(jo::as_hud(self())->combo_state) + 1); }

    void combo_draw() {
        if (jo::self<jo::Hud>(this)->combo_on == 0) return;
        if (joan::orig::arg::as<u32>(joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)))) != 0) return;  // 0x00428b70
        if (global<u32>(d::g_control_mode) == 4) {
            jo::self<jo::Hud>(this)->combo_count = 0;
            global<i16>(d::g_kill_combo) = 0;
            jo::self<jo::Hud>(this)->combo_state = 0;
            return;
        }
        c_combo.hit();
        (void)joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<u32>(d::g_player_unit_recno)));  // 0x0041b3c0 (result unused)
        const i32 state = jo::self<jo::Hud>(this)->combo_state;
        const char* const combo0 = str(d::s_COMBO0);  // "COMBO0"
        const char* const combo1 = str(d::s_COMBO1);  // "COMBO1"
        i16 x = 0;
        i16 y = 0;
        if (state == 1) {
            const u32 dt = global<u32>(d::g_now) - jo::self<jo::Hud>(this)->combo_time;
            const u32 t = dt < 0x1f4 ? global<u32>(d::g_now) - jo::self<jo::Hud>(this)->combo_time : 0x1f4u;
            const float v = static_cast<float>(t) / 500.0f * 255.0f;
            const i16 alpha = static_cast<i16>(joan::hook::x87::ftol(static_cast<double>(v)));
            combo_sprite(combo0, alpha, alpha_scale(alpha), joan::canvas::kComboImm[0], &x, &y);
            if (static_cast<i32>(alpha) == 0xff) bump_state();
            return;
        }
        if (state == 2) {
            const i16 alpha = 0xff;
            combo_sprite(combo1, alpha, alpha_scale(alpha), joan::canvas::kComboImm[1], &x, &y);
            bump_state();
            return;
        }
        if (state == 3) {
            const i16 alpha = 0xff;
            const float scale = alpha_scale(alpha);
            combo_sprite(combo1, alpha, scale, joan::canvas::kComboImm[2], &x, &y);
            const char* number = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_short(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Hud>(this)->combo_count)), joan::orig::arg::u32(i32{4})));  // 0x004e1360
            const i16 len = static_cast<i16>(joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(number))));  // strlen
            struct alignas(4) Str {
                u8 bytes[0x194];  // 0x191 used (0x005a11a0 / 0x005a1440 / 0x005a1550)
            } s;
            joan::orig::as_string(&s)->construct(str(d::s_C_9d8f4c));      // 0x005a11a0 ("C")
            y = static_cast<i16>(y - 0x1f);
            const i16 y2 = static_cast<i16>(static_cast<i32>(y) + 0x3e);
            for (i32 i = 0; i < static_cast<i32>(len); ++i) {
                joan::orig::as_string(&s)->assign(str(d::s_C_9d8f4c));            // 0x005a1440 ("C")
                const float dx = static_cast<float>((i + 1) * 0x50) * scale;
                const i16 dx1 = static_cast<i16>(joan::hook::x87::ftol(static_cast<double>(joan::canvas::combo_digits_x() - dx)));
                const float w = 80.0f * scale;
                const i16 dx2 =
                    static_cast<i16>(joan::hook::x87::ftol(static_cast<double>(static_cast<float>(static_cast<i32>(dx1)) + w)));
                i16 pow10 = 10;
                for (i32 j = 0; j < i; ++j) pow10 = static_cast<i16>(pow10 * 10);
                const i32 rem = static_cast<i32>(jo::self<jo::Hud>(this)->combo_count) % static_cast<i32>(pow10);
                const i32 divisor = static_cast<i32>(pow10) / 10;
                const i32 digit = rem / divisor;
                const char* dname = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(d::g_misc))->format_int_58fca0(joan::orig::arg::i32(digit), i32{1}));  // 0x0058fca0
                joan::orig::as_string(&s)->append(joan::orig::arg::str(dname));   // 0x005a1550 append
                joan::orig::as_string(&s)->append(str(d::s_1_9de958));                // 0x005a1550 append "-1"
                u32 slot = 0;
                const u32 color = color4(&slot, i32{0xff}, i32{0xff}, i32{0xff}, static_cast<i32>(static_cast<u8>(alpha)));
                jo::BmpRes* const res = jo::self<jo::Hud>(this)->exp_sprites;
                const char* name = joan::orig::arg::as<const char*>(joan::orig::as_string(&s)->c_str());  // 0x00417d90
                joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(static_cast<i32>(dx1)), joan::orig::arg::i16(static_cast<i32>(y)), 9u, joan::orig::arg::str(name), joan::orig::arg::mptr(res), joan::orig::arg::u32(color), 3, 0, 2, 1, -1, -1, -1, -1, 0, joan::orig::arg::i16(static_cast<i32>(dx2)), joan::orig::arg::i16(static_cast<i32>(y2)));  // 0x00633500
            }
            if (static_cast<i32>(jo::self<jo::Hud>(this)->combo_count) < static_cast<i32>(global<i16>(d::g_kill_combo))) {
                jo::self<jo::Hud>(this)->combo_count = global<i16>(d::g_kill_combo);
                jo::self<jo::Hud>(this)->combo_time = global<u32>(d::g_now);
            } else if (global<u32>(d::g_now) - jo::self<jo::Hud>(this)->combo_time >= 0x7d0) {
                bump_state();
                jo::self<jo::Hud>(this)->combo_time = global<u32>(d::g_now);
                jo::self<jo::Hud>(this)->combo_count = 0;
                global<i16>(d::g_kill_combo) = 0;
            }
            return;
        }
        if (state == 4) {
            const i16 alpha = 0xff;
            combo_sprite(combo0, alpha, alpha_scale(alpha), joan::canvas::kComboImm[3], &x, &y);
            bump_state();
            return;
        }
        if (state == 5) {
            const u32 dt = global<u32>(d::g_now) - jo::self<jo::Hud>(this)->combo_time;
            const u32 t = dt < 0x12c ? global<u32>(d::g_now) - jo::self<jo::Hud>(this)->combo_time : 0x12cu;
            const float v = 255.0f - static_cast<float>(t) / 300.0f * 255.0f;
            const i16 alpha = static_cast<i16>(joan::hook::x87::ftol(static_cast<double>(v)));
            combo_sprite(combo0, alpha, alpha_scale(alpha), joan::canvas::kComboImm[4], &x, &y);
            if (static_cast<i32>(alpha) == 0) {
                jo::self<jo::Hud>(this)->combo_state = 0;
                jo::self<jo::Hud>(this)->combo_on = 0;
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b116_09", name, addr, fn)
B("ComboCounter::draw_702740", 0x00702740, &H::combo_draw);
#undef B

}  // namespace game_b116_09
}  // namespace

// ---------------------------------------- from modules/game_b90_02.cpp (game/b90_02)
namespace {
namespace game_b90_02 {


struct H {
    // 0x00703680
    void reset_time() {
        jo::view<std::uint8_t>(jo::self<jo::Hud>(this)->level_on) = 1;
        jo::self<jo::Hud>(this)->level_state = 1;
        jo::self<jo::Hud>(this)->level_time = global<std::uint32_t>(d::g_now);
        std::uint8_t* const p =
            joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<std::uint32_t>(d::g_player_unit_recno))));  // 0x0041b3c0
        jo::self<jo::Hud>(this)->level_c3 = static_cast<std::uint8_t>(jo::as_unit(p)->level);  // Unit::level +0x124 on x86 (the low byte, as the original's byte load)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_02", name, addr, fn)
B("State::reset_time", 0x00703680, &H::reset_time);
#undef B

}  // namespace game_b90_02
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {


struct H {
    // ------------------------------------------------------------------ 0x007036f0
    u32 mark() {
        jo::view<u8>(jo::self<jo::Hud>(this)->combo_on) = 1;
        jo::view<u8>(jo::self<jo::Hud>(this)->combo_state) = 1;
        const u32 g = global<u32>(d::g_now);
        jo::self<jo::Hud>(this)->combo_time = g;
        jo::view<u16>(jo::self<jo::Hud>(this)->combo_count) = global<u16>(d::g_kill_combo);
        return g;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Obj::mark", 0x007036f0, &H::mark);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b87_06.cpp (game/b87_06)
namespace {
namespace game_b87_06 {

using joan::call::thiscall;

constexpr img::Addr kUnitTable = d::g_units;

JOAN_COUNTER(c_health, "UnitPanel::update_health_state");

std::int32_t unit_missing(std::int32_t id) { return joan::orig::as_dynarray(obj(kUnitTable))->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
void* unit_get(std::int32_t id) { return joan::orig::as_dynarray(obj(kUnitTable))->ptr_at(joan::orig::arg::i32(id)); }  // 0x0041b3c0

struct H {
    // ------------------------------------------------------------ 0x00703750 (ret 4)
    void update_health_state(std::int32_t id) {
        c_health.hit();
        if (unit_missing(id) != 0) return;
        void* const u = unit_get(id);
        jo::self<jo::Hud>(this)->shown_hp = jo::as_unit(u)->hit_points;
        jo::self<jo::Hud>(this)->shown_145 = jo::as_unit(u)->mana;
        jo::self<jo::Hud>(this)->shown_126 = jo::as_unit(u)->experience;
        const std::int32_t two_thirds =
            static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::as_unit(u)->hit_points_base) << 1) / 3;
        if (jo::self<jo::Hud>(this)->shown_hp >= two_thirds) {
            if (jo::self<jo::Hud>(this)->health_state != 0) joan::orig::as_hud(this)->set_health_state(static_cast<std::int8_t>(joan::orig::arg::i32(0)));  // 0x006fe4a0
        } else if (jo::self<jo::Hud>(this)->shown_hp >= jo::as_unit(u)->hit_points_base / 3) {
            if (jo::self<jo::Hud>(this)->health_state != 3) joan::orig::as_hud(this)->set_health_state(static_cast<std::int8_t>(joan::orig::arg::i32(3)));
        } else {
            if (jo::self<jo::Hud>(this)->health_state != 5) joan::orig::as_hud(this)->set_health_state(static_cast<std::int8_t>(joan::orig::arg::i32(5)));
        }
        jo::view<std::int32_t>(jo::self<jo::Hud>(this)->target_id) = 0;
    }
};

#define B06(name, addr, fn) JOAN_REPLACE("game/b87_06", name, addr, fn)
B06("UnitPanel::update_health_state", 0x00703750, &H::update_health_state);
#undef B06

}  // namespace game_b87_06
}  // namespace

// ---------------------------------------- from modules/game_b106_07.cpp (game/b106_07)
namespace {
namespace game_b106_07 {


int stricmp_(const char* a, const char* b) { return joan::crt::stricmp(joan::orig::arg::str(a), joan::orig::arg::str(b)); }  // _stricmp

JOAN_COUNTER(c_siege_prefix, "game/b106_07 Unit::siege_model_prefix");

// ---- 0x00703880 table --------------------------------------------------------------------
struct Siege {
    img::Addr name;
    img::Addr alt;     // second name (img::kNoAddr = none): match = either _stricmp == 0
    img::Addr prefix;
};
const Siege kSiege[] = {
    {d::s_ALEXANDER, img::kNoAddr, d::s_ALEXDLP},  // ALEXANDER -> ALEXDLP
    {d::s_Batterram, img::kNoAddr, d::s_Batter},  // Batterram -> Batter
    {d::s_bombard, img::kNoAddr, d::s_bombard},  // bombard -> bombard
    {d::s_CANNON, img::kNoAddr, d::s_canon},  // CANNON -> canon
    {d::s_CROSSBOW, img::kNoAddr, d::s_CROSSBOW},  // CROSSBOW -> CROSSBOW
    {d::s_AUTOBOW, img::kNoAddr, d::s_CROSSBOW},  // AUTOBOW -> CROSSBOW
    {d::s_BEGLEY, img::kNoAddr, d::s_BEGLEY},  // BEGLEY -> BEGLEY
    {d::s_AUTOBOW2, img::kNoAddr, d::s_CROSSBOW},  // AUTOBOW2 -> CROSSBOW
    {d::s_TREBUCHET, img::kNoAddr, d::s_TRE},  // TREBUCHET -> TRE
    {d::s_fauconneau, img::kNoAddr, d::s_FAU},  // fauconneau -> FAU
    {d::s_SIEGE_TOWER, img::kNoAddr, d::s_SIEGE_9deb34},  // SIEGE_TOWER -> SIEGE
    {d::s_GOLD_CART, d::s_GOLD_CART5, d::s_CART},  // GOLD_CART | GOLD_CART5 -> CART
    {d::s_MER_POLEMAN, img::kNoAddr, d::s_MPOLE},  // MER_POLEMAN -> MPOLE
    {d::s_MER_POLEMAN_CAPTAIN, img::kNoAddr, d::s_MPOLECAP},  // MER_POLEMAN_CAPTAIN -> MPOLECAP
    {d::s_MER_ARCHER, d::s_MER_EX_ARCHER, d::s_M_ARC},  // MER_ARCHER | MER_EX_ARCHER -> M_ARC
    {d::s_MER_ARCHER_CAPTAIN, d::s_MER_EX_ARCHER_CAPTAIN, d::s_M_ARCC},  // MER_ARCHER_CAPTAIN | MER_EX_ARCHER_CAPTAIN -> M_ARCC
    {d::s_MER_LIGHT_INFANTRY, img::kNoAddr, d::s_mltinf},  // MER_LIGHT_INFANTRY -> mltinf
    {d::s_MER_LIGHT_INFANTRY_CAPTAIN, img::kNoAddr, d::s_mltinfc},  // MER_LIGHT_INFANTRY_CAPTAIN -> mltinfc
    {d::s_LORD_TALBOT, img::kNoAddr, d::s_TALBOT},  // LORD_TALBOT -> TALBOT
    {d::s_BUR_ARCHER, img::kNoAddr, d::s_BURA},  // BUR_ARCHER -> BURA
    {d::s_BUR_ARCHER_CAPTAIN, img::kNoAddr, d::s_BURAC},  // BUR_ARCHER_CAPTAIN -> BURAC
    {d::s_BUR_HEAVYINFANTRY, img::kNoAddr, d::s_BURHI},  // BUR_HEAVYINFANTRY -> BURHI
    {d::s_BUR_HEAVYINFANTRY_CAPTAIN, img::kNoAddr, d::s_BURHIC},  // BUR_HEAVYINFANTRY_CAPTAIN -> BURHIC
    {d::s_BUR_LIGHTINFANTRY, img::kNoAddr, d::s_BURLI},  // BUR_LIGHTINFANTRY -> BURLI
    {d::s_BUR_LIGHTINFANTRY_CAPTAIN, img::kNoAddr, d::s_BURLIC},  // BUR_LIGHTINFANTRY_CAPTAIN -> BURLIC
    {d::s_John_de_la_pole, img::kNoAddr, d::s_JOHNDLP},  // John_de_la_pole -> JOHNDLP
    {d::s_william_de_la_pole, img::kNoAddr, d::s_WILLDLP},  // william_de_la_pole -> WILLDLP
    {d::s_Luxembourg, img::kNoAddr, d::s_LUX},  // Luxembourg -> LUX
    {d::s_MATTHEWGOUGH, img::kNoAddr, d::s_GOUGH},  // MATTHEWGOUGH -> GOUGH
    {d::s_MARTINGOUGH, img::kNoAddr, d::s_GOUGH},  // MARTINGOUGH -> GOUGH
    {d::s_Perrinet, img::kNoAddr, d::s_GRESSART},  // Perrinet -> GRESSART
    {d::s_BEDFORD, img::kNoAddr, d::s_BEDFORD},  // BEDFORD -> BEDFORD
    {d::s_Philip_the_good, img::kNoAddr, d::s_PHILIP},  // Philip_the_good -> PHILIP
    {d::s_WANDOMME, img::kNoAddr, d::s_WANDOMME},  // WANDOMME -> WANDOMME
    {d::s_HUDLEG, img::kNoAddr, d::s_WANDOMME},  // HUDLEG -> WANDOMME
    {d::s_ENGLISH_ARCHER, img::kNoAddr, d::s_ENGA},  // ENGLISH_ARCHER -> ENGA
    {d::s_ENGLISH_ARCHER_CAPTAIN, img::kNoAddr, d::s_ENGAC},  // ENGLISH_ARCHER_CAPTAIN -> ENGAC
    {d::s_ENGLISH_EX_ARCHER, img::kNoAddr, d::s_ENGA},  // ENGLISH_EX_ARCHER -> ENGA
    {d::s_ENGLISH_EX_ARCHER_CAPTAIN, img::kNoAddr, d::s_ENGAC},  // ENGLISH_EX_ARCHER_CAPTAIN -> ENGAC
    {d::s_ENGLISH_HEAVY_INFANTRY, img::kNoAddr, d::s_ENGHI},  // ENGLISH_HEAVY_INFANTRY -> ENGHI
    {d::s_ENGLISH_HEAVY_INFANTRY_CAPTA, img::kNoAddr, d::s_ENGHIC},  // ENGLISH_HEAVY_INFANTRY_CAPTAIN -> ENGHIC
    {d::s_ENGLISH_LIGHT_INFANTRY, img::kNoAddr, d::s_ENGLI},  // ENGLISH_LIGHT_INFANTRY -> ENGLI
    {d::s_ENGLISH_LIGHT_INFANTRY_CAPTA, img::kNoAddr, d::s_ENGLIC},  // ENGLISH_LIGHT_INFANTRY_CAPTAIN -> ENGLIC
    {d::s_ENGLISH_POLEMAN, img::kNoAddr, d::s_ENGP},  // ENGLISH_POLEMAN -> ENGP
    {d::s_ENGLISH_POLEMAN_CAPTAIN, img::kNoAddr, d::s_ENGPC},  // ENGLISH_POLEMAN_CAPTAIN -> ENGPC
    {d::s_ENGLISH_MACEMAN, img::kNoAddr, d::s_ENGM},  // ENGLISH_MACEMAN -> ENGM
    {d::s_ENGLISH_MACEMAN_CAPTAIN, img::kNoAddr, d::s_ENGMC},  // ENGLISH_MACEMAN_CAPTAIN -> ENGMC
    {d::s_ENGLISH_CROSSBOW_MAN, img::kNoAddr, d::s_ENGC},  // ENGLISH_CROSSBOW_MAN -> ENGC
    {d::s_ENGLISH_CROSSBOW_MAN_CAPTAIN, img::kNoAddr, d::s_ENGCC},  // ENGLISH_CROSSBOW_MAN_CAPTAIN -> ENGCC
    {d::s_WILLIAM_GLASDALE, img::kNoAddr, d::s_GLASDALE},  // WILLIAM_GLASDALE -> GLASDALE
    {d::s_LORD_SCALES, img::kNoAddr, d::s_SCALES},  // LORD_SCALES -> SCALES
    {d::s_JOHN_FASTOLF, img::kNoAddr, d::s_FASTOLF},  // JOHN_FASTOLF -> FASTOLF
    {d::s_MER_POLEMAN_CAPTAIN, img::kNoAddr, d::s_MPOLECAP},  // MER_POLEMAN_CAPTAIN -> MPOLECAP
    {d::s_MER_POLEMAN, img::kNoAddr, d::s_MPOLE},  // MER_POLEMAN -> MPOLE
    {d::s_MER_LIGHT_INFANTRY_CAPTAIN, img::kNoAddr, d::s_MLTINFC},  // MER_LIGHT_INFANTRY_CAPTAIN -> MLTINFC
    {d::s_MER_LIGHT_INFANTRY, img::kNoAddr, d::s_MLTINF},  // MER_LIGHT_INFANTRY -> MLTINF
};

struct H {
    // 0x00703880
    const char* siege_model_prefix(u32 id) {
        c_siege_prefix.hit();
        u8* const unit = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(id)));  // 0x0041b3c0
        u8* const type = joan::orig::arg::as<u8*>(joan::orig::as_records_203(obj(d::g_types))->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
        for (const Siege& s : kSiege) {
            if (stricmp_(jo::as_unit_type(type)->name, str(s.name)) == 0) return str(s.prefix);
            if (s.alt != img::kNoAddr && stricmp_(jo::as_unit_type(type)->name, str(s.alt)) == 0) return str(s.prefix);
        }
        return nullptr;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b106_07", name, addr, fn)
B("Unit::siege_model_prefix", 0x00703880, &H::siege_model_prefix);
#undef B

}  // namespace game_b106_07
}  // namespace

// ---------------------------------------- from modules/game_b108_16.cpp (game/b108_16)
namespace {
namespace game_b108_16 {


JOAN_COUNTER(c_arrows, "Hud::draw_arrows");

// 0x0042a260: 4-byte colour (r, g, b, a) built in place; the original reads the dword back.
u32 color_ffffffff() {
    alignas(4) u8 c[4] = {0, 0, 0, 0};
    const u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_colour(c)->construct(i32{0xff}, i32{0xff}, i32{0xff}, i32{0xff}));  // 0x0042a260
    return jo::view<u32>(*jo::as_colour(p));
}

struct H {
    // --------------------------------------------------------------------- 0x00704260
    i32 arrow_index(i32 i) { return 8 - static_cast<i32>(jo::self<jo::Hud>(this)->arrow_count) + i; }
    void draw_arrow(i32 i, img::Addr name, u32 dy) {
        const u32 color = color_ffffffff();
        jo::BmpRes* const sprite = jo::self<jo::Hud>(this)->sprites;
        const u32 y = global<u32>(d::g_screen_h) - dy;
        const u32 x = static_cast<u32>((&img::global<const i32>(d::k_9de64c))[arrow_index(i)]) +
                      global<u32>(d::g_screen_w) - 0x2d9u;
        joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), 9u, joan::orig::arg::str(str(name)), joan::orig::arg::mptr(sprite), joan::orig::arg::u32(color), 3, 0, 0, 0, -1, -1, -1, -1, 0, -1, -1);  // 0x00633500
    }
    u32& arrow_slot(i32 i) { return jo::self<jo::Hud>(this)->slot_unit[arrow_index(i)]; }
    void draw_arrows() {
        c_arrows.hit();
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Hud>(this)->arrow_count); ++i) {
            const u32 id = jo::self<jo::Hud>(this)->arrow_unit[i];
            if (joan::orig::as_dynarray(obj(d::g_units))->empty_at(joan::orig::arg::i32(id)) != 0) continue;  // 0x00428b70
            const u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(jo::self<jo::Hud>(this)->arrow_unit[i])));  // 0x0041b3c0
            if (jo::view<i8>(jo::as_unit(u)->levelup_flag) != 0) {
                if (arrow_slot(i) == 0) arrow_slot(i) = global<u32>(d::g_now);
                const u32 elapsed = global<u32>(d::g_now) - arrow_slot(i);
                const u32 phase = (elapsed / global<u32>(d::g_a3cc14)) % 4u;
                if (phase == 2) draw_arrow(i, d::s_ARROW1, 0x64);  // "ARROW1"
                if (phase == 2 || phase == 1) draw_arrow(i, d::s_ARROW2, 0x5c);  // "ARROW2"
                if (phase <= 2) draw_arrow(i, d::s_ARROW3, 0x54);  // "ARROW3"
                continue;
            }
            if (jo::as_unit(u)->attr_points != 0 || jo::as_unit(u)->stat_points != 0) {
                draw_arrow(i, d::s_ARROW1, 0x64);  // "ARROW1"
                draw_arrow(i, d::s_ARROW2, 0x5c);  // "ARROW2"
                draw_arrow(i, d::s_ARROW3, 0x54);  // "ARROW3"
            }
            arrow_slot(i) = 0;
        }
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_16", name, addr, fn)
B108("Hud::draw_arrows", 0x00704260, &H::draw_arrows);
#undef B108

}  // namespace game_b108_16
}  // namespace

// ---------------------------------------- from modules/game_b107_18.cpp (game/b107_18)
namespace {
namespace game_b107_18 {

using joan::call::thiscall;

float fbits(u32 v) {
    float f;
    std::memcpy(&f, &v, 4);
    return f;
}

JOAN_COUNTER(c_test_quad, "Scene::build_test_quad");

struct H {
    // The Hud's portrait mesh (this = the Hud: +0x21ed is portrait_actor).
    jo::Plane*& plane() { return jo::self<jo::Hud>(this)->portrait_plane; }

    // ---- 0x007069f0 ----
    struct Vtx {
        float x, y, z;
        u32 nx, ny, nz;
        u8 col[4];
        u32 u, v;
    };
    static_assert(sizeof(Vtx) == 0x24, "Vtx");

    u32 build_test_quad() {
        c_test_quad.hit();
        if (plane() != nullptr) {
            void* const p = plane();
            if (p != nullptr) joan::orig::as_plane(p)->destroy(1u);  // 0x00459c40 (scalar deleting dtor)
            plane() = nullptr;
        }
        if (jo::self<jo::Hud>(this)->portrait_actor != nullptr) {
            void* const p = jo::self<jo::Hud>(this)->portrait_actor;
            if (p != nullptr) joan::orig::as_scene_object(p)->destroy(1u);  // 0x00420840 (scalar deleting dtor)
            jo::self<jo::Hud>(this)->portrait_actor = nullptr;
        }
        void* mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::Plane)));  // operator new (0x11c on i686)
        void* mesh = mem != nullptr ? joan::orig::as_plane(mem)->construct(1u, 0u) : nullptr;  // 0x005cbaf0
        plane() = static_cast<jo::Plane*>(mesh);
        joan::orig::as_plane(plane())->init(0, 0, 0, 1u);  // 0x005cc7a0
        mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::SceneObject)));  // 0x10c on i686
        void* holder = mem != nullptr ? joan::orig::arg::as<void*>(joan::orig::as_scene_object(mem)->SceneObject_5aab20()) : nullptr;  // 0x005aab20
        jo::self<jo::Hud>(this)->portrait_actor = static_cast<jo::Actor*>(holder);
        mem = joan::crt::operator_new(static_cast<u32>(sizeof(jo::ActorDef)));  // 0x2d8 on i686
        void* const inner = mem != nullptr ? joan::orig::as_actor_def(mem)->construct() : nullptr;  // 0x005af390
        jo::self<jo::Hud>(this)->portrait_actor->def = static_cast<jo::ActorDef*>(inner);
        joan::orig::as_actor_def(jo::self<jo::Hud>(this)->portrait_actor->def)->create(plane(), nullptr);  // 0x005b02f0
        jo::self<jo::Hud>(this)->portrait_actor->anim = nullptr;
        joan::orig::as_scene_object(jo::self<jo::Hud>(this)->portrait_actor)->init();  // 0x005aab70
        plane() = jo::self<jo::Hud>(this)->portrait_actor->plane;
        const i32 mat = joan::orig::arg::as<i32>(joan::orig::as_plane(plane())->add_materials(1));  // 0x005cd470
        jo::Material* const material = &plane()->materials[mat];
        void* const tex_mgr = obj(d::g_texture_array);
        const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(tex_mgr)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(tex_mgr)->set_mode(1u);                     // 0x0060a740
        void* const tex = joan::orig::arg::as<void*>(joan::orig::as_texture_manager(tex_mgr)->load(str(d::s_test_sm), 1, 0u));  // 0x006099c0 'test_sm'
        joan::orig::as_texture_manager(tex_mgr)->set_mode(joan::orig::arg::u32(saved));                                         // 0x0060a740
        joan::orig::as_material(material)->construct();                                                // 0x005d52a0
        u8 col[4] = {0xf0, 0xf0, 0xf0, 0xff};
        joan::orig::as_material(material)->set_colour0(joan::orig::arg::mptr(static_cast<void*>(col)));  // 0x005d5540
        joan::orig::as_material(material)->set_texture(0, joan::orig::arg::mptr(tex));                   // 0x005d5650
        material->blend = 3;
        material->texture_mode = 0x10002;
        // (dead locals in the original: float(tex+0x3c - 1), float(tex+0x40 - 1))
        joan::orig::texture_ref::release(joan::orig::arg::mptr(tex));  // 0x0042cef0

        const float half = fbits(0x43960000u);           // 300.0
        const float two = global<float>(d::flt_2);     // 2.0
        const float nx = -half / two;
        const float px = half / two;
        const u32 one = 0x3f800000u;
        Vtx v[4];
        const float xs[4] = {nx, px, nx, px};
        const float ys[4] = {nx, nx, px, px};
        const u32 us[4] = {0, 0, one, one};
        const u32 vs[4] = {0, one, 0, one};
        for (int i = 0; i < 4; ++i) {
            v[i].x = xs[i];
            v[i].y = ys[i];
            v[i].z = 0.0f;
            v[i].nx = 0;
            v[i].ny = 0;
            v[i].nz = one;
            v[i].col[0] = 0xf0;
            v[i].col[1] = 0xf0;
            v[i].col[2] = 0xf0;
            v[i].col[3] = 0xff;
            v[i].u = us[i];
            v[i].v = vs[i];
        }
        const u16 idx[6] = {0, 1, 2, 1, 3, 2};
        const i32 first = joan::orig::arg::as<i32>(joan::orig::as_plane(plane())->add_vertices(4));  // 0x005cd7f0
        std::memcpy(joan::orig::arg::mptr(&plane()->vertices_f1[first]), static_cast<void*>(v), sizeof v);  // memcpy
        const i32 tri = joan::orig::arg::as<i32>(joan::orig::as_plane(plane())->add_triangles(2));  // 0x005ce6f0
        for (i32 i = 0; i < 6; ++i)
            plane()->triangles[tri + i / 3].v[i % 3] = static_cast<u16>(first + idx[i]);
        for (i32 i = 0; i < 2; ++i)
            plane()->triangle_materials[tri + i].value = static_cast<u16>(mat);
        joan::orig::as_plane(plane())->set_format(4);         // 0x005cdc70
        joan::orig::as_plane(plane())->invalidate(joan::orig::arg::u32(-1));        // 0x005cf920
        joan::orig::as_plane(plane())->sort_by_material(1);  // 0x005cf1d0
        return 0;  // 0x005cf1d0 returns nothing; 0x007069f0 has no callers
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b107_18", name, addr, fn)
B("Scene::build_test_quad", 0x007069f0, &H::build_test_quad);
#undef B

}  // namespace game_b107_18
}  // namespace

// ---------------------------------------- from modules/game_b102_15.cpp (game/b102_15)
namespace {
namespace game_b102_15 {

using joan::call::thiscall;

void* const kEntities = obj(d::g_units);

u8* entity(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kEntities)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0

JOAN_COUNTER(c_look, "Camera::look_player_707230");

struct H {
    // ------------------------------------------------------------------ 0x00707230
    i32 look_player_707230() {
        c_look.hit();
        u8* const e = entity(global<i32>(d::g_player_unit_recno));
        u32 pos[3];
        pos[0] = reinterpret_cast<const u32*>(jo::as_unit(e)->position_)[0];
        pos[1] = reinterpret_cast<const u32*>(jo::as_unit(e)->position_)[1];
        pos[2] = reinterpret_cast<const u32*>(jo::as_unit(e)->position_)[2];
        float z;
        std::memcpy(&z, &pos[2], 4);
        z = z + global<float>(d::flt_40);
        std::memcpy(&pos[2], &z, 4);
        joan::orig::as_scene_object(jo::view<void*>(jo::self<jo::Hud>(this)->portrait_actor))->set_angles(0u, 0u, 0u);  // 0x005acaf0
        joan::orig::as_actor(jo::view<void*>(jo::self<jo::Hud>(this)->portrait_actor))->set_position_5ac310(static_cast<void*>(pos));  // 0x005ac310
        return 0;  // 0x005ac310 returns nothing; 0x00707230 has no callers
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_15", name, addr, fn)
B("Camera::look_player_707230", 0x00707230, &H::look_player_707230);
#undef B

}  // namespace game_b102_15
}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
namespace {
namespace game_b85_05 {


JOAN_COUNTER(c_slot_remove, "SlotList::remove");

struct H {
    // ------------------------------------------------------------------ 0x007072c0
    void slot_remove(std::uint32_t id) {
        c_slot_remove.hit();
        for (std::int32_t i = 0; i < static_cast<std::int32_t>(jo::self<jo::Hud>(this)->obj_count); ++i) {
            if (jo::self<jo::Hud>(this)->obj_id[i] - 1u != id) continue;
            jo::self<jo::Hud>(this)->obj_id[i] = 0;
            for (std::int32_t j = i; j < static_cast<std::int32_t>(jo::self<jo::Hud>(this)->obj_count) - 1; ++j) {
                jo::self<jo::Hud>(this)->obj_id[j] = jo::self<jo::Hud>(this)->obj_id[j + 1];
                jo::self<jo::Hud>(this)->obj_alpha[j] = jo::self<jo::Hud>(this)->obj_alpha[j + 1];
                jo::self<jo::Hud>(this)->obj_time[j] = jo::self<jo::Hud>(this)->obj_time[j + 1];
            }
            jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->obj_count) = static_cast<std::uint16_t>(jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->obj_count) - 1);
            return;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("SlotList::remove", 0x007072c0, &H::slot_remove);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using joan::call::thiscall;

JOAN_COUNTER(c_log_push, "Log::push");

struct H {
    void log_push(i32 id) {  // 0x00707400 (the object is the Hud: blink_*/obj_* are what the Log view called visible/fade_*/msg_*; x64 drift +32)
        c_log_push.hit();
        for (i32 i = 0; i < static_cast<i32>(jo::self<jo::Hud>(this)->obj_count); ++i) {
            if (jo::self<jo::Hud>(this)->obj_id[i] - 1u == static_cast<u32>(id)) return;
        }
        for (i32 i = jo::self<jo::Hud>(this)->obj_count; i > 0; --i) {
            jo::self<jo::Hud>(this)->obj_id[i] = jo::self<jo::Hud>(this)->obj_id[i - 1];
            jo::self<jo::Hud>(this)->obj_alpha[i] = jo::self<jo::Hud>(this)->obj_alpha[i - 1];
            jo::self<jo::Hud>(this)->obj_time[i] = jo::self<jo::Hud>(this)->obj_time[i - 1];
            jo::self<jo::Hud>(this)->obj_1a7[i] = jo::self<jo::Hud>(this)->obj_1a7[i - 1];
        }
        jo::self<jo::Hud>(this)->obj_id[0] = static_cast<u32>(id) + 1u;
        bool skip = false;
        if (global<i32>(d::g_mission) == 6) {
            const jo::Scenario& sc = jo::as_game(obj(d::g_game_state))->scenario;  // 0x00a58fab index, 0x00a58fb7 entries
            if (jo::view<i32>(sc.entries[static_cast<i32>(sc.index) - 1].u32_19a) == 1) skip = true;
        }
        if (!skip) {
            const char* const s = joan::orig::arg::as<const char*>(joan::orig::as_main_game(obj(d::g_strings))->open_3());  // 0x004dcb30
            const u32 len = joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s)));  // strlen
            jo::self<jo::Hud>(this)->obj_alpha[0] = static_cast<u16>(len + 1u);
            jo::self<jo::Hud>(this)->obj_time[0] = global<u32>(d::g_now);
            jo::self<jo::Hud>(this)->obj_1a7[0] = 2;
        }
        jo::view<u16>(jo::self<jo::Hud>(this)->obj_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::Hud>(this)->obj_count) + 1u);
        if (jo::self<jo::Hud>(this)->blink_time == 0) {
            jo::self<jo::Hud>(this)->blink_time = 1;
            jo::self<jo::Hud>(this)->blink_on = 1;
            jo::self<jo::Hud>(this)->blink_start = global<u32>(d::g_now);
            jo::self<jo::Hud>(this)->blink_state = 0;
        }
        if (jo::self<jo::Hud>(this)->blink_time == 2) {
            jo::self<jo::Hud>(this)->blink_time = 1;
            const double now = static_cast<double>(global<u32>(d::g_now));  // fild qword (zero-extended)
            double t = static_cast<double>(global<u32>(d::g_now) - jo::self<jo::Hud>(this)->blink_start);
            t = fpu.div(t, global<float>(d::flt_3000));  // 3000
            t = fpu.mul(t, global<float>(d::flt_255));  // 255
            t = fpu.sub(global<float>(d::flt_255), t);  // fsubr
            t = fpu.mul(t, global<float>(d::flt_2000));  // 2000
            t = fpu.div(t, global<float>(d::flt_255));
            jo::view<i32>(jo::self<jo::Hud>(this)->blink_start) = joan::hook::x87::ftol(fpu.sub(now, t));  // fsubp st(1), st
            jo::self<jo::Hud>(this)->blink_on = 1;
            jo::self<jo::Hud>(this)->blink_state = 0;
        }
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Log::push", 0x00707400, &H::log_push);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {


JOAN_COUNTER(c_fade_push, "Fade::push");

struct H {
    // ------------------------------------------------------------------ 0x007078c0
    void fade_push(i32 v) {
        c_fade_push.hit();
        jo::view<i32>(jo::self<jo::Hud>(this)->obj_id[jo::self<jo::Hud>(this)->obj_count]) = v + 1;
        jo::self<jo::Hud>(this)->obj_alpha[jo::self<jo::Hud>(this)->obj_count] = 0;
        jo::self<jo::Hud>(this)->obj_time[jo::self<jo::Hud>(this)->obj_count] = 0;
        jo::view<u16>(jo::self<jo::Hud>(this)->obj_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::Hud>(this)->obj_count) + 1);
    }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Fade::push", 0x007078c0, &H::fade_push);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
namespace {
namespace game_b84_04 {


struct H {
    void reset_707df0() {  // 0x00707df0
        jo::view<std::uint16_t>(jo::self<jo::Hud>(this)->exp_count) = 0;
        jo::view<std::uint8_t>(jo::self<jo::Hud>(this)->exp_on) = 1;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Obj707df0::reset", 0x00707df0, &H::reset_707df0);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Screen::close_6f9720() -> void { joan::bind::member<&::game_b92_06::H::screen_close>(this); }  // 0x6f9720
auto joan::orig::Hud::load_resources() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_ui7::H::load_resources>(this)); }  // 0x6f97d0
auto joan::orig::Hud::free_resources() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_ui7::H::free_resources>(this)); }  // 0x6f98a0
auto joan::orig::Hud::draw_pause_caption(char a0) -> void { joan::bind::member<&::game_ui7::H::draw_pause_caption>(this, a0); }  // 0x6f9a60
auto joan::orig::Hud::clear_portrait() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::reset_6fe240>(this)); }  // 0x6fe240
auto joan::orig::Hud::set_item_portrait(std::int32_t a0) -> void { joan::bind::member<&::game_b83_05::H::item_set>(this, a0); }  // 0x6fe280
auto joan::orig::Hud::set_unit_portrait(std::int32_t a0) -> void { joan::bind::member<&::game_b83_05::H::unit_set>(this, a0); }  // 0x6fe340
auto joan::orig::Hud::nop_6fe480(void* a0, void* a1) -> void { joan::bind::member<&::game_b88_06::H::nop2>(this, a0, a1); }  // 0x6fe480
auto joan::orig::Hud::set_health_state(std::int8_t a0) -> void { joan::bind::member<&::game_b88_06::H::set_30>(this, a0); }  // 0x6fe4a0
auto joan::orig::Hud::nop_6fe500(void* a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b83_05::H::nothing>(this, a0, a1); }  // 0x6fe500
auto joan::orig::NameList::add_6fe520(std::int32_t a0) -> void { joan::bind::member<&::game_b89_05::H::list_add>(this, a0); }  // 0x6fe520
auto joan::orig::Hud::draw_exp_counter() -> void { joan::bind::member<&::game_b109_10::H::exp_draw>(this); }  // 0x6fe660
auto joan::orig::Squad::collect_slots() -> void { joan::bind::member<&::game_b96_08::H::collect_slots>(this); }  // 0x6ff950
auto joan::orig::Units::group_average(std::uint32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b100_16::H::group_average>(this, a0)); }  // 0x6ffb40
auto joan::orig::Unit::major_class(std::int32_t a0) -> std::int16_t { return joan::bind::conv<std::int16_t>(joan::bind::member<&::game_b98_08::H::major_class>(this, a0)); }  // 0x6ffc40
auto joan::orig::Hud::hit_portrait(std::int32_t a0, std::int32_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b91_04::H::hit_6ffd90>(this, a0, a1)); }  // 0x6ffd90
auto joan::orig::Hud::update_level_banner() -> void { joan::bind::member<&::game_b110_06::H::update>(this); }  // 0x6ffed0
auto joan::orig::Hud::start_level_banner() -> void { joan::bind::member<&::game_b90_02::H::reset_time>(this); }  // 0x703680
auto joan::orig::Hud::start_combo() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b95_08::H::mark>(this)); }  // 0x7036f0
auto joan::orig::Hud::update_health_state(std::int32_t a0) -> void { joan::bind::member<&::game_b87_06::H::update_health_state>(this, a0); }  // 0x703750
auto joan::orig::Hud::siege_model_prefix(std::uint32_t a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b106_07::H::siege_model_prefix>(this, a0)); }  // 0x703880
auto joan::orig::Hud::draw_arrows() -> void { joan::bind::member<&::game_b108_16::H::draw_arrows>(this); }  // 0x704260
auto joan::orig::Hud::remove_slot(std::uint32_t a0) -> void { joan::bind::member<&::game_b85_05::H::slot_remove>(this, a0); }  // 0x7072c0
auto joan::orig::Hud::push_log_entry(std::int32_t a0) -> void { joan::bind::member<&::game_b94_08::H::log_push>(this, a0); }  // 0x707400
auto joan::orig::Hud::blink(std::int32_t a0) -> void { joan::bind::member<&::game_b99_10::H::blink>(this, a0); }  // 0x7076c0
auto joan::orig::Hud::append_log_entry(std::int32_t a0) -> void { joan::bind::member<&::game_b97_08::H::fade_push>(this, a0); }  // 0x7078c0
auto joan::orig::Hud::restart_log_entry(std::int32_t a0) -> void { joan::bind::member<&::game_b96_08::H::fade_restart>(this, a0); }  // 0x707960
auto joan::orig::Hud::blink_all(std::int32_t a0) -> void { joan::bind::member<&::game_b100_16::H::blink_all>(this, a0); }  // 0x707b20
auto joan::orig::Hud::blink_restart() -> void { joan::bind::member<&::game_b100_16::H::blink_restart>(this); }  // 0x707ce0
auto joan::orig::Hud::reset_be() -> void { joan::bind::member<&::game_b84_04::H::reset_707df0>(this); }  // 0x707df0
