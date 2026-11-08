// game/unknown_clip.cpp: functions whose original source file is unknown (formerly game/_unplaced/clip.cpp;
// re/attribution.csv: between ww/uti3d/videocap.cpp and ww/enphysic/collimgr.cpp). They move to their original file
// when evidence names it (Phase 3).
//
// Moved from the batch modules by re/split_modules.py. Each block keeps the helpers of its batch module in a
// namespace named after that module and registers its functions with the module's key (openjoan.ini
// [Hooks] Disable= and the logs use it), name and address unchanged.

#include "../callconv.hpp"
#include "../original/helpers.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstddef>
#include <cstdarg>

#include "../lib/crt/crt.hpp"
#include "../original/direct.hpp"
#include "../data_names.hpp"
#include "../fn_names.hpp"
#include "../registry.hpp"
#include "../x87.hpp"
#include "../original/dynarray.hpp"
#include "../original/vecmath.hpp"
#include "../render/overrides.hpp"
#include "../original/bind.hpp"

// Helpers shared by the blocks below (identical in their batch modules).
namespace {

using i32 = std::int32_t;

using joan::call::cdecl_call;

using u8 = std::uint8_t;

using u32 = std::uint32_t;

}  // namespace

// ---------------------------------------- from modules/game_b93_06.cpp (game/b93_06)
namespace {
namespace game_b93_06 {

JOAN_COUNTER(c_outcode, "Clip::outcode");

// 0x0068cab0: x < xmin (or NaN) -> 8, else x > xmax -> 4; y < ymin (or NaN) -> 2, else
// y > ymax -> 1.
static i32 JOAN_CDECL outcode(float x, float y, float xmin, float xmax, float ymin, float ymax) {
    c_outcode.hit();
    i32 code = 0;
    if (!(x >= xmin)) {
        code |= 8;
    } else if (x > xmax) {
        code |= 4;
    }
    if (!(y >= ymin)) {
        code |= 2;
    } else if (y > ymax) {
        code |= 1;
    }
    return code;
}

#define B93(name, addr, fn) JOAN_REPLACE("game/b93_06", name, addr, fn)
B93("Clip::outcode", 0x0068cab0, &outcode);
#undef B93

}  // namespace game_b93_06
}  // namespace

// ---------------------------------------- from modules/game_b94_07.cpp (game/b94_07)
namespace {
namespace game_b94_07 {

JOAN_COUNTER(c_clip_line, "clip_line");

// ---------------------------------------------------------------------------- 0x0068cb50
static u32 outcode(float x, float y, float a, float b, float c, float d) {
    return joan::orig::arg::as<u32>(joan::orig::clip::outcode(joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::f32(a), joan::orig::arg::f32(b), joan::orig::arg::f32(c), joan::orig::arg::f32(d)));  // 0x0068cab0
}
// Bounds: x < xmin -> 8, x > xmax -> 4, y < ymin -> 2, y > ymax -> 1 (0x0068cab0).
static u32 JOAN_CDECL clip_line(float* x0, float* y0, float* x1, float* y1, float xmin, float xmax, float ymin,
                             float ymax) {
    c_clip_line.hit();
    u32 c0 = outcode(*x0, *y0, xmin, xmax, ymin, ymax);
    u32 c1 = outcode(*x1, *y1, xmin, xmax, ymin, ymax);
    u8 done = 0;
    u8 accept = 0;
    do {
        if ((c0 | c1) == 0) {
            accept = 1;
            done = 1;
            continue;
        }
        if ((c0 & c1) != 0) {
            done = 1;
            continue;
        }
        const u32 code = c0 != 0 ? c0 : c1;
        float nx = 0.0f;  // [ebp-0x14]: always written (code has one of bits 1/2/4/8)
        float ny = 0.0f;  // [ebp-0x18]
        if ((code & 1) != 0) {
            nx = (*x1 - *x0) * (ymax - *y0) / (*y1 - *y0) + *x0;
            ny = ymax;
        } else if ((code & 2) != 0) {
            nx = (*x1 - *x0) * (ymin - *y0) / (*y1 - *y0) + *x0;
            ny = ymin;
        } else if ((code & 4) != 0) {
            ny = (*y1 - *y0) * (xmax - *x0) / (*x1 - *x0) + *y0;
            nx = xmax;
        } else if ((code & 8) != 0) {
            ny = (*y1 - *y0) * (xmin - *x0) / (*x1 - *x0) + *y0;
            nx = xmin;
        }
        if (code == c0) {
            *x0 = nx;
            *y0 = ny;
            c0 = outcode(*x0, *y0, xmin, xmax, ymin, ymax);
        } else {
            *x1 = nx;
            *y1 = ny;
            c1 = outcode(*x1, *y1, xmin, xmax, ymin, ymax);
        }
    } while (done == 0);
    return accept;
}

#define B94(name, addr, fn) JOAN_REPLACE("game/b94_07", name, addr, fn)
B94("clip_line", 0x0068cb50, &clip_line);
#undef B94

}  // namespace game_b94_07
}  // namespace

