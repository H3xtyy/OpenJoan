// game/unknown_crithud.cpp: functions whose original source file is unknown (formerly game/_unplaced/crithud.cpp;
// re/attribution.csv: between ww/enlink/cpuusage.cpp and ww/ditemres.cpp). They move to their original file when
// evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstddef>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../runtime/replay/replay.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u8 = std::uint8_t;

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::hook::global;

using i32 = std::int32_t;

using joan::img::obj;


using u16 = std::uint16_t;

using i16 = std::int16_t;

using i8 = std::int8_t;
}  // namespace

// ---------------------------------------- from modules/game_b100_02.cpp (game/b100_02)
namespace {
namespace game_b100_02 {


struct H {

    // ------------------------------------------------------------------ 0x0045f7a0
    void* ctor_45f7a0() {
        joan::orig::dynarray(&jo::self<jo::CriticalHud>(this)->slots)->construct(2, 0xa, 0);  // 0x005843d0
        std::memset(this, 0, offsetof(jo::CriticalHud, slots));  // 0x39 bytes on x86
        return this;
    }
};

#define B100(name, addr, fn) JOAN_REPLACE("game/b100_02", name, addr, fn)
B100("Obj::ctor_45f7a0", 0x0045f7a0, &H::ctor_45f7a0);
#undef B100

}  // namespace game_b100_02
}  // namespace

// ---------------------------------------- from modules/game_b99_02.cpp (game/b99_02)
namespace {
namespace game_b99_02 {

struct H {

    // ------------------------------------------------------------------ 0x0045f7f0
    u32 dtor_45f7f0() {
        joan::orig::as_critical_hud(this)->release();  // 0x0045fa30
        joan::orig::as_dynarray(&jo::self<jo::CriticalHud>(this)->slots)->destruct_5844b0();  // 0x005844b0
        return 0;  // only caller: the atexit stub 0x0056b260 (result discarded); ~DynArray returns nothing
    }
};

#define B99(name, addr, fn) JOAN_REPLACE("game/b99_02", name, addr, fn)
B99("Obj::~Obj_45f7f0", 0x0045f7f0, &H::dtor_45f7f0);
#undef B99

}  // namespace game_b99_02
}  // namespace

// ---------------------------------------- from modules/game_b90_03.cpp (game/b90_03)
namespace {
namespace game_b90_03 {

JOAN_COUNTER(c_setup_slots, "Obj::setup_slots");

struct H {

    // 0x0045f860
    void setup_slots(std::int8_t fixed) {
        c_setup_slots.hit();
        jo::view<std::uint8_t>(jo::self<jo::CriticalHud>(this)->enabled) = 1;
        if (fixed != 0) {
            jo::self<jo::CriticalHud>(this)->sheet_a = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_critical));
            jo::self<jo::CriticalHud>(this)->sheet_b = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_criti_f));
            jo::self<jo::CriticalHud>(this)->sheet_c = static_cast<jo::BmpRes*>(img::obj(d::g_bmp_res_criti_k));
            return;
        }
        joan::orig::dynarray(&jo::self<jo::CriticalHud>(this)->slots)->zap(1);  // 0x00584580
        void* const u = joan::orig::as_dynarray(obj(d::g_units))->ptr_at(joan::orig::arg::i32(global<std::uint32_t>(d::g_player_unit_recno)));  // 0x0041b3c0
        void* const list = jo::as_unit(u)->anim_set;
        if (jo::as_unit(u)->anim_set != nullptr) {
            for (std::int16_t i = 1; static_cast<std::int32_t>(i) <= jo::as_anim_set(list)->group_count;
                 i = static_cast<std::int16_t>(i + 1)) {
                const std::uint8_t* const e =
                    joan::orig::arg::as<std::uint8_t*>(joan::orig::as_anim_set(list)->entry(joan::orig::arg::u32(static_cast<std::int32_t>(i))));  // 0x004168d0
                if (jo::as_anim_group_rec(e)->is_jump != 0) continue;
                if (jo::view<std::uint32_t>(jo::as_anim_group_rec(e)->kind) != 0x1c) continue;
                if (static_cast<std::int8_t>(jo::as_anim_group_rec(e)->name_8[0]) == 0) continue;
                bool ok = true;
                std::int32_t j = 0;
                for (; j < 6; ++j) {
                    const std::int32_t c = static_cast<std::int8_t>(jo::as_anim_group_rec(e)->name_8[j]);
                    if (c == 0x43 || c == 0x44) ok = false;
                    if (c == 0 || c == 0x20) break;
                }
                if (!ok) continue;
                if (j == 1) continue;
                joan::orig::dynarray(&jo::self<jo::CriticalHud>(this)->slots)->linkin(&i);  // 0x00584620 (element size 2)
            }
        }
        jo::view<std::uint8_t>(jo::self<jo::CriticalHud>(this)->combo_active) = 0;
        jo::view<std::uint8_t>(jo::self<jo::CriticalHud>(this)->combo_success) = 0;
        jo::self<jo::CriticalHud>(this)->u8_1c = 1;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_03", name, addr, fn)
