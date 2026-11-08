// game/unknown_fadetitl.cpp: functions whose original source file is unknown (formerly game/_unplaced/fadetitl.cpp;
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

using joan::orig::helpers::u;

using joan::img::str;

using joan::orig::helpers::at;

using i8 = std::int8_t;

using u8 = std::uint8_t;

}  // namespace

// ---------------------------------------- from modules/game_b99_10.cpp (game/b99_10)
namespace {
namespace game_b99_10 {

using joan::orig::helpers::crt_memset;

struct H {
    H* clear_30() {  // 0x007612a0
        crt_memset(this, 0, 0x30);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_10", name, addr, fn)
B("Obj::clear_30", 0x007612a0, &H::clear_30);
#undef B

}  // namespace game_b99_10
}  // namespace

// ---------------------------------------- from modules/game_b105_20.cpp (game/b105_20)
namespace {
namespace game_b105_20 {

struct H {
    // ------------------------------------------------------------------ empty stubs
    void nop() {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_20", name, addr, fn)
B("Obj::nop_7612d0", 0x007612d0, &H::nop);
#undef B

}  // namespace game_b105_20
}  // namespace

// ---------------------------------------- from modules/game_b103_16.cpp (game/b103_16)
namespace {
namespace game_b103_16 {

void* game_memset(void* p, i32 v, u32 n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

struct H {
    void nop() {}

    void* clear30() { return game_memset(this, 0, 0x30); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_16", name, addr, fn)
B("Screen::nop_7612f0", 0x007612f0, &H::nop);
B("Obj::clear30_761310", 0x00761310, &H::clear30);
#undef B

}  // namespace game_b103_16
}  // namespace

// ---------------------------------------- from modules/game_ui8.cpp (game/ui8)
namespace {
namespace game_ui8 {

using joan::call::thiscall;

using joan::img::obj;

constexpr img::Addr kFont(d::g_font_a53f28);

JOAN_COUNTER(c_fade, "FadeTitle::draw");

// 0x0042a260 Color(r, g, b, a) on a local; only the low bytes of the arguments are read.
std::uint32_t color(std::uint32_t r, std::uint32_t g, std::uint32_t b, std::uint32_t a) {
    std::uint32_t c = 0;
    std::uint32_t* const p = joan::orig::arg::as<std::uint32_t*>(joan::orig::as_colour(&c)->construct(r, g, b, a));  // 0x0042a260
    return *p;
}

struct H {
    // 0x00761340. ret 0.
    void fade_draw() {
        if (static_cast<std::int8_t>(jo::self<jo::FadeTitle>(this)->active) == 0) return;
        c_fade.hit();
        std::uint8_t text_alpha = 0xff;
        std::uint8_t back_alpha = 0xff;
        const char* const title = &jo::self<jo::FadeTitle>(this)->name;
        if (global<std::uint32_t>(d::g_now) < jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->a) + jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->c)) {
            text_alpha = static_cast<std::uint8_t>(((global<std::uint32_t>(d::g_now) - jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->a)) * 0xffu) / jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->c));
            if (joan::orig::arg::as<std::int32_t>(std::strlen(joan::orig::arg::str(title))) == 0) {  // CRT strlen
                back_alpha = static_cast<std::uint8_t>(((global<std::uint32_t>(d::g_now) - jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->a)) * 0xffu) / jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->c));
            }
        } else if (global<std::uint32_t>(d::g_now) > jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->a) + jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->b)) {
            text_alpha = 0;
            back_alpha = 0;
        } else if (global<std::uint32_t>(d::g_now) > jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->a) + jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->b) - jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->d)) {
            text_alpha = static_cast<std::uint8_t>(((jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->a) + jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->b) - global<std::uint32_t>(d::g_now)) * 0xffu) / jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->d));
            back_alpha = static_cast<std::uint8_t>(((jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->a) + jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->b) - global<std::uint32_t>(d::g_now)) * 0xffu) / jo::view<std::uint32_t>(jo::self<jo::FadeTitle>(this)->d));
        }
        {
            const std::uint32_t c = color(0, 0, 0, back_alpha);
            joan::orig::as_sprite2d(obj(d::g_sprites_a46128))->queue(0, 0, joan::orig::arg::i32(global<std::uint32_t>(d::g_screen_w) - 1u), joan::orig::arg::i32(// 0x00632a70
                                   global<std::uint32_t>(d::g_screen_h) - 1u), 1u, joan::orig::arg::u32(c), joan::orig::arg::mptr(0u), 3, 1, 1, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0);
        }
        const std::int16_t width = joan::orig::as_font(img::obj(kFont))->measure(joan::orig::arg::str(title), -1, 0, 0, joan::orig::arg::f32(0x40a00000u));  // 0x00624f00
        const std::int16_t height = joan::orig::arg::as<std::int16_t>(joan::orig::as_font(img::obj(kFont))->text_height(joan::orig::arg::i16(2), joan::orig::arg::f32(0x40a00000u)));             // 0x006253f0
        const std::int16_t x = static_cast<std::int16_t>(
            static_cast<std::int32_t>(global<std::uint32_t>(d::g_screen_w) - static_cast<std::uint32_t>(static_cast<std::int32_t>(width))) / 2);
        const std::int16_t y = static_cast<std::int16_t>(
            static_cast<std::int32_t>(global<std::uint32_t>(d::g_screen_h) - static_cast<std::uint32_t>(static_cast<std::int32_t>(height))) / 2);
        const std::uint32_t c = color(0xff, 0xff, 0xff, text_alpha);
        // x / y pushed as `mov cx/ax, word; push ecx/eax` with the colour in the upper half.
        const std::uint32_t hi = c & 0xffff0000u;
        joan::orig::as_text(img::obj(kFont))->draw_line(joan::orig::arg::i32(hi | static_cast<std::uint16_t>(x)), joan::orig::arg::i32(hi | static_cast<std::uint16_t>(y)), 1u, title, joan::orig::arg::u32(c), joan::orig::arg::i32(// 0x006255d0
                 0xffffffffu), 0, 1, 2, 1, joan::orig::arg::f32(0x40a00000u), joan::orig::arg::f32(0x40a00000u));
    }
};

