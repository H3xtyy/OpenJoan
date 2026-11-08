// ilib/bitmapt.cpp: functions of the original d:\ww\ilib\bitmapt.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
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
//      0x0057f520 Bitmap24::create cdecl, 0x0057f5f0 Bitmap16::pitch.
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

JOAN_COUNTER(c_create24, "Bitmap24::create");

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
// 0x0057f520
void* JOAN_CDECL bitmap24_create(i32 w, i32 h) {
    c_create24.hit();
    return bitmap_create(w, h, d::g_a15104, d::s_d_ww_ilib_bitmapt_cpp, fn::code_value(fn::bitmap_bytes24), fn::code_value(fn::Point16_set_57f3d0));  // 0x0057f4f0 / 0x0057f3d0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Bitmap24::create", 0x0057f520, &bitmap24_create);
#undef B

}  // namespace game_b103_07
}  // namespace

// ---------------------------------------- from modules/game_b98_05.cpp (game/b98_05)
//
//    - Point16::set 0x0057f3d0 (short, short), ret 8.
namespace {
namespace game_b98_05 {

using joan::orig::helpers::at;

struct H {

    // 0x0057f3d0
    void set_xy(std::int16_t x, std::int16_t y) {
        jo::self<jo::Bitmap24>(this)->width_ = x;
        jo::self<jo::Bitmap24>(this)->height_ = y;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_05", name, addr, fn)
B("Point16::set", 0x0057f3d0, &H::set_xy);
#undef B

}  // namespace game_b98_05
}  // namespace

// ---------------------------------------- from modules/game_b99_06.cpp (game/b99_06)
//
//    - bitmap_bytes24 0x0057f4f0 (cdecl, short, short): a * b * 3 + 4.
namespace {
namespace game_b99_06 {

using i16 = std::int16_t;
using i32 = std::int32_t;

// 0x0057f4f0
i32 JOAN_CDECL bitmap_bytes24(i16 a, i16 b) { return static_cast<i32>(a) * static_cast<i32>(b) * 3 + 4; }

#define B(name, addr, fn) JOAN_REPLACE("game/b99_06", name, addr, fn)
B("bitmap_bytes24", 0x0057f4f0, &bitmap_bytes24);
#undef B

}  // namespace game_b99_06
}  // namespace

// ---------------------------------------- from modules/game_b105_15.cpp (game/b105_15)
namespace {
namespace game_b105_15 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

using u16 = std::uint16_t;

struct H {
    // 0x0057ef60 / ...: mov ax, word [this] - upper half of eax is this' upper half.
    u32 get_word() {
        // bits 16..31 of eax were `this`' in the original (callers read ax only); zero here, no pointer bits in a u32.
        return static_cast<u32>(jo::view<u16>(jo::self<jo::Bitmap24>(this)->width_));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Obj::get_word_57f410", 0x0057f410, &H::get_word);
#undef B

}  // namespace game_b105_15
}  // namespace

// ---------------------------------------- from modules/game_b103_07_2.cpp (game/b103_07)
namespace {
namespace game_b103_07_2 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;

struct H {
    u8* self() { return reinterpret_cast<u8*>(this); }

    // 0x0057f430
    i32 bm24_pitch() { return jo::self<jo::Bitmap24>(this)->width_ * 3; }
    // 0x0057f460
    u8* bm24_pixel(i32 x, i32 y) {
        const u32 off = static_cast<u32>(static_cast<i32>(static_cast<i16>(y)) * jo::self<jo::Bitmap24>(this)->width_) * 3u + 4u;
        // 32-bit wrap as the original (a negative x / y moves back): the sum as a signed offset
        return self() + static_cast<i32>(off + static_cast<u32>(static_cast<i32>(static_cast<i16>(x))) * 3u);
    }
    // 0x0057f4b0
    i32 bm24_bytes() { return static_cast<i32>(static_cast<u32>(jo::self<jo::Bitmap24>(this)->width_ * jo::self<jo::Bitmap24>(this)->height_) * 3u + 4u); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Bitmap24::pitch", 0x0057f430, &H::bm24_pitch);
B("Bitmap24::pixel", 0x0057f460, &H::bm24_pixel);
B("Bitmap24::bytes", 0x0057f4b0, &H::bm24_bytes);
#undef B

}  // namespace game_b103_07_2
}  // namespace
