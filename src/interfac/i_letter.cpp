// interfac/i_letter.cpp: functions of the original d:\ww\interfac\i_letter.cpp (re/attribution.csv).
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

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
//
//      0x006e0520 Letter::free          (thiscall, ret 0) - i_letter.cpp free.
namespace {
namespace game_b93_06 {

void* const kMem = obj(d::g_mem);

JOAN_COUNTER(c_letter_free, "Letter::free");

struct H {
    void letter_free() {  // 0x006e0520
        c_letter_free.hit();
        joan::orig::as_letter(this)->release_texture();  // 0x006e0b20
        jo::self<jo::Letter>(this)->i16_d = 0;
        jo::self<jo::Letter>(this)->i32_13 = 0;
        if (jo::self<jo::Letter>(this)->text != nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a25104)) + 8;
            joan::orig::as_mem(kMem)->del(joan::orig::arg::mptr(jo::self<jo::Letter>(this)->text), str(d::s_d_ww_interfac_i_letter_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            jo::self<jo::Letter>(this)->text = nullptr;
        }
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Letter::free", 0x006e0520, &H::letter_free);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_ui6.cpp (game/ui6)
//
//      - Letter::draw 0x006e05a0 (interfac/i_letter.cpp, ret 0): 'bg' sprite centred on a
//        0x274 x 0x2e6 page, the 'e_seal' / 'f_seal' sprite by +0x17 (1 / 2) and the text +0xf
//        through Text::draw 0x00626320 (on 0x00a489c8).
namespace {
namespace game_ui6 {

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite queue
constexpr std::int32_t M1 = -1;

using joan::orig::helpers::screen_w;
using joan::orig::helpers::screen_h;

// 0x005e9640 Color(dword) on a 4-byte local, the dword copied out. ret 4.
std::uint32_t colour_of(std::uint32_t v) {
    std::uint32_t c = 0;
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_color(&c)->Color_dword(joan::orig::arg::u32(v)));  // 0x005e9640
}

// 0x00633500 on the sprite queue (17 dword arguments). ret 0x44.
void sprite(std::int32_t x, std::int32_t y, std::int32_t layer, const char* name, void* texture, std::uint32_t colour,
            std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10, std::int32_t a11, std::int32_t a12,
            std::int32_t a13, std::int32_t a14, std::int32_t a15, std::int32_t a16, std::int32_t a17) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(layer), joan::orig::arg::str(name), texture, joan::orig::arg::u32(colour), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), joan::orig::arg::i16(a15), joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));  // 0x00633500
}
// The common 'push -1,-1,0,-1,-1,-1,-1,1,0,0,3' tail.
void sprite_std(std::int32_t x, std::int32_t y, std::int32_t layer, const char* name, void* texture, std::uint32_t colour) {
    sprite(x, y, layer, name, texture, colour, 3, 0, 0, 1, M1, M1, M1, M1, 0, M1, M1);
}

JOAN_COUNTER(c_letter, "Letter::draw");

struct H {
    // 0x006e05a0 Letter::draw. ret 0.
    void letter_draw() {
        joan::canvas::ScreenScope canvas_scope(0x006e05a0);  // canvas: a panel centred on the screen (ui_center, UiScale)
        c_letter.hit();
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::Letter>(this)->open_)) == 0) return;
        {
            const std::uint32_t c = colour_of(0xffffffffu);
            sprite_std((screen_w() - 0x274) / 2, (screen_h() - 0x2e6) / 2, 6, str(d::s_bg), jo::self<jo::Letter>(this)->sheet, c);  // 'bg'
        }
        if (static_cast<std::int32_t>(jo::self<jo::Letter>(this)->seal) == 1) {
            const std::uint32_t c = colour_of(0xffffffffu);
            sprite_std((screen_w() - 0x274) / 2 + 0x34, (screen_h() - 0x2e6) / 2 + 0x58, 6, str(d::s_e_seal), jo::self<jo::Letter>(this)->sheet, c);  // 'e_seal'
        } else if (static_cast<std::int32_t>(jo::self<jo::Letter>(this)->seal) == 2) {
            const std::uint32_t c = colour_of(0xffffffffu);
            sprite_std((screen_w() - 0x274) / 2 + 0x1c0, (screen_h() - 0x2e6) / 2 + 0x22c, 6, str(d::s_f_seal), jo::self<jo::Letter>(this)->sheet, c);  // 'f_seal'
        }
        const std::uint32_t c = colour_of(0xffffffffu);
        const char* const text = jo::self<jo::Letter>(this)->text;
        const std::int32_t y2 = (screen_h() - 0x2e6) / 2 + 0x272;
        const std::int32_t x2 = (screen_w() - 0x274) / 2 + 0x230;
        const std::int32_t y1 = (screen_h() - 0x2e6) / 2 + 0xb1;
        const std::int32_t x1 = (screen_w() - 0x274) / 2 + 0x3c;
        joan::orig::as_text(obj(d::g_font_a489c8))->draw(joan::orig::arg::i32(x1), joan::orig::arg::i32(y1), joan::orig::arg::i32(x2), joan::orig::arg::i32(y2), 6u, text, joan::orig::arg::u32(c), 0, 1, 1, 0, 2, 0, 1.0f, 1.0f, 0, joan::orig::arg::i32(M1));  // 0x00626320
    }
};

