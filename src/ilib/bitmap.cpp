// ilib/bitmap.cpp: functions of the original d:\ww\ilib\bitmap.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include <cstdint>
#include <cstring>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/layout_access.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b103_07.cpp (game/b103_07)
//
//      0x0057f080 Bitmap8::create(w, h) cdecl, 0x0057f150 Bitmap32::pitch,
namespace {
namespace game_b103_07 {

using joan::hook::global;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

JOAN_COUNTER(c_create8, "Bitmap8::create");

// ---------------------------------------------------------------- bitmap allocators
// Shared shape of 0x0057f080 / 0x0057f230 / 0x0057f360 / 0x0057f520:
// size = size_fn(w, h); p = Alloc(0xa48838)(size, file, [line_global] + 1); ctor(p, w, h).
void* bitmap_create(i32 w, i32 h, img::Addr line_global, img::Addr file,
                    std::uintptr_t size_fn, std::uintptr_t ctor) {
    const i32 line = static_cast<i32>(global<i16>(line_global)) + 1;
    const i32 ws = static_cast<i16>(w);
    const i32 hs = static_cast<i16>(h);
    const i32 size = cdecl_call<i32>(size_fn, ws, hs);
    void* const p = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), joan::orig::arg::str(str(file)), joan::orig::arg::i32(line));  // 0x0058e7b0
    thiscall(ctor, p, ws, hs);
    return p;
}
// 0x0057f080
void* JOAN_CDECL bitmap8_create(i32 w, i32 h) {
    c_create8.hit();
    return bitmap_create(w, h, d::g_a150f8, d::s_d_ww_ilib_bitmap_cpp, fn::code_value(fn::bitmap_bytes8), fn::code_value(fn::Point16_set_57ef20));  // 0x0057f050 / 0x0057ef20
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Bitmap8::create", 0x0057f080, &bitmap8_create);
#undef B

}  // namespace game_b103_07
}  // namespace

// ---------------------------------------- from modules/game_b99_06.cpp (game/b99_06)
//
//    - Point16::set 0x0057ef20 (short, short), ret 8.
namespace {
namespace game_b99_06 {

using i16 = std::int16_t;


struct H {

    // 0x0057ef20
    void set_point16(i16 a, i16 b) {
        jo::self<jo::Bitmap8>(this)->width_ = a;
        jo::self<jo::Bitmap8>(this)->height_ = b;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_06", name, addr, fn)
B("Point16::set", 0x0057ef20, &H::set_point16);
#undef B

}  // namespace game_b99_06
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
//
//    - bitmap_bytes8 0x0057f050 (cdecl, short, short): a * b + 4;
//      bitmap_bytes16 0x0057f690 (cdecl, short, short): a * b * 2 + 4.
namespace {
namespace game_b96_05 {

// ---- small cdecl ----
// 0x0057f050
std::int32_t JOAN_CDECL bitmap_bytes8(std::int16_t a, std::int16_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a) * static_cast<std::uint32_t>(static_cast<std::int32_t>(b)) + 4u);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("bitmap_bytes8", 0x0057f050, &bitmap_bytes8);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
namespace {
namespace game_b105_15 {

using u32 = std::uint32_t;


using u16 = std::uint16_t;

struct H {
    // 0x0057ef60 / ...: mov ax, word [this] - upper half of eax is this' upper half.
    u32 get_word() {
        // The original leaves bits 16..31 of eax as `this`' (a 32-bit address there); callers read ax only (no C++ caller
        // exists), so the word is returned zero-extended: no pointer bits in a 32-bit value on x64.
        return static_cast<u32>(jo::view<u16>(jo::self<jo::Bitmap8>(this)->width_));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Obj::get_word_57ef60", 0x0057ef60, &H::get_word);
B("Obj::get_word_57ef80", 0x0057ef80, &H::get_word);
#undef B

}  // namespace game_b105_15
}  // namespace

// ---------------------------------------- from modules/game_b103_07.cpp (game/b103_07)
namespace {
namespace game_b103_07_2 {

using u8 = std::uint8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;


struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // ------------------------------------------------------------ bitmaps
    // 0x0057efa0
    u8* bm8_pixel(i32 x, i32 y) {
        return self() + static_cast<i16>(y) * jo::self<jo::Bitmap8>(this)->width_ + 4 + static_cast<i16>(x);
    }
    // 0x0057f020
    i32 bm8_bytes() { return jo::self<jo::Bitmap8>(this)->width_ * jo::self<jo::Bitmap8>(this)->height_ + 4; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Bitmap8::pixel", 0x0057efa0, &H::bm8_pixel);
B("Bitmap8::bytes", 0x0057f020, &H::bm8_bytes);
#undef B

}  // namespace game_b103_07_2
}  // namespace

// ---------------------------------------- from modules/game_b108_01.cpp (game/b108_01)
namespace {
namespace game_b108_01 {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::stdcall_call;

struct H {
    // -------------------------------------------------------------- 0x0057efe0
    u32 glyph_57efe0(u32 a, u32 b) {
        return joan::orig::arg::as<u32>(joan::orig::font::pixel_575940(joan::orig::arg::mptr(static_cast<void*>(&jo::self<jo::Font>(this)->name[3])), joan::orig::arg::u32(a), joan::orig::arg::u32(b)));  // 0x00575940
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_01", name, addr, fn)
B("Font::glyph_57efe0", 0x0057efe0, &H::glyph_57efe0);
#undef B

}  // namespace game_b108_01
}  // namespace
