// game/unknown_surface.cpp: functions whose original source file is unknown (formerly game/_unplaced/surface.cpp;
// re/attribution.csv: between ww/en3d/vga.cpp and ww/en3d/vgabuf.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <limits>
#include <cstdarg>

#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b104_11.cpp (game/b104_11)
namespace {
namespace game_b104_11 {

struct H {
    void nop() {}
    void nop4(void*) {}
};

#define B(name, addr, fn) JOAN_REPLACE("game/b104_11", name, addr, fn)
B("Obj::nop_60f910", 0x0060f910, &H::nop);
B("Obj::nop_60f960", 0x0060f960, &H::nop4);
#undef B

}  // namespace game_b104_11
}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

struct H {
    void nop() {}
    void nop4(u32) {}
    u32 zero4(u32) { return 0; }
};

JOAN_REPLACE("game/b105_17", "Obj::nop4_60f980", 0x0060f980, &H::nop4);
JOAN_REPLACE("game/b105_17", "Obj::nop4_60f9a0", 0x0060f9a0, &H::nop4);
JOAN_REPLACE("game/b105_17", "Obj::nop_60f9c0", 0x0060f9c0, &H::nop);
JOAN_REPLACE("game/b105_17", "Obj::nop_60f9e0", 0x0060f9e0, &H::nop);
JOAN_REPLACE("game/b105_17", "Obj::nop4_60fa00", 0x0060fa00, &H::nop4);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
namespace {
namespace game_b98_06 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::orig::helpers::crt_memset;

struct H {
    // ------------------------------------------------------------------ 0x0060f540
    H* clear_40c() {
        crt_memset(this, 0, static_cast<u32>(sizeof(jo::Display)));  // 0x40c on x86
        jo::self<jo::Display>(this)->pixel_format = -1;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("Obj::clear_40c", 0x0060f540, &H::clear_40c);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
namespace {
namespace game_b100_11 {

using joan::call::thiscall;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x0060f590
    u32 call_60f700() { joan::orig::as_display_palette(this)->reset(); return 0; }  // 0x0060f700 (void; static dtor)
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("Obj::call_60f700", 0x0060f590, &H::call_60f700);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b96_06.cpp (game/b96_06)
namespace {
namespace game_b96_06 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using i8 = std::int8_t;

// COM (stdcall, this pushed) call through vtable byte offset.
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

struct H {
    // ------------------------------------------------------------------ 0x0060f6d0
    i32 state_ge_3() { return static_cast<i32>(jo::view<i8>(jo::self<jo::Display>(this)->state)) >= 3 ? 1 : 0; }

    // ------------------------------------------------------------------ 0x0060f700
    void reset_60f700() {
        joan::orig::as_display_palette(this)->clear_flag_1();  // 0x0060f930
        if (jo::self<jo::Display>(this)->device != nullptr) {
            com_call<u32>(jo::view<void*>(jo::self<jo::Display>(this)->device), 8);  // Release
            jo::self<jo::Display>(this)->device = nullptr;
        }
        jo::self<jo::Display>(this)->state = 0;
    }
};

#define B96(name, addr, fn) JOAN_REPLACE("game/b96_06", name, addr, fn)
B96("Obj::state_ge_3", 0x0060f6d0, &H::state_ge_3);
B96("Obj::reset_60f700", 0x0060f700, &H::reset_60f700);
#undef B96

}  // namespace game_b96_06
}  // namespace

// ---------------------------------------- from modules/game_b111_15.cpp (fix/uninit)
namespace {
namespace game_b111_15 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

using joan::img::obj;

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

JOAN_COUNTER(c_palette, "Palette::load");

struct alignas(4) File {
    u8 raw[((sizeof(jo::File) > 0x110 ? sizeof(jo::File) : 0x110) + (alignof(void*) - 1)) & ~(alignof(void*) - 1)];  // [ebp-0x420] .. [ebp-0x310]; fields up to +0x10d (x64: the larger File)
};

struct H {
    // ------------------------------------------------------------------ 0x0060f760 (fix)
    i32 palette_load(const char* path) {
        c_palette.hit();
        File file;
        joan::orig::as_file(static_cast<void*>(&file))->construct();  // 0x004179c0 File::File
        alignas(4) u8 buf[0x300];
        std::memset(buf, 0, sizeof buf);  // fix/uninit: short / missing file -> zero entries
        joan::orig::as_file(static_cast<void*>(&file))->file_open(joan::orig::arg::str(path), 1, static_cast<char>(0));  // 0x00586da0 open
        joan::orig::as_file(static_cast<void*>(&file))->file_seek(8, joan::orig::arg::i32(-1));  // 0x00587ea0 seek
        joan::orig::as_file(static_cast<void*>(&file))->file_read(static_cast<void*>(buf), 0x300u);  // 0x005873a0 read
        joan::orig::as_file(static_cast<void*>(&file))->file_close();                      // 0x00587260 close
        // this: the Display (g_a4f888); the palette bytes and the loaded flag through jo::Display (r8_x64)
        u8* const pal = jo::self<jo::Display>(this)->colours;
        for (i32 i = 0; i < 0x100; ++i) {
            pal[i * 4] = buf[i * 3];
            pal[i * 4 + 1] = buf[i * 3 + 1];
            pal[i * 4 + 2] = buf[i * 3 + 2];
        }
        jo::self<jo::Display>(this)->flag = 1;
        joan::orig::as_display_palette(this)->nop_60f910();                       // 0x0060f910
        joan::orig::as_file(static_cast<void*>(&file))->destruct();  // 0x005872b0 File::~File
        return 1;
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// palette bytes past a short / missing file's end -> 0
FIX("Palette::load", 0x0060f760, &H::palette_load);
#undef FIX

}  // namespace game_b111_15
}  // namespace

// ---------------------------------------- from modules/game_b95_06.cpp (game/b95_06)
namespace {
namespace game_b95_06 {

using u8 = std::uint8_t;

using i8 = std::int8_t;

struct H {
    // ------------------------------------------------------------------------ 0x0060f930
    void clear_1() {
        if (jo::view<i8>(jo::self<jo::Display>(this)->flag) != 0) jo::self<jo::Display>(this)->flag = 0;
    }
};

#define B95(name, addr, fn) JOAN_REPLACE("game/b95_06", name, addr, fn)
B95("Flag::clear_1", 0x0060f930, &H::clear_1);
#undef B95

}  // namespace game_b95_06
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

using u32 = std::uint32_t;

struct H {
    u32 volume() { return jo::self<jo::Surface>(this)->width * jo::self<jo::Surface>(this)->height * jo::view<u32>(jo::self<jo::Surface>(this)->bytes_per_pixel); }  // 0x0060fa20
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::volume", 0x0060fa20, &H::volume);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

struct H {
    std::uint32_t camera_1c() { return jo::self<jo::Surface>(this)->width; }                         // 0x0060fa50
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("Camera::get_1c", 0x0060fa50, &H::camera_1c);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::DisplayPalette::is_attached() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b96_06::H::state_ge_3>(this)); }  // 0x60f6d0
auto joan::orig::DisplayPalette::reset() -> void { joan::bind::member<&::game_b96_06::H::reset_60f700>(this); }  // 0x60f700
auto joan::orig::DisplayPalette::load(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b111_15::H::palette_load>(this, a0)); }  // 0x60f760
auto joan::orig::DisplayPalette::nop_60f910() -> void { joan::bind::member<&::game_b104_11::H::nop>(this); }  // 0x60f910
auto joan::orig::DisplayPalette::clear_flag_1() -> void { joan::bind::member<&::game_b95_06::H::clear_1>(this); }  // 0x60f930
auto joan::orig::DisplayPalette::nop_60f960(void* a0) -> void { joan::bind::member<&::game_b104_11::H::nop4>(this, a0); }  // 0x60f960
auto joan::orig::Camera::get_1c() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_world::H::camera_1c>(this)); }  // 0x60fa50