M("Obj::setup_slots", 0x0045f860, &H::setup_slots);
#undef M

}  // namespace game_b90_03
}  // namespace

// ---------------------------------------- from modules/game_b92_02.cpp (game/b92_02)
namespace {
namespace game_b92_02 {

struct H {

    // 0x0045fa30
    std::uint32_t buffers_release() {
        if (jo::self<jo::CriticalHud>(this)->tex_critical != nullptr) {
            joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(jo::self<jo::CriticalHud>(this)->tex_critical));  // 0x0060a550
            jo::self<jo::CriticalHud>(this)->tex_critical = nullptr;
        }
        if (jo::self<jo::CriticalHud>(this)->tex_criti_k != nullptr) {
            joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(jo::self<jo::CriticalHud>(this)->tex_criti_k));  // 0x0060a550
            jo::self<jo::CriticalHud>(this)->tex_criti_k = nullptr;
        }
        if (jo::self<jo::CriticalHud>(this)->tex_4 != nullptr) {
            joan::orig::as_texture_manager(obj(d::g_texture_array))->release(joan::orig::arg::mptr(jo::self<jo::CriticalHud>(this)->tex_4));  // 0x0060a550
            jo::self<jo::CriticalHud>(this)->tex_4 = nullptr;
        }
        joan::orig::as_dynarray(&jo::self<jo::CriticalHud>(this)->slots)->zap_584580(1);  // 0x00584580
        return 0;  // callers (0x0045f7f0, Game::reset_all 0x00470630) discard the result; zap returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_02", name, addr, fn)
B("Buffers::release", 0x0045fa30, &H::buffers_release);
#undef B

}  // namespace game_b92_02
}  // namespace

// ---------------------------------------- from modules/game_b110_10.cpp (game/b110_10)
namespace {
namespace game_b110_10 {

using i16 = std::int16_t;

using joan::img::str;

using i8 = std::int8_t;

using joan::hook::x87::Fpu;

using joan::hook::x87::ftol;

using joan::orig::helpers::at;
using joan::orig::helpers::add32;
i32 sub32(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) - static_cast<u32>(b)); }
// x87 fcomp + test ah,1 taken: a < b or unordered.
bool lt(double a, double b) { return !(a >= b); }

void* const kSprites = obj(d::g_sprites_a46128);
void* const kTextures = obj(d::g_texture_array);

JOAN_COUNTER(c_crit_draw, "CriticalHud::draw_45fae0");

// 0x0042a260 colour(r, g, b, a) on a local 4-byte object, packed dword read back.
u32 make_colour(u32 r, u32 g, u32 b, u32 a) {
    u32 tmp = 0;
    return *joan::orig::arg::as<u32*>(joan::orig::as_colour(&tmp)->construct(r, g, b, a));  // 0x0042a260
}
// 0x00633500 (ret 0x44) sprite queue entry.
void sprite(i32 x, i32 y, i32 layer, std::uintptr_t name, void* sheet, u32 colour, i32 a8, i32 a9, i32 a10,
            i32 x2, i32 y2) {
    joan::orig::as_sprite_queue(kSprites)->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(layer), joan::orig::arg::str(str(name)), joan::orig::arg::mptr(sheet), joan::orig::arg::u32(colour), 3, joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, joan::orig::arg::i16(x2), joan::orig::arg::i16(y2));  // 0x00633500
}
// 0x00620b40 frame lookup by name on a sprite sheet.
void* frame(void* sheet, std::uintptr_t name) { return joan::orig::arg::as<void*>(joan::orig::as_bmp_res(sheet)->find(joan::orig::arg::str(str(name)))); }
// cdq; sub eax,edx; sar eax,1
i32 half(i32 v) { return v / 2; }

struct Section {
    std::int8_t jo::CriticalHud::*flag;  // byte flag of this section
    jo::BmpRes* jo::CriticalHud::*sheet;  // sprite sheet pointer field
    std::uintptr_t name;  // main sprite
    std::uintptr_t g1, g2, g3;
    i32 g2_layer;  // layer argument of the g2 sprite
    i32 dy;        // subtracted from the screen centre y
};

struct H {
    template <class T>
    T& f(i32 off) {
        return at<T>(this, off);
    }

    // ------------------------------------------------------------------------------ 0x0045fae0
    void* sheet(const Section& s) { return jo::self<jo::CriticalHud>(this)->*s.sheet; }

