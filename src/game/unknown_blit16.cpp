// game/unknown_blit16.cpp: functions whose original source file is unknown (formerly game/_unplaced/blit16.cpp;
// re/attribution.csv: between ww/world_s.cpp and ww/ilib/angle.cpp). They move to their original file when evidence
// names it (Phase 3).
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
#include "../registry.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;

using u16 = std::uint16_t;

using joan::hook::global;

}  // namespace

// ---------------------------------------- from modules/game_b115_02.cpp (game/b115_02)
namespace {
namespace game_b115_02 {

using i32 = std::int32_t;

// ------------------------------------------------------------------ 0x00572feb Rle::skip (thunk 0x00408f2b)
// Skips `count` pixels of an RLE stream (byte < 0xf8: one pixel; 0xf9..0xff: run of 256 - code pixels; 0xf8 n: run
// of n pixels). *consumed (in / out) = pixels already consumed of the current run (0 at a code boundary). Returns
// the advanced stream; when the skip ends inside a run, the stream points back at the run code and *consumed = the
// pixels of it consumed, else *consumed = 0 (unchanged when count is 0 on entry or after a run).
// The original had a register contract (ecx = count, ah = consumed, esi = stream; out esi, ah) and one caller, the
// asm glue of Font::pixel_575940 (game_b111_06 below, through the thunk); both are C++ since P1, so this is a plain
// function now. The 8-bit arithmetic of al / ah / cl / dl is kept (cl: the low byte of the remaining count).
const u8* JOAN_CDECL skip_572feb(const u8* s, u32 count, u8* consumed) {
    u8 ah = *consumed;
    for (;;) {
        if (count == 0) break;  // jecxz: ah unchanged
        u8 al = 0;
        for (;;) {               // single pixels
            al = *s++;
            if (al >= 0xf8) break;
            if (--count == 0) {  // loop
                *consumed = 0;
                return s;
            }
        }
        u32 run = 0;
        u32 back = 0;
        if (al != 0xf8) {
            run = static_cast<u8>(static_cast<u8>(-al) - ah);  // neg dl / sub dl, ah
            back = 1;
        } else {
            run = static_cast<u8>(*s++ - ah);  // lodsb / sub dl, ah
            back = 2;
        }
        const u8 left = static_cast<u8>(static_cast<u8>(count) + ah);  // mov al, cl / add al, ah
        const bool borrow = count < run;
        count -= run;
        ah = 0;
        if (borrow) {
            s -= back;
            ah = left;
            break;
        }
    }
    *consumed = ah;
    return s;
}

// The two keyed blitters: an 8-bit sprite (u16 width, u16 height, width * height bytes) drawn through a palette of
// dwords into a 24 / 32-bit surface at (dx, dy); source byte 0xff is transparent. No clipping (the caller clips).
// Like the original: the surface base is stored in 0xa15010, the destination offset pitch * dy + dx * bpp and the
// row step pitch - width * bpp wrap mod 2^32, and width / height 0 count as 2^32 (the loop instruction). Returns
// eax as the original leaves it (the last source byte, or the last palette value as left by the blitter).
// ------------------------------------------------------------------ 0x00574561 Blit::blit8to24_key (thunk 0x00401e9c)
// stdcall, 6 stack args. Callers: Surface::blit_sprite 0x610e10 (game_b108_13).
u32 JOAN_STDCALL blit8to24_key_574561(void* base, u32 pitch, u32 dx, u32 dy, void* sprite, void* palette) {
    global<u32>(d::g_surface_base) = joan::call::pointer_low_dword(base);  // 4-byte image slot only the original's asm read; see pointer_low_dword
    const u8* s = static_cast<const u8*>(sprite);
    const u8* const pal = static_cast<const u8*>(palette);
    u16 w16 = 0, h16 = 0;
    std::memcpy(&w16, s, 2);
    std::memcpy(&h16, s + 2, 2);
    s += 4;
    const u32 width = w16;
    u32 rows = h16;
    const i32 step = static_cast<i32>(pitch - width * 3u);
    u8* d = static_cast<u8*>(base) + static_cast<u32>(pitch * dy + dx * 3u);
    u32 eax = rows;
    do {
        u32 cols = width;
        do {
            eax = *s;
            d += 3;
            if (eax != 0xffu) {
                u32 v = 0;
                std::memcpy(&v, pal + eax * 4u, 4);
                d[-3] = static_cast<u8>(v);
                d[-2] = static_cast<u8>(v >> 8);
                d[-1] = static_cast<u8>(v >> 16);
                eax = (v >> 8) | (v << 24);  // ror eax, 8
            }
            ++s;
        } while (--cols != 0);
        d += step;
    } while (--rows != 0);
    return eax;
}

// ------------------------------------------------------------------ 0x005745da Blit::blit8to32_key_5745da (thunk 0x0040737e)
// stdcall, 6 stack args. Callers: Surface::blit_sprite 0x610e10 (game_b108_13).
u32 JOAN_STDCALL blit8to32_key_5745da(void* base, u32 pitch, u32 dx, u32 dy, void* sprite, void* palette) {
    global<u32>(d::g_surface_base) = joan::call::pointer_low_dword(base);  // 4-byte image slot only the original's asm read; see pointer_low_dword
    const u8* s = static_cast<const u8*>(sprite);
    const u8* const pal = static_cast<const u8*>(palette);
    u16 w16 = 0, h16 = 0;
    std::memcpy(&w16, s, 2);
    std::memcpy(&h16, s + 2, 2);
    s += 4;
    const u32 width = w16;
    u32 rows = h16;
    const i32 step = static_cast<i32>(pitch - width * 4u);
    u8* d = static_cast<u8*>(base) + static_cast<u32>(pitch * dy + dx * 4u);
    u32 eax = rows;
    do {
        u32 cols = width;
        do {
            eax = *s;
            d += 4;
            if (eax != 0xffu) {
                std::memcpy(&eax, pal + eax * 4u, 4);
                std::memcpy(d - 4, &eax, 4);
            }
            ++s;
        } while (--cols != 0);
        d += step;
    } while (--rows != 0);
    return eax;
}

#define R(name, addr, fn) JOAN_REPLACE("game/b115_02", name, addr, &fn)
R("Rle::skip", 0x00572feb, skip_572feb);
R("Blit::blit8to24_key", 0x00574561, blit8to24_key_574561);
R("Blit::blit8to32_key_5745da", 0x005745da, blit8to32_key_5745da);
#undef R

}  // namespace game_b115_02
}  // namespace