#define U6(name, addr, fn) JOAN_REPLACE("game/ui6", name, addr, fn)
U6("Letter::draw", 0x006e05a0, &H::letter_draw);
JOAN_CANVAS_SELF(0x006e05a0);  // ScreenScope in the body (no thunk wrapper)
#undef U6

}  // namespace game_ui6
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
//
//    - Letter::poll 0x006e0830 (ret 0) -> al: 0 when +8 is 0; else 1, and when [0xa51d5e] is
//      0x1b / 0x20, or 0x594700(2) on 0xa516b0, or (0x63e420 && byte [0xa51d6b] in 0..3):
//      0x6e09d0, +0x19 = +0xd (word).
//
//    - Letter::open 0x006e0900 (ret 0): +8 = 1, +0x19 = 0, 0x6e0dc0 / 0x4af8a0 / 0x6fe240,
//      +9 = 0x6e0df0 (> 0: 0x6f7e90), 0x6e0ab0, resumes the game speed when [0xa500d2].
//
//  - Return values: callers of 0x6e0900, 0x6e4c00, 0x6e4cc0, 0x6ed3e0, 0x6f1ff0, 0x6f8050,
//    0x707b20, 0x707ce0, 0x7180f0, 0x723f90, 0x732940, 0x732af0, 0x736d80, 0x7388a0, 0x742f90
//    ignore eax (checked, none is referenced as data / vtable); the replacements return void or
//    a simple value.
namespace {
namespace game_b100_16 {

using i8 = std::int8_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;

void* const kMouse = obj(d::g_mouse);
void* const kSpeed = obj(d::g_game_a50088);

i32 ftol(double v) { return joan::hook::x87::ftol(v); }

// Resumes the game speed after a pause (shared tail of 0x6e0900 / 0x6ed3e0).
void resume_speed() {
    if (global<u8>(d::g_game_running) != 0) {
        global<u8>(d::g_game_running) = 0;
        global<i16>(d::g_saved_game_speed) = static_cast<i16>(ftol(global<float>(d::g_game_speed)));
        joan::orig::as_game(kSpeed)->set_speed(joan::orig::arg::bits(0.0f));  // 0x0042f930
    }
}

JOAN_COUNTER(c_letter_poll, "Letter::poll");
JOAN_COUNTER(c_letter_open, "Letter::open");

struct H {
    // ------------------------------------------------------------------ 0x006e0830
    bool letter_poll() {
        c_letter_poll.hit();
        if (jo::self<jo::Letter>(this)->open_ == 0) return false;
        bool go;
        if (global<i32>(d::g_key) == 0x1b || global<i32>(d::g_key) == 0x20) {
            go = true;
        } else if (joan::orig::arg::as<i32>(joan::orig::as_parser(kMouse)->value(2)) != 0) {  // 0x00594700
            go = true;
        } else if (joan::orig::arg::as<i32>(joan::orig::as_input(kMouse)->flag_66d()) == 0) {     // 0x0063e420
            go = false;
        } else {
            const i32 m = global<i8>(d::g_a51d6b);
            go = m == 0 || m == 1 || m == 2 || m == 3;
        }
        if (go) {
            joan::orig::as_letter(this)->close();  // 0x006e09d0
            jo::self<jo::Letter>(this)->u16_19 = jo::view<u16>(jo::self<jo::Letter>(this)->i16_d);
        }
        return true;
    }