    // Glow sprite: centred on (cx, cy) by the frame's size, alpha from the fade.
    void glow_sprite(const Section& s, std::uintptr_t name, i32 layer, i32 alpha, i32 cx, i32 cy) {
        const i32 x = sub32(cx, half(static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(frame(sheet(s), name))->width))));
        const i32 y = sub32(cy, half(static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(frame(sheet(s), name))->height))));
        const u32 colour = make_colour(0xff, 0xff, 0xff, static_cast<u32>(alpha));
        sprite(x, y, layer, name, sheet(s), colour, 0, 2, 0, -1, -1);
    }

    void section(const Section& s) {
        const Fpu fpu{true};
        const double dt = fpu.sub(static_cast<double>(global<u32>(d::g_now)), jo::self<jo::CriticalHud>(this)->success_time);
        const i32 w = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(frame(sheet(s), s.name))->width));
        const i32 h = static_cast<i32>(jo::view<i16>(jo::as_bmp_entry(frame(sheet(s), s.name))->height));
        const i32 cx = half(global<i32>(d::g_screen_w));
        const i32 cy = sub32(half(global<i32>(d::g_screen_h)), s.dy);
        auto zoom_in = [&]() -> double {  // dt / A clamped to 1.0 (qword)
            const double q = fpu.div(dt, static_cast<double>(global<i32>(d::g_critical_fade_in1)));
            return lt(q, 1.0) ? q : 1.0;
        };
        // fild c; fld r; fmul 1.1; fimul size; fdiv 2.0; fsubp / faddp
        auto zoom_edge = [&](i32 c, i32 size, bool plus) -> i32 {
            const double t = fpu.div(fpu.mul(fpu.mul(zoom_in(), 1.1), static_cast<double>(size)), 2.0);
            return ftol(plus ? fpu.add(static_cast<double>(c), t) : fpu.sub(static_cast<double>(c), t));
        };
        if (!lt(static_cast<double>(global<i32>(d::g_critical_fade_in1)), dt)) {
            const i32 x0 = zoom_edge(cx, w, false);
            const i32 y0 = zoom_edge(cy, h, false);
            const i32 x1 = zoom_edge(cx, w, true);
            const i32 y1 = zoom_edge(cy, h, true);
            const i32 alpha = ftol(fpu.mul(255.0, zoom_in()));
            const u32 colour = make_colour(0xff, 0xff, 0xff, static_cast<u32>(alpha));
            sprite(x0, y0, 0xa, s.name, sheet(s), colour, 0, 2, 1, x1, y1);
        } else if (!lt(static_cast<double>(add32(global<i32>(d::g_critical_fade_in1), global<i32>(d::g_a3ccf8))), dt)) {
            const float g = static_cast<float>(fpu.sub(dt, static_cast<double>(global<i32>(d::g_critical_fade_in1))));
            const double q = fpu.div(static_cast<double>(g), static_cast<double>(global<i32>(d::g_a3ccf8)));
            const float r = lt(q, 1.0) ? static_cast<float>(q) : 1.0f;
            const float k = static_cast<float>(fpu.sub(1.1, fpu.div(static_cast<double>(r), 10.0)));
            // fild c; fild size; fmul k; fdiv 2.0f; fsubp / faddp
            auto edge = [&](i32 c, i32 size, bool plus) -> i32 {
                const double t = fpu.div(fpu.mul(static_cast<double>(size), static_cast<double>(k)), 2.0);
                return ftol(plus ? fpu.add(static_cast<double>(c), t) : fpu.sub(static_cast<double>(c), t));
            };
            const i32 x0 = edge(cx, w, false);
            const i32 y0 = edge(cy, h, false);
            const i32 x1 = edge(cx, w, true);
            const i32 y1 = edge(cy, h, true);
            const u32 colour = make_colour(0xff, 0xff, 0xff, 0xff);
            sprite(x0, y0, 0xa, s.name, sheet(s), colour, 0, 2, 1, x1, y1);
        } else {
            const i32 x0 = sub32(cx, half(w));
            const i32 y0 = sub32(cy, half(h));
            const u32 colour = make_colour(0xff, 0xff, 0xff, 0xff);
            sprite(x0, y0, 0xa, s.name, sheet(s), colour, 0, 0, 0, -1, -1);
        }

        // Glow fade steps.
        const float e0 = static_cast<float>(add32(global<i32>(d::g_critical_fade_in1), global<i32>(d::g_a3ccf8)));
        const float e1 = static_cast<float>(fpu.add(static_cast<double>(global<i32>(d::g_a3cbfc)), e0));
        const float e2 = static_cast<float>(fpu.add(static_cast<double>(global<i32>(d::g_critical_glow_in2)), e1));
        // fild step; fdivr g; clamp to 1.0f (stored float)
        auto ratio = [&](float g, img::Addr step) -> float {
            const double q = fpu.div(static_cast<double>(g), static_cast<double>(global<i32>(step)));
            return lt(q, 1.0) ? static_cast<float>(q) : 1.0f;
        };
        auto fade_in = [&](float r) { return ftol(fpu.mul(static_cast<double>(r), 255.0)); };
        auto fade_out = [&](float r) { return ftol(fpu.mul(fpu.sub(1.0, static_cast<double>(r)), 255.0)); };
        auto in_step = [&](float start, img::Addr step) {
            return lt(start, dt) &&
                   !lt(fpu.add(static_cast<double>(global<i32>(step)), static_cast<double>(start)), dt);
        };
        if (in_step(e0, d::g_a3cbfc)) {
            const float g = static_cast<float>(fpu.sub(dt, e0));
            const i32 a = fade_in(ratio(g, d::g_a3cbfc));
            glow_sprite(s, s.g1, 0xa, a, cx, cy);
        } else if (in_step(e1, d::g_critical_glow_in2)) {
            const float g = static_cast<float>(fpu.sub(dt, e1));
            const i32 a = fade_out(ratio(g, d::g_critical_glow_in2));
            glow_sprite(s, s.g1, 0xa, a, cx, cy);
            const i32 b = fade_in(ratio(g, d::g_critical_glow_in2));
            glow_sprite(s, s.g2, s.g2_layer, b, cx, cy);
        } else if (in_step(e2, d::g_critical_glow_in3)) {
            const float g = static_cast<float>(fpu.sub(dt, e2));
            const i32 a = fade_out(ratio(g, d::g_critical_glow_in3));
            glow_sprite(s, s.g2, s.g2_layer, a, cx, cy);
            const i32 b = fade_in(ratio(g, d::g_critical_glow_in3));
            glow_sprite(s, s.g3, 0xa, b, cx, cy);
        } else if (lt(fpu.add(static_cast<double>(global<i32>(d::g_critical_glow_in3)), static_cast<double>(e2)), dt)) {
            jo::view<u8>(jo::self<jo::CriticalHud>(this)->*s.flag) = 0;
        }
    }

    // LIGHT / HEAVY style marker i (0x41 / 0x42 at +0x1f + i).
    void marker(i32 i, img::Addr light, img::Addr heavy) {
        const i8 c = jo::self<jo::CriticalHud>(this)->keys[i];
        std::uintptr_t name = 0;
        if (c == 0x41) {
            name = img::addr(light);
        } else if (c == 0x42) {
            name = img::addr(heavy);
        } else {
            return;
        }
        const u32 colour = make_colour(0xff, 0xff, 0xff, 0xff);
        sprite(add32(i * 0x2d, 0x12), sub32(global<i32>(d::g_screen_h), 0xa1), 0xa, name, jo::view<void*>(jo::self<jo::CriticalHud>(this)->sheet_a), colour, 0, 0,
               0, -1, -1);
    }

    void crit_draw() {
        if (jo::self<jo::CriticalHud>(this)->enabled == 0) return;
        c_crit_draw.hit();
        const u32 saved = joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kTextures)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kTextures)->set_mode(joan::orig::arg::u32(i32{1}));              // 0x0060a740
        if (jo::self<jo::CriticalHud>(this)->tex_critical == nullptr) jo::self<jo::CriticalHud>(this)->tex_critical = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(kTextures)->load(str(d::s_CRITICAL), i32{1}, joan::orig::arg::u32(i32{0})));  // 0x006099c0 CRITICAL
        if (jo::self<jo::CriticalHud>(this)->tex_criti_k == nullptr) jo::self<jo::CriticalHud>(this)->tex_criti_k = joan::orig::arg::as<jo::Texture*>(joan::orig::as_texture_manager(kTextures)->load(str(d::s_CRITI_K), i32{1}, joan::orig::arg::u32(i32{0})));  // CRITI_K
        joan::orig::as_texture_manager(kTextures)->set_mode(joan::orig::arg::u32(saved));
        {
            const Fpu fpu{true};
            const double until = fpu.add(static_cast<double>(global<i32>(d::g_a33750)), jo::self<jo::CriticalHud>(this)->start_time);
            if (!lt(static_cast<double>(global<u32>(d::g_now)), until)) jo::view<u8>(jo::self<jo::CriticalHud>(this)->combo_active) = 0;
        }
        if (jo::self<jo::CriticalHud>(this)->combo_active != 0) {
            for (i32 i = 0; i < static_cast<i32>(jo::self<jo::CriticalHud>(this)->progress); ++i) marker(i, d::s_LIGHT, d::s_HEAVY);  // LIGHT / HEAVY
            for (i32 i = static_cast<i32>(jo::self<jo::CriticalHud>(this)->progress); i < static_cast<i32>(jo::self<jo::CriticalHud>(this)->key_count); ++i)
                marker(i, d::s_LIGHTOFF, d::s_HEAVYOFF);  // LIGHTOFF / HEAVYOFF
        }
        if (jo::self<jo::CriticalHud>(this)->i8_1a != 0) {
            section(Section{&jo::CriticalHud::i8_1a, &jo::CriticalHud::sheet_b, img::addr(d::s_CF), img::addr(d::s_CFGLOW01), img::addr(d::s_CFGLOW02), img::addr(d::s_CFGLOW03), 0xa, 0});  // CFGLOW01..03
        } else if (jo::self<jo::CriticalHud>(this)->i8_1b != 0) {
            section(Section{&jo::CriticalHud::i8_1b, &jo::CriticalHud::sheet_c, img::addr(d::s_CK), img::addr(d::s_CKGLOW01), img::addr(d::s_CKGLOW02), img::addr(d::s_CKGLOW03), 0xa, 0});  // CKGLOW01..03
            jo::view<u8>(jo::self<jo::CriticalHud>(this)->combo_success) = 0;
        } else if (jo::self<jo::CriticalHud>(this)->combo_success != 0) {
            section(Section{&jo::CriticalHud::combo_success, &jo::CriticalHud::sheet_a, img::addr(d::s_CA), img::addr(d::s_GLOW01), img::addr(d::s_GLOW02), img::addr(d::s_GLOW03), 2, 0x64});  // GLOW01..03
        }
    }
};
static_assert(sizeof(i16) == 2 && sizeof(double) == 8 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b110_10", name, addr, fn)
B("CriticalHud::draw_45fae0", 0x0045fae0, &H::crit_draw);
#undef B

}  // namespace game_b110_10
}  // namespace

