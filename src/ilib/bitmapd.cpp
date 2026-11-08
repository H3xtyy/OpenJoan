// ilib/bitmapd.cpp: functions of the original d:\ww\ilib\bitmapd.cpp (re/attribution.csv).
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

namespace jo = joan::orig;

// ---------------------------------------- from modules/game_b103_07.cpp (game/b103_07)
//
//      0x0057f230 Bitmap32::create cdecl, 0x0057f2e0 Bitmap1::bytes,
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

JOAN_COUNTER(c_create32, "Bitmap32::create");

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
// 0x0057f230
void* JOAN_CDECL bitmap32_create(i32 w, i32 h) {
    c_create32.hit();
    return bitmap_create(w, h, d::g_a150fc, d::s_d_ww_ilib_bitmapd_cpp, fn::code_value(fn::bitmap32_bytes), fn::code_value(fn::Size16_set));  // 0x0057f200 / 0x0057f0f0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Bitmap32::create", 0x0057f230, &bitmap32_create);
#undef B

}  // namespace game_b103_07
}  // namespace

// ---------------------------------------- from modules/game_b95_05.cpp (game/b95_05)
//
//    - Size16::set 0x0057f0f0 (short, short), ret 8.
namespace {
namespace game_b95_05 {

using joan::orig::helpers::at;

struct H {

    // 0x0057f0f0
    void size_set(std::uint16_t a, std::uint16_t b) {
        jo::view<std::uint16_t>(jo::self<jo::Bitmap32>(this)->width_) = a;
        jo::view<std::uint16_t>(jo::self<jo::Bitmap32>(this)->height_) = b;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b95_05", name, addr, fn)
B("Size16::set", 0x0057f0f0, &H::size_set);
#undef B

}  // namespace game_b95_05
}  // namespace

// ---------------------------------------- from modules/game_b97_05.cpp (game/b97_05)
//
//    - bitmap32_bytes 0x0057f200 (cdecl, short, short): a * b * 4 + 4.
namespace {
namespace game_b97_05 {

// 0x0057f200
std::int32_t JOAN_CDECL bitmap32_bytes(std::int16_t a, std::int16_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(static_cast<std::int32_t>(a) * static_cast<std::int32_t>(b)) *
                                         4u +
                                     4u);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b97_05", name, addr, fn)
B("bitmap32_bytes", 0x0057f200, &bitmap32_bytes);
#undef B

}  // namespace game_b97_05
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
        return static_cast<u32>(jo::view<u16>(jo::self<jo::Bitmap32>(this)->width_));
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b105_15", name, addr, fn)
B("Obj::get_word_57f130", 0x0057f130, &H::get_word);
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

    // 0x0057f150
    i32 bm32_pitch() { return static_cast<i32>(static_cast<u32>(static_cast<i32>(jo::self<jo::Bitmap32>(this)->width_)) << 2); }
    // 0x0057f180
    u8* bm32_pixel(i32 x, i32 y) {
        const u32 off = static_cast<u32>(static_cast<i32>(static_cast<i16>(y)) * jo::self<jo::Bitmap32>(this)->width_) * 4u + 4u;
        // 32-bit wrap as the original (a negative x / y moves back): the sum as a signed offset
        return self() + static_cast<i32>(off + static_cast<u32>(static_cast<i32>(static_cast<i16>(x))) * 4u);
    }
    // 0x0057f1c0
    i32 bm32_bytes() { return static_cast<i32>(static_cast<u32>(jo::self<jo::Bitmap32>(this)->width_ * jo::self<jo::Bitmap32>(this)->height_) * 4u + 4u); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Bitmap32::pitch", 0x0057f150, &H::bm32_pitch);
B("Bitmap32::pixel", 0x0057f180, &H::bm32_pixel);
B("Bitmap32::bytes", 0x0057f1c0, &H::bm32_bytes);
#undef B

}  // namespace game_b103_07_2
}  // namespace