    // ------------------------------------------------------------------ 0x006e0900
    void letter_open() {
        c_letter_open.hit();
        jo::view<u8>(jo::self<jo::Letter>(this)->open_) = 1;
        jo::self<jo::Letter>(this)->u16_19 = 0;
        joan::orig::as_panel(obj(d::g_mini_map))->set_flag_e(static_cast<std::uint8_t>(joan::orig::arg::u32(0)));  // 0x006e0dc0
        joan::orig::as_message_indicator(obj(d::g_message_indicator))->set_mode(0u);  // 0x004af8a0
        joan::orig::as_hud(obj(d::g_player_hud))->clear_portrait();     // 0x006fe240
        jo::self<jo::Letter>(this)->timer = joan::orig::arg::as<i32>(joan::orig::as_unit_attr_screen(obj(d::g_a56398))->unit_6af());  // 0x006e0df0
        if (jo::self<jo::Letter>(this)->timer > 0) joan::orig::as_panel(obj(d::g_a56398))->close();  // 0x006f7e90
        joan::orig::as_letter(this)->load_6e0ab0();  // 0x006e0ab0
        resume_speed();
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Letter::poll", 0x006e0830, &H::letter_poll);
B("Letter::open", 0x006e0900, &H::letter_open);
#undef B

}  // namespace game_b100_16
}  // namespace

// ---------------------------------------- from modules/game_b84_04.cpp (game/b84_04)
//
//      0x006e09d0 Letter::close()                       - interfac/i_letter.cpp
namespace {
namespace game_b84_04 {

JOAN_COUNTER(c_letter_close, "Letter::close");

struct H {
    // ---- interface -------------------------------------------------------------------------
    void letter_close() {  // 0x006e09d0
        c_letter_close.hit();
        jo::view<std::uint8_t>(jo::self<jo::Letter>(this)->open_) = 0;
        joan::orig::as_letter(this)->release_texture();  // 0x006e0b20
        jo::self<jo::Letter>(this)->u16_19 = 0;
        if (global<std::uint8_t>(d::g_game_running) == 0) {
            global<std::uint8_t>(d::g_game_running) = 1;
            joan::orig::as_game(obj(d::g_game_a50088))->set_speed(joan::orig::arg::bits(static_cast<float>(global<std::int16_t>(d::g_saved_game_speed))));  // 0x0042f930
        }
        joan::orig::as_panel(obj(d::g_mini_map))->set_flag_e(static_cast<std::uint8_t>(joan::orig::arg::u32(1)));  // 0x006e0dc0
        joan::orig::as_message_indicator(obj(d::g_message_indicator))->set_mode(1u);  // 0x004af8a0
        if (jo::self<jo::Letter>(this)->timer > 0) {
            joan::orig::as_screen(obj(d::g_a56398))->open_level(joan::orig::arg::i32(jo::self<jo::Letter>(this)->timer));  // 0x006f7d80
            jo::self<jo::Letter>(this)->timer = 0;
        }
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("Letter::close", 0x006e09d0, &H::letter_close);
#undef M

}  // namespace game_b84_04
}  // namespace

// ---------------------------------------- from modules/game_b91_04.cpp (game/b91_04)
//
//      0x006e0a90 Letter::get_8()                  - byte +8
namespace {
namespace game_b91_04 {

struct H {
    // 0x006e0a90
    std::uint8_t letter_get_8() { return jo::view<std::uint8_t>(jo::self<jo::Letter>(this)->open_); }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_04", name, addr, fn)
B91("Letter::get_8", 0x006e0a90, &H::letter_get_8);
#undef B91

}  // namespace game_b91_04
}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
//
//    - Letter::load 0x006e0ab0 / QuitPopup::load 0x006e4d60 (ret 0): "LETTER" / "QUIT" window.
namespace {
namespace game_b99_10 {

void* const kWindows = obj(d::g_texture_array);

// Window-image load with the "current window" saved / restored (0x60a720 / 0x60a740).
u32 window_save() { return joan::orig::arg::as<u32>(joan::orig::as_texture_manager(kWindows)->mode()); }                // 0x0060a720
// void callee; window_set's result only reaches Letter::load (0x006e0ab0), whose one caller discards it.
u32 window_set(u32 v) {
    joan::orig::as_texture_manager(kWindows)->set_mode_60a740(joan::orig::arg::u32(v));  // 0x0060a740
    return 0;
}
void* window_load(const char* name) { return joan::orig::as_texture_manager(kWindows)->load(joan::orig::arg::str(name), 1, 0u); }  // 0x006099c0

struct H {
    // ------------------------------------------------------------------ 0x006e0ab0 / 0x006e4d60
    u32 load_window_at0(const char* name) {
        const u32 saved = window_save();
        window_set(1);
        if (jo::self<jo::Letter>(this)->tex == nullptr) jo::self<jo::Letter>(this)->tex = window_load(name);
        return window_set(saved);
    }
    u32 letter_load() { return load_window_at0(str(d::s_LETTER)); }  // "LETTER"
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Letter::load", 0x006e0ab0, &H::letter_load);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b85_05.cpp (game/b85_05)
//
//    - Panel::release_textures4 0x006e8e40, Panel::release_textures2 0x006e7c60 (+0xd18,
//      +0xd1c), TexHandle::release 0x006d3540 / 0x006e0440 / 0x006e0b20 (i_letter.cpp),
//      TexHandle::release_and_reset 0x006d6310 (+ 0x0059b150 on 0x00a5d0d0).
namespace {
namespace game_b85_05 {

void* const kTextures = obj(d::g_texture_array);

void release_texture(void* t) { joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(t)); }         // 0x0060a550

struct H {
    // ------------------------------------------------------------------ 0x006d3540 / 0x006e0440 / 0x006e0b20
    void release_handle() {
        if (jo::view<void*>(jo::self<jo::Letter>(this)->tex) != nullptr) {
            release_texture(jo::view<void*>(jo::self<jo::Letter>(this)->tex));
            jo::view<void*>(jo::self<jo::Letter>(this)->tex) = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("Letter::release_texture", 0x006e0b20, &H::release_handle);
#undef B

}  // namespace game_b85_05
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
//
//      0x006e0b70 Letter::load(short item)                      thiscall, ret 4 (SEH frame: File local)
namespace {
namespace game_b92_06 {

using joan::call::cdecl_call;

void* const kMemory = obj(d::g_mem);
void* const kItems = obj(d::g_item_res);

std::uint8_t* item_rec(std::int16_t id) { return joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_2bf(kItems)->get(joan::orig::arg::i16(id))); }  // 0x00425ce0

std::uint32_t u(std::int32_t v) { return static_cast<std::uint32_t>(v); }
using joan::orig::helpers::s;

struct alignas(4) File {
    std::uint8_t bytes[0x210];  // 0x110 used (0x004179c0)
};

JOAN_COUNTER(c_letter, "Letter::load");

struct H {
    // 0x006e0b70
    void letter_load(std::int16_t item) {
        c_letter.hit();
        if (jo::view<void*>(jo::self<jo::Letter>(this)->text) != nullptr) {
            joan::orig::as_mem(kMemory)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Letter>(this)->text)), str(d::s_d_ww_interfac_i_letter_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a25108)) + 3));  // 0x0058ed70
            jo::self<jo::Letter>(this)->text = nullptr;
            jo::view<std::uint32_t>(jo::self<jo::Letter>(this)->i32_13) = 0;
        }
        File file;
        joan::orig::as_file(&file)->construct();  // 0x004179c0
        char path[0x100];
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(path))), str(d::s_ITEM_DES));  // strcpy "ITEM_DES\\"
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(static_cast<char*>(path))), joan::orig::arg::str(jo::as_item_def(item_rec(item))->str_25f));
        if (joan::crt::stricmp(joan::orig::arg::str(jo::as_item_def(item_rec(item))->str_27f), str(d::s_English)) ==
            0)  // _stricmp "English"
            jo::self<jo::Letter>(this)->seal = 1;
        else if (joan::crt::stricmp(joan::orig::arg::str(jo::as_item_def(item_rec(item))->str_27f), str(d::s_French)) == 0)  // "French"
            jo::self<jo::Letter>(this)->seal = 2;
        else
            jo::self<jo::Letter>(this)->seal = 0;
        joan::orig::as_file(&file)->file_open(joan::orig::arg::str(static_cast<const char*>(path)), 1, static_cast<char>(0));  // 0x00586da0 open
        jo::self<jo::Letter>(this)->i32_13 = s(u(joan::orig::as_file(&file)->file_size()) + 1u);             // 0x00587f30 size
        jo::view<void*>(jo::self<jo::Letter>(this)->text) = joan::orig::as_mem(kMemory)->add(joan::orig::arg::u32(s(u(jo::self<jo::Letter>(this)->i32_13) + 1u)), str(d::s_d_ww_interfac_i_letter_cpp), joan::orig::arg::i32(static_cast<std::int32_t>(global<std::int16_t>(d::g_a25108)) + 0x1b));  // 0x0058e7b0
        std::memset(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Letter>(this)->text)), 0, joan::orig::arg::u32(s(u(jo::self<jo::Letter>(this)->i32_13) + 1u)));  // memset
        joan::orig::as_file(&file)->file_read(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::Letter>(this)->text)), joan::orig::arg::u32(jo::self<jo::Letter>(this)->i32_13));  // 0x005873a0 read
        jo::self<jo::Letter>(this)->i16_d = item;
        joan::orig::as_file(&file)->destruct();  // 0x005872b0 ~File
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("Letter::load", 0x006e0b70, &H::letter_load);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
//
//      0x006e03d0 Letter::load_kill     (thiscall, ret 0) - "KILL".
namespace {
namespace game_b93_06_2 {

using joan::img::obj;
using joan::img::str;

using i32 = std::int32_t;
using u32 = std::uint32_t;

void* const kRes = obj(d::g_texture_array);

struct H {
    void load_kill() {  // 0x006e03d0
        const i32 saved = joan::orig::arg::as<i32>(joan::orig::as_texture_manager(kRes)->mode());  // 0x0060a720
        joan::orig::as_texture_manager(kRes)->set_mode(1u);                     // 0x0060a740
        if (jo::self<jo::Letter>(this)->tex == nullptr) {
            jo::self<jo::Letter>(this)->tex = joan::orig::as_texture_manager(kRes)->load(str(d::s_KILL), 1, 0u);  // 0x006099c0 "KILL"
        }
        joan::orig::as_texture_manager(kRes)->set_mode(joan::orig::arg::u32(saved));
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Letter::load_kill", 0x006e03d0, &H::load_kill);
#undef B93

}  // namespace game_b93_06_2
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::memset_;  // memset

struct H {
    H* zero_14() {  // 0x006df0c0
        memset_(this, 0, static_cast<u32>(sizeof(jo::GoalPanel)));  // 0x14 on x86 (the GoalPanel 0xa50518)
        return this;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("Struct14::zero", 0x006df0c0, &H::zero_14);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b100_16.cpp (game/b100_16)
namespace {
namespace game_b100_16_2 {

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x006df0f0
    void* ctor_6df0f0() {
        jo::self<jo::GoalPanel>(this)->sheet = obj(d::g_bmp_res_kill);
        jo::self<jo::GoalPanel>(this)->tex = nullptr;
        jo::view<u32>(jo::self<jo::GoalPanel>(this)->goal_main) = 0;
        jo::view<u32>(jo::self<jo::GoalPanel>(this)->goal_side) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_16", name, addr, fn)
B("Obj::ctor_6df0f0", 0x006df0f0, &H::ctor_6df0f0);
#undef B

}  // namespace game_b100_16_2
}  // namespace

// ---------------------------------------- from modules/game_b93_06_3.cpp (game/b93_06)
namespace {
namespace game_b93_06_3 {

using joan::call::thiscall;

using i32 = std::int32_t;

struct H {
    void reset_6df140() {  // 0x006df140
        joan::orig::as_goal_panel(this)->free_sprite();  // 0x006e0440
        jo::self<jo::GoalPanel>(this)->goal_main = 0;
        jo::self<jo::GoalPanel>(this)->goal_side = 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6d::reset_c", 0x006df140, &H::reset_6df140);
#undef B93

}  // namespace game_b93_06_3
}  // namespace

// ---------------------------------------- from modules/game_ui6.cpp (game/ui6)
namespace {
namespace game_ui6_2 {

using joan::call::thiscall;

using u32 = std::uint32_t;

using joan::call::cdecl_call;

using joan::hook::global;

using joan::img::obj;

using i16 = std::int16_t;

using joan::img::str;

constexpr img::Addr kSprites = d::g_sprites_a46128;  // 2D sprite queue
constexpr img::Addr kTimer = d::g_misc;    // number formatting
constexpr std::int32_t M1 = -1;

// 0x005e9640 Color(dword) on a 4-byte local, the dword copied out. ret 4.
std::uint32_t colour_of(std::uint32_t v) {
    std::uint32_t c = 0;
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_color(&c)->Color_dword(joan::orig::arg::u32(v)));  // 0x005e9640
}
// 0x0042a260 Color(r, g, b, a). ret 0x10.
std::uint32_t colour4(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t c = 0;
    return *joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
}

// 0x00633500 on the sprite queue (17 dword arguments). ret 0x44.
void sprite(std::int32_t x, std::int32_t y, std::int32_t layer, const char* name, void* texture, std::uint32_t colour,
            std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10, std::int32_t a11, std::int32_t a12,
            std::int32_t a13, std::int32_t a14, std::int32_t a15, std::int32_t a16, std::int32_t a17) {
    joan::orig::as_sprite_queue(obj(kSprites))->draw_named(joan::orig::arg::i16(x), joan::orig::arg::i16(y), joan::orig::arg::u32(layer), joan::orig::arg::str(name), texture, joan::orig::arg::u32(colour), joan::orig::arg::i16(a7), joan::orig::arg::i16(a8), joan::orig::arg::i16(a9), joan::orig::arg::i16(a10), joan::orig::arg::i16(a11), joan::orig::arg::i16(a12), joan::orig::arg::i16(a13), joan::orig::arg::i16(a14), joan::orig::arg::i16(a15), joan::orig::arg::i16(a16), joan::orig::arg::i16(a17));  // 0x00633500
}
// The common 'push -1,-1,0,-1,-1,-1,-1,1,0,0,3' tail.
void sprite_std(std::int32_t x, std::int32_t y, std::int32_t layer, const char* name, void* texture, std::uint32_t colour) {
    sprite(x, y, layer, name, texture, colour, 3, 0, 0, 1, M1, M1, M1, M1, 0, M1, M1);
}

JOAN_COUNTER(c_goal, "GoalPanel::draw");
JOAN_COUNTER(c_goal_fix, "GoalPanel::draw (fix/uninit: kind outside 0..5, empty text)");

struct H {
    // ---- 0x006df180 helpers ----
    // "<found>/<total>" into buf: 0x0058fca0(value, 4) (ret 8), strcpy, strcat "/", strcat.
    static void counts(char* buf, std::uintptr_t found, std::int32_t total) {
        const char* const a = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(kTimer))->format_int_58fca0(joan::orig::arg::i32(global<std::int32_t>(found)), 4));  // 0x0058fca0
        std::strcpy(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(a));                   // strcpy
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(buf)), str(d::s_slash));                            // strcat '/'
        const char* const b = joan::orig::arg::as<const char*>(joan::orig::as_misc(obj(kTimer))->format_int_58fca0(joan::orig::arg::i32(total), 4));
        std::strcat(static_cast<char*>(joan::orig::arg::mptr(buf)), joan::orig::arg::str(b));
    }
    static void time_left(char* buf, std::int32_t limit) {
        const std::uint32_t secs = global<std::uint32_t>(d::g_get_a59171) / 0x3e8u;
        const std::int32_t t = static_cast<std::int32_t>(static_cast<std::uint32_t>(limit) - secs);
        joan::orig::libc::sprintf(joan::orig::arg::str(buf), str(d::s_d_d_9de198), (t % 0xe10) / 0x3c, (t % 0xe10) % 0x3c);  // sprintf '%d:%d'
    }
    // One goal kind: its sprite at (0x1ac, y) and its string. False for a kind outside 0..5.
    void goal_kind(std::int32_t kind, std::int32_t y, std::int32_t total, char* buf) {
        std::uintptr_t name = 0, found = 0;
        switch (kind) {
            case 0: name = img::addr(d::s_icon); found = img::addr(d::g_get_a5918d); break;  // 'icon'
            case 1: name = img::addr(d::s_ficon); found = img::addr(d::g_get_a59199); break;  // 'ficon'
            case 2: name = img::addr(d::s_hicon); found = img::addr(d::g_get_a591a5); break;  // 'hicon'
            case 3: name = img::addr(d::s_ticon); break;                      // 'ticon'
            case 4: name = img::addr(d::s_icon); found = img::addr(d::g_get_a591c8); break;  // 'icon'
            case 5: name = img::addr(d::s_gold); found = img::addr(d::g_get_a591b6); break;  // 'gold'
            default: return;
        }
        const std::uint32_t c = colour_of(0xffffffffu);
        sprite_std(0x1ac, y, 6, str(name), jo::self<jo::GoalPanel>(this)->sheet, c);
        if (kind == 3) time_left(buf, total);
        else counts(buf, found, total);
    }
    static void goal_text(std::int32_t y, char* buf) {
        const std::uint32_t c = colour4(0xff, 0, 0, 0);
        joan::orig::as_text(obj(d::g_font_celtic_10pt_english))->draw_3(0x1f4, joan::orig::arg::i32(y), 0x250, joan::orig::arg::i32(y + 0x18), 6u, buf, joan::orig::arg::u32(c), 0, 0, 0, 0, 1.0f, 1.0f);  // 0x00626130
    }

    // 0x006df180 GoalPanel::draw. ret 0.
    void goal_draw() {
        joan::canvas::ScreenScope canvas_scope(0x006df180);  // canvas: what the thunk wrapper did (canvas_api.hpp)
        c_goal.hit();
        if (static_cast<std::int8_t>(global<std::uint8_t>(d::g_a5d384)) != 0) return;
        const auto bad = [](std::uint8_t k) { return static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int8_t>(k))) > 5u; };
        if ((static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->show_main)) != 0 && bad(jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->kind_main))) ||
            (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->show_side)) != 0 && bad(jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->kind_side)))) {
            // Uninitialised text buffer: joan.exe's body no longer exists (stage S5): the C++ version
            // with that buffer empty.
            c_goal_fix.hit();
        }
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->show_main)) != 0) {
            char buf[256];
            buf[0] = 0;  // [ebp-0x14]: written by goal_kind for kinds 0..5; fix/uninit: "" otherwise
            const std::uint32_t c = colour_of(0xffffffffu);
            sprite_std(0x1ac, 8, 6, str(d::s_panel), jo::self<jo::GoalPanel>(this)->sheet, c);  // 'panel'
            goal_kind(static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->kind_main)), 8, jo::self<jo::GoalPanel>(this)->goal_main, buf);
            goal_text(0xc, buf);
        }
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->show_side)) == 0) return;
        std::int32_t y = 8, ty = 0xc;
        if (static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->show_main)) != 0) {
            y += 0x2b;
            ty += 0x2b;
        }
        char buf[256];
        buf[0] = 0;  // [ebp-0x30]: written by goal_kind for kinds 0..5; fix/uninit: "" otherwise
        const std::uint32_t c = colour_of(0xffffffffu);
        sprite_std(0x1ac, static_cast<std::int16_t>(y), 6, str(d::s_panel), jo::self<jo::GoalPanel>(this)->sheet, c);  // 'panel'
        goal_kind(static_cast<std::int8_t>(jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->kind_side)), static_cast<std::int16_t>(y), jo::self<jo::GoalPanel>(this)->goal_side, buf);
        goal_text(ty, buf);
    }
};

