// ilib/bitmapw.cpp: functions of the original d:\ww\ilib\bitmapw.cpp (re/attribution.csv).
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
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b103_08.cpp (game/b103_08)
//
//    - Bitmap::create 0x0057f6c0 (cdecl, short w, short h): 0x0058e7b0 on the allocator 0xa48838
//      (size = 0x0057f690(w, h), 'd:\ww\ilib\bitmapw.cpp', short [0xa15108] + 1), then
//      0x0057f590(w, h) on the block; returns the block (no null check, as the original). Dead.
//
//  - 0x0057f6c0 pushes the shorts as 32-bit registers with stale upper halves; the callees
//    (0x0057f690 / 0x0057f590) read words only, so the arguments are passed through unchanged.
namespace {
namespace game_b103_08 {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::img::obj;
using joan::img::str;

JOAN_COUNTER(c_create, "Bitmap::create");

// ---------------------------------------------------------------------------- 0x0057f6c0
static void* JOAN_CDECL bitmap_create(u32 w, u32 h) {
    c_create.hit();
    const i32 line = static_cast<i32>(global<i16>(d::g_a15108)) + 1;
    const u32 size = joan::orig::arg::as<u32>(joan::orig::bitmap_bytes16(joan::orig::arg::i16(w), joan::orig::arg::i16(h)));  // 0x0057f690
    void* const p = joan::orig::as_mem(obj(d::g_mem))->add(joan::orig::arg::u32(size), str(d::s_d_ww_ilib_bitmapw_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0
    joan::orig::as_bitmap(p)->set_size(joan::orig::arg::u32(w), joan::orig::arg::u32(h));  // 0x0057f590
    return p;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("Bitmap::create", 0x0057f6c0, &bitmap_create);
#undef B

}  // namespace game_b103_08
}  // namespace

// ---------------------------------------- from modules/game_b100_10.cpp (game/b100_10)
//
//    - Point16::set 0x0057f2a0 / 0x0057f590 (short, short), ret 8.
namespace {
namespace game_b100_10 {

using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {

    void set_point16(u32 a, u32 b) {                                          // 0x0057f2a0 / 0x0057f590
        jo::view<u16>(jo::self<jo::Bitmap16>(this)->width_) = static_cast<u16>(a);
        jo::view<u16>(jo::self<jo::Bitmap16>(this)->height_) = static_cast<u16>(b);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Point16::set_57f590", 0x0057f590, &H::set_point16);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
//
//    - bitmap_bytes8 0x0057f050 (cdecl, short, short): a * b + 4;
//      bitmap_bytes16 0x0057f690 (cdecl, short, short): a * b * 2 + 4.
namespace {
namespace game_b96_05 {

// 0x0057f690
std::int32_t JOAN_CDECL bitmap_bytes16(std::int16_t a, std::int16_t b) {
    const std::uint32_t p = static_cast<std::uint32_t>(static_cast<std::int32_t>(a)) * static_cast<std::uint32_t>(static_cast<std::int32_t>(b));
    return static_cast<std::int32_t>(p + p + 4u);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("bitmap_bytes16", 0x0057f690, &bitmap_bytes16);
#undef B

}  // namespace game_b96_05
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
        return static_cast<u32>(jo::view<u16>(jo::self<jo::Bitmap16>(this)->width_));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Obj::get_word_57f5d0", 0x0057f5d0, &H::get_word);
#undef B

}  // namespace game_b105_15
}  // namespace

// ---------------------------------------- from modules/game_b103_07.cpp (game/b103_07)
namespace {
namespace game_b103_07 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

using i16 = std::int16_t;
using i32 = std::int32_t;

struct H {

    // 0x0057f5f0
    i32 bm16_pitch() { return static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::self<jo::Bitmap16>(this)->width_)) << 1); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Bitmap16::pitch", 0x0057f5f0, &H::bm16_pitch);
#undef B

}  // namespace game_b103_07
}  // namespace

// ---------------------------------------- from modules/game_b103_08.cpp (game/b103_08)
namespace {
namespace game_b103_08_2 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

using i16 = std::int16_t;
using i32 = std::int32_t;

struct H {

    // ------------------------------------------------------------------------ bitmap / arrays
    std::uint8_t* pixel_ptr(u32 x, u32 y) {
        const u32 w = static_cast<u32>(static_cast<i32>(jo::self<jo::Bitmap16>(this)->width_));
        const u32 yy = static_cast<u32>(static_cast<i32>(static_cast<i16>(y)));
        const u32 xx = static_cast<u32>(static_cast<i32>(static_cast<i16>(x)));
        // 32-bit wrap as the original (a negative x / y moves back): the offset as a signed number
        return reinterpret_cast<std::uint8_t*>(this) + static_cast<i32>((yy * w) * 2 + 4 + xx * 2);
    }
    i32 byte_size() {
        return static_cast<i32>(jo::self<jo::Bitmap16>(this)->width_) * static_cast<i32>(jo::self<jo::Bitmap16>(this)->height_) * 2 + 4;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("Bitmap::pixel_ptr", 0x0057f620, &H::pixel_ptr);
B("Bitmap::byte_size", 0x0057f660, &H::byte_size);
#undef B

}  // namespace game_b103_08_2
}  // namespace

// ---------------------------------------- from modules/game_b103_08_3.cpp (game/b103_08)
namespace {
namespace game_b103_08_3 {

using u32 = std::uint32_t;

using joan::orig::helpers::at;

struct H {

    H* ctor_57f730() {
        jo::self<jo::Blob>(this)->data = nullptr;
        jo::view<u32>(jo::self<jo::Blob>(this)->size) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_08", name, addr, fn)
B("Pair::ctor_57f730", 0x0057f730, &H::ctor_57f730);
#undef B

}  // namespace game_b103_08_3
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Bitmap::set_size(std::uint32_t a0, std::uint32_t a1) -> void { joan::bind::member<&::game_b100_10::H::set_point16>(this, a0, a1); }  // 0x57f590
auto joan::orig::bitmap_bytes16(std::int16_t a0, std::int16_t a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b96_05::bitmap_bytes16>(a0, a1)); }  // 0x57f690