// ---------------------------------------- from modules/game_b101_01.cpp (game/b101_01)
namespace {
namespace game_b101_01 {

using joan::img::str;

using joan::orig::helpers::at;

void copy_dwords(u32* dst, const u32* src, i32 n) {  // rep movsd
    for (i32 i = 0; i < n; ++i) dst[i] = src[i];
}

JOAN_COUNTER(c_record_copy, "Record::copy_462610");

struct H {
    // ------------------------------------------------------------------ 0x00462610
    void record_copy_462610(const u8* s) {
        c_record_copy.hit();
        jo::self<jo::UnitSnapshot>(this)->unit_id = jo::view<u16>(jo::as_unit(s)->type_id);
        jo::self<jo::UnitSnapshot>(this)->layer = jo::view<u16>(jo::as_unit(s)->layer);
        // the unit's stats block (UnitStats, 0x3b bytes): 14 dwords, a word, a byte
        copy_dwords(jo::self<jo::UnitSnapshot>(this)->block, reinterpret_cast<const u32*>(&jo::as_unit(s)->stats_block), 0xe);
        jo::self<jo::UnitSnapshot>(this)->u16_3c = jo::as_unit(s)->stats_block.u16_38;
        jo::self<jo::UnitSnapshot>(this)->u8_3e = jo::as_unit(s)->stats_block.gap_3a[0];
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::UnitSnapshot>(this)->name)), joan::orig::arg::str(jo::as_unit(s)->name_));  // strcpy
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(jo::self<jo::UnitSnapshot>(this)->name2)), joan::orig::arg::str(jo::as_unit(s)->name_c));
        jo::self<jo::UnitSnapshot>(this)->rank = jo::view<u32>(jo::as_unit(s)->rank);
        jo::self<jo::UnitSnapshot>(this)->kingdom_id = jo::view<u16>(jo::as_unit(s)->kingdom_id);
        jo::self<jo::UnitSnapshot>(this)->race_id = jo::view<u16>(jo::as_unit(s)->race_id);
        jo::self<jo::UnitSnapshot>(this)->level = jo::view<u16>(jo::as_unit(s)->level);
        jo::self<jo::UnitSnapshot>(this)->u16_10e = jo::view<u16>(jo::as_unit(s)->i16_12e);
        jo::self<jo::UnitSnapshot>(this)->hit_points = jo::view<u32>(jo::as_unit(s)->hit_points);
        jo::self<jo::UnitSnapshot>(this)->hit_points_base = jo::view<u32>(jo::as_unit(s)->hit_points_base);
        jo::self<jo::UnitSnapshot>(this)->regen_hp = jo::view<u32>(jo::as_unit(s)->regen_hp);
        jo::self<jo::UnitSnapshot>(this)->regen_ep = jo::view<u32>(jo::as_unit(s)->regen_ep);
        jo::self<jo::UnitSnapshot>(this)->strength = jo::view<u16>(jo::as_unit(s)->stats[0]);
        jo::self<jo::UnitSnapshot>(this)->dexterity = jo::view<u16>(jo::as_unit(s)->stats[1]);
        jo::self<jo::UnitSnapshot>(this)->experience = jo::view<u32>(jo::as_unit(s)->experience);
        jo::self<jo::UnitSnapshot>(this)->u16_128 = jo::view<u16>(jo::as_unit(s)->stats[6]);
        jo::self<jo::UnitSnapshot>(this)->u16_12a = jo::view<u16>(jo::as_unit(s)->stats[7]);
        jo::self<jo::UnitSnapshot>(this)->u16_12c = jo::view<u16>(jo::as_unit(s)->stats[8]);
        jo::self<jo::UnitSnapshot>(this)->idle_time = jo::view<u32>(jo::as_unit(s)->idle_time);
        jo::self<jo::UnitSnapshot>(this)->morale = jo::view<u16>(jo::as_unit(s)->morale);
        jo::self<jo::UnitSnapshot>(this)->u16_13c = jo::as_unit(s)->u16_1d5;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b101_01", name, addr, fn)