// ---------------------------------------- from modules/game_b106_14.cpp (game/b106_14)
namespace {
namespace game_b106_14 {

using joan::orig::helpers::as_float;
u32 as_bits(float f) {
    u32 b;
    std::memcpy(&b, &f, 4);
    return b;
}

JOAN_COUNTER(c_poly_rect, "polygon_overlaps_rect");

// ---------------------------------------------------------------- 0x0068ce00 helpers
u32 outcode(u32 x, u32 y, u32 xmin, u32 xmax, u32 ymin, u32 ymax) {
    return joan::orig::arg::as<u32>(joan::orig::clip::outcode(joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::f32(xmin), joan::orig::arg::f32(xmax), joan::orig::arg::f32(ymin), joan::orig::arg::f32(ymax)));  // 0x0068cab0
}
using joan::orig::helpers::op_new;
using joan::orig::helpers::op_delete;
// ((b1 - b0) * (v - a0)) / (a1 - a0) + b0, all float operands.
// One statement per x87 operation (no contraction).
float clip_at(float a0, float b0, float a1, float b1, float v) {
    const float db = b1 - b0;
    const float dv = v - a0;
    const float num = db * dv;
    const float da = a1 - a0;
    const float q = num / da;
    return q + b0;
}

// 0x0068ce00
u8 JOAN_CDECL polygon_overlaps_rect(const u32* pts, i32 n, u32 xmin, u32 xmax, u32 ymin, u32 ymax) {
    c_poly_rect.hit();
    u32* fl = static_cast<u32*>(op_new(static_cast<u32>(n) << 1 << 2));
    u32 all = 0xffffffffu, any = 0;
    for (i32 i = 0; i < n; ++i) {
        const u32 c = outcode(pts[i * 2], pts[i * 2 + 1], xmin, xmax, ymin, ymax);
        fl[i] = c;
        all &= c;
        any |= c;
    }
    if (all != 0) {
        op_delete(fl);
        return 0;
    }
    if (any == 0) {
        op_delete(fl);
        return 1;
    }
    i32 m = n;
    u32* cp = static_cast<u32*>(op_new(static_cast<u32>(m) << 1 << 1 << 2));
    std::memcpy(static_cast<void*>(cp), static_cast<const void*>(pts), joan::orig::arg::u32(static_cast<u32>(m) << 1 << 2));  // memcpy
    i32 k = 0;
    u32* out = static_cast<u32*>(op_new(static_cast<u32>(m) << 1 << 1 << 2));
    // x pass
    for (i32 i = 0; i < m; ++i) {
        const i32 j = (i + 1) % n;
        const float x0 = as_float(cp[i * 2]), y0 = as_float(cp[i * 2 + 1]);
        const float x1 = as_float(cp[j * 2]), y1 = as_float(cp[j * 2 + 1]);
        if (fl[i] & 8) {
            if (fl[j] & 8) continue;
            out[k * 2] = xmin;
            out[k * 2 + 1] = as_bits(clip_at(x0, y0, x1, y1, as_float(xmin)));
            ++k;
        } else if (fl[i] & 4) {
            if (fl[j] & 4) continue;
            out[k * 2] = xmax;
            out[k * 2 + 1] = as_bits(clip_at(x0, y0, x1, y1, as_float(xmax)));
            ++k;
        } else {
            out[k * 2] = cp[i * 2];
            out[k * 2 + 1] = cp[i * 2 + 1];
            ++k;
        }
        if (fl[j] & 8) {
            out[k * 2] = xmin;
            out[k * 2 + 1] = as_bits(clip_at(x0, y0, x1, y1, as_float(xmin)));
            ++k;
        } else if (fl[j] & 4) {
            out[k * 2] = xmin;  // the original clips at xmin here too
            out[k * 2 + 1] = as_bits(clip_at(x0, y0, x1, y1, as_float(xmin)));
            ++k;
        }
    }
    all = 0xffffffffu;
    any = 0;
    for (i32 i = 0; i < k; ++i) {
        const u32 c = outcode(out[i * 2], out[i * 2 + 1], xmin, xmax, ymin, ymax);
        fl[i] = c;
        all &= c;
        any |= c;
    }
    if (all != 0) {
        op_delete(fl);
        op_delete(cp);
        op_delete(out);
        return 0;
    }
    if (any == 0) {
        op_delete(fl);
        op_delete(cp);
        op_delete(out);
        return 1;
    }
    m = k;
    std::memcpy(static_cast<void*>(cp), static_cast<const void*>(out), joan::orig::arg::u32(static_cast<u32>(k) << 1 << 2));  // memcpy
    k = 0;
    // y pass (successor index modulo the original n)
    for (i32 i = 0; i < m; ++i) {
        const i32 j = (i + 1) % n;
        const float x0 = as_float(cp[i * 2]), y0 = as_float(cp[i * 2 + 1]);
        const float x1 = as_float(cp[j * 2]), y1 = as_float(cp[j * 2 + 1]);
        if (fl[i] & 2) {
            if (fl[j] & 2) continue;
            out[k * 2] = as_bits(clip_at(y0, x0, y1, x1, as_float(ymin)));
            out[k * 2 + 1] = ymin;
            ++k;
        } else if (fl[i] & 1) {
            if (fl[j] & 1) continue;
            out[k * 2] = as_bits(clip_at(y0, x0, y1, x1, as_float(ymax)));
            out[k * 2 + 1] = ymax;
            ++k;
        } else {
            out[k * 2] = cp[i * 2];
            out[k * 2 + 1] = cp[i * 2 + 1];
            ++k;
        }
        if (fl[j] & 2) {
            out[k * 2] = as_bits(clip_at(y0, x0, y1, x1, as_float(ymin)));
            out[k * 2 + 1] = ymin;
            ++k;
        } else if (fl[j] & 1) {
            out[k * 2] = as_bits(clip_at(y0, x0, y1, x1, as_float(ymax)));
            out[k * 2 + 1] = ymax;
            ++k;
        }
    }
    op_delete(fl);
    op_delete(cp);
    op_delete(out);
    return k != 0 ? 1 : 0;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b106_14", name, addr, fn)
B("polygon_overlaps_rect", 0x0068ce00, &polygon_overlaps_rect);
#undef B

}  // namespace game_b106_14
}  // namespace

// ---------------------------------------- from modules/game_b92_06.cpp (game/b92_06)
namespace {
namespace game_b92_06 {

// 0x0068d690
std::int32_t JOAN_CDECL outcode(float x, float y, float z, float x0, float x1, float y0, float y1, float z0, float z1) {
    std::int32_t r = 0;
    if (!(x >= x0))
        r |= 0x10;
    else if (x > x1)
        r |= 0x20;
    if (!(y >= y0))
        r |= 4;
    else if (y > y1)
        r |= 8;
    if (!(z >= z0))
        r |= 2;
    else if (z > z1)
        r |= 1;
    return r;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b92_06", name, addr, fn)
B("outcode", 0x0068d690, &outcode);
#undef B

}  // namespace game_b92_06
}  // namespace

// ---------------------------------------- from modules/game_b104_13.cpp (game/b104_13)
namespace {
namespace game_b104_13 {

JOAN_COUNTER(c_clip3d, "clip_segment_3d");

// ------------------------------------------------------------------ 0x0068d760
u32 outcode(float x, float y, float z, float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) {
    return joan::orig::arg::as<u32>(joan::orig::outcode(joan::orig::arg::f32(x), joan::orig::arg::f32(y), joan::orig::arg::f32(z), joan::orig::arg::f32(xmin), joan::orig::arg::f32(xmax), joan::orig::arg::f32(ymin), joan::orig::arg::f32(ymax), joan::orig::arg::f32(zmin), joan::orig::arg::f32(zmax)));  // 0x0068d690
}

u32 JOAN_CDECL clip_segment_3d(float x0, float y0, float z0, float x1, float y1, float z1, float xmin, float xmax,
                            float ymin, float ymax, float zmin, float zmax) {
    c_clip3d.hit();
    u32 c0 = outcode(x0, y0, z0, xmin, xmax, ymin, ymax, zmin, zmax);
    u32 c1 = outcode(x1, y1, z1, xmin, xmax, ymin, ymax, zmin, zmax);
    bool done = false;
    bool accept = false;
    do {
        if ((c0 | c1) == 0) {
            accept = true;
            done = true;
        } else if ((c0 & c1) != 0) {
            done = true;
        } else {
            const u32 c = c0 != 0 ? c0 : c1;
            float nx = 0.0f, ny = 0.0f, nz = 0.0f;  // [ebp-0x18/-0x1c/-0x20]
            if ((c & 1) != 0) {
                const float num = zmax - z0;
                const float den = z1 - z0;
                const float t = num / den;
                float a = x1 - x0;
                a = a * t;
                nx = a + x0;
                float b = y1 - y0;
                b = b * t;
                ny = b + y0;
                nz = zmax;
            } else if ((c & 2) != 0) {
                const float num = zmin - z0;
                const float den = z1 - z0;
                const float t = num / den;
                float a = x1 - x0;
                a = a * t;
                nx = a + x0;
                float b = y1 - y0;
                b = b * t;
                ny = b + y0;
                nz = zmin;
            } else if ((c & 4) != 0) {
                const float num = ymin - y0;
                const float den = y1 - y0;
                const float t = num / den;
                float a = x1 - x0;
                a = a * t;
                nx = a + x0;
                ny = ymin;
                float b = z1 - z0;
                b = b * t;
                nz = b + z0;
            } else if ((c & 8) != 0) {
                const float num = ymax - y0;
                const float den = y1 - y0;
                const float t = num / den;
                float a = x1 - x0;
                a = a * t;
                nx = a + x0;
                ny = ymax;
                float b = z1 - z0;
                b = b * t;
                nz = b + z0;
            } else if ((c & 0x10) != 0) {
                const float num = xmin - x0;
                const float den = x1 - x0;
                const float t = num / den;
                nx = xmin;
                float a = y1 - y0;
                a = a * t;
                ny = a + y0;
                float b = z1 - z0;
                b = b * t;
                nz = b + z0;
            } else if ((c & 0x20) != 0) {
                const float num = xmax - x0;
                const float den = x1 - x0;
                const float t = num / den;
                nx = xmax;
                float a = y1 - y0;
                a = a * t;
                ny = a + y0;
                float b = z1 - z0;
                b = b * t;
                nz = b + z0;
            }
            if (c == c0) {
                x0 = nx;
                y0 = ny;
                z0 = nz;
                c0 = outcode(x0, y0, z0, xmin, xmax, ymin, ymax, zmin, zmax);
            } else {
                x1 = nx;
                y1 = ny;
                z1 = nz;
                c1 = outcode(x1, y1, z1, xmin, xmax, ymin, ymax, zmin, zmax);
            }
        }
    } while (!done);
    return accept ? 1u : 0u;
}

#define B(name, addr, fn) JOAN_REPLACE("game/b104_13", name, addr, fn)
B("clip_segment_3d", 0x0068d760, &clip_segment_3d);
#undef B

}  // namespace game_b104_13
}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::clip::outcode(float a0, float a1, float a2, float a3, float a4, float a5) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b93_06::outcode>(a0, a1, a2, a3, a4, a5)); }  // 0x68cab0
auto joan::orig::clip_line(void* a0, void* a1, void* a2, void* a3, float a4, float a5, float a6, float a7) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::game_b94_07::clip_line>(a0, a1, a2, a3, a4, a5, a6, a7)); }  // 0x68cb50
auto joan::orig::polygon_overlaps_rect(void* a0, std::int32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4, std::uint32_t a5) -> std::uint8_t { return joan::bind::conv<std::uint8_t>(joan::bind::free<&::game_b106_14::polygon_overlaps_rect>(a0, a1, a2, a3, a4, a5)); }  // 0x68ce00
auto joan::orig::outcode(float a0, float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::game_b92_06::outcode>(a0, a1, a2, a3, a4, a5, a6, a7, a8)); }  // 0x68d690