#define U6(name, addr, fn) JOAN_REPLACE("game/ui6", name, addr, fn)
U6("GoalPanel::draw", 0x006df180, &H::goal_draw);
JOAN_CANVAS_SELF(0x006df180);  // ScreenScope in the body (no thunk wrapper)
#undef U6

}  // namespace game_ui6_2
}  // namespace

// ---------------------------------------- from modules/game_b103_15.cpp (game/b103_15)
namespace {
namespace game_b103_15 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

using joan::orig::helpers::addr_of;

struct H {
    u32 self_hi() { return addr_of(this) & 0xffffff00u; }

    // ------------------------------------------------------------------------ 0x006dfdd0
    u32 any_6dfdd0() {
        if (static_cast<i32>(jo::self<jo::GoalPanel>(this)->show_main) != 0) return self_hi() | 1u;
        const i32 b9 = static_cast<i32>(jo::self<jo::GoalPanel>(this)->show_side);
        if (b9 != 0) return (static_cast<u32>(b9) & 0xffffff00u) | 1u;
        return 0;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_15", name, addr, fn)
B("Obj::any_6dfdd0", 0x006dfdd0, &H::any_6dfdd0);
#undef B

}  // namespace game_b103_15
}  // namespace

// ---------------------------------------- from modules/game_b98_08.cpp (game/b98_08)
namespace {
namespace game_b98_08 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

void* const kTypes = obj(d::g_types);

using joan::orig::helpers::addr;

void* const kSoldiers = obj(d::g_units);

JOAN_COUNTER(c_set_mode, "X::set_mode");

struct H {
    // ------------------------------------------------------------------------ 0x006dfe10
    std::uintptr_t set_mode(i8 mode) {  // eax: this or 0
        c_set_mode.hit();
        jo::view<u8>(jo::self<jo::GoalPanel>(this)->show_main) = 1;
        jo::self<jo::GoalPanel>(this)->kind_main = mode;
        const i32 m = static_cast<i32>(mode);
        if (m == 0 || m == 1 || m == 4) {
            for (i32 i = joan::orig::as_dynarray(kSoldiers)->size(); i >= 1; --i) {  // 0x00417ad0
                if (joan::orig::as_dynarray(kSoldiers)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00428b70
                u8* const u = joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kSoldiers)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3c0
                u8* const t = joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(u)->type_id))));  // 0x0042d0a0
                if (static_cast<i32>(jo::as_unit_type(t)->unit_class) == 0 && jo::as_unit(u)->draw != nullptr) jo::as_unit(u)->draw->u8_52 = 0;
            }
        }
        if (static_cast<i32>(jo::self<jo::GoalPanel>(this)->show_side) != 0) return addr(this);
        joan::orig::as_letter(this)->load_kill();  // 0x006e03d0
        return 0;  // void callee; the one caller (esfuncs.cpp, GoalPanel::open_first) discards the result
    }
};

