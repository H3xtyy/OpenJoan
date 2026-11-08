// en3d/vgabuf.cpp: functions of the original d:\ww\en3d\vgabuf.cpp (re/attribution.csv).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include <cstdint>
#include "../original/helpers.hpp"
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
#include "../original/vecmath.hpp"
#include "../d3d8/d3d8_api.hpp"
#include "../d3d8/d3d8_types.hpp"
#include "../original/engine.hpp"
#include "../render/nvlink_com.hpp"
#include "../render/nvlink_log.hpp"
#include "../render/overrides.hpp"
#include "../original/layout_access.hpp"
#include "../original/bind.hpp"
#include "../save/io.hpp"
#include "../game/pixfmt.hpp"

namespace jo = joan::orig;

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::img::obj;
using joan::img::str;

void* const kHeap = obj(d::g_mem);

using joan::orig::helpers::crt_memcpy;

}  // namespace

// ---------------------------------------- from modules/game_b100_11.cpp (game/b100_11)
//
//    - VgaBuf::init 0x00612600 (a, b, c, d), ret 0x10 -> this: allocates 0x400 / 0x200 /
//      0x100 / 0x300 buffers ("vgabuf.cpp"), 0x612ee0, copies the given tables in.
namespace {
namespace game_b100_11 {


i32 line_at(img::Addr a, i32 add_) { return static_cast<i32>(global<i16>(a)) + add_; }
void* heap_alloc(u32 n, const char* file, i32 line) { return joan::orig::as_mem(kHeap)->add(joan::orig::arg::u32(n), joan::orig::arg::str(file), joan::orig::arg::i32(line)); }  // 0x0058e7b0

JOAN_COUNTER(c_vga_init, "VgaBuf::init");

struct H {
    // ------------------------------------------------------------------ 0x00612600
    H* vga_init(const void* a, const void* b, const void* c, const void* d) {
        c_vga_init.hit();
        void* const p400 = (a != nullptr && d != nullptr) ? heap_alloc(0x400, str(d::s_d_ww_en3d_vgabuf_cpp), line_at(d::g_a18d44, 4)) : nullptr;
        void* const p200 = (a != nullptr && c != nullptr) ? heap_alloc(0x200, str(d::s_d_ww_en3d_vgabuf_cpp), line_at(d::g_a18d44, 3)) : nullptr;
        void* const p100 = (a != nullptr && b != nullptr) ? heap_alloc(0x100, str(d::s_d_ww_en3d_vgabuf_cpp), line_at(d::g_a18d44, 2)) : nullptr;
        void* const p300 = a != nullptr ? heap_alloc(0x300, str(d::s_d_ww_en3d_vgabuf_cpp), line_at(d::g_a18d44, 1)) : nullptr;
        joan::orig::as_rect(this)->set_612ee0(static_cast<jo::Palette24*>(p300), static_cast<jo::Palette8*>(p100), static_cast<jo::Palette16*>(p200), static_cast<jo::Palette32*>(p400));  // 0x00612ee0
        if (a != nullptr) crt_memcpy(jo::view<void*>(jo::self<jo::VgaBuf>(this)->rgb), a, 0x300);
        if (jo::view<void*>(jo::self<jo::VgaBuf>(this)->map8) != nullptr && b != nullptr) crt_memcpy(jo::view<void*>(jo::self<jo::VgaBuf>(this)->map8), b, 0x100);
        if (jo::view<void*>(jo::self<jo::VgaBuf>(this)->pal16) != nullptr && c != nullptr) crt_memcpy(jo::view<void*>(jo::self<jo::VgaBuf>(this)->pal16), c, 0x200);
        if (jo::view<void*>(jo::self<jo::VgaBuf>(this)->pal32) != nullptr && d != nullptr) crt_memcpy(jo::view<void*>(jo::self<jo::VgaBuf>(this)->pal32), d, 0x400);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b100_11", name, addr, fn)
B("VgaBuf::init", 0x00612600, &H::vga_init);
#undef B

}  // namespace game_b100_11
}  // namespace

// ---------------------------------------- from modules/game_b97_06.cpp (game/b97_06)
//
//    - VgaBuf::free_all 0x00612a00: frees +0 / +4 / +8 / +0xc via 0x58ed70 on 0xa48838.
namespace {
namespace game_b97_06 {


JOAN_COUNTER(c_vgabuf_free, "VgaBuf::free_all");

struct H {