B("Record::copy_462610", 0x00462610, &H::record_copy_462610);
#undef B

}  // namespace game_b101_01
}  // namespace

// ---------------------------------------- from modules/game_b91_02.cpp (game/b91_02)
namespace {
namespace game_b91_02 {

using joan::hook::global;

using joan::call::thiscall;

using joan::img::obj;
using joan::img::str;

using joan::hook::global;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

void* const kUnits = obj(d::g_units);

std::uint8_t* unit_get(std::int32_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }   // 0x0041b3c0

JOAN_COUNTER(c_crit_input, "CritCombo::input");

struct H {
    // 0x00462810
    void crit_input(std::int8_t key, std::int8_t reset) {
        c_crit_input.hit();
        if (jo::self<jo::CriticalHud>(this)->enabled == 0) return;
        jo::view<std::uint8_t>(jo::as_unit(unit_get(global<std::int32_t>(d::g_player_unit_recno)))->crit_ready) = 0;
        if (jo::self<jo::CriticalHud>(this)->combo_active == 0) return;
        if (reset != 0) jo::self<jo::CriticalHud>(this)->progress = 0;
        if (jo::self<jo::CriticalHud>(this)->progress == jo::self<jo::CriticalHud>(this)->key_count || jo::self<jo::CriticalHud>(this)->combo_active == 0) return;
        if (key == jo::self<jo::CriticalHud>(this)->keys[jo::self<jo::CriticalHud>(this)->progress]) {
            jo::self<jo::CriticalHud>(this)->progress = static_cast<std::int16_t>(jo::self<jo::CriticalHud>(this)->progress + 1);
            if (jo::self<jo::CriticalHud>(this)->progress == jo::self<jo::CriticalHud>(this)->key_count) {
                joan::orig::as_sound(obj(d::g_sfx_player))->play_kind(str(d::s_CRITATT));  // 0x00492c40 "CRITATT"
                jo::self<jo::CriticalHud>(this)->done = 1;
                jo::view<std::uint8_t>(jo::self<jo::CriticalHud>(this)->combo_active) = 0;
                jo::view<std::uint8_t>(jo::self<jo::CriticalHud>(this)->combo_success) = 1;
                jo::view<std::uint8_t>(jo::as_unit(unit_get(global<std::int32_t>(d::g_player_unit_recno)))->crit_ready) = 1;
                jo::self<jo::CriticalHud>(this)->success_time = static_cast<double>(global<std::uint32_t>(d::g_now));  // fild qword (hi 0)
            }
        } else {
            jo::self<jo::CriticalHud>(this)->progress = 0;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b91_02", name, addr, fn)
B("CritCombo::input", 0x00462810, &H::crit_input);
#undef B

}  // namespace game_b91_02
}  // namespace

// ---------------------------------------- from modules/game_b95_01.cpp (game/b95_01)
namespace {
namespace game_b95_01 {

using joan::hook::global;

using joan::call::thiscall;

using joan::img::obj;
using joan::img::str;

using joan::orig::helpers::at;

void* const kUnits = obj(d::g_units);

JOAN_COUNTER(c_random_name, "Obj::pick_random_name");

u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnits)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
i32 list_count(void* list) { return joan::orig::as_dynarray(list)->size(); }  // 0x00417ad0

struct H {