#define B98(name, addr_, fn) JOAN_REPLACE("game/b98_08", name, addr_, fn)
B98("X::set_mode", 0x006dfe10, &H::set_mode);
#undef B98

}  // namespace game_b98_08
}  // namespace

// ---------------------------------------- from modules/game_b84_04_2.cpp (game/b84_04)
namespace {
namespace game_b84_04_2 {

using joan::call::thiscall;

using i32 = std::int32_t;

using joan::img::obj;

using i16 = std::int16_t;

void* const kGameUnits = obj(d::g_units);
void* const kUnitTypes = obj(d::g_types);

JOAN_COUNTER(c_markers_a, "UnitMarkers::refresh_a");
JOAN_COUNTER(c_markers_b, "UnitMarkers::refresh_b");

// unit-marker loop shared by 0x006dff10 / 0x006e01d0
void mark_units() {
    for (std::int32_t i = joan::orig::as_dynarray(kGameUnits)->size(); i >= 1; --i) {  // 0x00417ad0
        if (joan::orig::as_dynarray(kGameUnits)->empty_at(joan::orig::arg::i32(i)) != 0) continue;  // 0x00428b70
        std::uint8_t* const unit = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_dynarray(kGameUnits)->ptr_at(joan::orig::arg::i32(i)));  // 0x0041b3c0
        std::uint8_t* const type = joan::orig::arg::as<std::uint8_t*>(joan::orig::as_records_203(kUnitTypes)->get(joan::orig::arg::i16(jo::as_unit(unit)->type_id)));  // 0x0042d0a0
        if (jo::as_unit_type(type)->unit_class != 0) continue;
        if (jo::as_unit(unit)->draw == nullptr) continue;
        jo::as_unit(unit)->draw->u8_52 = 1;
    }
}

struct H {
    void markers_a() {  // 0x006dff10
        c_markers_a.hit();
        jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->show_main) = 0;
        if (jo::self<jo::GoalPanel>(this)->show_side == 0) {
            mark_units();
            joan::orig::as_goal_panel(this)->free_sprite();  // 0x006e0440
            return;
        }
        // the original's +0x13 test (== 0 / == 1, then == 4) always falls through to the loop
        mark_units();
    }
    void markers_b() {  // 0x006e01d0
        c_markers_b.hit();
        if (jo::self<jo::GoalPanel>(this)->show_side == 0) return;
        jo::view<std::uint8_t>(jo::self<jo::GoalPanel>(this)->show_side) = 0;
        if (jo::self<jo::GoalPanel>(this)->show_main == 0) {
            mark_units();
            joan::orig::as_goal_panel(this)->free_sprite();  // 0x006e0440
            return;
        }
        // +0x12 test always falls through, as in 0x006dff10
        mark_units();
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b84_04", name, addr, fn)
M("UnitMarkers::refresh_b", 0x006e01d0, &H::markers_b);
M("UnitMarkers::refresh_a", 0x006dff10, &H::markers_a);
#undef M

}  // namespace game_b84_04_2
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;

using joan::img::obj;

using i8 = std::int8_t;
using i16 = std::int16_t;

void* const kTypes = obj(d::g_types);

void* const kUnitTable = obj(d::g_units);

JOAN_COUNTER(c_begin, "Obj::begin");

i32 unit_missing(i32 id) { return joan::orig::as_dynarray(kUnitTable)->empty_at(joan::orig::arg::i32(id)); }  // 0x00428b70
u8* unit_get(i32 id) { return joan::orig::arg::as<u8*>(joan::orig::as_dynarray(kUnitTable)->ptr_at(joan::orig::arg::i32(id))); }  // 0x0041b3c0
u8* type_of(const void* unit) {
    return joan::orig::arg::as<u8*>(joan::orig::as_records_203(kTypes)->get(joan::orig::arg::i16(static_cast<i32>(jo::as_unit(unit)->type_id))));  // 0x0042d0a0
}

struct H {
    // ------------------------------------------------------------------ 0x006e00b0
    void begin(i8 kind) {
        c_begin.hit();
        if (jo::self<jo::GoalPanel>(this)->show_side != 0) return;
        jo::view<u8>(jo::self<jo::GoalPanel>(this)->show_side) = 1;
        jo::self<jo::GoalPanel>(this)->kind_side = kind;
        if (jo::self<jo::GoalPanel>(this)->kind_side == 0 || jo::self<jo::GoalPanel>(this)->kind_side == 1 || jo::self<jo::GoalPanel>(this)->kind_side == 4) {
            for (i32 i = joan::orig::as_dynarray(kUnitTable)->size(); i >= 1; --i) {  // 0x00417ad0
                if (unit_missing(i) != 0) continue;
                u8* const unit = unit_get(i);
                if (jo::as_unit_type(type_of(unit))->unit_class == 0 && jo::as_unit(unit)->draw != nullptr)
                    jo::as_unit(unit)->draw->u8_52 = 0;
            }
        }
        if (jo::self<jo::GoalPanel>(this)->show_main == 0) joan::orig::as_letter(this)->load_kill();  // 0x006e03d0
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Obj::begin", 0x006e00b0, &H::begin);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

using u8 = std::uint8_t;

using i8 = std::int8_t;

struct H {
    // ------------------------------------------------------------------ 0x006e0380
    u8 flag_8_or_9() { return (jo::self<jo::GoalPanel>(this)->show_main != 0 || jo::self<jo::GoalPanel>(this)->show_side != 0) ? 1 : 0; }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::flag_8_or_9", 0x006e0380, &H::flag_8_or_9);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- from modules/game_b85_05_2.cpp (game/b85_05)
namespace {
namespace game_b85_05_2 {

using joan::img::obj;

void* const kTextures = obj(d::g_texture_array);

void release_texture(void* t) { joan::orig::as_texture_manager(kTextures)->release(joan::orig::arg::mptr(t)); }         // 0x0060a550

struct H {
    // ------------------------------------------------------------------ 0x006d3540 / 0x006e0440 / 0x006e0b20
    void release_handle() {
        if (jo::view<void*>(jo::self<jo::GoalPanel>(this)->tex) != nullptr) {
            release_texture(jo::view<void*>(jo::self<jo::GoalPanel>(this)->tex));
            jo::view<void*>(jo::self<jo::GoalPanel>(this)->tex) = nullptr;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b85_05", name, addr, fn)
B("TexHandle::release_6e0440", 0x006e0440, &H::release_handle);
#undef B

}  // namespace game_b85_05_2
}  // namespace

// ---------------------------------------- from modules/game_b99_10_2.cpp (game/b99_10)
namespace {
namespace game_b99_10_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::crt_memset;

struct H {
    H* clear_1b() {  // 0x006e0490
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::Letter)));  // 0x1b on x86 (the Letter 0xa46040)
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::clear_1b", 0x006e0490, &H::clear_1b);
#undef B

}  // namespace game_b99_10_2
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i16 = std::int16_t;

struct H {
    // ------------------------------------------------------------------ 0x006e04c0
    void* init_6e04c0() {
        jo::self<jo::Letter>(this)->sheet = img::obj(d::g_bmp_res_letter);
        jo::self<jo::Letter>(this)->tex = nullptr;
        jo::self<jo::Letter>(this)->i16_d = 0;
        jo::self<jo::Letter>(this)->i32_13 = 0;
        jo::self<jo::Letter>(this)->text = nullptr;
        return this;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Obj6e04c0::init", 0x006e04c0, &H::init_6e04c0);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_ui6_3.cpp (game/ui6)
namespace {
namespace game_ui6_3 {

struct H {
    // 0x006e0dc0. ret 4.
    void set_flag_e(std::uint8_t v) { jo::self<jo::MiniMap>(this)->active = v; }
};

#define U6(name, addr, fn) JOAN_REPLACE("game/ui6", name, addr, fn)
U6("Panel::set_flag_e", 0x006e0dc0, &H::set_flag_e);
#undef U6

}  // namespace game_ui6_3
}  // namespace

// ---------------------------------------- from modules/game_b93_06_4.cpp (game/b93_06)
namespace {
namespace game_b93_06_4 {

using i32 = std::int32_t;

struct H {
    i32 get_6af() { return static_cast<i32>(jo::self<jo::UnitAttScreen>(this)->unit_id); }  // 0x006e0df0 (UnitAttrScreen::unit_6af)
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Obj6e::get_6af", 0x006e0df0, &H::get_6af);
#undef B93

}  // namespace game_b93_06_4
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::GoalPanel::release() -> void { joan::bind::member<&::game_b93_06_3::H::reset_6df140>(this); }  // 0x6df140
auto joan::orig::GoalPanel::open_first(std::int8_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b98_08::H::set_mode>(this, a0)); }  // 0x6dfe10
auto joan::orig::GoalPanel::close_first() -> void { joan::bind::member<&::game_b84_04_2::H::markers_a>(this); }  // 0x6dff10
auto joan::orig::GoalPanel::open_second(std::int8_t a0) -> void { joan::bind::member<&::game_b95_08::H::begin>(this, a0); }  // 0x6e00b0
auto joan::orig::GoalPanel::close_second() -> void { joan::bind::member<&::game_b84_04_2::H::markers_b>(this); }  // 0x6e01d0
auto joan::orig::Letter::load_kill() -> void { joan::bind::member<&::game_b93_06_2::H::load_kill>(this); }  // 0x6e03d0
auto joan::orig::GoalPanel::free_sprite() -> void { joan::bind::member<&::game_b85_05_2::H::release_handle>(this); }  // 0x6e0440
auto joan::orig::Letter::free() -> void { joan::bind::member<&::game_b93_06::H::letter_free>(this); }  // 0x6e0520
auto joan::orig::Letter::draw() -> void { joan::bind::member<&::game_ui6::H::letter_draw>(this); }  // 0x6e05a0
auto joan::orig::Letter::open() -> void { joan::bind::member<&::game_b100_16::H::letter_open>(this); }  // 0x6e0900
auto joan::orig::Letter::close() -> void { joan::bind::member<&::game_b84_04::H::letter_close>(this); }  // 0x6e09d0
auto joan::orig::Letter::get_8() -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::member<&::game_b91_04::H::letter_get_8>(this)); }  // 0x6e0a90
auto joan::orig::Letter::load_6e0ab0() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b99_10::H::letter_load>(this)); }  // 0x6e0ab0
auto joan::orig::Letter::release_texture() -> void { joan::bind::member<&::game_b85_05::H::release_handle>(this); }  // 0x6e0b20
auto joan::orig::Letter::load_6e0b70(std::int16_t a0) -> void { joan::bind::member<&::game_b92_06::H::letter_load>(this, a0); }  // 0x6e0b70
auto joan::orig::Panel::set_flag_e(std::uint8_t a0) -> void { joan::bind::member<&::game_ui6_3::H::set_flag_e>(this, a0); }  // 0x6e0dc0
auto joan::orig::UnitAttrScreen::unit_6af() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_06_4::H::get_6af>(this)); }  // 0x6e0df0
