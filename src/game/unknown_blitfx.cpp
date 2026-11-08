// game/unknown_blitfx.cpp: functions whose original source file is unknown (formerly game/_unplaced/blitfx.cpp;
// re/attribution.csv: between ww/world_s.cpp and ww/ilib/angle.cpp). They move to their original file when evidence
// names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
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

using i32 = std::int32_t;

}  // namespace

// ---------------------------------------- from modules/game_b96_05.cpp (game/b96_05)
namespace {
namespace game_b96_05 {

JOAN_COUNTER(c_fill_rect, "fill_rect");

// ---- hand-written asm ----
// 0x0056fba0: fills [x0..x1] x [y0..y1] with `color` (pixel size from [0xa15039]).
// Counters behave like the asm's `dec ecx; jnz` (an empty row count wraps).
void JOAN_STDCALL fill_rect(void* base, std::uint32_t pitch, std::uint32_t x0, std::uint32_t y0,
                         std::uint32_t x1, std::uint32_t y1, std::uint32_t color) {
    c_fill_rect.hit();
    global<std::uint32_t>(d::g_surface_base) = joan::call::pointer_low_dword(base);  // 4-byte image slot only the original's asm read; see pointer_low_dword
    const std::uint32_t mode = global<std::uint32_t>(d::g_a15039);
    const std::uint32_t width = x1 - x0 + 1u;
    std::uint32_t rows = y1 - y0 + 1u;
    if (mode == 3) {
        const std::int32_t quads = static_cast<std::int32_t>(width) >> 2;
        const std::uint32_t rem = (width & 3u) * 3u;
        const std::uint32_t stride = pitch - width - width - width;
        std::uint8_t pattern[16];
        std::memcpy(pattern + 0, &color, 4);
        std::memcpy(pattern + 3, &color, 4);
        std::memcpy(pattern + 6, &color, 4);
        std::memcpy(pattern + 9, &color, 4);
        std::uint8_t* d = static_cast<std::uint8_t*>(base) + (pitch * y0 + x0 * 3u);
        if (quads != 0) {
            do {
                std::uint32_t n = static_cast<std::uint32_t>(quads);
                do {
                    std::memcpy(d, pattern, 12);
                    d += 12;
                } while (--n != 0);
                std::memcpy(d, pattern, rem);
                d += rem;
                d += stride;
            } while (--rows != 0);
        } else {
            do {
                std::memcpy(d, pattern, rem);
                d += rem;
                d += stride;
            } while (--rows != 0);
        }
    } else if (mode == 4) {
        const std::uint32_t stride = pitch - width * 4u;
        std::uint8_t* d = static_cast<std::uint8_t*>(base) + (pitch * y0 + x0 * 4u);
        if (width == 1) {
            do {
                std::memcpy(d, &color, 4);
                d += pitch;
            } while (--rows != 0);
        } else {
            do {
                for (std::uint32_t i = 0; i < width; ++i) {
                    std::memcpy(d, &color, 4);
                    d += 4;
                }
                d += stride;
            } while (--rows != 0);
        }
    } else {
        const std::uint32_t stride = pitch - width - width;
        const std::uint16_t c16 = static_cast<std::uint16_t>(color);
        std::uint8_t* d = static_cast<std::uint8_t*>(base) + (pitch * y0 + x0 * 2u);
        if (width == 1) {
            do {
                std::memcpy(d, &c16, 2);
                d += pitch;
            } while (--rows != 0);
        } else {
            const std::uint32_t c32 = (static_cast<std::uint32_t>(c16) << 16) | c16;
            const std::uint32_t pairs = width >> 1;
            const bool odd = (width & 1u) != 0;
            do {
                for (std::uint32_t i = 0; i < pairs; ++i) {
                    std::memcpy(d, &c32, 4);
                    d += 4;
                }
                if (odd) {
                    std::memcpy(d, &c16, 2);
                    d += 2;
                }
                d += stride;
            } while (--rows != 0);
        }
    }
}

#define B(name, addr, fn) JOAN_REPLACE("game/b96_05", name, addr, fn)
B("fill_rect", 0x0056fba0, &fill_rect);
#undef B

}  // namespace game_b96_05
}  // namespace

// ---------------------------------------- from modules/game_b106_20.cpp (game/b106_20)
namespace {
namespace game_b106_20 {

JOAN_COUNTER(c_shade_56fdb0, "build_shade_tables");

// ------------------------------------------------------------------ 0x0056fdb0
u32 JOAN_STDCALL build_shade_tables(i32 mode) {
    c_shade_56fdb0.hit();
    global<i32>(d::g_a0c780) = mode;
    if (mode >= 4 && mode <= 7) {
        u8* const tab = &img::global<u8>(d::g_a0f844);
        u32 k = 0;
        for (i32 c = -32; c <= 32; ++c) {
            for (u32 i = 0; i < 0x100; ++i) {
                u32 v;
                if (c <= 0) {
                    v = (i * static_cast<u32>(c + 32)) >> 5;
                } else {
                    v = (i * static_cast<u32>(32 - c) + static_cast<u32>(c) * 255u) >> 5;
                }
                tab[k++] = static_cast<u8>(v);
            }
        }
        return 0x100;
    }
    u16* const t0 = &img::global<u16>(d::g_a0c784);
    u16* const t1 = &img::global<u16>(d::g_a0d7c4);
    u16* const t2 = &img::global<u16>(d::g_a0e804);
    u32 k = 0;
    for (i32 c = -32; c <= 32; ++c) {
        for (u32 i = 0; i < 0x20; ++i) {
            u32 v;
            if (c <= 0) {
                v = (i * static_cast<u32>(c + 32)) >> 5;
            } else {
                v = (i * static_cast<u32>(32 - c) + static_cast<u32>(c) * 31u) >> 5;
            }
            if (static_cast<i32>(v) > 0x1f) v = 0x1f;
            if (static_cast<i32>(v) < 0) v = 0;
            const u16 a = static_cast<u16>(v);
            if (mode == 0) {
                t0[k] = a;
                t1[k] = static_cast<u16>(a << 5);
                t2[k] = static_cast<u16>(a << 10);
            } else if (mode == 1) {
                t0[k] = a;
                t1[k] = static_cast<u16>(a << 6);
                t2[k] = static_cast<u16>(a << 11);
            } else if (mode == 2) {
                t2[k] = a;
                t1[k] = static_cast<u16>(a << 5);
                t0[k] = static_cast<u16>(a << 10);
            } else {
                t2[k] = a;
                t1[k] = static_cast<u16>(a << 6);
                t0[k] = static_cast<u16>(a << 11);
            }
            ++k;
        }
    }
    return 0x20;
}

JOAN_REPLACE("game/b106_20", "build_shade_tables", 0x0056fdb0, &build_shade_tables);

}  // namespace game_b106_20
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::fill_rect(void* a0, std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5, std::uint32_t a6) -> void { joan::bind::free<&::game_b96_05::fill_rect>(a0, a1, a2, a3, a4, a5, a6); }  // 0x56fba0
auto joan::orig::build_shade_tables(std::int32_t a0) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b106_20::build_shade_tables>(a0)); }  // 0x56fdb0
