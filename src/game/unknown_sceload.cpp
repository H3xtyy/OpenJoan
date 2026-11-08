// game/unknown_sceload.cpp: functions whose original source file is unknown (formerly game/_unplaced/sceload.cpp;
// re/attribution.csv: between ww/interfac/optmenui.cpp and end). They move to their original file when evidence names
// it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
#include <cstring>
#include <cstddef>
#include <cmath>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../canvas_api.hpp"
#include "../original/vecmath.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

using i32 = std::int32_t;

using joan::hook::global;

using joan::call::thiscall;

using i16 = std::int16_t;
using u8 = std::uint8_t;

using joan::img::obj;

using joan::img::str;

using i8 = std::int8_t;

using u16 = std::uint16_t;

using joan::hook::x87::Fpu;

}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using joan::orig::helpers::memset_;  // memset

struct H {
    void* clear_94() { memset_(this, 0, static_cast<u32>(sizeof(jo::Progress))); return this; }  // 0x94 on x86 (g_progress)
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj755df0::Obj", 0x00755df0, &H::clear_94);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b101_16.cpp (game/b101_16)
namespace {
namespace game_b101_16 {

// Module fix/uninit enabled ([Fixes] UninitReads=1 and not disabled; registry module_enabled),
// read on the first call (settings are loaded before install_all). A plain int, not a
// function-local static (no thread-safe-init guard); a racing first call computes the same value.
volatile std::int32_t g_fix_uninit = -1;
bool fix_uninit() {
    if (g_fix_uninit < 0) g_fix_uninit = joan::hook::module_enabled("fix/uninit") ? 1 : 0;
    return g_fix_uninit != 0;
}

void* const kWindows = obj(d::g_texture_array);

u32 window_save() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kWindows)->mode()); }         // 0x0060a720
void window_set(u32 v) { joan::orig::as_texture_manager(kWindows)->set_mode_60a740(v); }  // 0x0060a740
void* window_load(img::Addr name) { return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(kWindows)->load(joan::orig::arg::str(str(name)), 1, 0u)); }  // 0x006099c0

JOAN_COUNTER(c_load_images, "SaveLoad::load_images_755e30");

struct H {
    // ------------------------------------------------------------------ 0x00755e30
    void* load_images_755e30() {
        c_load_images.hit();
        // Byte +0x8f outside 0 / 1 / 2: the original reads an uninitialised local as the image
        // selector. Keep that case on the original body (the calls before the read only touch
        // the window manager 0xa403a0, which never writes +0x8f of this object). Batch 118/03:
        // with fix/uninit enabled (module_enabled, read on the first call) the C++ version runs
        // with the selector = -1 (no "M<n>" image is loaded; everything else as for valid modes).
        {
            const u8 m = jo::self<jo::SaveLoad>(this)->mode;
            if (m != 0 && m != 1 && m != 2 && !fix_uninit()) {
                using Fn = void* (H::*)();
                return (this->*joan::hook::original<Fn>(fn::SaveLoad_load_images_755e30_body))();
            }
        }
        jo::self<jo::SaveLoad>(this)->fonts[0] = obj(d::g_bmp_res_ld_bg);
        const u32 saved = window_save();
        window_set(1);
        jo::self<jo::SaveLoad>(this)->fonts[1] = obj(d::g_bmp_res_ld_cp);
        jo::self<jo::SaveLoad>(this)->fonts[2] = obj(d::g_bmp_res_sl_bg1);
        jo::self<jo::SaveLoad>(this)->fonts[3] = obj(d::g_bmp_res_sl_bg2);
        jo::self<jo::SaveLoad>(this)->fonts[4] = obj(d::g_bmp_res_sl_com);
        jo::self<jo::SaveLoad>(this)->fonts[5] = obj(d::g_bmp_res_m1);
        jo::self<jo::SaveLoad>(this)->fonts[6] = obj(d::g_bmp_res_m2);
        jo::self<jo::SaveLoad>(this)->fonts[7] = obj(d::g_bmp_res_m3);
        jo::self<jo::SaveLoad>(this)->fonts[8] = obj(d::g_bmp_res_m4);
        jo::self<jo::SaveLoad>(this)->fonts[9] = obj(d::g_bmp_res_m5);
        jo::self<jo::SaveLoad>(this)->fonts[10] = obj(d::g_bmp_res_m6);
        jo::self<jo::SaveLoad>(this)->fonts[11] = obj(d::g_bmp_res_m7);
        jo::self<jo::SaveLoad>(this)->fonts[12] = obj(d::g_bmp_res_m8);
        jo::self<jo::SaveLoad>(this)->images[2] = window_load(d::s_SL_BG1);   // "SL_BG1"
        jo::self<jo::SaveLoad>(this)->images[4] = window_load(d::s_SL_COM);  // "SL_COM"
        i16 sel;
        const u8 mode = jo::self<jo::SaveLoad>(this)->mode;
        if (mode == 0)
            sel = global<i16>(d::g_mission);
        else if (mode == 1)
            sel = static_cast<i16>(jo::as_game_file(jo::self<jo::SaveLoad>(this)->save)->u32_19a);  // the low word
        else if (mode == 2)
            sel = global<i16>(d::g_mission);
        else
            sel = -1;  // fix/uninit only (batch 118/03): never written; "none" - no mission image loaded
        switch (static_cast<u32>(static_cast<i32>(sel))) {
        case 0:
        case 1: jo::self<jo::SaveLoad>(this)->images[5] = window_load(d::s_M1); break;  // "M1"
        case 2: jo::self<jo::SaveLoad>(this)->images[6] = window_load(d::s_M2); break;  // "M2"
        case 3: jo::self<jo::SaveLoad>(this)->images[7] = window_load(d::s_M3); break;  // "M3"
        case 4: jo::self<jo::SaveLoad>(this)->images[8] = window_load(d::s_M4); break;  // "M4"
        case 5: jo::self<jo::SaveLoad>(this)->images[9] = window_load(d::s_M5); break;  // "M5"
        case 6: jo::self<jo::SaveLoad>(this)->images[10] = window_load(d::s_M6); break;  // "M6"
        case 7: jo::self<jo::SaveLoad>(this)->images[11] = window_load(d::s_M7); break;  // "M7"
        case 8: jo::self<jo::SaveLoad>(this)->images[12] = window_load(d::s_M8); break;  // "M8"
        default: break;
        }
        window_set(saved);
        jo::view<u8>(jo::self<jo::SaveLoad>(this)->started) = 0;
        jo::view<u32>(jo::self<jo::SaveLoad>(this)->pct) = 0;
        jo::view<u32>(jo::self<jo::SaveLoad>(this)->shown_pct) = 0;
        jo::view<u32>(jo::self<jo::SaveLoad>(this)->base_pct) = 0;
        jo::view<u32>(jo::self<jo::SaveLoad>(this)->stages) = 0;
        jo::view<u32>(jo::self<jo::SaveLoad>(this)->stage) = 0;
        jo::view<u32>(jo::self<jo::SaveLoad>(this)->substages) = 0;
        jo::view<u32>(jo::self<jo::SaveLoad>(this)->substage) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_16", name, addr, fn)
B("SaveLoad::load_images_755e30", 0x00755e30, &H::load_images_755e30);
#undef B

}  // namespace game_b101_16
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

void* const kTextures = obj(d::g_texture_array);

void texture_release(void* t) { joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(t)); }  // 0x0060a550

JOAN_COUNTER(c_release13, "Obj756::release");

struct H {
    void* self() { return this; }

