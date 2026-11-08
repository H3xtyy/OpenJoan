// game/unknown_descbox.cpp: functions whose original source file is unknown (formerly game/_unplaced/descbox.cpp;
// re/attribution.csv: between ww/interfac/combobox.cpp and ww/interfac/i_barr.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cmath>
#include <cstdint>
#include <cstring>

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

using i32 = std::int32_t;

using joan::call::thiscall;

using joan::hook::global;

using joan::img::obj;
using joan::img::str;

}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

struct H {
    // ------------------------------------------------------------------ 0x006b8d20 HelpBox ctor (the global
    // kTooltip_a59780, static init 0x0056cfa0)
    void* clear8() {
        std::memset(this, 0, sizeof(jo::HelpBox));  // 8 on x86 (sprite, owner)
        return this;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Pair::clear", 0x006b8d20, &H::clear8);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

struct H {
    // ------------------------------------------------------------------ 0x006b8d50
    void* set_owner_a47c88() {
        jo::self<jo::HelpBox>(this)->owner = img::obj(d::g_bmp_res_helpbox);
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj::set_owner_a47c88", 0x006b8d50, &H::set_owner_a47c88);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

struct H {
    void reset_6b8d80() {  // 0x006b8d80
        joan::orig::as_help_box(this)->free_sprite();  // 0x006b9490
        jo::self<jo::HelpBox>(this)->owner = nullptr;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6b::reset_b", 0x006b8d80, &H::reset_6b8d80);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b109_10.cpp (game/b109_10)
namespace {
namespace game_b109_10 {

using i16 = std::int16_t;
using u8 = std::uint8_t;

// D3DCOLOR -> Colour 0x005e9640 on a stack temporary, then its dword.
u32 colour_from(u32 argb) {
    u32 tmp = 0;
    const u32* const c = joan::orig::arg::as<const u32*>(joan::orig::as_color(&tmp)->Color_dword(joan::orig::arg::u32(argb)));  // 0x005e9640
    return *c;
}
// Sprite queue 0x00633500 (thunk 0x00401bbd) on 0xa46128, ret 0x44.
u32 sprite(i32 x, i32 y, u32 layer, const char* name, void* sprites, u32 color, i32 a7, i32 a8, i32 a9, i32 a10, i32 a16,
           i32 a17) {
    return joan::orig::as_sprite_queue(obj(d::g_sprites_a46128))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(layer), name, joan::orig::arg::mptr(sprites), joan::orig::arg::u32(color), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), -1, -1, -1, -1, 0, joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));
}

JOAN_COUNTER(c_box, "Frame::draw_box_6b8db0");

struct H {
    // 0x006b8db0
    u32 draw_box(u32 x_arg, u32 y_arg, const char* text1, const char* text2, u32 surf) {
        c_box.hit();
        joan::orig::as_font(obj(d::g_text_a58ac0))->measure(text1, -1, 0xdd, 0, 1.0f);  // 0x00624f00 (unused)
        joan::orig::as_font(obj(d::g_font_a489c8))->measure(text2, -1, 0xdd, 0, 1.0f);  // 0x00624f00 (unused)
        const i16 th = static_cast<i16>(joan::orig::arg::as<i16>(joan::orig::as_font(obj(d::g_font_a489c8))->text_height(joan::orig::arg::i16(i32{2}), 1.0f)) + 3);  // 0x006253f0
        const i16 bw = 0xf8;
        const i16 bh = static_cast<i16>(th + 0x39);
        const i32 xa = static_cast<i16>(x_arg & 0xffff);
        const i32 ya = static_cast<i16>(y_arg & 0xffff);
        const i16 x0 = xa + bw > global<i32>(d::g_screen_w) ? static_cast<i16>(global<i32>(d::g_screen_w) - bw)
                                                          : static_cast<i16>(xa);
        const i16 y0 = ya + bh > global<i32>(d::g_screen_h) ? static_cast<i16>(global<i32>(d::g_screen_h) - bh)
                                                          : static_cast<i16>(ya);
        auto frame = [&](img::Addr name) -> const u8* {
            return joan::orig::arg::as<const u8*>(static_cast<jo::BmpRes*>(jo::self<jo::HelpBox>(this)->owner)->find(joan::orig::arg::str(str(name))));  // 0x00620b40 (+4: the HelpBox's bitmap set, not the Frame view)
        };
        const i16 l = static_cast<i16>(x0 + jo::view<i16>(jo::as_bmp_entry(frame(d::s_des_p_l))->width));       // 'des_p_l'
        const i16 r = static_cast<i16>(x0 + bw - jo::view<i16>(jo::as_bmp_entry(frame(d::s_des_p_r))->width));  // 'des_p_r'
        const i16 u = static_cast<i16>(y0 + jo::view<i16>(jo::as_bmp_entry(frame(d::s_des_p_u))->height));       // 'des_p_u'
        const i16 b = static_cast<i16>(y0 + bh - jo::view<i16>(jo::as_bmp_entry(frame(d::s_des_p_b))->height));  // 'des_p_b'
        auto piece = [&](i32 px, i32 py, img::Addr name, i32 a16, i32 a17) {
            const u32 col = colour_from(0xffffffffu);
            sprite(px, py, surf, img::str(name), static_cast<jo::BmpRes*>(jo::self<jo::HelpBox>(this)->owner), col, 3, 0, 0, 1, a16, a17);
        };
        piece(x0, y0, d::s_des_p_ul, -1, -1);         // 'des_p_ul'
        piece(l, y0, d::s_des_p_u, r - 1, -1);       // 'des_p_u'
        piece(r, y0, d::s_des_p_ur, -1, -1);          // 'des_p_ur'
        piece(x0, u, d::s_des_p_l, -1, b - 1);       // 'des_p_l'
        piece(l, u, d::s_des_p_c, r - 1, b - 1);     // 'des_p_c'
        piece(r, u, d::s_des_p_r, -1, b - 1);        // 'des_p_r'
        piece(x0, b, d::s_des_p_bl, -1, -1);          // 'des_p_bl'
        piece(l, b, d::s_des_p_b, r - 1, -1);        // 'des_p_b'
        piece(r, b, d::s_des_p_br, -1, -1);           // 'des_p_br'
        {
            const u32 col = colour_from(0xffffffffu);
            joan::orig::as_text_renderer(obj(d::g_text_a58ac0))->draw_wrapped(joan::orig::arg::u32(x0 + 0x14), joan::orig::arg::u32(y0 + 8), joan::orig::arg::u32(x0 + 0xe4), joan::orig::arg::u32(y0 + 0x28), joan::orig::arg::u32(surf), text1, joan::orig::arg::u32(col), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), 1.0f, 1.0f, joan::orig::arg::u32(i32{2}));  // 0x00626e00
        }
        const u32 col = colour_from(0xffffffffu);
        joan::orig::as_text_renderer(obj(d::g_font_a489c8))->draw_wrapped(joan::orig::arg::u32(x0 + 0xd), joan::orig::arg::u32(y0 + 0x2c), joan::orig::arg::u32(x0 + 0xeb), joan::orig::arg::u32(y0 + th + 0x2c), joan::orig::arg::u32(surf), text2,
 joan::orig::arg::u32(col), joan::orig::arg::u32(i32{2}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), joan::orig::arg::u32(i32{0}), 1.0f, 1.0f, joan::orig::arg::u32(i32{2}));  // 0x00626e00
        return 0;  // callers discard the result; TextRenderer::draw_wrapped returns nothing
    }
};
static_assert(sizeof(i16) == 2 && sizeof(u8) == 1 && sizeof(float) == 4, "widths");

#define B(name, addr, fn) JOAN_REPLACE("game/b109_10", name, addr, fn)
B("Frame::draw_box_6b8db0", 0x006b8db0, &H::draw_box);
#undef B

}  // namespace game_b109_10
}  // namespace

// ---------------------------------------- from modules/game_b87_06.cpp (game/b87_06)
namespace {
namespace game_b87_06 {

constexpr img::Addr kResources = d::g_texture_array;

std::int32_t res_mode() { return joan::orig::arg::as<std::int32_t>(joan::orig::as_texture_manager(obj(kResources))->mode()); }                       // 0x0060a720
void res_set_mode(std::int32_t m) { joan::orig::as_texture_manager(obj(kResources))->set_mode(joan::orig::arg::u32(m)); }                               // 0x0060a740
void* res_load(const char* name) { return joan::orig::arg::as<void*>(joan::orig::as_texture_manager(obj(kResources))->load(joan::orig::arg::str(name), 1, 0u)); }          // 0x006099c0

struct H {
    void load_one(const char* name) {
        if (jo::self<jo::HelpBox>(this)->sprite != nullptr) return;
        const std::int32_t mode = res_mode();
        res_set_mode(1);
        jo::self<jo::HelpBox>(this)->sprite = res_load(name);
        res_set_mode(mode);
    }
    // ------------------------------------------------------------ 0x006b9420 (ret 0)
    void helpbox_load_sprite() { load_one(&img::global<const char>(d::s_helpbox)); }  // 'helpbox'
};

#define B06(name, addr, fn) JOAN_REPLACE("game/b87_06", name, addr, fn)
B06("HelpBox::load_sprite", 0x006b9420, &H::helpbox_load_sprite);
#undef B06

}  // namespace game_b87_06
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
namespace {
namespace game_b91_04 {

void* const kTextures = obj(d::g_texture_array);

void texture_release(void* t) { joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(t)); }  // 0x0060a550

struct H {
    void* self() { return this; }

    // 0x006b9490
    void* release_6b9490() {
        if (jo::self<jo::HelpBox>(this)->sprite != nullptr) {
            texture_release(jo::self<jo::HelpBox>(this)->sprite);
            jo::self<jo::HelpBox>(this)->sprite = nullptr;
        }
        return self();
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Obj6b9::release", 0x006b9490, &H::release_6b9490);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::HelpBox::release() -> void { joan::bind::member<&::game_b93_06::H::reset_6b8d80>(this); }  // 0x6b8d80
auto joan::orig::HelpBox::draw_box(std::uint32_t a0, std::uint32_t a1, const char* a2, const char* a3, std::uint32_t a4) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b109_10::H::draw_box>(this, a0, a1, a2, a3, a4)); }  // 0x6b8db0
auto joan::orig::HelpBox::load_sprite() -> void { joan::bind::member<&::game_b87_06::H::helpbox_load_sprite>(this); }  // 0x6b9420
auto joan::orig::HelpBox::free_sprite() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_04::H::release_6b9490>(this)); }  // 0x6b9490