// ---------------------------------------- from modules/game_b99_06.cpp (game/b99_06)
namespace {
namespace game_b99_06 {

JOAN_COUNTER(c_blit16k, "blit8to16_key");

// ---- hand-written asm blitter ----
// 0x005744f0: 8-bit source (word width, word height, rows) into a 16-bit surface at (dx, dy);
// source byte 0xff is transparent.
void JOAN_STDCALL blit8to16_key(void* base, u32 pitch, u32 dx, u32 dy, u8* src, const u16* palette) {
    c_blit16k.hit();
    global<u32>(d::g_surface_base) = joan::call::pointer_low_dword(base);  // 4-byte image slot only the original's asm read; see pointer_low_dword
    const u32 width = *reinterpret_cast<const u16*>(src);
    u32 rows = *reinterpret_cast<const u16*>(src + 2);
    u8* s = src + 4;
    const u32 step = pitch - (width + width);
    u8* d = static_cast<u8*>(base) + static_cast<u32>(pitch * dy + dx * 2u);  // g_surface_base as just stored
    do {
        u32 cols = width;
        do {
            const u32 b = *s;
            d += 2;
            if (b != 0xffu) {
                const u16 v = palette[b];
                std::memcpy(d - 2, &v, 2);
            }
            ++s;
        } while (--cols != 0);
        d += step;
    } while (--rows != 0);
}

#define B(name, addr, fn) JOAN_REPLACE("game/b99_06", name, addr, fn)
B("blit8to16_key", 0x005744f0, &blit8to16_key);
#undef B

}  // namespace game_b99_06
}  // namespace

// ---------------------------------------- from modules/game_b111_06.cpp (game/b111_06)
namespace {
namespace game_b111_06 {

using joan::orig::helpers::at;

JOAN_COUNTER(c_575940, "Font::pixel_575940");

// ------------------------------------------------------------------ 0x00575940
// The original's asm glue called Rle::skip 0x00572feb (thunk 0x00408f2b) with ecx = count, ah = 0, esi = stream and
// took the advanced esi; now a direct call of the C++ Rle::skip (game_b115_02 above).
const u8* rle_skip(const u8* stream, u32 count) {
    u8 consumed = 0;
    return ::game_b115_02::skip_572feb(stream, count, &consumed);
}

static u32 JOAN_STDCALL font_pixel_575940(const u8* glyph, u32 x, u32 y) {
    c_575940.hit();
    const u32 width = at<u16>(glyph, 0);  // x64: byte stream (RLE glyph data, not an object); lods ax into a zeroed eax ([ebp-0x8] height: dead)
    const u32 count = y * width + x;      // mul (low dword) + x
    const u8* const p = rle_skip(glyph + 4, count);
    const u8 v = *p;
    return v >= 0xf8 ? 0xffu : v;
}

#define G(name, addr, fn) JOAN_REPLACE("game/b111_06", name, addr, fn)
G("Font::pixel_575940", 0x00575940, &font_pixel_575940);
#undef G

}  // namespace game_b111_06
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::blit8to16_key(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, void* a5) -> void { joan::bind::free<&::game_b99_06::blit8to16_key>(a0, a1, a2, a3, a4, a5); }  // 0x5744f0
auto joan::orig::blit::blit8to24_key(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, void* a5) -> void { joan::bind::free<&::game_b115_02::blit8to24_key_574561>(a0, a1, a2, a3, a4, a5); }  // 0x574561
auto joan::orig::blit::blit8to32_key_5745da(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, void* a4, void* a5) -> void { joan::bind::free<&::game_b115_02::blit8to32_key_5745da>(a0, a1, a2, a3, a4, a5); }  // 0x5745da
auto joan::orig::font::pixel_575940(void* a0, std::uint32_t a1, std::uint32_t a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b111_06::font_pixel_575940>(a0, a1, a2)); }  // 0x575940