#define U8(name, addr, fn) JOAN_REPLACE("game/ui8", name, addr, fn)
U8("FadeTitle::draw", 0x00761340, &H::fade_draw);
#undef U8

}  // namespace game_ui8
}  // namespace

// ---------------------------------------- from modules/game_b94_08.cpp (game/b94_08)
namespace {
namespace game_b94_08 {

struct H {
    bool is_state_13() {  // 0x007615e0
        if (static_cast<i8>(jo::self<jo::FadeTitle>(this)->active) == 0) return false;
        return global<i32>(d::g_key) == 0xd;
    }
};

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_08", name, addr, fn)
B94("X::is_state_13", 0x007615e0, &H::is_state_13);
#undef B94

}  // namespace game_b94_08
}  // namespace

// ---------------------------------------- from modules/game_b95_08.cpp (game/b95_08)
namespace {
namespace game_b95_08 {

struct H {
    // ------------------------------------------------------------------ 0x00761630
    std::uintptr_t expire() {
        if (jo::view<i8>(jo::self<jo::Expiry>(this)->active) == 0) return u(this);
        const u32 sum = jo::self<jo::Expiry>(this)->start + jo::self<jo::Expiry>(this)->duration;
        if (global<u32>(d::g_now) > sum) jo::view<u8>(jo::self<jo::Expiry>(this)->active) = 0;
        return sum;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_08", name, addr, fn)
B95("Timer::expire", 0x00761630, &H::expire);
#undef B95

}  // namespace game_b95_08
}  // namespace

// ---------------------------------------- from modules/game_b96_08.cpp (game/b96_08)
namespace {
namespace game_b96_08 {

JOAN_COUNTER(c_record_set, "Record::set");

struct H {
    // ------------------------------------------------------------------ 0x00761680
    char* record_set(i32 a, const char* name, i32 b, i32 c, i32 d) {
        c_record_set.hit();
        jo::self<jo::FadeTitle>(this)->active = 1;
        jo::self<jo::FadeTitle>(this)->a = a;
        jo::self<jo::FadeTitle>(this)->b = b;
        jo::self<jo::FadeTitle>(this)->c = c;
        jo::self<jo::FadeTitle>(this)->d = d;
#if defined(__GNUC__) && !defined(__clang__)
// false positive: FadeTitle::name is a 0x1e-byte field in the original; the generated layout types it as one char
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
        return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(&jo::self<jo::FadeTitle>(this)->name)), joan::orig::arg::str(name), 0x1eu);  // strncpy
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_08", name, addr, fn)
B96("Record::set", 0x00761680, &H::record_set);
#undef B96

}  // namespace game_b96_08
}  // namespace

// ---------------------------------------- from modules/game_b97_08.cpp (game/b97_08)
namespace {
namespace game_b97_08 {

struct H {
    // ------------------------------------------------------------------ 0x007616f0
    u32 is_empty_string() { return jo::view<i8>(jo::self<jo::FadeTitle>(this)->active) == 0 ? 1u : 0u; }
};

#define B97(name, addr, fn) JOAN_REPLACE("game/b97_08", name, addr, fn)
B97("Obj::is_empty_string", 0x007616f0, &H::is_empty_string);
#undef B97

}  // namespace game_b97_08
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::FadeTitle::zero() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_10::H::clear_30>(this)); }  // 0x7612a0
auto joan::orig::Screen::nop_7612f0() -> void { joan::bind::member<&::game_b103_16::H::nop>(this); }  // 0x7612f0
auto joan::orig::FadeTitle::draw() -> void { joan::bind::member<&::game_ui8::H::fade_draw>(this); }  // 0x761340
auto joan::orig::FadeTitle::enter_pressed() -> bool { return joan::bind::conv<bool>(joan::bind::member<&::game_b94_08::H::is_state_13>(this)); }  // 0x7615e0
auto joan::orig::FadeTitle::show(std::int32_t a0, const char* a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) -> const char* { return joan::bind::conv<const char*>(joan::bind::member<&::game_b96_08::H::record_set>(this, a0, a1, a2, a3, a4)); }  // 0x761680
auto joan::orig::FadeTitle::is_idle() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b97_08::H::is_empty_string>(this)); }  // 0x7616f0