    // ------------------------------------------------------------------ 0x00462970
    void pick_random_name() {
        c_random_name.hit();
        if (jo::self<jo::CriticalHud>(this)->enabled == 0) return;
        void* const list = &jo::self<jo::CriticalHud>(this)->slots;
        if (list_count(list) == 0) return;
        jo::self<jo::CriticalHud>(this)->start_time = static_cast<double>(global<u32>(d::g_now));  // fild qword (now, 0)
        i32 k = joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(list_count(list))) + 1;  // 0x00590540
        i32 key = *static_cast<const i16*>(joan::orig::arg::as<void*>(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(k))))  /* the slots hold words */;  // 0x00417200
        u8* const unit = unit_get(global<i32>(d::g_player_unit_recno));
        void* const names = jo::as_unit(unit)->anim_set;
        if (names == nullptr) return;
        u8* rec = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(names)->entry(joan::orig::arg::u32(key)));  // 0x004168d0
        while (joan::orig::arg::as<i32>(joan::orig::as_anim_set_entry(rec)->is_yes()) == 0) {     // 0x00416ed0
            k = joan::orig::as_misc(obj(d::g_misc))->random(joan::orig::arg::i32(list_count(list))) + 1;
            key = *static_cast<const i16*>(joan::orig::arg::as<void*>(joan::orig::as_dynarray(list)->get(joan::orig::arg::i32(k))))  /* the slots hold words */;
            rec = joan::orig::arg::as<u8*>(joan::orig::as_anim_set(names)->entry(joan::orig::arg::u32(key)));
        }
        char* const dst = reinterpret_cast<char*>(jo::self<jo::CriticalHud>(this)->keys);
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dst)), str(d::kEmpty));  // strcpy ""
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(dst)), joan::orig::arg::str(jo::as_anim_group_rec(rec)->name_8));  // strcpy
        jo::view<u16>(jo::self<jo::CriticalHud>(this)->key_count) = 0;
        for (i32 i = 0; i < 6; ++i) {
            const i32 c = jo::self<jo::CriticalHud>(this)->keys[i];
            if (c == 0x41 || c == 0x42) jo::view<u16>(jo::self<jo::CriticalHud>(this)->key_count) = static_cast<u16>(jo::view<u16>(jo::self<jo::CriticalHud>(this)->key_count) + 1);
            if (jo::self<jo::CriticalHud>(this)->keys[i] == 0) break;
        }
        jo::view<u16>(jo::self<jo::CriticalHud>(this)->progress) = 0;
        jo::self<jo::CriticalHud>(this)->done = 0;
        jo::view<u8>(jo::self<jo::CriticalHud>(this)->combo_active) = 1;
        jo::view<u8>(jo::self<jo::CriticalHud>(this)->combo_success) = 0;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_01", name, addr, fn)