    // 0x00612a00
    void vgabuf_free_all() {
        c_vgabuf_free.hit();
        // The four palette tables (rgb, map8, pal16, pal32).
        void* const* const tables[4] = {&jo::view<void*>(jo::self<jo::VgaBuf>(this)->rgb), &jo::view<void*>(jo::self<jo::VgaBuf>(this)->map8),
                                        &jo::view<void*>(jo::self<jo::VgaBuf>(this)->pal16), &jo::view<void*>(jo::self<jo::VgaBuf>(this)->pal32)};
        for (int k = 0; k < 4; ++k) {
            if (*tables[k] != nullptr) {
                const std::int32_t line = static_cast<std::int32_t>(global<std::int16_t>(d::g_a18d4c)) + 2 * (k + 1);
                joan::orig::as_mem(obj(d::g_mem))->del(joan::orig::arg::mptr(*tables[k]), str(d::s_d_ww_en3d_vgabuf_cpp), joan::orig::arg::i32(line));  // 0x0058ed70
            }
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b97_06", name, addr, fn)
B("VgaBuf::free_all", 0x00612a00, &H::vgabuf_free_all);
#undef B

}  // namespace game_b97_06
}  // namespace

// ---------------------------------------- from modules/game_b98_06.cpp (game/b98_06)
//
//    - VgaBuf::expand_palette 0x00612dd0 (transparent_last), ret 4: RGB -> 0xffRRGGBB table
//      (+0xc, allocated 0x400 bytes by 0x58e7b0 "vgabuf.cpp" when missing).
//
//  - Void functions whose eax is not meaningful (0x5b96a0, 0x5bfa70 - its only caller
//    0x5bfbd0 ignores eax, 0x612dd0, 0x618090, 0x62e9a0, 0x603570 - _initterm entry) return
//    void. 0x62b510 is tail-called by 0x62b5e0 (which returns its eax), so it returns this /
//    the last 0x60a740 result exactly.
namespace {
namespace game_b98_06 {


JOAN_COUNTER(c_expand_palette, "VgaBuf::expand_palette");

struct H {
    // ------------------------------------------------------------------ 0x00612dd0
    void expand_palette(i32 transparent_last) {
        c_expand_palette.hit();
        if (jo::view<void*>(jo::self<jo::VgaBuf>(this)->pal32) == nullptr) {
            const i32 line = static_cast<i32>(global<i16>(d::g_a18d58)) + 3;
            jo::view<void*>(jo::self<jo::VgaBuf>(this)->pal32) = joan::orig::as_mem(kHeap)->add(0x400u, str(d::s_d_ww_en3d_vgabuf_cpp), joan::orig::arg::i32(line));  // 0x0058e7b0
        }
        for (i32 i = 0; i < 0x100; ++i) {
            const u8* const pal = jo::view<u8*>(jo::self<jo::VgaBuf>(this)->rgb);
            const u32 c = static_cast<u32>(pal[i * 3 + 2]) | (static_cast<u32>(pal[i * 3 + 1]) << 8) |
                          (static_cast<u32>(pal[i * 3]) << 16) | 0xff000000u;
            jo::self<jo::VgaBuf>(this)->pal32->colour[i] = c;
        }
        if (transparent_last != 0) jo::self<jo::VgaBuf>(this)->pal32->colour[255] = jo::self<jo::VgaBuf>(this)->pal32->colour[255] & 0xffffff;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b98_06", name, addr, fn)
B("VgaBuf::expand_palette", 0x00612dd0, &H::expand_palette);
#undef B

}  // namespace game_b98_06
}  // namespace

// ---------------------------------------- from modules/game_b108_02.cpp (game/b108_02)
//
//    - Unpacker::words_610210 0x00610210 (ret 0xc): for count > 0 iterations out[k] (word) =
//      0x00610160(src), src += 3.
//
//  - 0x00610210 leaves eax stale when count <= 0; only one call site (0x00612d14, count 0x100)
//    passes eax on, so the returned value (0 after the loop) is exact where it is used.
namespace {
namespace game_b108_02 {

using i32 = std::int32_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

struct H {
    // 0x00610210
    i32 words_610210(const std::uint8_t* src, i32 count, u16* out) {
        while (count > 0) {
            *out = joan::orig::arg::as<u16>(joan::orig::as_image(this)->rgb_to_pixel(joan::orig::arg::mptr(src)));  // 0x00610160
            count = count - 1;
            src += 3;
            ++out;
        }
        return count;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_02", name, addr, fn)
B("Unpacker::words_610210", 0x00610210, &H::words_610210);
#undef B

}  // namespace game_b108_02
}  // namespace

// ---------------------------------------- from modules/game_b108_05.cpp (game/b108_05)
//
//    - Pal::convert 0x00610270 (ret 0xc, void: eax unused at 25 of the 26 call sites;
//      the 26th, 0x00612d59, is a tail call in 0x00612c40 whose only caller is the dead 0x00612af0).
namespace {
namespace game_b108_05_2 {

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::thiscall;

JOAN_COUNTER(c_convert, "Pal::convert_610270");

struct H {
    // ------------------------------------------------------------------ 0x00610270
    void pal_convert(const u8* src, i32 count, u32* dst) {
        c_convert.hit();
        while (count > 0) {
            *dst = joan::orig::arg::as<u32>(joan::orig::as_image(this)->rgb_to_pixel(joan::orig::arg::mptr(src)));  // 0x00610160
            --count;
            src += 3;
            ++dst;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_05", name, addr, fn)
B("Pal::convert_610270", 0x00610270, &H::pal_convert);
#undef B

}  // namespace game_b108_05_2
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
//
//  * module game/b91_03:
//    TextCursor::advanced 0x0058c120, to_lower 0x0058e330, empty methods 0x0058e720 /
//    0x0059d1b0, Stream::forward 0x005939f0, Joystick::in_rect 0x00594360,
//    Joystick::pressed_kind 0x00594a10, Key::repeat_count 0x00595fb0, word getters
//    0x005979a0 / 0x005979c0, Pad::axis 0x00597c80, Pad::axis_range 0x00597e00,
//    same16 0x00598700, Pair::clear 0x005a1730, Bitmap::address 0x005a93b0,
//    MusicStream::release 0x005a98a0, MusicStream::read_header 0x005a9ad0,
//    MusicStream::reset 0x005a9b40, MusicStream::rewind 0x005aa040, Actor::set_byte_fd
//    0x005ad000, Table::get 0x005af330, Table::missing 0x005af360, AnimDef::lookup_word
//    0x005b4a00, Names::get 0x005ba910, Rect::set 0x005bed90 / 0x00612ee0, Array::at1
//    0x005ea410, RenderTarget::create_depth 0x006032d0, RenderTarget::release_depth
//    0x00603380, z_plane_hit 0x0060e6a0, BitWriter::put 0x0060ff70, BitWriter::drop
//    0x006100a0, Viewport::set_int 0x00614110, Sound3d::set 0x00616a90, getter 0x00616b60,
//    Dialog::reset_name 0x00616f80, base dtor bodies 0x00628ad0 / 0x0062c720,
//    TextBox::init 0x0062b8d0, Scroll::set 0x0062be10 / 0x0062cd40, Scroll::at_end
//    0x0062c040 / 0x0062cf70, Scroll::set_end 0x0062ced0, TextBox::cursor_x 0x00630770,
//    TextBox::line_y 0x00630ab0, getter 0x00631770, Table16::entry 0x00638150.
namespace {
namespace game_b91_03 {


struct H {
    // 0x005bed90 / 0x00612ee0
    H* set4(jo::Palette24* a, jo::Palette8* b, jo::Palette16* c, jo::Palette32* d) {
        jo::self<jo::VgaBuf>(this)->rgb = a;
        jo::self<jo::VgaBuf>(this)->map8 = b;
        jo::self<jo::VgaBuf>(this)->pal16 = c;
        jo::self<jo::VgaBuf>(this)->pal32 = d;
        return this;
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("Rect::set_612ee0", 0x00612ee0, &H::set4);
#undef B91

}  // namespace game_b91_03
}  // namespace

// ---------------------------------------- from modules/game_b108_07.cpp (game/b108_07)
namespace {
namespace game_b108_07 {

using joan::call::stdcall_call;

using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x00610160
    u32 rgb_to_pixel(const void* rgb) {
        u32 packed = 0;
        std::memcpy(static_cast<void*>(&packed), joan::orig::arg::ptr(rgb), joan::orig::arg::u32(u32{3}));  // memmove
        return joan::orig::arg::as<u32>(joan::orig::pixel::pack_color(joan::orig::arg::u32(packed)));                             // 0x00571205
    }
};

JOAN_REPLACE("game/b108_07", "rgb_to_pixel", 0x00610160, &H::rgb_to_pixel);

}  // namespace game_b108_07
}  // namespace

// ---------------------------------------- from modules/game_b108_04.cpp (game/b108_04)
namespace {
namespace game_b108_04 {

using joan::call::thiscall;

using i32 = std::int32_t;
using u32 = std::uint32_t;

struct H {
    // ------------------------------------------------------------------ 0x006101b0
    std::uintptr_t to_bytes_6101b0(u32 color, void* out) {  // returns `out` (memmove's result, a pointer)
        // 0x00571215 is a one-argument stdcall function (Pixel::unpack_color, pixfmt.cpp); the generated Colour facade
        // calls it as a member, passing `this` first: with MSVC's thiscall `this` sits in ecx and the colour stays the
        // stack argument, but GCC / Clang pass `this` on the stack (Linux x86) or in the first argument register
        // (x64), so every pixel unpacked the object's address and the save previews came out one flat colour.
        static_cast<void>(this);
        u32 packed = joan::pixfmt::unpack_color(color);  // 0x00571215
        return joan::orig::arg::as<std::uintptr_t>(std::memcpy(joan::orig::arg::mptr(out), static_cast<void*>(&packed), joan::orig::arg::u32(i32{3})));  // memmove
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_04", name, addr, fn)
B("Color::to_bytes_6101b0", 0x006101b0, &H::to_bytes_6101b0);
#undef B

}  // namespace game_b108_04
}  // namespace

// ---------------------------------------- from modules/game_b108_15.cpp (game/b108_15)
namespace {
namespace game_b108_15 {


using joan::hook::global;
using i8 = std::int8_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::thiscall;
using joan::call::stdcall_call;
using joan::img::obj;

JOAN_COUNTER(c_blit612, "Blit::draw_612f30");

struct H {
    // ------------------------------------------------------------------ 0x00612f30
    void blit_612f30(u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
        c_blit612.hit();
        if (jo::self<jo::Blit>(this)->blend_mode != 0) joan::orig::as_input(obj(d::g_mouse))->lock_area(joan::orig::arg::u32(a1), joan::orig::arg::u32(a2), joan::orig::arg::u32(a3), joan::orig::arg::u32(a4));  // 0x00592c90
        if (jo::self<jo::Blit>(this)->format_bits != global<u32>(d::g_a4fc90)) joan::orig::set_pixel_format_5706f0(joan::orig::arg::i32(jo::self<jo::Blit>(this)->format_bits));
        const i32 depth = jo::self<jo::Blit>(this)->bytes_per_pixel;
        if (depth == 2 || depth == 3 || depth == 4) {
            joan::orig::fill_rect(jo::self<jo::Blit>(this)->bits, joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Blit>(this)->pitch)), joan::orig::arg::u32(a1), joan::orig::arg::u32(a2), joan::orig::arg::u32(a3), joan::orig::arg::u32(a4), joan::orig::arg::u32(a5));  // 0x0056fba0
        }
        if (jo::self<jo::Blit>(this)->format_bits != global<u32>(d::g_a4fc90)) joan::orig::set_pixel_format_5706f0(joan::orig::arg::i32(global<u32>(d::g_a4fc90)));
        if (jo::self<jo::Blit>(this)->blend_mode != 0) joan::orig::as_input(obj(d::g_mouse))->release_592cd0();  // 0x00592cd0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_15", name, addr, fn)
B("Blit::draw_612f30", 0x00612f30, &H::blit_612f30);
#undef B

}  // namespace game_b108_15
}  // namespace

// ---------------------------------------- from modules/game_b92_05.cpp (game/b92_05)
namespace {
namespace game_b92_05 {


struct H {
    // 0x00613090
    std::int32_t palette_channel(const std::uint8_t* table, std::int32_t sel, std::int32_t i) {
        switch (static_cast<std::int16_t>(sel)) {
            case 0: return static_cast<std::int32_t>(reinterpret_cast<const joan::d3d8::GammaRamp*>(table)->red[i]) >> 8;
            case 1: return static_cast<std::int32_t>(reinterpret_cast<const joan::d3d8::GammaRamp*>(table)->green[i]) >> 8;
            case 2: return static_cast<std::int32_t>(reinterpret_cast<const joan::d3d8::GammaRamp*>(table)->blue[i]) >> 8;
            default: return -1;
        }
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b92_05", name, addr, fn)
B("Palette::channel", 0x00613090, &H::palette_channel);
#undef B

}  // namespace game_b92_05
}  // namespace

// ---------------------------------------- from modules/game_b93_03.cpp (game/b93_03)
namespace {
namespace game_b93_03 {

using i32 = std::int32_t;
using u32 = std::uint32_t;

using i16 = std::int16_t;

// One pixel of a locked surface (raw pixel bytes, not an object layout: the same on x64).
template <class T>
T pixel(const void* p) {
    return *static_cast<const T*>(p);
}

struct H {
    i32 read_pixel(const void* p) {  // 0x00610550
        const i32 bpp = jo::self<jo::Surface>(this)->bytes_per_pixel;
        if (bpp == 2) return static_cast<i32>(pixel<i16>(p));
        if (bpp == 3) return static_cast<i32>(pixel<u32>(p) & 0xffffffu);
        if (bpp == 4) return pixel<i32>(p);
        return 0;
    }
};

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_03", name, addr, fn)
B93("Surface::read_pixel", 0x00610550, &H::read_pixel);
#undef B93

}  // namespace game_b93_03
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13 {

using joan::call::thiscall;
using joan::call::stdcall_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::img::obj;

using i8 = std::int8_t;

using i16 = std::int16_t;


void* const kScreen = obj(d::g_mouse);

JOAN_COUNTER(c_blit_sprite, "Surface::blit_sprite");

struct H {
    void select_mode() {
        const u32 cur = joan::orig::arg::as<u32>(joan::orig::asm_get_a15020());                         // 0x005711b7
        if (jo::self<jo::Surface>(this)->format_bits != cur) joan::orig::set_pixel_format_5706f0(joan::orig::arg::i32(jo::self<jo::Surface>(this)->format_bits));  // 0x005706f0
    }

    // ------------------------------------------------------------------ 0x00610e10
    void blit_sprite(i32 x, i32 y, void* sprite, void* src, i32 unused, void* pal16, void* pal32) {
        c_blit_sprite.hit();
        (void)unused;
        if (jo::self<jo::Surface>(this)->blend_mode != 0) {
            const i32 h = sx16_ret(joan::orig::arg::as<i16>(joan::orig::as_bitmap(sprite)->height()));  // 0x005979c0
            const u32 y1 = static_cast<u32>(y) + static_cast<u32>(h) - 1u;
            const i32 w = sx16_ret(joan::orig::arg::as<i16>(joan::orig::as_bitmap(sprite)->width()));  // 0x005979a0
            const u32 x1 = static_cast<u32>(x) + static_cast<u32>(w) - 1u;
            joan::orig::as_input(kScreen)->lock_area(joan::orig::arg::u32(x), joan::orig::arg::u32(y), joan::orig::arg::u32(x1), joan::orig::arg::u32(y1));  // 0x00592c90 lock
        }
        select_mode();
        switch (jo::self<jo::Surface>(this)->bytes_per_pixel) {
            case 2: {
                u8 buf[0x200];  // [ebp-0x208]
                void* pal = pal16;
                if (pal == nullptr) {
                    pal = buf;
                    joan::orig::as_unpacker(this)->words_610210(static_cast<const u8*>(src), 0x100, joan::orig::arg::mptr(pal));  // 0x00610210
                }
                joan::orig::blit8to16_key(jo::self<jo::Surface>(this)->bits, joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Surface>(this)->pitch)), joan::orig::arg::u32(x), joan::orig::arg::u32(y), joan::orig::arg::mptr(sprite), joan::orig::arg::mptr(pal));  // 0x005744f0
                break;
            }
            case 3: {
                u8 buf[0x400];  // [ebp-0x60c]
                void* pal = pal32;
                if (pal == nullptr) {
                    pal = buf;
                    joan::orig::as_pal(this)->convert_610270(joan::orig::arg::mptr(src), 0x100, joan::orig::arg::mptr(pal));  // 0x00610270
                }
                joan::orig::blit::blit8to24_key(jo::self<jo::Surface>(this)->bits, joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Surface>(this)->pitch)), joan::orig::arg::u32(x), joan::orig::arg::u32(y), joan::orig::arg::mptr(sprite), joan::orig::arg::mptr(pal));  // 0x00574561
                break;
            }
            case 4: {
                u8 buf[0x400];  // [ebp-0xa10]
                void* pal = pal32;
                if (pal == nullptr) {
                    pal = buf;
                    joan::orig::as_pal(this)->convert_610270(joan::orig::arg::mptr(src), 0x100, joan::orig::arg::mptr(pal));  // 0x00610270
                }
                joan::orig::blit::blit8to32_key_5745da(jo::self<jo::Surface>(this)->bits, joan::orig::arg::u32(jo::view<u32>(jo::self<jo::Surface>(this)->pitch)), joan::orig::arg::u32(x), joan::orig::arg::u32(y), joan::orig::arg::mptr(sprite), joan::orig::arg::mptr(pal));  // 0x005745da
                break;
            }
            default:
                break;
        }
        if (jo::self<jo::Surface>(this)->blend_mode != 0) joan::orig::as_input(kScreen)->release_592cd0();  // 0x00592cd0 unlock
    }
    static i32 sx16_ret(i16 v) { return static_cast<i32>(v); }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("Surface::blit_sprite", 0x00610e10, &H::blit_sprite);
#undef B

}  // namespace game_b108_13
}  // namespace

// ---------------------------------------- from modules/game_b115_08.cpp (fix/uninit)
namespace {
namespace game_b115_08 {

using joan::call::thiscall;
using joan::call::stdcall_call;

using joan::hook::global;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

using u16 = std::uint16_t;

using uptr = std::uintptr_t;

template <class T>
T& at(const void* p, i32 offset) {
    return *reinterpret_cast<T*>(static_cast<u8*>(const_cast<void*>(p)) + offset);
}
// COM-style method (stdcall, this first) from the object's vtable.
template <class R = void, class... A>
R com_call(void* self, i32 off, A... args) {
    return stdcall_call<R>((*static_cast<const uptr* const*>(self))[static_cast<u32>(off) / 4], self, args...);  // off: the x86 vtable byte offset
}
uptr addr(const void* p) { return reinterpret_cast<uptr>(p); }
using joan::orig::helpers::wsub;
i32 wmul(i32 a, i32 b) { return static_cast<i32>(static_cast<u32>(a) * static_cast<u32>(b)); }

JOAN_COUNTER(c_bmp, "Texture::save_bmp_611e70");

// ---------------------------------------------------------------- 0x00611e70 frame
// ebp-0x1374 .. ebp-0xc of Texture::save_bmp.
struct BmpFrame {
    static constexpr i32 kLow = 0x1374;
    static constexpr i32 kSize = 0x1374 - 0xc;
    alignas(4) u8 b[kSize];
    u8* p(i32 ebp_off) { return b + kLow + ebp_off; }
    template <class T>
    T& v(i32 ebp_off) {
        return at<T>(b, kLow + ebp_off);
    }
    bool in(uptr a) const { return a >= addr(b) && a < addr(b) + kSize; }
    void put(uptr a, u8 value) {
        if (in(a)) b[a - addr(b)] = value;  // at / above ebp-0xc: dropped
    }
};

struct H {
    // ------------------------------------------------------------------ 0x00611e70 (fix)
    i32 save_bmp(const char* name) {
        c_bmp.hit();
        BmpFrame fr;
        std::memset(fr.b, 0, sizeof fr.b);
#if UINTPTR_MAX == 0xffffffffu
        // x86: `this` at ebp-0x10, where an overlong 24-bit row reaches it as in the original.
        fr.v<void*>(-0x10) = this;
        auto self = [&fr]() { return fr.v<u8*>(-0x10); };
#else
        // x64: an 8-byte pointer does not fit below ebp-0xc, and File (0x112 bytes) reaches ebp-0xe: kept outside.
        u8* const this_ = reinterpret_cast<u8*>(this);
        auto self = [this_]() { return this_; };
#endif
        void* const file = fr.p(-0x120);  // File, 0x10e bytes on x86 (to ebp-0x12)
        joan::orig::as_file(file)->construct();  // 0x004179c0 File::File
        fr.v<u32>(-0x124) = 0;
        fr.v<u32>(-0x128) = 0;
        fr.v<u32>(-0x12c) = 0;
        fr.v<u32>(-0x130) = 1;
        joan::orig::as_bit_writer(self())->push_bit();                // 0x00610040
        const u32 mode = joan::orig::arg::as<u32>(joan::orig::asm_get_a15020());  // 0x005711b7
        if (jo::as_image(self())->format_bits != mode) joan::orig::set_pixel_format_5706f0(joan::orig::arg::i32(jo::as_image(self())->format_bits));  // 0x005706f0
        joan::orig::as_file(file)->file_create(joan::orig::arg::str(name), 1, static_cast<char>(0));  // 0x00586f00 create
        fr.v<u16>(-0x140) = 0x4d42;
        fr.v<u32>(-0x13e) = 0;
        fr.v<u16>(-0x13a) = 0;
        fr.v<u16>(-0x138) = 0;
        fr.v<u32>(-0x136) = 0x36;
        if (fr.v<u32>(-0x12c) != 0) fr.v<u32>(-0x136) = fr.v<u32>(-0x136) + 0x400;
        fr.v<u32>(-0x168) = 0x28;
        fr.v<i32>(-0x164) = joan::orig::arg::as<i32>(joan::orig::as_camera(self())->get_1c());  // 0x0060fa50 width
        fr.v<i32>(-0x160) = joan::orig::arg::as<i32>(joan::orig::as_camera(self())->get_20());  // 0x0060fa70 height
        fr.v<u16>(-0x15c) = 1;
        fr.v<u16>(-0x15a) = static_cast<u16>(fr.v<u32>(-0x12c) != 0 ? 8 : 0x18);
        fr.v<u32>(-0x158) = 0;
        {
            const i32 bits = wmul(wmul(fr.v<i32>(-0x164), fr.v<i32>(-0x160)), static_cast<i32>(fr.v<u32>(-0x15a) & 0xffff));
            fr.v<i32>(-0x154) = bits / 8;
        }
        fr.v<u32>(-0x150) = 0;
        fr.v<u32>(-0x14c) = 0;
        fr.v<u32>(-0x148) = 0;
        fr.v<u32>(-0x144) = 0;
        fr.v<u32>(-0x13e) = fr.v<u32>(-0x136) + fr.v<u32>(-0x154);
        // BITMAPFILEHEADER (ebp-0x140, 0xe bytes) and BITMAPINFOHEADER (ebp-0x168, 0x28 bytes), little-endian field by
        // field ([Debug] SaveFieldwise=0: the frame bytes as before; the same bytes).
        if (joan::save::fieldwise()) {
            const joan::save::Port port = joan::save::port(*joan::orig::as_file(file));
            {
                joan::save::Out o(port, 0xeu, "BITMAPFILEHEADER", fr.p(-0x140));
                o.u16(fr.v<u16>(-0x140));  // bfType 'BM'
                o.u32(fr.v<u32>(-0x13e));  // bfSize
                o.u16(fr.v<u16>(-0x13a));  // bfReserved1
                o.u16(fr.v<u16>(-0x138));  // bfReserved2
                o.u32(fr.v<u32>(-0x136));  // bfOffBits
                o.end();                   // 0x005872e0 write
            }
            joan::save::Out o(port, 0x28u, "BITMAPINFOHEADER", fr.p(-0x168));
            o.u32(fr.v<u32>(-0x168));  // biSize
            o.i32(fr.v<i32>(-0x164));  // biWidth
            o.i32(fr.v<i32>(-0x160));  // biHeight
            o.u16(fr.v<u16>(-0x15c));  // biPlanes
            o.u16(fr.v<u16>(-0x15a));  // biBitCount
            o.u32(fr.v<u32>(-0x158));  // biCompression
            o.i32(fr.v<i32>(-0x154));  // biSizeImage
            o.i32(fr.v<i32>(-0x150));  // biXPelsPerMeter
            o.i32(fr.v<i32>(-0x14c));  // biYPelsPerMeter
            o.u32(fr.v<u32>(-0x148));  // biClrUsed
            o.u32(fr.v<u32>(-0x144));  // biClrImportant
            o.end();
        } else {
            joan::orig::as_file(file)->file_write(static_cast<void*>(fr.p(-0x140)), 0xeu);     // 0x005872e0 write
            joan::orig::as_file(file)->file_write(static_cast<void*>(fr.p(-0x168)), 0x28u);
        }
        if ((fr.v<u32>(-0x15a) & 0xffff) == 8 && jo::as_image(self())->bytes_per_pixel == 1) {
            for (i32 y = wsub(joan::orig::arg::as<i32>(joan::orig::as_camera(self())->get_20()), 1); y >= 0; --y) {
                const i32 w = joan::orig::arg::as<i32>(joan::orig::as_camera(self())->get_1c());
                void* const row = joan::orig::arg::as<void*>(joan::orig::as_bitmap(self())->address(0u, joan::orig::arg::u32(y)));  // 0x005a93b0
                joan::orig::as_file(file)->file_write(joan::orig::arg::ptr(row), joan::orig::arg::u32(w));
            }
        } else if ((fr.v<u32>(-0x15a) & 0xffff) == 0x18) {
            com_call(global<void*>(d::g_game_device), 0x4c, static_cast<void*>(fr.p(-0x76c)));  // GetGammaRamp
            fr.v<u32>(-0x770) = 0x400;
            fr.v<i32>(-0x774) = wmul(fr.v<i32>(-0x164), 3);
            joan::orig::vector(joan::orig::arg::mptr(static_cast<void*>(fr.p(-0x1374))), 3, 0x400, joan::orig::arg::mptr(fn::code_value(fn::Obj_ctor_596bd0)));  // 0x00449af0 (ctor 0x00596bd0)
            for (i32 y = wsub(joan::orig::arg::as<i32>(joan::orig::as_camera(self())->get_20()), 1); y >= 0; --y) {
                uptr src = addr(joan::orig::arg::as<void*>(joan::orig::as_bitmap(self())->address(0u, joan::orig::arg::u32(y))));
                uptr p = addr(fr.p(-0x1374));
                for (i32 x = wsub(joan::orig::arg::as<i32>(joan::orig::as_camera(self())->get_1c()), 1); x >= 0; --x) {
                    const u32 px = joan::orig::arg::as<u32>(joan::orig::as_image(self())->read_pixel(joan::orig::arg::mptr(src)));  // 0x00610550
                    u8 t[3] = {0, 0, 0};  // the three bytes at p (shadow of the dropped ones)
                    joan::orig::as_color(self())->to_bytes_6101b0(joan::orig::arg::u32(px), joan::orig::arg::mptr(static_cast<void*>(t)));  // 0x006101b0
                    for (i32 k = 0; k < 3; ++k) fr.put(p + static_cast<uptr>(k), t[k]);
                    for (i32 k = 0; k < 3; ++k) {
                        t[k] = static_cast<u8>(joan::orig::arg::as<u32>(joan::orig::as_image(self())->ramp_channel(joan::orig::arg::mptr(static_cast<void*>(fr.p(-0x76c))), joan::orig::arg::i32(k), joan::orig::arg::i32(static_cast<u32>(t[k])))));  // 0x00613090
                        fr.put(p + static_cast<uptr>(k), t[k]);
                    }
                    const u8 tmp = t[0];
                    t[0] = t[2];
                    fr.put(p, t[0]);
                    t[2] = tmp;
                    fr.put(p + 2, tmp);
                    src += static_cast<uptr>(jo::view<u32>(jo::as_image(self())->bytes_per_pixel));
                    p += 3;
                }
                const u32 len = fr.v<u32>(-0x774);
                // fix: a clobbered row size reaching past the emulated frame -> row not written
                if (len <= static_cast<u32>(BmpFrame::kSize))
                    joan::orig::as_file(file)->file_write(static_cast<void*>(fr.p(-0x1374)), joan::orig::arg::u32(len));
            }
        } else {
            fr.v<u32>(-0x130) = 0;
        }
        joan::orig::as_file(file)->file_close();   // 0x00587260 close
        joan::orig::as_bit_writer(self())->drop();     // 0x006100a0
        const i32 r = fr.v<i32>(-0x130);
        joan::orig::as_file(file)->destruct();  // 0x005872b0 File::~File
        return r;
    }
};

#define FIX(name, address, fn) JOAN_REPLACE("fix/uninit", name, address, fn)
// fix/uninit: row buffer overrun for widths > 1024 emulated in a zeroed frame copy
FIX("VgaBuf::write_bmp_file", 0x00611e70, &H::save_bmp);
#undef FIX

}  // namespace game_b115_08
}  // namespace

// ---------------------------------------- from modules/game_b105_17.cpp (game/b105_17)
namespace {
namespace game_b105_17 {

struct H {
    void nop() {}
    void nop4(u32) {}
    u32 zero4(u32) { return 0; }
};

JOAN_REPLACE("game/b105_17", "Obj::nop_60fb40", 0x0060fb40, &H::nop);
JOAN_REPLACE("game/b105_17", "Obj::nop4_60fe20", 0x0060fe20, &H::nop4);
JOAN_REPLACE("game/b105_17", "Obj::zero_60fe40", 0x0060fe40, &H::zero4);

}  // namespace game_b105_17
}  // namespace

// ---------------------------------------- from modules/game_b108_18.cpp (game/b108_18)
namespace {
namespace game_b108_18 {

using joan::call::thiscall;
using joan::call::stdcall_call;

using i32 = std::int32_t;
using u8 = std::uint8_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::img::str;

using u16 = std::uint16_t;

using joan::img::obj;

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

using joan::orig::helpers::u;

struct H {
    // ------------------------------------------------------------------ 0x0060ffd0
    std::uintptr_t bit_shift() {
        const u32 bit = jo::self<jo::Surface>(this)->lock_stack & 1u;
        const u16 word = static_cast<u16>(jo::self<jo::Surface>(this)->lock_stack >> 1);
        jo::self<jo::Surface>(this)->lock_stack = word;
        const u8 count = static_cast<u8>(jo::self<jo::Surface>(this)->lock_depth - 1);
        jo::self<jo::Surface>(this)->lock_depth = count;
        if (bit != 0) return joan::orig::arg::as<u32>(joan::orig::as_texture(this)->lock());  // 0x0060fe60
        return (u(this) & 0xffff0000u) | (word & 0xff00u) | count;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_18", name, addr, fn)
B("VgaBuf::temp_restore_lock", 0x0060ffd0, &H::bit_shift);
#undef B

}  // namespace game_b108_18
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

JOAN_COUNTER(c_lock, "Texture::lock");

struct H {
    // ------------------------------------------------------------------ 0x0060fe60 (fix)
    std::uintptr_t lock() {
        c_lock.hit();
        if (jo::view<i32>(jo::self<jo::Surface>(this)->locked) != 0) return reinterpret_cast<std::uintptr_t>(this);
        if (jo::view<void*>(jo::self<jo::Surface>(this)->surface) != nullptr) {
            struct {
                i32 pitch;
                void* bits;
            } rect = {0, nullptr};  // fix/uninit: never written when LockRect fails
            const i32 hr = com_call<i32>(jo::view<void*>(jo::self<jo::Surface>(this)->surface), 9, static_cast<void*>(&rect), static_cast<void*>(nullptr),
                                         static_cast<u32>(0x800));  // IDirect3DSurface8::LockRect
            if (hr != 0) joan::orig::error::fatal(joan::orig::arg::mptr(obj(d::g_err)), str(d::s_Cannot_lock_surface_x), hr);  // 0x00586cb0
            jo::view<void*>(jo::self<jo::Surface>(this)->lock_bits) = rect.bits;
            jo::view<void*>(jo::self<jo::Surface>(this)->bits) = rect.bits;
            jo::self<jo::Surface>(this)->lock_pitch = rect.pitch;
            jo::self<jo::Surface>(this)->pitch = rect.pitch;
        }
        jo::view<i32>(jo::self<jo::Surface>(this)->locked) = 1;
        return joan::orig::arg::as<u32>(joan::orig::as_texture(this)->read_desc(0u));  // 0x0060fca0
    }
};

#define FIX(name, addr, fn) JOAN_REPLACE("fix/uninit", name, addr, fn)
// D3DLOCKED_RECT unwritten when LockRect fails -> pitch 0 / bits null
FIX("VgaBuf::lock_buf", 0x0060fe60, &H::lock);
#undef FIX

}  // namespace game_b111_15
}  // namespace

// ---------------------------------------- from modules/game_b103_12.cpp (game/b103_12)
namespace {
namespace game_b103_12 {

using u32 = std::uint32_t;

struct H {
    H* reset_size() {                                                      // 0x0060fa90
        jo::self<jo::Surface>(this)->bits = jo::self<jo::Surface>(this)->lock_bits;
        jo::view<u32>(jo::self<jo::Surface>(this)->pitch) = jo::view<u32>(jo::self<jo::Surface>(this)->lock_pitch);
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b103_12", name, addr, fn)
B("Obj::reset_size", 0x0060fa90, &H::reset_size);
#undef B

}  // namespace game_b103_12
}  // namespace

// ---------------------------------------- from modules/game_world.cpp (game/world1)
namespace {
namespace game_world {

struct H {
    std::uint32_t camera_20() { return jo::self<jo::Surface>(this)->height; }                         // 0x0060fa70
};

#define W1(name, addr, fn) JOAN_REPLACE("game/world1", name, addr, fn)
W1("VgaBuf::buf_height", 0x0060fa70, &H::camera_20);
#undef W1

}  // namespace game_world
}  // namespace

// ---------------------------------------- from modules/en3d_b85_04.cpp (en3d/b85_04)
namespace {
namespace en3d_b85_04 {

using joan::call::thiscall;

using joan::hook::global;
using i32 = std::int32_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;

// COM method (stdcall, this first) through the vtable slot (byte offset).
template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t offset, A... args) {
    using Fn = R(JOAN_STDCALL*)(void*, A...);
    return reinterpret_cast<Fn>(joan::call::vtable_of(object)[joan::call::x86_vslot(offset)])(object, args...);
}

using joan::orig::helpers::device;

void* game_memset(void* p, std::int32_t v, std::uint32_t n) { return std::memset(joan::orig::arg::mptr(p), joan::orig::arg::i32(v), joan::orig::arg::u32(n)); }

JOAN_COUNTER(c_image_create, "Image::create");

struct H {
    // 0x0060fb60
    std::uint32_t image_create(std::uint32_t w, std::uint32_t h, std::uint32_t, std::int32_t format) {
        c_image_create.hit();
        const std::uint32_t fmt = format == -1 ? joan::orig::arg::as<std::uint32_t>(joan::orig::d3d_format(joan::orig::arg::u32(global<std::uint32_t>(d::g_a4fc90))))
                                               : joan::orig::d3d_format(joan::orig::arg::u32(format));  // 0x00600080
        com_call<std::int32_t>(device(), 0x6c, w, h, fmt, static_cast<void*>(this));  // CreateImageSurface
        jo::self<jo::Surface>(this)->owned = 1;
        jo::self<jo::Surface>(this)->locked = 0;
        jo::self<jo::Surface>(this)->lock_stack = 0;
        jo::self<jo::Surface>(this)->lock_depth = 0;
        const std::uint32_t r = joan::orig::arg::as<std::uint32_t>(joan::orig::as_texture(this)->read_desc(1u));  // 0x0060fca0
        jo::view<std::uint8_t>(jo::self<jo::Surface>(this)->blend_mode) = 0;
        return r;
    }

    // 0x0060fd20
    std::uintptr_t image_release() {
        if (jo::view<void*>(jo::self<jo::Surface>(this)->surface) == nullptr) return reinterpret_cast<std::uintptr_t>(this);
        const std::uint32_t r = com_call<std::uint32_t>(jo::view<void*>(jo::self<jo::Surface>(this)->surface), 8);  // Release
        jo::view<void*>(jo::self<jo::Surface>(this)->surface) = nullptr;
        return r;
    }

    // 0x0060fad0
    H* image_ctor() {
        game_memset(this, 0, static_cast<std::uint32_t>(sizeof(jo::Surface)));  // 0x34 on x86
        jo::view<std::int32_t>(jo::self<jo::Surface>(this)->format_bits) = -1;
        return this;
    }

    std::uint32_t dtor_60fb10() { return joan::orig::arg::as<std::uint32_t>(joan::orig::as_image(this)->release()); }  // 0x0060fd20
};

#define E3D(name, addr, fn) JOAN_REPLACE("en3d/b85_04", name, addr, fn)
E3D("VgaBuf::init_back", 0x0060fb60, &H::image_create);
E3D("Image::release", 0x0060fd20, &H::image_release);
E3D("Image::Image", 0x0060fad0, &H::image_ctor);
E3D("Image::~Image", 0x0060fb10, &H::dtor_60fb10);
#undef E3D

}  // namespace en3d_b85_04
}  // namespace

// ---------------------------------------- from modules/game_b99_07.cpp (game/b99_07)
namespace {
namespace game_b99_07 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

// COM method: vtable slot at byte offset `off`, stdcall with the interface as first argument.
template <class R = std::int32_t, class... A>
R com_call(void* iface, std::uint32_t off, A... args) {
    const std::uintptr_t* vt = *reinterpret_cast<std::uintptr_t* const*>(iface);
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(vt[joan::call::x86_vslot(off)])(iface, args...);
}

struct H {
    // ------------------------------------------------------------------ 0x0060fc20
    H* attach_60fc20(void* iface) {
        jo::view<void*>(jo::self<jo::Surface>(this)->surface) = iface;
        com_call<u32>(jo::view<void*>(jo::self<jo::Surface>(this)->surface), 4);  // AddRef
        jo::self<jo::Surface>(this)->owned = 0;
        jo::self<jo::Surface>(this)->locked = 0;
        jo::self<jo::Surface>(this)->lock_stack = 0;
        jo::self<jo::Surface>(this)->lock_depth = 0;
        joan::orig::as_texture(this)->read_desc(1u);  // 0x0060fca0
        jo::view<u8>(jo::self<jo::Surface>(this)->blend_mode) = 0;
        return this;
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b99_07", name, addr, fn)
B("Obj::attach_60fc20", 0x0060fc20, &H::attach_60fc20);
#undef B

}  // namespace game_b99_07
}  // namespace

// ---------------------------------------- from modules/game_b86_01.cpp (en3d/b86_01)
namespace {
namespace game_b86_01 {

using joan::call::cdecl_call;

template <class R = std::int32_t, class... A>
R com_call(void* object, std::int32_t index, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>(joan::call::vtable_of(object)[index])(object, args...);
}

JOAN_COUNTER(c_read_desc, "Texture::read_desc");

struct H {
    // 0x0060fca0 Texture::read_desc(unused). ret 4. GetDesc (vtable slot 8, D3DSURFACE_DESC
    // 0x20 bytes) on the +0 surface; +0x24 = 0x005fff90(Format), +0x1c Width, +0x20 Height,
    // +0x28 = 0x005ffe60(Format) (also eax). The original leaves the desc uninitialised if
    // GetDesc fails (never for a live surface); zero-initialised here.
    std::uint32_t read_desc(std::uint32_t) {
        c_read_desc.hit();
        std::uint32_t desc[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        com_call<std::int32_t>(jo::view<void*>(jo::self<jo::Surface>(this)->surface), 8, static_cast<void*>(desc));
        jo::self<jo::Surface>(this)->format_bits = joan::orig::arg::as<std::uint32_t>(joan::orig::format::bit_info(joan::orig::arg::u32(desc[0])));  // 0x005fff90
        jo::self<jo::Surface>(this)->width = desc[6];
        jo::self<jo::Surface>(this)->height = desc[7];
        const std::uint32_t r = joan::orig::arg::as<std::uint32_t>(joan::orig::format::bytes_per_pixel(joan::orig::arg::u32(desc[0])));  // 0x005ffe60
        jo::view<std::uint32_t>(jo::self<jo::Surface>(this)->bytes_per_pixel) = r;
        return r;
    }
};

#define EN3D86(name, addr, fn) JOAN_REPLACE("en3d/b86_01", name, addr, fn)
EN3D86("VgaBuf::update_buf_desc", 0x0060fca0, &H::read_desc);
#undef EN3D86

}  // namespace game_b86_01
}  // namespace

// ---------------------------------------- from modules/game_b90_04.cpp (game/b90_04)
namespace {
namespace game_b90_04 {

struct H {
    // 0x0060ff20
    // eax = this, or the +0x28 call's result (two callers tail-return it).
    std::uintptr_t release_60ff20() {
        std::uintptr_t r = reinterpret_cast<std::uintptr_t>(this);
        if (jo::self<jo::Surface>(this)->locked == 0) return r;
        if (jo::view<void*>(jo::self<jo::Surface>(this)->surface) != nullptr) {
            void* const o = jo::view<void*>(jo::self<jo::Surface>(this)->surface);
            r = reinterpret_cast<std::uint32_t(JOAN_STDCALL*)(void*)>(static_cast<std::uintptr_t*>(jo::as_idirect3dsurface8(o)->vtable)[10])(o);  // +0x28
        }
        jo::self<jo::Surface>(this)->locked = 0;
        return r;
    }
};

#define M(name, addr, fn) JOAN_REPLACE("game/b90_04", name, addr, fn)
M("VgaBuf::unlock_buf", 0x0060ff20, &H::release_60ff20);
#undef M

}  // namespace game_b90_04
}  // namespace

// ---------------------------------------- from modules/game_b108_06.cpp (game/b108_06)
namespace {
namespace game_b108_06 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

using joan::orig::helpers::addr;

struct H {
    std::uintptr_t push_bit() {  // 0x00610040
        jo::self<jo::Surface>(this)->lock_stack = static_cast<u16>((static_cast<u32>(jo::self<jo::Surface>(this)->lock_stack) << 1) | jo::self<jo::Surface>(this)->locked);
        jo::self<jo::Surface>(this)->lock_depth = static_cast<u8>(jo::self<jo::Surface>(this)->lock_depth + 1);
        if (jo::self<jo::Surface>(this)->locked != 0) return addr(this);
        return joan::orig::arg::as<u32>(joan::orig::as_texture(this)->lock());  // 0x0060fe60
    }
};

#define B108(name, addr, fn) JOAN_REPLACE("game/b108_06", name, addr, fn)
B108("VgaBuf::temp_lock", 0x00610040, &H::push_bit);
#undef B108

}  // namespace game_b108_06
}  // namespace

// ---------------------------------------- from modules/game_b108_13.cpp (game/b108_13)
namespace {
namespace game_b108_13_2 {

using joan::call::thiscall;

using u8 = std::uint8_t;
using u32 = std::uint32_t;

template <class R = void, class... A>
R com_call(void* object, u32 offset, A... args) {  // COM: stdcall, this first
    return reinterpret_cast<R(JOAN_STDCALL*)(void*, A...)>((*static_cast<std::uintptr_t**>(object))[joan::call::x86_vslot(offset)])(
        object, args...);
}

struct H {
    // ------------------------------------------------------------------ 0x0060fd70
    u32 com_set(void* p) {
        const u32 had_owner = jo::self<jo::Surface>(this)->locked;
        if (jo::self<jo::Surface>(this)->locked != 0) joan::orig::as_image(this)->unlock();  // 0x0060ff20
        if (jo::view<void*>(jo::self<jo::Surface>(this)->surface) != nullptr) {
            com_call<u32>(jo::view<void*>(jo::self<jo::Surface>(this)->surface), 0x8);  // Release
            jo::self<jo::Surface>(this)->surface = nullptr;
        }
        jo::view<void*>(jo::self<jo::Surface>(this)->surface) = p;
        com_call<u32>(jo::view<void*>(jo::self<jo::Surface>(this)->surface), 0x4);  // AddRef
        if (had_owner != 0) return joan::orig::arg::as<u32>(joan::orig::as_texture(this)->lock());  // 0x0060fe60
        return joan::orig::arg::as<u32>(joan::orig::as_texture(this)->read_desc(1u));                   // 0x0060fca0
    }
};

#define B(name, addr, fn) JOAN_REPLACE("game/b108_13", name, addr, fn)
B("VgaBuf::handle_changed", 0x0060fd70, &H::com_set);
#undef B

}  // namespace game_b108_13_2
}  // namespace

// ---------------------------------------- from modules/game_b91_03.cpp (game/b91_03)
namespace {
namespace game_b91_03_2 {

using joan::call::thiscall;

struct H {
    // 0x0060ff70
    void bit_put() {
        jo::self<jo::Surface>(this)->lock_stack = static_cast<std::uint16_t>((static_cast<std::uint32_t>(jo::self<jo::Surface>(this)->lock_stack) << 1) |
                                                            jo::self<jo::Surface>(this)->locked);
        jo::self<jo::Surface>(this)->lock_depth = static_cast<std::uint8_t>(jo::self<jo::Surface>(this)->lock_depth + 1);
        if (jo::self<jo::Surface>(this)->locked != 0) joan::orig::as_image(this)->unlock();  // 0x0060ff20
    }

    // 0x006100a0
    void bit_drop() {
        const std::uint32_t bit = jo::self<jo::Surface>(this)->lock_stack & 1u;
        jo::self<jo::Surface>(this)->lock_stack = static_cast<std::uint16_t>(jo::self<jo::Surface>(this)->lock_stack >> 1);
        jo::self<jo::Surface>(this)->lock_depth = static_cast<std::uint8_t>(jo::self<jo::Surface>(this)->lock_depth - 1);
        if (bit == 0) joan::orig::as_image(this)->unlock();  // 0x0060ff20
    }
};

#define B91(name, addr, fn) JOAN_REPLACE("game/b91_03", name, addr, fn)
B91("VgaBuf::temp_unlock", 0x0060ff70, &H::bit_put);
B91("VgaBuf::temp_restore_unlock", 0x006100a0, &H::bit_drop);
#undef B91

}  // namespace game_b91_03_2
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::Image::rgb_to_pixel(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_07::H::rgb_to_pixel>(this, a0)); }  // 0x610160
auto joan::orig::Color::to_bytes_6101b0(std::uint32_t a0, void* a1) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_04::H::to_bytes_6101b0>(this, a0, a1)); }  // 0x6101b0
auto joan::orig::Unpacker::words_610210(const std::uint8_t* a0, std::int32_t a1, void* a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b108_02::H::words_610210>(this, a0, a1, a2)); }  // 0x610210
auto joan::orig::Pal::convert_610270(void* a0, std::int32_t a1, void* a2) -> void { joan::bind::member<&::game_b108_05_2::H::pal_convert>(this, a0, a1, a2); }  // 0x610270
auto joan::orig::Image::read_pixel(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b93_03::H::read_pixel>(this, a0)); }  // 0x610550
auto joan::orig::Image::blit_sprite(std::int32_t a0, std::int32_t a1, void* a2, void* a3, std::int32_t a4, void* a5, void* a6) -> void { joan::bind::member<&::game_b108_13::H::blit_sprite>(this, a0, a1, a2, a3, a4, a5, a6); }  // 0x610e10
auto joan::orig::Texture::save_bmp(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b115_08::H::save_bmp>(this, a0)); }  // 0x611e70
auto joan::orig::VgaBuf::construct(void* a0, void* a1, void* a2, void* a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b100_11::H::vga_init>(this, a0, a1, a2, a3)); }  // 0x612600
auto joan::orig::VgaBuf::free_all() -> void { joan::bind::member<&::game_b97_06::H::vgabuf_free_all>(this); }  // 0x612a00
auto joan::orig::VgaBuf::expand_palette(std::int32_t a0) -> void { joan::bind::member<&::game_b98_06::H::expand_palette>(this, a0); }  // 0x612dd0
auto joan::orig::Rect::set_612ee0(Palette24* a0, Palette8* a1, Palette16* a2, Palette32* a3) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b91_03::H::set4>(this, a0, a1, a2, a3)); }  // 0x612ee0
auto joan::orig::Image::fill_rect(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4) -> void { joan::bind::member<&::game_b108_15::H::blit_612f30>(this, a0, a1, a2, a3, a4); }  // 0x612f30
auto joan::orig::Image::ramp_channel(void* a0, std::int32_t a1, std::int32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::member<&::game_b92_05::H::palette_channel>(this, a0, a1, a2)); }  // 0x613090
auto joan::orig::Camera::get_20() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_world::H::camera_20>(this)); }  // 0x60fa70
auto joan::orig::Image::construct() -> void* { return joan::bind::conv<void*>(joan::bind::member<&::en3d_b85_04::H::image_ctor>(this)); }  // 0x60fad0
auto joan::orig::Image::dtor() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_04::H::dtor_60fb10>(this)); }  // 0x60fb10
auto joan::orig::Image::create(std::uint32_t a0, std::uint32_t a1, std::uint32_t a2, std::int32_t a3) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_04::H::image_create>(this, a0, a1, a2, a3)); }  // 0x60fb60
auto joan::orig::Image::attach_surface(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::member<&::game_b99_07::H::attach_60fc20>(this, a0)); }  // 0x60fc20
auto joan::orig::Texture::read_desc(std::uint32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b86_01::H::read_desc>(this, a0)); }  // 0x60fca0
auto joan::orig::Image::release() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::en3d_b85_04::H::image_release>(this)); }  // 0x60fd20
auto joan::orig::Image::set_surface(void* a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_13_2::H::com_set>(this, a0)); }  // 0x60fd70
auto joan::orig::Texture::lock() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b111_15::H::lock>(this)); }  // 0x60fe60
auto joan::orig::Image::unlock() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b90_04::H::release_60ff20>(this)); }  // 0x60ff20
auto joan::orig::BitWriter::put() -> void { joan::bind::member<&::game_b91_03_2::H::bit_put>(this); }  // 0x60ff70
auto joan::orig::Image::pop_lock() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_18::H::bit_shift>(this)); }  // 0x60ffd0
auto joan::orig::BitWriter::push_bit() -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::member<&::game_b108_06::H::push_bit>(this)); }  // 0x610040
auto joan::orig::BitWriter::drop() -> void { joan::bind::member<&::game_b91_03_2::H::bit_drop>(this); }  // 0x6100a0
