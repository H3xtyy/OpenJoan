// en3d/vgalock.cpp: functions of the original d:\ww\en3d\vgalock.cpp (re/attribution.csv).
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
#include <cmath>
#include <limits>
#include <cstdarg>
#include "../d3d8/d3d8_api.hpp"
#include "../original/vecmath.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;

using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::at;
using joan::img::obj;
using joan::img::str;

void* const kHeap = obj(d::g_mem);

}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
//
//    - VgaLock::release 0x00613210 (ret 0): 0x6134a0, heap free of +0 ("vgalock.cpp").
namespace {
namespace game_b103_12 {

using joan::call::thiscall;

struct H {
    // eax as the original leaves it: this (uintptr_t: the whole register on x64)
    std::uintptr_t self() { return reinterpret_cast<std::uintptr_t>(this); }

    // ------------------------------------------------------------------ 0x00613210
    std::uintptr_t vgalock_release() {
        joan::orig::as_vga_lock(this)->always_1();  // 0x006134a0
        if (jo::self<jo::VgaLock>(this)->buffer == nullptr) return self();
        joan::orig::as_mem(kHeap)->del(joan::orig::arg::mptr(jo::view<void*>(jo::self<jo::VgaLock>(this)->buffer)), joan::orig::arg::str(str(d::s_d_ww_en3d_vgalock_cpp)),
                                       joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a18d5c)) + 3));  // 0x0058ed70 "vgalock.cpp"
        return 0;  // no callers (0x00613210 is never called)
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("VgaLock::release", 0x00613210, &H::vgalock_release);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
//
//    - VgaLock::ensure_buffer 0x00613270 (ret 0): 0x400-byte buffer (0x58e7b0 "vgalock.cpp"),
//      returns 1 (the free branch behind `xor eax,eax; test eax,eax` is dead, no calls).
namespace {
namespace game_b99_07 {

using u8 = std::uint8_t;

struct H {
    // ------------------------------------------------------------------ 0x00613270
    i32 ensure_buffer() {
        if (jo::self<jo::VgaLock>(this)->buffer == nullptr) {
            jo::view<void*>(jo::self<jo::VgaLock>(this)->buffer) = joan::orig::as_mem(kHeap)->add(0x400u, str(d::s_d_ww_en3d_vgalock_cpp), joan::orig::arg::i32(static_cast<i32>(global<i16>(d::g_a18d60)) + 3));  // 0x0058e7b0
        }
        return 1;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("VgaLock::ensure_buffer", 0x00613270, &H::ensure_buffer);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
//
//    - Obj::ctor_6131c0 0x006131c0 (arg), ret 4: +0 = 0; 0x613270 && arg -> 0x613310(arg).
namespace {
namespace game_b103_12_2 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::cdecl_call;
using joan::orig::helpers::at;

struct H {
    // ------------------------------------------------------------------ 0x006131c0
    H* ctor_6131c0(const char* arg) {  // arg: the .act palette path (a pointer; u32 truncated it on x64)
        jo::self<jo::VgaLock>(this)->buffer = nullptr;
        if (joan::orig::arg::as<i32>(joan::orig::as_vga_lock(this)->ensure_buffer()) != 0 && arg != nullptr) {  // 0x00613270
            joan::orig::palette::load_act(joan::orig::arg::str(arg));                     // 0x00613310
        }
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::ctor_6131c0", 0x006131c0, &H::ctor_6131c0);
#undef B

}  // namespace game_b103_12_2
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
//
//      0x006134a0 Obj::always_1                     (thiscall, ret 0) - 1.
namespace {
namespace game_b96_06 {

using joan::orig::helpers::at;

using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x006134a0
    i32 always_1() {
        (void)*static_cast<std::uint8_t* volatile*>(&jo::self<jo::VgaLock>(this)->buffer);  // the original reads [this]
        return 1;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Obj::always_1", 0x006134a0, &H::always_1);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b94_06.cpp (game/b94_06)
namespace {
namespace game_b94_06 {

using i32 = std::int32_t;

using joan::img::str;

JOAN_COUNTER(c_palette, "Palette::load_act");

// 0x00613310 Palette::load_act(name)
std::int32_t JOAN_CDECL palette_load(const char* file_name) {
    c_palette.hit();
    struct alignas(4) File {
        std::uint8_t bytes[((sizeof(joan::orig::File) > 0x110 ? sizeof(joan::orig::File) : 0x110) + 4 - 1) / 4 * 4];  // 0x110 on x86; File holds a HANDLE
    };
    File file;
    std::uint8_t rgb[0x300];
    std::uint8_t table[0x400];
    joan::orig::as_file(&file)->construct();                     // 0x004179c0
    joan::orig::as_file(&file)->file_open(joan::orig::arg::str(file_name), 1, static_cast<char>(0));  // 0x00586da0 open
    joan::orig::as_file(&file)->file_seek(8, joan::orig::arg::i32(-1));  // 0x00587ea0 seek
    joan::orig::as_file(&file)->file_read(static_cast<void*>(rgb), 0x300u);                         // 0x005873a0 read
    joan::orig::as_file(&file)->file_close();                             // 0x00587260 close
    for (std::int32_t i = 0; i < 0x100; ++i) {  // local, discarded (as in the original)
        table[i * 4] = rgb[i * 3];
        table[i * 4 + 1] = rgb[i * 3 + 1];
        table[i * 4 + 2] = rgb[i * 3 + 2];
        table[i * 4 + 3] = 0;
    }
    static_cast<void>(table);
    joan::orig::as_file(&file)->destruct();  // 0x005872b0 ~File
    return 0;
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_06", name, addr, fn)
B94("VgaCustomPalette::set_custom_palette", 0x00613310, &palette_load);
#undef B94

}  // namespace game_b94_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::VgaLock::ensure_buffer() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b99_07::H::ensure_buffer>(this)); }  // 0x613270
auto joan::orig::palette::load_act(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b94_06::palette_load>(a0)); }  // 0x613310
auto joan::orig::VgaLock::always_1() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_06::H::always_1>(this)); }  // 0x6134a0