B95("Obj::pick_random_name", 0x00462970, &H::pick_random_name);
#undef B95

}  // namespace game_b95_01
}  // namespace

// ---------------------------------------- from modules/game_b104_02.cpp (game/b104_02)
namespace {
namespace game_b104_02 {

using joan::call::thiscall;

using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x00462b30
    u32 dtor_462b30() {
        jo::self<jo::Order0c_b>(this)->vtable = img::obj(d::vt_Obj_9d3b50);
        joan::orig::as_unit(this)->dtor_43c1c0();         // 0x0043c1c0
        return joan::orig::arg::as<u32>(joan::orig::as_map_object(this)->destruct());  // 0x00415790
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_02", name, addr, fn)
B("Unit::dtor_462b30", 0x00462b30, &H::dtor_462b30);
#undef B

}  // namespace game_b104_02
}  // namespace

// ---------------------------------------- from modules/game_b108_11.cpp (game/b108_11)
namespace {
namespace game_b108_11 {

using joan::call::thiscall;

using joan::img::obj;

JOAN_COUNTER(c_draw_button, "draw_button_462b90");

// Colour ctor 0x0042a260 (thunk 0x004098b8, ret 0x10): bytes a, b, c, d; returns this.
u32 rgba(u32 a, u32 b, u32 c, u32 d) {
    u32 v = 0;
    const void* const p = joan::orig::as_colour(&v)->construct(a, b, c, d);
    return jo::view<u32>(*jo::as_colour(p));
}

// ------------------------------------------------------------------ 0x00462b90 (dead)
static u32 JOAN_CDECL draw_button(void* b, u32 a, u32 flags) {
    c_draw_button.hit();
    const u32 fl = flags & 0xffu;
    if (fl != 7u && (fl & 1u) != 0) return 0;
    i16 w4 = jo::as_button(b)->x;
    i16 w8 = jo::as_button(b)->y;
    i16 wc = jo::as_button(b)->x2;
    i16 w10 = jo::as_button(b)->y2;
    float f14 = 1.0f;
    float f18 = 1.0f;
    if (jo::as_button(b)->fit != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->fit_rect(joan::orig::arg::i16(static_cast<i32>(jo::as_button(b)->x)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(b)->y)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(b)->x2)), joan::orig::arg::i16(static_cast<i32>(jo::as_button(b)->y2)), joan::orig::arg::mptr(// 0x00633e60
                      &f14), &f18, &w4, &w8, &wc, &w10, joan::orig::arg::i16(i32{0x280}), joan::orig::arg::i16(i32{0x1e0}));
    }
    u32 color = 0;
    if (jo::as_button(b)->pressed != 0) {
        color = rgba(0xff, 0xdd, 0xbb, 0xff);
    } else if (joan::orig::arg::as<i32>(joan::orig::as_point(obj(d::g_mouse))->inside(static_cast<i32>(w4), static_cast<i32>(w8), static_cast<i32>(wc), static_cast<i32>(w10))) != 0) {  // 0x00593980
        color = rgba(0x88, 0x88, 0xcc, 0xff);
    } else {
        color = rgba(0x77, 0xff, 0x00, 0x88);
    }
    if ((fl & 2u) != 0) {
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue(static_cast<i32>(w4), static_cast<i32>(w8), joan::orig::arg::i32(// 0x00632a70
                      static_cast<i32>(wc)), static_cast<i32>(w10), joan::orig::arg::u32(a), joan::orig::arg::u32(color), joan::orig::arg::mptr(i32{0}), i32{3}, i32{0}, i32{0}, i32{0}, i32{-1}, i32{-1}, i32{-1}, i32{-1}, i32{0});
        const u32 black = rgba(0, 0, 0, 0xff);
        joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue_frame(joan::orig::arg::i16(static_cast<i32>(w4)), joan::orig::arg::i16(static_cast<i32>(w8)), joan::orig::arg::i16(// 0x00633070
                      static_cast<i32>(wc)), joan::orig::arg::i16(static_cast<i32>(w10)), joan::orig::arg::u32(a), joan::orig::arg::u32(black), joan::orig::arg::i16(i32{1}), nullptr, joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{0}), joan::orig::arg::i16(i32{-1}), joan::orig::arg::i16(i32{-1}), joan::orig::arg::i16(i32{-1}), joan::orig::arg::i16(i32{-1}), joan::orig::arg::i16(i32{0}));
    }
    if (jo::view<i8>(jo::as_button(b)->caption[0]) != 0 && (fl & 4u) != 0) {
        i32 dy = 0;
        if (jo::as_button(b)->pressed != 0) {
            const float f = 1.0f * f18;
            dy = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(f)))));  // 0x00590a20
        }
        i32 dx = 0;
        if (jo::as_button(b)->pressed != 0) {
            const float f = 1.0f * f14;
            dx = static_cast<i32>(joan::orig::arg::as<i16>(joan::orig::as_misc(obj(d::g_misc))->round(joan::orig::arg::f64(static_cast<double>(f)))));
        }
        u32 c = 0;
        const void* const cp = joan::orig::arg::as<void*>(joan::orig::as_color(&c)->Color_dword(joan::orig::arg::u32(u32{0xff003366})));  // 0x005e9640
        const u32 col = jo::view<u32>(*jo::as_colour(cp));
        joan::orig::as_text(obj(d::g_font_verdana_12))->draw_3(joan::orig::arg::i32(static_cast<i32>(w4) + dx), joan::orig::arg::i32(static_cast<i32>(w8) + dy), joan::orig::arg::i32(// 0x00626130
                      static_cast<i32>(wc)), static_cast<i32>(w10), joan::orig::arg::u32(a), joan::orig::arg::str(jo::as_button(b)->caption), joan::orig::arg::u32(col), i32{0}, i32{1}, i32{2}, i32{0}, 1.0f, 1.0f);
    }
    return 1;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b108_11", name, addr, fn)
