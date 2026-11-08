// ilib/bitmapm.cpp: functions of the original d:\ww\ilib\bitmapm.cpp (re/attribution.csv).
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
//      0x0057f360 Bitmap1::create cdecl, 0x0057f430 Bitmap24::pitch,
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

JOAN_COUNTER(c_create1, "Bitmap1::create");

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
// 0x0057f360
void* JOAN_CDECL bitmap1_create(i32 w, i32 h) {
    c_create1.hit();
    return bitmap_create(w, h, d::g_a15100, d::s_d_ww_ilib_bitmapm_cpp, fn::code_value(fn::bitmap_bytes), fn::code_value(fn::Point16_set_57f2a0));  // 0x0057f320 / 0x0057f2a0
}

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Bitmap1::create", 0x0057f360, &bitmap1_create);
#undef B

}  // namespace game_b103_07
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
        jo::view<u16>(jo::self<jo::Bitmap1>(this)->width_) = static_cast<u16>(a);
        jo::view<u16>(jo::self<jo::Bitmap1>(this)->height_) = static_cast<u16>(b);
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_10", name, addr, fn)
B("Point16::set_57f2a0", 0x0057f2a0, &H::set_point16);
#undef B

}  // namespace game_b100_10
}  // namespace

// ---------------------------------------- from modules/game_b94_05.cpp (game/b94_05)
//
//    - bitmap_bytes 0x0057f320 (cdecl, short, short): (a * b + 7) / 8 + 4.
namespace {
namespace game_b94_05 {

// ---- small functions ----
// 0x0057f320
std::int32_t JOAN_CDECL bitmap_bytes(std::int16_t a, std::int16_t b) {
    return (static_cast<std::int32_t>(a) * static_cast<std::int32_t>(b) + 7) / 8 + 4;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b94_05", name, addr, fn)
B("bitmap_bytes", 0x0057f320, &bitmap_bytes);
#undef B

}  // namespace game_b94_05
}  // namespace

// ---------------------------------------- from modules/game_b103_07_2.cpp (game/b103_07)
namespace {
namespace game_b103_07_2 {

using joan::orig::helpers::at;

using i16 = std::int16_t;
using i32 = std::int32_t;

struct H {

    // 0x0057f2e0
    i32 bm1_bytes() { return (jo::self<jo::Bitmap1>(this)->width_ * jo::self<jo::Bitmap1>(this)->height_ + 7) / 8 + 4; }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_07", name, addr, fn)
B("Bitmap1::bytes", 0x0057f2e0, &H::bm1_bytes);
#undef B

}  // namespace game_b103_07_2
}  // namespace