    // 0x00756190
    void* release_756190() {
        c_release13.hit();
        for (std::int32_t k = 0; k < 13; ++k) {
            if (jo::self<jo::SaveLoad>(this)->images[k] != nullptr) {
                texture_release(jo::self<jo::SaveLoad>(this)->images[k]);
                jo::self<jo::SaveLoad>(this)->images[k] = nullptr;
            }
        }
        return self();
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj756::release", 0x00756190, &H::release_756190);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b90_06.cpp (game/b90_06)
namespace {
namespace game_b90_06 {

struct H {
    // 0x007563f0
    void set_90(jo::GameFile* v) {
        if (v != nullptr) {
            jo::self<jo::SaveLoad>(this)->save = v;
            jo::self<jo::SaveLoad>(this)->mode = 1;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b90_06", name, addr, fn)
B("Obj::set_90", 0x007563f0, &H::set_90);
#undef B

}  // namespace game_b90_06
}  // namespace

// ---------------------------------------- from modules/game_b110_01.cpp (game/b110_01)
//
//    - Seq::draw_756430 0x00756430 (thiscall on the SaveLoad / Progress object, no args, ret 0): the mission
//      loading screen, a state machine on +0x85 (state) timed from +0x86 (state_time, g_a500b2 ticks):
//        0  picture, background and three black bands; after 3000 -> 1
//        1  + the mission title fading in over 1000 -> 2
//        2  + the mission name (and S5's part) fading in over 1000 -> 3
//        3  the middle band fading out over 2000 -> 4
//        4  the progress bar (empty, width from +0x6d) and "loading" text fading in over 1000 (mode 2: no bar,
//           and the state stays at 4) -> 5, progress reset
//        5  the bar filling: +0x71 (shown, float) follows +0x6d (pct) through the +0x8a tick sum / 50, the
//           marker at 0x195 + 2 * pct; at pct == 100 -> 6
//        6  full bar, the text fading out over 2000 -> 7, progress reset
//        7  everything under a black quad fading in over 500 (mode 2: no bar) -> 8
//      Nothing at all while state_time is 0. The pictures come from SaveLoad::fonts (load_images 0x00755e30):
//      [2] background, [4] bar, [5 + m - 1] mission m (1..8; any other m: mission 1's). m is g_mission (mode
//      0 / 2) or the save's +0x19a (mode 1). Modes other than 0..2 leave the original's mission local
//      uninitialised; here (as in the earlier port) it is 0, i.e. mission 1's pictures.
//  - x87 (PC24): the fades fild the elapsed ticks (qword, zero-extended), fdiv / fmul by the float constants,
//    _ftol (low word kept); the bar width filds +0x6d, fdiv 100, fmul 200, fadd 4, _ftol; the shown value is the
//    fild qword of the tick sum / 50 stored as a float; the compare is fild +0x6d against fld dword +0x71 (false
//    when unordered). Colour 0x0042a260 keeps the low byte of each argument (the original's alpha dwords carry
//    the previous call's eax in their upper bytes there).
namespace {
namespace game_b110_01 {

using jo::SaveLoad;
using i64 = std::int64_t;

JOAN_COUNTER(c_draw, "Seq::draw_756430");

void* const kSprites = obj(d::g_sprites_a46128);
constexpr u32 kLayer = 5;
constexpr u32 kOpaque = 0xff;

// Colour(r, g, b, a) 0x0042a260 on a temporary, its dword.
u32 rgba(u32 r, u32 g, u32 b, u32 a) {
    jo::Colour c{};
    u32 v;
    std::memcpy(&v, c.construct(r, g, b, a), sizeof v);
    return v;
}
u32 white(u32 a) { return rgba(0xff, 0xff, 0xff, a); }
u32 black(u32 a) { return rgba(0, 0, 0, a); }

// SpriteQueue::draw_named 0x00633500: layer 5, a20 = 3, a24 = 0, a28 / a2c = 2 / 1 ("fade") or 0 / 0, the source
// rectangle (-1 = the sprite's own), no rotation, default destination corner.
void sprite(i16 x, i16 y, const char* name, void* set, u32 colour, bool fade, i16 sx0 = -1, i16 sy0 = -1, i16 sx1 = -1,
            i16 sy1 = -1) {
    const i16 a28 = fade ? i16{2} : i16{0}, a2c = fade ? i16{1} : i16{0};
    joan::orig::as_sprite_queue(kSprites)->draw_named(x, y, kLayer, name, set, colour, 3, 0, a28, a2c, sx0, sy0, sx1, sy1, 0, -1, -1);
}

// Sprite2D::queue 0x00632a70 of an untextured black rectangle.
void band(i32 y0, i32 y1, u32 alpha) {
    joan::orig::as_sprite2d(kSprites)->queue(0, y0, 0x3ff, y1, kLayer, black(alpha), nullptr, 3, 1, 1, 0, -1, -1, -1, -1, 0);
}

u32 now() { return global<u32>(d::g_game_time_ms); }
u32 elapsed(const SaveLoad* s) { return now() - s->state_time; }

// The fade level: min(elapsed, cap) / length * 255 at PC24, _ftol, low word.
u16 fade_level(const SaveLoad* s, u32 cap, img::Sym<float> length) {
    const Fpu fpu{true};
    const u32 t = elapsed(s) >= cap ? cap : elapsed(s);
    const double q = fpu.div(static_cast<double>(static_cast<i64>(t)), static_cast<double>(global<float>(length)));
    return static_cast<u16>(joan::hook::x87::ftol(fpu.mul(q, static_cast<double>(global<float>(d::flt_255)))));
}
u16 fade_out_level(const SaveLoad* s, u32 cap, img::Sym<float> length) {
    const u16 l = fade_level(s, cap, length);
    return static_cast<u16>(0xffu - static_cast<u32>(static_cast<i32>(static_cast<i16>(l))));
}

// The bar's source width: +0x6d / 100 * 200 + 4 at PC24, _ftol (low word).
i16 bar_width(const SaveLoad* s) {
    const Fpu fpu{true};
    const double w = fpu.add(fpu.mul(fpu.div(static_cast<double>(s->pct), static_cast<double>(global<float>(d::flt_100))),
                                     static_cast<double>(global<float>(d::flt_200))),
                             static_cast<double>(global<float>(d::flt_4)));
    return static_cast<i16>(joan::hook::x87::ftol(w));
}

void next_state(SaveLoad* s) {
    ++jo::view<u8>(s->state);
    s->state_time = now();
}
void reset_progress(SaveLoad* s) {
    s->u32_8a = 0;
    jo::view<u32>(s->shown_pct) = 0;
    jo::view<u32>(s->pct) = 0;
}

struct Screen {
    SaveLoad* s;
    i16 mission;

    // Mission m's pictures: fonts[5 + m - 1] for m = 1..8, else fonts[5].
    void* pictures() const {
        const u32 k = static_cast<u32>(static_cast<i32>(mission)) - 1u;
        return s->fonts[5 + (k > 7u ? 0u : k)];
    }
    void background(u32 middle_alpha, bool middle) const {
        sprite(0, 0xc2, str(d::s_scr), pictures(), white(kOpaque), false);
        sprite(0, 0, str(d::s_bg), s->fonts[2], white(kOpaque), false);
        if (middle) band(0xc2, 0x245, middle_alpha);
        band(0, 0xc1, kOpaque);
        band(0x246, 0x307, kOpaque);
    }
    void title(u32 colour, bool fade) const { sprite(0x1a0, 0x48, str(d::s_title), pictures(), colour, fade); }
    // The name (mission 6's further left) and mission 5's part picture (+0x8e, 1..4).
    void name(u32 colour, bool fade) const {
        sprite(mission == 6 ? i16{0x98} : i16{0x114}, 0x160, str(d::s_name), pictures(), colour, fade);
        if (mission != 5) return;
        const u32 part = static_cast<u32>(static_cast<i32>(static_cast<i8>(s->u8_8e))) - 1u;
        if (part > 3u) return;
        static constexpr img::Sym<char[9]> kParts[4] = {d::s_s5_part1, d::s_s5_part2, d::s_s5_part3, d::s_s5_part4};
        sprite(0x140, 0x1ac, str(kParts[part]), pictures(), colour, fade);
    }
    void labels() const {
        title(white(kOpaque), false);
        name(white(kOpaque), false);
    }
    // The bar's empty / full sprite (source width w), its frame and the text under it.
    void bar(u32 colour, i16 w) const { sprite(0x198, 0x2c6, str(d::s_bg), s->fonts[4], colour, true, 0, 0, w, 0x1c); }
    void frame(u32 colour, bool fade) const { sprite(0x19a, 0x2c8, str(d::s_frame), s->fonts[4], colour, fade); }
    void text(u32 colour) const {
        const char* const t = joan::orig::as_txt_res(obj(d::g_strings))->sceload_text_1();  // 0x004de470
        joan::orig::as_text(obj(d::g_font_goudy_bold_18))->draw_line(0x1c2, 0x2e8, kLayer, t, colour, -1, 0, 1, 2, 0, 1.0f, 1.0f);  // 0x006255d0
    }
};

i16 mission_of(const SaveLoad* s) {
    switch (s->mode) {
    case 0:
    case 2: return static_cast<i16>(global<u16>(d::g_mission));
    case 1: return static_cast<i16>(static_cast<u16>(s->save->u32_19a));  // its low word
    default: return 0;  // uninitialised in the original (see header)
    }
}

struct H {
    // 0x00756430
    void draw() {
        c_draw.hit();
        SaveLoad* const s = jo::self<SaveLoad>(this);
        if (s->state_time == 0) return;
        joan::orig::as_cursor(obj(d::g_mouse))->set_pos();  // 0x00592bd0
        const Screen sc{s, mission_of(s)};
        switch (s->state) {
        case 0:
            sc.background(kOpaque, true);
            if (elapsed(s) >= 3000) next_state(s);
            return;
        case 1: {
            const u16 a = fade_level(s, 1000, d::flt_1000);
            sc.background(kOpaque, true);
            sc.title(white(a), true);
            if (elapsed(s) >= 1000) next_state(s);
            return;
        }
        case 2: {
            const u16 a = fade_level(s, 1000, d::flt_1000);
            sc.background(kOpaque, true);
            sc.title(white(kOpaque), false);
            sc.name(white(a), true);
            if (elapsed(s) >= 1000) next_state(s);
            return;
        }
        case 3: {
            const u16 a = fade_out_level(s, 2000, d::flt_2000);
            sc.background(a, true);
            sc.labels();
            if (elapsed(s) >= 2000) next_state(s);
            return;
        }
        case 4: {
            const u16 a = fade_level(s, 1000, d::flt_1000);
            sc.background(kOpaque, false);
            if (s->mode != 2) {
                const i16 w = bar_width(s);
                sc.bar(white(a), w);
                sc.frame(white(a), true);
                sc.text(white(a));
            }
            sc.labels();
            if (elapsed(s) >= 1000 && s->mode != 2) {
                next_state(s);
                reset_progress(s);
            }
            return;
        }
        case 5: {
            sc.background(kOpaque, false);
            const i16 w = bar_width(s);
            sc.bar(white(kOpaque), w);
            sc.frame(white(kOpaque), false);
            sc.text(white(kOpaque));
            if (static_cast<double>(s->pct) > static_cast<double>(jo::view<float>(s->shown_pct))) {
                s->u32_8a += global<u32>(d::g_frame_elapsed_ms);
                jo::view<float>(s->shown_pct) = static_cast<float>(static_cast<double>(static_cast<i64>(s->u32_8a / 50u)));
            }
            const i16 x = static_cast<i16>(static_cast<u32>(s->pct) + static_cast<u32>(s->pct) + 0x195u);
            sprite(x, 0x2c4, str(d::s_bar_9dd9f8), s->fonts[4], white(kOpaque), false);
            sc.labels();
            if (s->pct == 100) next_state(s);
            return;
        }
        case 6: {
            const u16 a = fade_out_level(s, 2000, d::flt_2000);
            sc.background(kOpaque, false);
            sc.bar(white(kOpaque), 0xcc);
            sc.frame(white(kOpaque), false);
            sc.text(white(a));
            sc.labels();
            if (elapsed(s) >= 2000) {
                next_state(s);
                reset_progress(s);
            }
            return;
        }
        case 7: {
            const u16 a = fade_level(s, 500, d::flt_500);
            sc.background(kOpaque, false);
            if (s->mode != 2) {
                sc.bar(white(kOpaque), 0xcc);
                sc.frame(white(kOpaque), false);
            }
            sc.labels();
            joan::orig::as_sprite2d(kSprites)->queue(0, 0, 0x400, 0x300, kLayer, black(a), nullptr, 3, 0, 2, 1, -1, -1, -1, -1, 0);  // 0x00632a70
            if (elapsed(s) >= 500) jo::view<u8>(s->state) = 8;
            return;
        }
        default: return;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b110_01", name, addr, fn)
B("Seq::draw_756430", 0x00756430, &H::draw);
#undef B

}  // namespace game_b110_01
}  // namespace

// ---------------------------------------- from modules/game_b110_15.cpp (game/b110_15)
namespace {
namespace game_b110_15 {

using joan::call::cdecl_call;


struct Vec3 {
    float x, y, z;
};
static_assert(sizeof(Vec3) == 12, "Vec3");
struct alignas(4) Text {  // 0x005a11a0 / 0x005a1170 path string (0x194 bytes on the stack)
    u8 b[0x194];
};

void* const kUnits = obj(d::g_units);
void* const kKinds = obj(d::g_types);
void* const kSfx = obj(d::g_sfx_player);
void* const kRandom = obj(d::g_misc);
void* const kPlayer = obj(d::g_player_unit_recno);

JOAN_COUNTER(c_message, "Unit::on_message");

// ---------------------------------------------------------------------------- shared helpers
bool dead(i32 id) { return joan::orig::as_dynarray(kUnits)->empty_at(joan::orig::arg::i32(id)) != 0; }  // 0x00428b70
u8* unit(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
bool busy(const void* u) { return joan::orig::arg::as<i8>(joan::orig::as_unit(u)->dead()) != 0; }      // 0x0042d0e0 (al)
void text_set(Text* t, const char* s) { joan::orig::as_string(t)->assign(joan::orig::arg::str(s)); }  // 0x005a1440
void text_add(Text* t, const char* s) { joan::orig::as_string(t)->append(joan::orig::arg::str(s)); }  // 0x005a1550
const char* c_str(Text* t) { return joan::orig::arg::as<const char*>(joan::orig::as_string(t)->c_str()); }  // 0x00417d90
bool ne_ordered(double a, double b) { return a < b || a > b; }  // fcomp + test ah,0x40 je

struct H {
    // ======================================================================== 0x0075d540
    i32 health_pct() {
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hit_points_max()) == 0) return 0;  // 0x00523320
        const i32 num = static_cast<i32>(static_cast<u32>(jo::self<jo::Unit>(this)->hit_points) * 100u);
        const i32 den = joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hit_points_max());
        return num / den;
    }
    i32 me() { return static_cast<i32>(jo::self<jo::Unit>(this)->id); }
    u8* kind() { return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kKinds)->get(joan::orig::arg::i16(static_cast<i32>(jo::self<jo::Unit>(this)->type_id)))); }  // 0x0042d0a0
    // Unless the +0x339 order is running (mode 0xe), drop the current order 0x00422f10.
    void reset_order() {
        if (jo::self<jo::Unit>(this)->name_index != 0 && jo::self<jo::Unit>(this)->task.kind == 0xe && joan::orig::arg::as<i32>(joan::orig::as_list(&jo::self<jo::Unit>(this)->path)->count_66e8a0()) != 0 &&
            jo::self<jo::Unit>(this)->cell != nullptr && jo::as_cell(jo::view<u8*>(jo::self<jo::Unit>(this)->cell))->tracks != nullptr)  // 0x0066e8a0
            return;
        joan::orig::as_squad(this)->clear();  // 0x00422f10
    }
    void stop() { joan::orig::as_unit(this)->push_message(1, joan::orig::arg::i16(0), joan::orig::arg::i16(0)); }  // 0x00414f50
    void anim(i32 code) { joan::orig::as_unit(this)->set_action(joan::orig::arg::i32(code), 0, 1); }  // 0x00501190
    bool keeps_target() {
        const i32 k = static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind);
        return k == 0x1c || k == 0x49 || k == 0x30 || k == 0x32 || k == 0x36 || k == 0x33 || k == 0x39 || k == 0x38;
    }
    void count_player_hit(const u8* msg) {
        if (static_cast<i32>(jo::as_trigger(msg)->ref_id) != global<i32>(d::g_player_unit_recno)) return;
        global<i16>(d::g_kill_combo) = static_cast<i16>(global<i16>(d::g_kill_combo) + 1);
        if (global<i8>(d::g_a4d6e4) == 0 && global<i16>(d::g_kill_combo) > 1) joan::orig::as_hud(obj(d::g_player_hud))->start_combo();  // 0x007036f0
    }
    // Hit reaction animation by the relative angle g (0x23 front, 0x27 left, 0x28 right, 0x26 back).
    void face(float g) {
        const float a = global<float>(d::flt_m45);  // -45
        const float b = global<float>(d::flt_45);  // 45
        const float c = global<float>(d::flt_m135);  // -135
        const float d = global<float>(d::flt_135);  // 135
        if (g >= a && !(g > b))
            anim(0x23);
        else if (g >= c && !(g >= a))
            anim(0x27);
        else if (g > b && !(g >= d))
            anim(0x28);
        else
            anim(0x26);
    }
    void turn(float g) {
        if (global<i32>(d::g_player_unit_recno) == me()) return;
        if (jo::self<jo::Unit>(this)->anim_kind == 0x29) return;
        if (jo::as_unit_type(kind())->kind == 0xd) return;
        if (jo::as_unit_type(kind())->kind == 0xc) return;
        const float q = jo::self<jo::Unit>(this)->heading / global<float>(d::flt_0_017453292);
        const float h = q - g;
        joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(h));  // 0x0050abd0
    }
    void retarget(const u8* msg, u8* u) {
        if (dead(static_cast<i32>(jo::self<jo::Unit>(this)->target_building)) || busy(unit(static_cast<i32>(jo::self<jo::Unit>(this)->target_building))))
            jo::self<jo::Unit>(this)->target_building = jo::as_trigger(msg)->ref_id;
        if (dead(static_cast<i32>(jo::as_unit(u)->target_building)) || busy(unit(static_cast<i32>(jo::as_unit(u)->target_building))))
            jo::as_unit(u)->target_building = jo::self<jo::Unit>(this)->id;
        if (joan::orig::arg::as<u8*>(joan::orig::as_unit(this)->leader()) != nullptr) {  // 0x004216d0
            if (dead(static_cast<i32>(jo::as_unit(joan::orig::arg::as<u8*>(joan::orig::as_unit(this)->leader()))->target_building))) {
                u8* const o = joan::orig::arg::as<u8*>(joan::orig::as_unit(this)->leader());
                jo::as_unit(o)->target_building = jo::as_trigger(msg)->ref_id;
            }
        }
    }
    void autobow(Vec3* v) {
        if (jo::self<jo::Unit>(this)->actor == nullptr) return;
        if (jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->record) == 0) return;
        u8* const k = kind();
        if (joan::crt::stricmp(joan::orig::arg::str(k + 4), str(d::s_AUTOBOW)) == 0) return;  // _stricmp "AUTOBOW"
        void* const o = joan::orig::as_players(obj(d::g_physic_sys))->list.ptr_at(joan::orig::arg::i32(jo::view<u32>(jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->record)));  // 0x006af310
        joan::orig::as_body(o)->init_target(joan::orig::arg::mptr(v), joan::orig::arg::u32(global<u32>(d::g_now)), 0x64u);  // 0x006b11a0
    }
    // type 1 (0x0075d661)
    void msg_hit(const u8* msg) {
        if (jo::view<i32>(jo::as_trigger(msg)->ref_kind) != 1) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_world(this)->flag_1fd()) != 0) return;  // 0x0042d690
        const float r = joan::orig::arg::as<float>(joan::orig::as_actor(this)->attack_value(static_cast<i32>(jo::as_trigger(msg)->ref_id)));  // 0x005048b0
        if (!ne_ordered(r, global<float>(d::k_9cf5c4))) return;
        u8* const tgt = unit(static_cast<i32>(jo::as_trigger(msg)->ref_id));  // [ebp-0x14]
        count_player_hit(msg);
        if (dead(static_cast<i32>(jo::self<jo::Unit>(this)->target_building)) && !keeps_target()) jo::self<jo::Unit>(this)->target_building = jo::as_trigger(msg)->ref_id;
        if (dead(static_cast<i32>(jo::as_unit(tgt)->target_building)) || busy(unit(static_cast<i32>(jo::as_unit(tgt)->target_building))))
            jo::as_unit(tgt)->target_building = jo::self<jo::Unit>(this)->id;
        reset_order();
        stop();
        if (joan::orig::arg::as<i8>(joan::orig::as_game(this)->byte_2e1()) == 0) {  // 0x00432b90
            const i32 k = static_cast<i32>(jo::as_unit(tgt)->anim_kind);
            if (k == 0x1e || k == 0x1f || k == 0x20 || k == 0x21 || k == 0x53) {
                if (joan::orig::as_misc(kRandom)->random(2) == 1) joan::orig::as_unit(this)->pick_idle_anim(0);  // 0x00590540, 0x00509000
            } else if (k == 0x1c || k == 0x49) {
                const float g = joan::orig::arg::as<float>(joan::orig::as_unit(this)->heading_to_unit(static_cast<i32>(jo::as_unit(tgt)->id)));  // 0x00505960
                face(g);
                turn(g);
            }
        }
        joan::orig::as_unit(this)->use_spiritual_item_4fd980(1);  // 0x004fd980
        if (!busy(this)) {
            counter_attack(msg, tgt);
        } else {
            joan::orig::as_unit(this)->die_7c6240(static_cast<i32>(jo::as_trigger(msg)->ref_id));  // 0x007c6240
        }
        jo::view<u16>(jo::self<jo::Unit>(this)->hit_counter) = 1;
    }
    void counter_attack(const u8* msg, u8* tgt) {
        i32 a = 0xbb8;  // [ebp-0x20]
        i32 b = 0x1f4;  // [ebp-0x24]
        if (jo::as_unit(tgt)->anim_set != nullptr) {
            u8* const p = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(jo::as_unit(tgt)->anim_set)->entry(joan::orig::arg::u32(jo::view<u32>(jo::as_unit(tgt)->anim_entry))));  // 0x004168d0
            i32 c = jo::as_anim_group_rec(p)->model_ref;
            if (jo::as_trigger(msg)->sub != 0) c = static_cast<i32>(jo::as_trigger(msg)->sub);
            if (c != 0) {
                a = jo::view<i32>(jo::as_model_slot(joan::orig::as_model_refs(obj(d::g_model_refs))->get(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(c)))))->attack_force);  // 0x00416f20
                b = jo::view<i32>(jo::as_model_slot(joan::orig::as_model_refs(obj(d::g_model_refs))->get(joan::orig::arg::i16(static_cast<i32>(static_cast<i16>(c)))))->vertical_attack_force);
            }
        }
        const i32 sum = static_cast<i32>(static_cast<u32>(a) + static_cast<u32>(b));
        if (sum < global<i32>(d::g_a3cc84) && b < global<i32>(d::g_a3ccc0)) {
            const i32 q = sum / global<i32>(d::g_a3cc34);
            const i32 v = static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::self<jo::Unit>(this)->morale)) - static_cast<u32>(q));
            joan::orig::as_unit(this)->set_morale(joan::orig::arg::i32(v));  // 0x004fea80
        } else {
            joan::orig::as_unit(this)->set_morale(0);
        }
        u32 ang = 0;  // [ebp-0x2c] (one byte used)
        joan::orig::as_angle(&ang)->construct();  // 0x00576120
        u8* other = nullptr;  // [ebp-0x30]
        if (!dead(jo::as_unit(tgt)->mount_id)) other = unit(jo::as_unit(tgt)->mount_id);
        const Fpu fpu{true};
        const double c180 = static_cast<double>(global<float>(d::flt_180));
        const double pi = global<double>(d::dbl_3_141592653589793);
        if (jo::as_unit(tgt)->anim_kind == 0x46 ||
            (jo::as_trigger(msg)->sub != 0 && other != nullptr && jo::as_unit(tgt)->anim_kind == 0x49 && jo::as_unit(other)->anim_kind == 0xb)) {
            const float g = joan::orig::arg::as<float>(joan::orig::as_unit(tgt)->heading_to_unit(joan::orig::arg::i32(me())));  // 0x00505960
            const double h = static_cast<double>(joan::orig::arg::as<float>(joan::orig::as_ground_object(tgt)->heading()));  // 0x004ff850
            const double t = fpu.div(fpu.mul(h, c180), pi);
            double v = 0.0;
            if (g >= global<float>(d::k_9cf5c4))  // test ah,1 jne: < 0 or NaN -> + 90
                v = fpu.sub(t, global<double>(d::dbl_90));  // - 90.0
            else
                v = fpu.add(t, global<double>(d::dbl_90));  // + 90.0
            joan::orig::as_angle(&ang)->set_byte(joan::orig::arg::f32(static_cast<float>(v)));  // 0x005762c0
        } else {
            const double h = static_cast<double>(joan::orig::arg::as<float>(joan::orig::as_ground_object(tgt)->heading()));
            const double t = fpu.div(fpu.mul(h, c180), pi);  // stored as qword (exact)
            const float g = joan::orig::arg::as<float>(joan::orig::as_unit(tgt)->heading_to_unit(joan::orig::arg::i32(me())));
            joan::orig::as_angle(&ang)->set_byte(joan::orig::arg::f32(static_cast<float>(fpu.sub(t, static_cast<double>(g)))));
        }
        Vec3 vec{0.0f, 0.0f, 0.0f};  // [ebp-0x3c]
        const float fa = static_cast<float>(a);
        vec.x = joan::orig::arg::as<float>(joan::orig::as_angle(&ang)->cos()) * fa;  // 0x00576ac0
        const float fa2 = static_cast<float>(a);
        vec.y = joan::orig::arg::as<float>(joan::orig::as_angle(&ang)->sin()) * fa2;  // 0x005768d0
        vec.z = static_cast<float>(b);
        if (jo::self<jo::Unit>(this)->anim_kind == 0x4e)
            joan::orig::vec_scale_in_place(&vec, 2.0f);  // 0x0060ce80
        else if (jo::self<jo::Unit>(this)->anim_kind == 0x29)
            joan::orig::vec_scale_in_place(&vec, joan::orig::arg::f32(kSeventy));
        autobow(&vec);
        if (jo::self<jo::Unit>(this)->orders != nullptr) {
            const i32 st = jo::as_order(jo::view<u8*>(jo::self<jo::Unit>(this)->orders))->current.kind;
            if ((st == 0xc || st == 0xf) && joan::orig::arg::as<i32>(joan::orig::as_order(jo::self<jo::Unit>(this)->orders)->check_condition(this)) != 0)  // 0x004bb3e0
                joan::orig::as_action(jo::self<jo::Unit>(this)->orders)->dispatch(joan::orig::arg::mptr(this));  // 0x004bb660
        }
    }
    static constexpr float kSeventy = 0.699999988079071f;  // 0x3f333333

    // type 0x400 (0x0075dd8f): true -> return 2
    bool msg_stun(const u8* msg) {
        if (jo::view<i32>(jo::as_trigger(msg)->ref_kind) != 1) return false;
        if (joan::orig::arg::as<i32>(joan::orig::as_world(this)->flag_1fd()) != 0) return false;
        const float r = joan::orig::arg::as<float>(joan::orig::as_actor(this)->attack_value(static_cast<i32>(jo::as_trigger(msg)->ref_id)));  // 0x005048b0
        if (!ne_ordered(r, global<float>(d::k_9cf5c4))) return false;
        u8* const u = unit(static_cast<i32>(jo::as_trigger(msg)->ref_id));  // [ebp-0x44]
        if (jo::as_unit(u)->anim_kind != 0x22) return false;
        const i32 k = static_cast<i32>(jo::self<jo::Unit>(this)->anim_kind);
        if (k != 0x40 && k != 0x41 && k != 0x42 && k != 0x29) return false;
        count_player_hit(msg);
        jo::self<jo::Unit>(this)->downed_hit = 1;
        jo::view<u32>(jo::self<jo::Unit>(this)->fell_timer) = 0;
        anim(0x29);
        reset_order();
        stop();
        joan::orig::as_unit(this)->use_spiritual_item_4fd980(1);  // 0x004fd980
        if (busy(this)) joan::orig::as_unit(this)->die_7c6240(static_cast<i32>(jo::as_trigger(msg)->ref_id));  // 0x007c6240
        jo::view<u16>(jo::self<jo::Unit>(this)->hit_counter) = 1;
        return true;
    }

    // Knock-back after a ranged hit: angle object, vector 3000 / 500, scale, AUTOBOW.
    void ranged_knock(u8* u, float s4e) {
        u32 ang = 0;
        joan::orig::as_angle(&ang)->construct();  // 0x00576120
        const Fpu fpu{true};
        const double h = static_cast<double>(joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading()));  // 0x004ff850
        const double t = fpu.div(fpu.mul(h, static_cast<double>(global<float>(d::flt_180))), global<double>(d::dbl_3_141592653589793));
        const float g = joan::orig::arg::as<float>(joan::orig::as_unit(u)->heading_to_unit(joan::orig::arg::i32(me())));  // 0x00505960
        joan::orig::as_angle(&ang)->set_byte(joan::orig::arg::f32(static_cast<float>(fpu.sub(t, static_cast<double>(g)))));  // 0x005762c0
        Vec3 v{0.0f, 0.0f, 0.0f};
        v.x = joan::orig::arg::as<float>(joan::orig::as_angle(&ang)->cos()) * global<float>(d::flt_3000);  // 0x00576ac0 * 3000
        v.y = joan::orig::arg::as<float>(joan::orig::as_angle(&ang)->sin()) * global<float>(d::flt_3000);  // 0x005768d0 * 3000
        v.z = 500.0f;
        if (jo::self<jo::Unit>(this)->anim_kind == 0x4e)
            joan::orig::vec_scale_in_place(&v, joan::orig::arg::f32(s4e));  // 0x0060ce80
        else if (jo::self<jo::Unit>(this)->anim_kind == 0x29)
            joan::orig::vec_scale_in_place(&v, joan::orig::arg::f32(kSeventy));
        autobow(&v);
    }

    // types 2 / 0x40 / 0x2000 (0x0075df3b)
    void msg_shot(const u8* msg) {
        if ((jo::view<i32>(jo::as_trigger(msg)->flags) == 2 || jo::view<i32>(jo::as_trigger(msg)->flags) == 0x2000) && jo::self<jo::Unit>(this)->modifiers != nullptr) {
            const i32 chance = joan::orig::arg::as<i32>(joan::orig::as_modifiers(jo::self<jo::Unit>(this)->modifiers)->sum(0xa, joan::orig::arg::i32(me())));  // 0x00497e10
            if (joan::orig::as_misc(kRandom)->random(0x64) < chance) return;  // 0x00590540
        }
        if (jo::view<i32>(jo::as_trigger(msg)->ref_kind) != 1) return;
        if (dead(static_cast<i32>(jo::as_trigger(msg)->ref_id))) return;
        if (joan::orig::arg::as<i32>(joan::orig::as_world(this)->flag_1fd()) != 0) return;  // 0x0042d690
        if (jo::view<i32>(jo::as_trigger(msg)->flags) != 0x2000) {
            const float r = joan::orig::arg::as<float>(joan::orig::as_unit(this)->hit_value_5051e0(static_cast<i32>(jo::as_trigger(msg)->ref_id), joan::orig::arg::u32(jo::view<i32>(jo::as_trigger(msg)->flags)), 0u));  // 0x005051e0
            if (ne_ordered(r, global<float>(d::k_9cf5c4))) {
                shot_hit(msg);
                return;
            }
        }
        if (jo::view<i32>(jo::as_trigger(msg)->flags) != 0x2000) return;
        const float r = joan::orig::arg::as<float>(joan::orig::as_unit(this)->hit_value_5051e0(static_cast<i32>(jo::as_trigger(msg)->ref_id), joan::orig::arg::u32(jo::view<i32>(jo::as_trigger(msg)->flags)), 1u));
        if (!ne_ordered(r, global<float>(d::k_9cf5c4))) return;
        u8* const u = unit(static_cast<i32>(jo::as_trigger(msg)->ref_id));  // [ebp-0x68]
        retarget(msg, u);
        if (static_cast<i16>(joan::orig::arg::as<u32>(joan::orig::as_unit(this)->is_kind_40_42())) != 0) {  // 0x00508fb0 (ax)
            jo::self<jo::Unit>(this)->downed_hit = 1;
            anim(0x29);
            reset_order();
            stop();
            jo::view<u16>(jo::self<jo::Unit>(this)->hit_counter) = 1;
        } else {
            reset_order();
            stop();
            if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) != 0 &&
                static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_collision_grid(obj(d::g_collision_grid))->is_free(joan::orig::arg::mptr(jo::self<jo::Unit>(this)->actor), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Unit>(this)->layer))))) != 0) {  // 0x00430190, 0x00693170
                anim(0x4e);
            } else if ((jo::view<i32>(jo::as_trigger(msg)->flags) == 0x40 || jo::view<i32>(jo::as_trigger(msg)->flags) == 0x2000) && joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) == 0) {
                joan::orig::as_unit(this)->pick_idle_anim(0);  // 0x00509000
            } else {
                const float g = joan::orig::arg::as<float>(joan::orig::as_unit(this)->heading_to_unit(static_cast<i32>(jo::as_unit(u)->id)));  // 0x00505960
                face(g);
                turn(g);
            }
        }
        joan::orig::as_unit(this)->use_spiritual_item_4fd980(1);  // 0x004fd980
        if (!busy(this)) {
            ranged_knock(u, 3.0f);
        } else {
            joan::orig::as_unit(this)->die_7c6240(static_cast<i32>(jo::as_trigger(msg)->ref_id));  // 0x007c6240
            jo::view<u16>(jo::self<jo::Unit>(this)->hit_counter) = 1;
        }
    }
    void shot_hit(const u8* msg) {  // 0x0075dffa
        u8* const u = unit(static_cast<i32>(jo::as_trigger(msg)->ref_id));  // [ebp-0x4c]
        retarget(msg, u);
        reset_order();
        stop();
        if (joan::orig::arg::as<i8>(joan::orig::as_game(this)->byte_2e1()) == 0) {  // 0x00432b90
            if ((jo::view<i32>(jo::as_trigger(msg)->flags) == 0x40 || jo::view<i32>(jo::as_trigger(msg)->flags) == 0x2000) && joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) == 0) {  // 0x00430190
                joan::orig::as_unit(this)->pick_idle_anim(0);  // 0x00509000
                jo::view<u16>(jo::self<jo::Unit>(this)->hit_counter) = 1;
            } else {
                const float g = joan::orig::arg::as<float>(joan::orig::as_unit(this)->heading_to_unit(static_cast<i32>(jo::as_unit(u)->id)));  // 0x00505960
                face(g);
                turn(g);
            }
        }
        joan::orig::as_unit(this)->use_spiritual_item_4fd980(1);  // 0x004fd980
        if (!busy(this)) {
            ranged_knock(u, 2.5f);
            const i32 v = static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::self<jo::Unit>(this)->morale)) - global<u32>(d::g_a3cc08));
            joan::orig::as_unit(this)->set_morale(joan::orig::arg::i32(v));  // 0x004fea80
        } else {
            joan::orig::as_unit(this)->die_7c6240(static_cast<i32>(jo::as_trigger(msg)->ref_id));  // 0x007c6240
            jo::view<u16>(jo::self<jo::Unit>(this)->hit_counter) = 1;
        }
    }

    // type 0x4000 (0x0075e948)
    void msg_4000(const u8* msg) {
        if (jo::view<i32>(jo::as_trigger(msg)->ref_kind) != 2) return;
        if (joan::orig::as_units(obj(d::g_items))->list.empty_at(static_cast<i32>(jo::as_trigger(msg)->ref_id)) != 0) return;  // 0x00425d50
        if (joan::orig::arg::as<i32>(joan::orig::as_world(this)->flag_1fd()) != 0) return;  // 0x0042d690
        const float r = joan::orig::arg::as<float>(joan::orig::as_unit(this)->attack_value_505650(joan::orig::arg::u32(static_cast<i32>(jo::as_trigger(msg)->ref_id))));  // 0x00505650
        if (!ne_ordered(r, global<float>(d::k_9cf5c4))) return;
        stop();
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->mounted()) != 0) {  // 0x00430190
            anim(0x4e);
        } else {
            joan::orig::as_unit(this)->pick_idle_anim(0);  // 0x00509000
            jo::view<u16>(jo::self<jo::Unit>(this)->hit_counter) = 1;
        }
        joan::orig::as_unit(this)->use_spiritual_item_4fd980(1);  // 0x004fd980
        if (busy(this)) {
            joan::orig::as_unit(this)->on_removed();  // 0x0064f320
            jo::view<u16>(jo::self<jo::Unit>(this)->hit_counter) = 1;
        }
    }

    // type 0x10 (0x0075ea11): false -> common tail, true -> return 2
    bool msg_grab(const u8* msg) {
        if (jo::view<i32>(jo::as_trigger(msg)->ref_kind) != 1) return true;
        if (dead(static_cast<i32>(jo::as_trigger(msg)->ref_id))) return false;
        if (joan::orig::arg::as<i32>(joan::orig::as_unit(this)->hostile_to(joan::orig::arg::i16(static_cast<i32>(jo::as_trigger(msg)->ref_id)))) == 0) return true;  // 0x005094d0
        u8* const u = unit(static_cast<i32>(jo::as_trigger(msg)->ref_id));  // [ebp-0x84]
        if (static_cast<i32>(jo::as_trigger(msg)->ref_id) == global<i32>(d::g_player_unit_recno))
            joan::orig::as_hud(obj(d::g_player_hud))->set_item_portrait(joan::orig::arg::i32(me()));  // 0x006fe280
        if (dead(static_cast<i32>(jo::self<jo::Unit>(this)->target_building)) && !keeps_target()) jo::self<jo::Unit>(this)->target_building = jo::as_trigger(msg)->ref_id;
        if (dead(static_cast<i32>(jo::as_unit(u)->target_building)) || busy(unit(static_cast<i32>(jo::as_unit(u)->target_building))))
            jo::as_unit(u)->target_building = jo::self<jo::Unit>(this)->id;
        reset_order();
        joan::orig::as_unit(this)->apply_damage_5(joan::orig::arg::u32(static_cast<i32>(jo::as_trigger(msg)->ref_id)));  // 0x00509d90
        joan::orig::as_unit(this)->push_message(2, joan::orig::arg::i16(0), joan::orig::arg::i16(0));                              // 0x00414f50
        jo::as_actor(jo::view<u8*>(jo::self<jo::Unit>(this)->actor))->aim_mode = 0;
        const float r = joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading());  // 0x004ff850, [ebp-0x88]
        const float r_pi = r + global<float>(d::flt_3_1415927);
        joan::orig::as_unit(this)->set_target(joan::orig::arg::f32(r_pi));  // 0x004ff870
        const float q = r / global<float>(d::flt_0_017453292);
        const float deg = q + global<float>(d::flt_180);
        joan::orig::as_unit(this)->set_heading(joan::orig::arg::f32(deg));  // 0x0050abd0
        u32 ang = 0;  // [ebp-0x8c]
        joan::orig::as_angle(&ang)->construct();  // 0x00576120
        joan::orig::as_angle(&ang)->set(joan::orig::arg::f32(joan::orig::arg::as<float>(joan::orig::as_ground_object(u)->heading())));  // 0x00576240
        Vec3 pa{0.0f, 0.0f, 0.0f};  // [ebp-0x98]
        Vec3 pb{0.0f, 0.0f, 0.0f};  // [ebp-0xa4]
        joan::orig::as_unit(this)->position(&pa);  // 0x004fedb0
        joan::orig::as_unit(u)->position(&pb);
        const float cx = joan::orig::arg::as<float>(joan::orig::as_angle(&ang)->cos()) * global<float>(d::flt_60);  // 0x00576ac0 * 60
        pa.x = cx + pb.x;
        const float cy = joan::orig::arg::as<float>(joan::orig::as_angle(&ang)->sin()) * global<float>(d::flt_60);  // 0x005768d0 * 60
        pa.y = cy + pb.y;
        joan::orig::as_ground_object(this)->set_position(pa);  // 0x004fedf0
        anim(0x3b);
        if (jo::self<jo::Unit>(this)->orders != nullptr && jo::view<u32>(jo::as_order(jo::view<u8*>(jo::self<jo::Unit>(this)->orders))->current.kind) != 0) jo::view<u32>(jo::as_order(jo::view<u8*>(jo::self<jo::Unit>(this)->orders))->current.kind) = 0;
        joan::orig::as_unit(this)->use_spiritual_item_4fd980(1);  // 0x004fd980
        if (!busy(this))
            unit(static_cast<i32>(jo::as_unit(u)->target_building));  // result unused ([ebp-0xa8])
        else
            joan::orig::as_unit(this)->die_7c6240(static_cast<i32>(jo::as_trigger(msg)->ref_id));  // 0x007c6240
        return true;
    }

    void health_speech(img::Addr suffix) {
        Text s;
        joan::orig::as_string(&s)->construct();  // 0x005a1170
        const char* const name = joan::orig::arg::as<const char*>(joan::orig::as_player(kPlayer)->name_tag(joan::orig::arg::u32(me())));  // 0x00453da0 (x64: a pointer)
        if (name == nullptr) return;
        text_set(&s, str(d::s_RTS));  // "RTS"
        text_add(&s, name);
        text_add(&s, str(suffix));
        joan::orig::as_sound(kSfx)->play_kind(c_str(&s));  // 0x00492c40
    }

    u8 on_message(u32 arg) {
        c_message.hit();
        u8* const msg = joan::orig::arg::as<u8*>(joan::orig::as_list(obj(d::g_a5ccd0))->at_485f20(joan::orig::arg::u32(static_cast<i32>(static_cast<i16>(arg)))));  // 0x00485f20
        if (joan::orig::arg::as<i8>(joan::orig::as_game(this)->byte_2e1()) != 0 && jo::view<i32>(jo::as_trigger(msg)->flags) != 0x200 && jo::view<i8>(jo::self<jo::Unit>(this)->prisoner_attackable) == 0) return 0;
        if (joan::orig::arg::as<i32>(joan::orig::as_world(this)->flag_1fd()) != 0) return 0;  // 0x0042d690
        const i32 pct1 = health_pct();  // [ebp-0x10]
        const i32 type = jo::view<i32>(jo::as_trigger(msg)->flags);
        if (type > 0x200) {
            if (type == 0x400) {
                if (msg_stun(msg)) return 2;
            } else if (type == 0x2000) {
                msg_shot(msg);
            } else if (type == 0x4000) {
                msg_4000(msg);
            }
        } else if (type == 0x200) {
            if (jo::view<i32>(jo::as_trigger(msg)->ref_kind) == 1 && joan::orig::arg::as<i32>(joan::orig::as_unit(this)->can_face_505820(joan::orig::arg::u32(static_cast<i32>(jo::as_trigger(msg)->ref_id)))) != 0) {  // 0x00505820
                u8* const u = unit(static_cast<i32>(jo::as_trigger(msg)->ref_id));
                jo::as_unit(u)->hit_by = jo::self<jo::Unit>(this)->id;
            }
        } else {
            const u32 k = static_cast<u32>(type) - 1u;  // index table 0x0075ef44
            if (k == 0) {
                msg_hit(msg);
            } else if (k == 1 || k == 0x3f) {
                msg_shot(msg);
            } else if (k == 0xf) {
                if (msg_grab(msg)) return 2;
            }
        }
        const i32 pct2 = health_pct();  // [ebp-0xb0]
        if (jo::self<jo::Unit>(this)->rank >= 4) {
            if (pct1 > 0x19 && pct2 < 0x19 && pct2 != 0)
                health_speech(d::s_010);  // "010"
            else if (pct1 > 0x32 && pct2 < 0x32 && pct2 != 0)
                health_speech(d::s_009);  // "009"
        }
        return 1;
    }
};