B("draw_button_462b90", 0x00462b90, &draw_button);
#undef B

}  // namespace game_b108_11
}  // namespace

// ---------------------------------------- from modules/game_b102_01.cpp (game/b102_01)
namespace {
namespace game_b102_01 {

using joan::call::thiscall;

using joan::img::obj;

u8* add(const void* p, std::int32_t off) { return static_cast<u8*>(const_cast<void*>(p)) + off; }

JOAN_COUNTER(c_rotate_462e90, "Queue::rotate_462e90");

struct H {
    // ---------------------------------------------------------------- 0x00462e90
    u32 rotate_462e90(u32) {
        c_rotate_462e90.hit();
        u8 first[0x16];
        // Queue+0x7a entries, 0x16 bytes each, at raw x86 offsets: dead code (no caller)
        std::memcpy(first, add(this, 0x7a), 0x16);
        i32 i = 1;
        for (; i < static_cast<i32>(jo::self<jo::Queue>(this)->i8_1); ++i)
            std::memcpy(add(this, 0x7a + (i - 1) * 0x16), add(this, 0x7a + i * 0x16), 0x16);
        std::memcpy(add(this, 0x7a + (i - 1) * 0x16), first, 0x16);
        joan::orig::as_canvas(this)->callback_597350(joan::orig::as_canvas(obj(d::g_vga_back)), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Queue>(this)->i8_1) - 1));           // 0x00597350
        joan::orig::as_canvas(this)->callback_597250(joan::orig::as_canvas(obj(d::g_vga_back)), joan::orig::arg::u32(static_cast<i32>(jo::self<jo::Queue>(this)->i8_1) - 1));  // 0x00597250
        return 0;  // no caller of 0x00462e90 (no call or address reference); 0x00597250 returns nothing
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b102_01", name, addr, fn)
B("Queue::rotate_462e90", 0x00462e90, &H::rotate_462e90);
#undef B

}  // namespace game_b102_01
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::CriticalHud::setup_combos(std::int8_t a0) -> void { joan::bind::member<&::game_b90_03::H::setup_slots>(this, a0); }  // 0x45f860
auto joan::orig::CriticalHud::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b92_02::H::buffers_release>(this)); }  // 0x45fa30
auto joan::orig::CriticalHud::draw() -> void { joan::bind::member<&::game_b110_10::H::crit_draw>(this); }  // 0x45fae0
auto joan::orig::CriticalHud::input(std::int8_t a0, std::int8_t a1) -> void { joan::bind::member<&::game_b91_02::H::crit_input>(this, a0, a1); }  // 0x462810
auto joan::orig::CriticalHud::pick_random_combo() -> void { joan::bind::member<&::game_b95_01::H::pick_random_name>(this); }  // 0x462970