#define B110(name, addr, fn) JOAN_REPLACE("game/b110_15", name, addr, fn)
B110("Unit::on_message", 0x0075d540, &H::on_message);
#undef B110

}  // namespace game_b110_15
}  // namespace

// ---------------------------------------- from modules/game_b108_19.cpp (game/b108_19)
namespace {
namespace game_b108_19 {

using joan::orig::helpers::addr_of;

JOAN_COUNTER(c_seq, "Seq::step_75f180");

// ------------------------------------------------------------------ thiscall functions
struct H {
    // 0x0075f180
    std::uintptr_t seq_step() {
        c_seq.hit();
        if (jo::self<jo::SaveLoad>(this)->started == 0) {
            jo::view<u8>(jo::self<jo::SaveLoad>(this)->started) = 1;
            jo::self<jo::SaveLoad>(this)->state_time = global<u32>(d::g_game_time_ms);
            return addr_of(this) & 0xffffff00u;
        }
        void* const snd = obj(d::g_game_a50088);
        if (jo::self<jo::SaveLoad>(this)->state == 0) {
            return thiscall<u32>(fn::Game_set_speed, snd, u32{0}) & 0xffffff00u;  // 0x0042f930
        }
        if (jo::self<jo::SaveLoad>(this)->state == 4) {
            if (jo::view<i8>(jo::self<jo::SaveLoad>(this)->mode) != 2) return addr_of(this);
            const u32 diff = global<u32>(d::g_game_time_ms) - jo::self<jo::SaveLoad>(this)->state_time;
            if (diff < 0x1388) return diff;
            jo::view<u8>(jo::self<jo::SaveLoad>(this)->state) = 7;
            const u32 now = global<u32>(d::g_game_time_ms);
            jo::self<jo::SaveLoad>(this)->state_time = now;
            return now;
        }
        if (jo::self<jo::SaveLoad>(this)->state == 5 && jo::view<u32>(jo::self<jo::SaveLoad>(this)->pct) == 0) {
            const u8 m = jo::self<jo::SaveLoad>(this)->mode;
            if (m == 0) {
                joan::orig::as_battle(obj(d::g_scenarios))->start_scenario();  // 0x0041c810
                jo::view<u8>(jo::self<jo::SaveLoad>(this)->state) = 6;
                jo::self<jo::SaveLoad>(this)->state_time = global<u32>(d::g_game_time_ms);
            } else if (m == 1) {
                joan::orig::as_battle(obj(d::g_scenarios))->start_41ea60(joan::orig::arg::mptr(jo::self<jo::SaveLoad>(this)->save));  // 0x0041ea60
                jo::view<u8>(jo::self<jo::SaveLoad>(this)->state) = 6;
                jo::self<jo::SaveLoad>(this)->state_time = global<u32>(d::g_game_time_ms);
            }
            return thiscall<u32>(fn::Game_set_speed, snd, u32{0}) & 0xffffff00u;
        }
        const i32 st = jo::self<jo::SaveLoad>(this)->state;
        if (st == 8) {
            joan::orig::as_game(snd)->set_state(joan::orig::arg::u32(i32{9}));  // 0x004d7e70
            jo::view<u8>(jo::self<jo::SaveLoad>(this)->started) = 0;
            jo::view<u8>(jo::self<jo::SaveLoad>(this)->state) = 0;
            jo::self<jo::SaveLoad>(this)->mode = 0;
            const u32 r = thiscall<u32>(fn::Game_set_speed, snd, u32{0x3f800000});
            jo::self<jo::SaveLoad>(this)->u8_8e = 0;
            return (r & 0xffffff00u) | 1u;
        }
        return static_cast<u32>(st) & 0xffffff00u;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_19", name, addr, fn)
B("Seq::step_75f180", 0x0075f180, &H::seq_step);
#undef B

}  // namespace game_b108_19
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

struct H {
    // ------------------------------------------------------------------ empty stubs
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Obj::nop_75f380", 0x0075f380, &H::nop);
B("Obj::nop_75f3a0", 0x0075f3a0, &H::nop);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- from modules/game_world4.cpp (game/world4)
namespace {
namespace game_world4 {

JOAN_COUNTER(c_progress, "Progress::update");

struct H {
    // 0x0075f3c0 Progress::update: +0x6d = clamp(+0x69 base + the stage (+0x79 of +0x75) and
    // sub-stage (+0x81 of +0x7d) shares of the rest, 0, 100).
    void progress_update() {
        c_progress.hit();
        if (jo::self<jo::SaveLoad>(this)->stages == 0) {
            jo::self<jo::SaveLoad>(this)->pct = jo::self<jo::SaveLoad>(this)->base_pct;
            return;
        }
        const std::int32_t rest = static_cast<std::int32_t>(100u - jo::view<std::uint32_t>(jo::self<jo::SaveLoad>(this)->base_pct));
        auto mul = [](std::int32_t a, std::int32_t b) {
            return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(b));
        };
        if (jo::self<jo::SaveLoad>(this)->substages != 0) {
            const std::int32_t per_stage = rest / jo::self<jo::SaveLoad>(this)->stages;
            const std::int32_t stage = mul(rest, jo::self<jo::SaveLoad>(this)->stage - 1) / jo::self<jo::SaveLoad>(this)->stages;
            const std::int32_t sub = mul(per_stage, jo::self<jo::SaveLoad>(this)->substage - 1) / jo::self<jo::SaveLoad>(this)->substages;
            jo::self<jo::SaveLoad>(this)->pct = static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::self<jo::SaveLoad>(this)->base_pct) + static_cast<std::uint32_t>(stage) +
                                                              static_cast<std::uint32_t>(sub));
        } else {
            const std::int32_t stage = mul(rest, jo::self<jo::SaveLoad>(this)->stage - 1) / jo::self<jo::SaveLoad>(this)->stages;
            jo::self<jo::SaveLoad>(this)->pct = static_cast<std::int32_t>(static_cast<std::uint32_t>(jo::self<jo::SaveLoad>(this)->base_pct) + static_cast<std::uint32_t>(stage));
        }
        jo::self<jo::SaveLoad>(this)->pct = joan::orig::arg::as<std::int32_t>(joan::orig::as_misc(img::obj(d::g_misc))->limit_int(joan::orig::arg::i32(jo::self<jo::SaveLoad>(this)->pct), 0, 100));  // 0x00590aa0 clamp
    }
};

#define W4(name, addr, fn) JOAN_REPLACE("game/world4", name, addr, fn)
W4("Progress::update", 0x0075f3c0, &H::progress_update);
#undef W4

}  // namespace game_world4
}  // namespace

// ---------------------------------------- from modules/game_leaves80a.cpp (game/leaves80a)
namespace {
namespace game_leaves80a {

constexpr img::Addr kGame(d::g_game_a50088);

JOAN_COUNTER(c_redraw, "Progress::redraw");

struct H {
    void redraw() {  // 0x0075f4f0
        c_redraw.hit();
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_game_mode)) != 12) return;
        joan::orig::as_game(img::obj(kGame))->draw();  // 0x004d48f0 Game::draw
        joan::orig::as_game(img::obj(kGame))->flush_loader();  // 0x00603af0
        if (jo::self<jo::SaveLoad>(this)->pct == 100) joan::orig::as_game(img::obj(kGame))->draw();  // 0x004d48f0
    }
};

#define L(name, addr, fn) JOAN_REPLACE("game/leaves80a", name, addr, fn)
L("Progress::redraw", 0x0075f4f0, &H::redraw);
#undef L

}  // namespace game_leaves80a
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Progress::load_images() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b101_16::H::load_images_755e30>(this)); }  // 0x755e30
auto joan::orig::Progress::free_images() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::release_756190>(this)); }  // 0x756190
auto joan::orig::Progress::set_image(GameFile* a0) -> void { joan::bind::member<&::game_b90_06::H::set_90>(this, a0); }  // 0x7563f0
auto joan::orig::Progress::draw() -> void { joan::bind::member<&::game_b110_01::H::draw>(this); }  // 0x756430
auto joan::orig::Unit::on_message_75d540(std::uint32_t a0) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b110_15::H::on_message>(this, a0)); }  // 0x75d540
auto joan::orig::Progress::step() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_19::H::seq_step>(this)); }  // 0x75f180
auto joan::orig::Progress::update() -> void { joan::bind::member<&::game_world4::H::progress_update>(this); }  // 0x75f3c0
auto joan::orig::Progress::redraw() -> void { joan::bind::member<&::game_leaves80a::H::redraw>(this); }  // 0x75f4f0
