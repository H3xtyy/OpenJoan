// Texture file decoding and D3DX8-exact texture contents (see image.hpp).
//
// Every routine below names the joan.exe code it reproduces. x87 details that matter for the
// results are kept: which intermediate values are rounded to float (stored) and which stay in
// a register at the precision-control setting (template parameter R: Single = 24-bit, double =
// 53-bit significands), and the code sections D3DX runs with the rounding mode set to truncate
// (0x0078f5b5 .. 0x0078f5e7, its pixel encoders).
//
// Must not be compiled with floating-point contraction (FMA) or /fp:fast: the PC24 DXT compressor
// then rounds differently (the hook builds with MSVC /arch:SSE2 /fp:precise, which has neither).
// Verified against joan.exe's own code run in a 32-bit harness, at both precisions: all game
// texture files and ~200 synthetic BMP / DIB / DDS / TGA variants, plus mutation fuzzing.
#include "image.hpp"

#include "../fp/fma.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace joan::render {
namespace {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using namespace d3dfmt;

u16 rd16(const u8* p) { return static_cast<u16>(p[0] | p[1] << 8); }
u32 rd32(const u8* p) { return u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24; }
void wr16(u8* p, u16 v) { p[0] = static_cast<u8>(v); p[1] = static_cast<u8>(v >> 8); }
void wr32(u8* p, u32 v) {
    p[0] = static_cast<u8>(v);
    p[1] = static_cast<u8>(v >> 8);
    p[2] = static_cast<u8>(v >> 16);
    p[3] = static_cast<u8>(v >> 24);
}
float f32(u32 bits) {
    float f;
    std::memcpy(&f, &bits, 4);
    return f;
}
bool fail(std::string* why, const char* text) {
    if (why) *why = text;
    return false;
}

// D3DX constants.
const float kInv255 = f32(0x3b808081);  // 0x009e3408 (float)(1 / 255)
const float kWeight[3] = {f32(0x3da7ef9e), f32(0x3f1c01a3), f32(0x3e9e00d2)};  // 0x00a2f708: blue, green, red
// 0x00a2f668: ordered dither added before truncation (rows y & 3, 8 entries each, indexed z + x & 3).
const float kDither[32] = {0.96875f, 0.46875f, 0.84375f, 0.34375f, 0.96875f, 0.46875f, 0.84375f, 0.34375f,
                           0.21875f, 0.71875f, 0.09375f, 0.59375f, 0.21875f, 0.71875f, 0.09375f, 0.59375f,
                           0.78125f, 0.28125f, 0.90625f, 0.40625f, 0.78125f, 0.28125f, 0.90625f, 0.40625f,
                           0.03125f, 0.53125f, 0.15625f, 0.65625f, 0.03125f, 0.53125f, 0.15625f, 0.65625f};
constexpr u32 kFilterDither = 0x80000;
// Largest decoded image / texture level handled here (bytes). Larger ones go to D3DX: in the 32-bit
// process their sizes can wrap size_t (32768 x 32768 x 4) or exhaust the address space.
constexpr std::uint64_t kMaxBytes = 0x10000000;

// ---------------------------------------------------------------------------
// D3DX format table (0x00a2dcb0, lookup 0x00763fc0): type 0 RGB / 1 palette / 2 luminance / 3 bump,
// bits per pixel, five channel widths compared by 0x00763fe4, "convertible" (a conversion target).
struct FormatInfo {
    u32 format, type, bpp, bits[5], convertible;
};
constexpr FormatInfo kFormatTable[] = {
    {kR8G8B8, 0, 24, {0, 0, 8, 8, 8}, 0},     {kA8R8G8B8, 0, 32, {0, 8, 8, 8, 8}, 1},
    {kX8R8G8B8, 0, 32, {0, 0, 8, 8, 8}, 1},   {kR5G6B5, 0, 16, {0, 0, 5, 6, 5}, 1},
    {kX1R5G5B5, 0, 16, {0, 0, 5, 5, 5}, 1},   {kA8P8, 1, 16, {0, 8, 8, 8, 8}, 1},
    {kP8, 1, 8, {0, 8, 8, 8, 8}, 1},          {kA1R5G5B5, 0, 16, {0, 1, 5, 5, 5}, 1},
    {kA4R4G4B4, 0, 16, {0, 4, 4, 4, 4}, 1},   {kR3G3B2, 0, 8, {0, 0, 3, 3, 2}, 1},
    {kA8, 0, 8, {0, 8, 0, 0, 0}, 1},          {kA8R3G3B2, 0, 16, {0, 8, 3, 3, 2}, 1},
    {kX4R4G4B4, 0, 16, {0, 0, 4, 4, 4}, 1},   {kL8, 2, 8, {0, 0, 8, 8, 8}, 1},
    {kA8L8, 2, 16, {0, 8, 8, 8, 8}, 1},       {kA4L4, 2, 8, {0, 4, 4, 4, 4}, 1},
    {60, 3, 16, {0, 0, 0, 8, 8}, 1},          {61, 3, 16, {6, 0, 0, 5, 5}, 1},
    {62, 3, 32, {8, 0, 0, 8, 8}, 1},          {63, 3, 32, {0, 8, 8, 8, 8}, 1},
    {64, 3, 32, {0, 0, 0, 16, 16}, 1},        {65, 3, 32, {0, 0, 11, 11, 10}, 1},
    {0x59565955, 0, 16, {0, 0, 8, 8, 8}, 0},  {0x32595559, 0, 16, {0, 0, 8, 8, 8}, 0},
    {kDXT1, 0, 16, {0, 1, 5, 6, 5}, 0},       {kDXT2, 0, 16, {0, 4, 5, 6, 5}, 0},
    {kDXT3, 0, 16, {0, 4, 5, 6, 5}, 0},       {kDXT4, 0, 16, {0, 3, 5, 6, 5}, 0},
    {kDXT5, 0, 16, {0, 3, 5, 6, 5}, 0},
};
constexpr FormatInfo kNoFormat = {0, 0, 32, {0, 8, 8, 8, 8}, 0};  // the table's terminator

const FormatInfo& format_info(u32 format) {
    for (const FormatInfo& f : kFormatTable)
        if (f.format == format) return f;
    return kNoFormat;
}

// 0x00763fe4: how far `entry` is from the wanted format (lower is better, ~0 = incompatible).
u32 format_distance(const FormatInfo& entry, const FormatInfo& want) {
    if (!entry.convertible) return ~0u;
    if (entry.type != want.type) {
        if (entry.type == 0 && want.type == 3) return ~0u;
        if (entry.type != 0) return ~0u;
    }
    u32 score = 0;
    for (int i = 0; i < 5; ++i) {
        const u32 req = want.bits[i], have = entry.bits[i];
        if (req < have)
            score += req == 0 ? 0x100u : have - req;
        else if (req > have)
            score += have == 0 ? 0x1000000u : (req - have) << 16;
    }
    return score;
}

// ---------------------------------------------------------------------------
// x87 helpers.

/// A value in an x87 register at 24-bit precision control: every operation is rounded to float.
/// Computing in double and rounding once to float gives the correctly rounded float result for
/// + - * / and sqrt (53 >= 2 * 24 + 2), whatever the compiler does with float expressions.
struct Single {
    float v = 0.0f;
    Single() = default;
    explicit Single(float f) : v(f) {}
    explicit operator float() const { return v; }
    explicit operator double() const { return v; }
    friend Single operator+(Single a, Single b) { return Single(static_cast<float>(static_cast<double>(a.v) + b.v)); }
    friend Single operator-(Single a, Single b) { return Single(static_cast<float>(static_cast<double>(a.v) - b.v)); }
    friend Single operator*(Single a, Single b) { return Single(static_cast<float>(static_cast<double>(a.v) * b.v)); }
    friend Single operator/(Single a, Single b) { return Single(static_cast<float>(static_cast<double>(a.v) / b.v)); }
    friend Single operator-(Single a) { return Single(-a.v); }
    friend bool operator<(Single a, Single b) { return a.v < b.v; }
    friend bool operator==(Single a, Single b) { return a.v == b.v; }
};
Single sqrt(Single a) { return Single(static_cast<float>(std::sqrt(static_cast<double>(a.v)))); }
double sqrt(double a) { return std::sqrt(a); }

/// Round-toward-zero of the exact value hi + lo (|lo| <= half an ulp of hi) to float.
float rz_float(double hi, double lo) {
    float f = static_cast<float>(hi);
    if (!std::isfinite(hi)) return f;
    const double d = hi - static_cast<double>(f);  // exact
    const double diff = d != 0.0 ? d : lo;          // sign of (hi + lo) - f
    if (diff == 0.0) return f;
    if (f > 0.0f && diff < 0.0) return std::nextafter(f, 0.0f);
    if (f < 0.0f && diff > 0.0) return std::nextafter(f, 0.0f);
    if (f == 0.0f) {  // |x| below the float range around 0: not reached for pixel values
        const float g = static_cast<float>(diff);
        return std::fabs(static_cast<double>(g)) > std::fabs(diff) ? std::nextafter(g, 0.0f) : g;
    }
    return f;
}
double rz_double(double hi, double lo) {
    if (lo == 0.0 || !std::isfinite(hi)) return hi;
    if ((hi > 0.0 && lo < 0.0) || (hi < 0.0 && lo > 0.0)) return std::nextafter(hi, 0.0);
    return hi;
}
// a + b and a * b with the rounding mode set to truncate, at the register precision.
template <class R>
R rz_add(R a, R b);
template <>
Single rz_add<Single>(Single x, Single y) {
    const double a = x.v, b = y.v;
    const double s = a + b;
    const double bb = s - a;
    const double err = (a - (s - bb)) + (b - bb);
    return Single(rz_float(s, err));
}
template <>
double rz_add<double>(double a, double b) {
    const double s = a + b;
    const double bb = s - a;
    const double err = (a - (s - bb)) + (b - bb);
    return rz_double(s, err);
}
template <class R>
R rz_mul(R a, R b);
template <>
Single rz_mul<Single>(Single a, Single b) {
    return Single(rz_float(static_cast<double>(a.v) * static_cast<double>(b.v), 0.0));  // exact in double
}
template <>
double rz_mul<double>(double a, double b) {
    const double p = a * b;
    return rz_double(p, joan::fp::fma_exact(a, b, -p));
}
float rz_store(Single v) { return v.v; }
float rz_store(double v) { return rz_float(v, 0.0); }

/// One channel of D3DX's pixel encoders (e.g. 0x0078f92d), run with RC = truncate:
/// fld f; fmul 255.0; fadd d; fstp m32; fld m32; fistp.
template <class R>
u32 encode_channel(float f, float d) {
    const R p = rz_mul<R>(static_cast<R>(f), static_cast<R>(255.0f));
    const R s = rz_add<R>(p, static_cast<R>(d));
    const float v = rz_store(s);
    return static_cast<u32>(static_cast<i64>(v));  // values are in [0, 256)
}

/// MSVC 6 _ftol (0x007b26cc): truncation to 64 bits; the low dword is what callers use.
u32 ftol_low(double v) {
    if (!(std::fabs(v) < 9223372036854775808.0)) return 0;  // NaN / overflow: integer indefinite
    return static_cast<u32>(static_cast<i64>(v));
}

/// Pixel byte -> float as D3DX's decoders do it (fild; fmul (float)(1/255); fstp m32): one rounding.
float unorm8(u32 b) { return static_cast<float>(static_cast<double>(b) * static_cast<double>(kInv255)); }

// ---------------------------------------------------------------------------
// DXT blocks, D3DX semantics. Pixels are dwords A8R8G8B8 (bytes blue, green, red, alpha).

// 0x00799e49: 5:6:5 -> bytes blue, green, red (bit replication).
void unpack565(u16 v, u8 out[3]) {
    const u32 b = v & 31u, g = (v >> 5) & 63u, r = v >> 11;
    out[0] = static_cast<u8>((b << 3) | (b >> 2));
    out[1] = static_cast<u8>((g << 2) | (g >> 4));
    out[2] = static_cast<u8>((r << 3) | (r >> 2));
}
// 0x00799e19: bytes blue, green, red -> 5:6:5.
u16 pack565(const u8 c[3]) { return static_cast<u16>((u32(c[2] >> 3) << 11) | (u32(c[1] >> 2) << 5) | u32(c[0] >> 3)); }

// 0x0079a5ea: colour block (with the 3-colour + transparent black mode).
void decode_color_block(const u8* block, u32 px[16]) {
    u8 c[4][4];
    const u16 c0 = rd16(block), c1 = rd16(block + 2);
    unpack565(c0, c[0]);
    unpack565(c1, c[1]);
    c[0][3] = c[1][3] = c[2][3] = 0xff;
    if (c0 > c1) {
        for (int i = 0; i < 3; ++i) {
            c[2][i] = static_cast<u8>((2 * c[0][i] + c[1][i] + 1) / 3);
            c[3][i] = static_cast<u8>((c[0][i] + 2 * c[1][i] + 1) / 3);
        }
        c[3][3] = 0xff;
    } else {
        for (int i = 0; i < 3; ++i) {
            c[2][i] = static_cast<u8>((c[0][i] + c[1][i]) / 2);
            c[3][i] = 0;
        }
        c[3][3] = 0;
    }
    u32 bits = rd32(block + 4);
    for (int i = 0; i < 16; ++i, bits >>= 2) {
        const u8* e = c[bits & 3];
        px[i] = u32(e[0]) | u32(e[1]) << 8 | u32(e[2]) << 16 | u32(e[3]) << 24;
    }
}
// 0x00799d8c (DXT1), 0x0079a6d3 (DXT2/3), 0x0079a721 (DXT4/5).
void decode_dxt_block(u32 format, const u8* block, u32 px[16]) {
    if (format == kDXT1) {
        decode_color_block(block, px);
        for (int i = 0; i < 16; ++i)
            if ((px[i] >> 24) == 0) px[i] = 0;
        return;
    }
    decode_color_block(block + 8, px);
    if (format == kDXT2 || format == kDXT3) {
        for (int row = 0; row < 4; ++row) {
            u32 w = rd16(block + 2 * row);
            for (int j = 0; j < 4; ++j, w >>= 4) {
                const u32 n = w & 0xf;
                px[row * 4 + j] = (px[row * 4 + j] & 0x00ffffffu) | (((n | n << 4) & 0xff) << 24);
            }
        }
        return;
    }
    const i32 a0 = block[0], a1 = block[1];
    i32 a[8] = {a0, a1};
    if (a0 > a1) {
        a[2] = (6 * a0 + a1) / 7;
        a[3] = (5 * a0 + 2 * a1) / 7;
        a[4] = (4 * a0 + 3 * a1) / 7;
        a[5] = (3 * a0 + 4 * a1) / 7;
        a[6] = (2 * a0 + 5 * a1) / 7;
        a[7] = (a0 + 6 * a1) / 7;
    } else {
        a[2] = (4 * a0 + a1) / 5;
        a[3] = (3 * a0 + 2 * a1) / 5;
        a[4] = (2 * a0 + 3 * a1) / 5;
        a[5] = (a0 + 4 * a1) / 5;
        a[6] = 0;
        a[7] = 255;
    }
    u32 bits = 0;
    for (int i = 0; i < 16; ++i) {
        if ((i & 7) == 0) {
            const u8* b = block + (i == 0 ? 2 : 5);
            bits = u32(b[0]) | u32(b[1]) << 8 | u32(b[2]) << 16;
        }
        px[i] = (px[i] & 0x00ffffffu) | (static_cast<u32>(a[bits & 7]) & 0xff) << 24;
        bits >>= 3;
    }
}

// D3DX's DXT colour compressor (0x0079a153 and helpers), templated on the x87 register precision.
template <class R>
struct ColorEncoder {
    // 0x00799db2: bytes blue, green, red -> luminance-weighted floats.
    static void weighted(const u8* c, float out[3]) {
        for (int i = 0; i < 3; ++i) {
            R t = static_cast<R>(static_cast<float>(c[i])) * static_cast<R>(kWeight[i]);
            t = t * static_cast<R>(kInv255);
            out[i] = static_cast<float>(t);
        }
    }
    // 0x00799de4: weighted floats -> bytes (truncating _ftol of the register value).
    static void unweighted(const float in[3], u8 out[3]) {
        for (int i = 0; i < 3; ++i) {
            R t = static_cast<R>(in[i]) / static_cast<R>(kWeight[i]);
            t = t * static_cast<R>(255.0f);
            out[i] = static_cast<u8>(ftol_low(static_cast<double>(t)));
        }
    }
    static void bytes_of(u32 px, u8 out[3]) {
        out[0] = static_cast<u8>(px);
        out[1] = static_cast<u8>(px >> 8);
        out[2] = static_cast<u8>(px >> 16);
    }

    // 0x0079a0f2: one colour (the last pixel that is not keyed), keyed pixels index 3.
    static void single_color(const u32 px[16], u8* out, u32 mask) {
        u8 c[3];
        u32 last = px[0];
        bytes_of(last, c);
        u16 c0 = pack565(c);
        u32 indices = 0;
        if ((mask & 0xffff) != 0xffff) {
            for (int i = 0; i < 16; ++i) {
                if (!(mask & (1u << i)))
                    indices |= 3u << (2 * i);
                else
                    last = px[i];
            }
            bytes_of(last, c);
            c0 = pack565(c);
        }
        wr16(out, c0);
        wr16(out + 2, c0);
        wr32(out + 4, indices);
    }

    // 0x00799e96: square of the packed symmetric matrix (a b c / d e / f at 0, 4, 8, 0x10, 0x14, 0x20).
    static void square(const float m[12], float t[12]) {
        const R a(m[0]), b(m[1]), c(m[2]), d(m[4]), e(m[5]), f(m[8]);
        t[0] = static_cast<float>((a * a + b * b) + c * c);
        t[1] = static_cast<float>((d + a) * b + e * c);
        t[2] = static_cast<float>((f + a) * c + e * b);
        t[4] = static_cast<float>((e * e + d * d) + b * b);
        t[5] = static_cast<float>((f + d) * e + c * b);
        t[8] = static_cast<float>((f * f + e * e) + c * c);
    }

    static bool less(R a, R b) { return a < b; }  // ordered comparisons as the x87 code tests them

    // 0x0079a153: px are A8R8G8B8 dwords; pixels whose RGB lies in [key_lo, key_hi] are keyed out.
    /// false: D3DX would continue with an uninitialised index (not reproducible).
    static bool encode(const u32 px[16], u8* out, u32 key_lo, u32 key_hi) {
        u32 mask = 0, count = 0;
        u8 lo[3], hi[3];
        bytes_of(key_lo, lo);
        bytes_of(key_hi, hi);
        for (int i = 15; i >= 0; --i) {
            mask <<= 1;
            u8 c[3];
            bytes_of(px[i], c);
            const bool keyed = lo[2] <= c[2] && c[2] <= hi[2] && lo[1] <= c[1] && c[1] <= hi[1] && lo[0] <= c[0] &&
                               c[0] <= hi[0];
            if (keyed) {
                mask &= 0xfffe;
            } else {
                mask |= 1;
                ++count;
            }
        }
        if (count == 0) {
            wr16(out, 0);
            wr16(out + 2, 0xffff);
            wr32(out + 4, 0xffffffffu);
            return true;
        }
        bool same = true;
        for (int i = 1; i < 16; ++i)
            if ((px[i] & 0x00ffffff) != (px[i - 1] & 0x00ffffff)) same = false;
        if (same) return single_color(px, out, mask), true;

        float v[16][3];
        for (int i = 0; i < 16; ++i) {
            u8 c[3];
            bytes_of(px[i], c);
            weighted(c, v[i]);
        }
        const R inv = static_cast<R>(1.0f) / static_cast<R>(static_cast<float>(count));
        float mean[3];
        for (int k = 0; k < 3; ++k) {
            mean[k] = 0.0f;
            for (int i = 0; i < 16; ++i)
                if (mask & (1u << i)) mean[k] = static_cast<float>(static_cast<R>(v[i][k]) + static_cast<R>(mean[k]));
            mean[k] = static_cast<float>(inv * static_cast<R>(mean[k]));
        }
        for (int k = 0; k < 3; ++k)
            for (int i = 0; i < 16; ++i) v[i][k] = static_cast<float>(static_cast<R>(v[i][k]) - static_cast<R>(mean[k]));

        float m[12] = {};  // rows of 4: (a b c -) (d e - -) (f - - -)
        for (int r = 0; r < 3; ++r)
            for (int j = 0; j < 3 - r; ++j) {
                float& s = m[r * 4 + j];
                s = 0.0f;
                for (int i = 0; i < 16; ++i)
                    if (mask & (1u << i))
                        s = static_cast<float>(static_cast<R>(v[i][r + j]) * static_cast<R>(v[i][r]) +
                                               static_cast<R>(s));
            }

        for (int it = 0; it < 9; ++it) {  // power iteration: M <- (M^4) * 3 / trace
            float t[12] = {};
            square(m, t);
            square(t, m);
            const R s = (static_cast<R>(m[8]) + static_cast<R>(m[4])) + static_cast<R>(m[0]);
            if (s == static_cast<R>(0.0f)) return single_color(px, out, mask), true;
            const R scale = static_cast<R>(3.0f) / s;
            for (int r = 0; r < 3; ++r)
                for (int j = 0; j < 3 - r; ++j) m[r * 4 + j] = static_cast<float>(scale * static_cast<R>(m[r * 4 + j]));
        }

        const float full[9] = {m[0], m[1], m[2], m[1], m[4], m[5], m[2], m[5], m[8]};
        R best = static_cast<R>(0.0f);
        int column = -1;
        const float diag[3] = {m[0], m[4], m[8]};
        for (int k = 0; k < 3; ++k)
            if (less(best, static_cast<R>(diag[k]))) {
                best = static_cast<R>(diag[k]);
                column = k;
            }
        if (column < 0) return false;  // D3DX would index with garbage here
        const R inv_len = static_cast<R>(1.0f) / sqrt(best);
        float axis[3];
        for (int k = 0; k < 3; ++k) axis[k] = static_cast<float>(inv_len * static_cast<R>(full[k * 3 + column]));
        R norm2 = static_cast<R>(0.0f);
        for (int k = 0; k < 3; ++k) norm2 = norm2 + static_cast<R>(axis[k]) * static_cast<R>(axis[k]);
        if (norm2 == static_cast<R>(0.0f)) return single_color(px, out, mask), true;

        float lo_t = f32(0x47c34f80), hi_t = f32(0xc7c34f80);  // 99999, -99999
        for (int i = 0; i < 16; ++i) {
            if (!(mask & (1u << i))) continue;
            R t = static_cast<R>(0.0f);
            for (int k = 0; k < 3; ++k) t = t + static_cast<R>(axis[k]) * static_cast<R>(v[i][k]);
            t = t / norm2;
            if (less(t, static_cast<R>(lo_t))) lo_t = static_cast<float>(t);
            if (less(static_cast<R>(hi_t), t)) hi_t = static_cast<float>(t);
        }
        float p0[3], p1[3];  // ends of the segment (0x20 / 0x10 in the original frame)
        for (int k = 0; k < 3; ++k) {
            p0[k] = static_cast<float>(static_cast<R>(lo_t) * static_cast<R>(axis[k]) + static_cast<R>(mean[k]));
            p1[k] = static_cast<float>(static_cast<R>(hi_t) * static_cast<R>(axis[k]) + static_cast<R>(mean[k]));
        }
        clamp(p0, p1);

        // 0x00799f35: quantise, order the endpoints, back to weighted space.
        u8 b0[3], b1[3];
        unweighted(p0, b0);
        u16 c0 = pack565(b0);
        unweighted(p1, b1);
        u16 c1 = pack565(b1);
        if ((c1 < c0) != (count == 16)) std::swap(c0, c1);
        wr16(out, c0);
        wr16(out + 2, c1);
        unpack565(c0, b0);
        weighted(b0, p0);
        unpack565(c1, b1);
        weighted(b1, p1);

        R dist2 = static_cast<R>(0.0f);
        for (int k = 0; k < 3; ++k) {
            const R d = static_cast<R>(p1[k]) - static_cast<R>(p0[k]);
            dist2 = dist2 + d * d;
        }
        if (dist2 == static_cast<R>(0.0f) && count == 16) return single_color(px, out, mask), true;

        static const u32 kCode4[4] = {0, 2, 3, 1}, kCode3[4] = {0, 2, 1, 3};
        u32 indices = 0;
        for (int i = 15; i >= 0; --i) {
            indices <<= 2;
            if (!(mask & (1u << i))) {
                indices |= 3;
                continue;
            }
            R dot = static_cast<R>(0.0f);
            for (int k = 0; k < 3; ++k) {
                const R p = static_cast<R>(mean[k]) + static_cast<R>(v[i][k]);
                const R d = static_cast<R>(p1[k]) - static_cast<R>(p0[k]);
                dot = dot + d * (p - static_cast<R>(p0[k]));
            }
            R s = dot / dist2;
            if (count == 16) {
                s = s * static_cast<R>(4.0f);
                if (less(s, static_cast<R>(0.0f)))
                    s = static_cast<R>(0.0f);
                else if (!(s < static_cast<R>(4.0f)) && !std::isnan(static_cast<double>(s)))
                    s = static_cast<R>(3.0f);
                indices |= kCode4[ftol_low(static_cast<double>(s)) & 3];
            } else {
                s = s * static_cast<R>(3.0f);
                if (less(s, static_cast<R>(0.0f)))
                    s = static_cast<R>(0.0f);
                else if (!(s < static_cast<R>(3.0f)) && !std::isnan(static_cast<double>(s)))
                    s = static_cast<R>(2.0f);
                indices |= kCode3[ftol_low(static_cast<double>(s)) & 3];
            }
        }
        wr32(out + 4, indices);
        return true;
    }

    // 0x00799fc0: clip the segment p0..p1 to [0, weight] per channel.
    static void clamp(float p0[3], float p1[3]) {
        const R zero = static_cast<R>(0.0f), one = static_cast<R>(1.0f);
        auto move = [&](float* target, R s) {
            for (int ch = 2; ch >= 0; --ch)
                target[ch] = static_cast<float>((static_cast<R>(p1[ch]) - static_cast<R>(p0[ch])) * s +
                                                static_cast<R>(target[ch]));
        };
        for (int c = 0; c < 3; ++c) {
            const bool min_neg = less(static_cast<R>(p0[c]), zero), max_neg = less(static_cast<R>(p1[c]), zero);
            if (min_neg != max_neg) {
                const R t = -(static_cast<R>(p0[c]) / (static_cast<R>(p1[c]) - static_cast<R>(p0[c])));
                if (less(static_cast<R>(p0[c]), zero))
                    move(p0, t);
                else
                    move(p1, t - one);
            }
            const R w = static_cast<R>(kWeight[c]);
            const bool min_big = less(w, static_cast<R>(p0[c])), max_big = less(w, static_cast<R>(p1[c]));
            if (min_big != max_big) {
                const R s = (w - static_cast<R>(p0[c])) / (static_cast<R>(p1[c]) - static_cast<R>(p0[c]));
                if (less(w, static_cast<R>(p0[c])))
                    move(p0, s);
                else
                    move(p1, s - one);
            }
        }
    }
};

// 0x0079a8ac: DXT4/5 alpha block.
void encode_alpha_dxt5(const u32 px[16], u8* out) {
    u8 mx = static_cast<u8>(px[0] >> 24), mn = mx;
    for (int i = 1; i < 16; ++i) {
        const u8 a = static_cast<u8>(px[i] >> 24);
        if (a > mx) mx = a;
        if (a < mn) mn = a;
    }
    bool six = false;
    if (mx == 0xff && mn == 0) {
        u8 lo = 0xff, hi = 0;  // smallest non-zero, largest non-255
        for (int i = 0; i < 16; ++i) {
            const u8 a = static_cast<u8>(px[i] >> 24);
            if (a < lo && a != 0) lo = a;
            if (a > hi && a != 0xff) hi = a;
        }
        if (lo < hi) {
            six = true;
            mx = lo;
            mn = hi;
        } else {
            mx = 0xff;
            mn = 0;
        }
    }
    out[0] = mx;
    out[1] = mn;
    if (mx == mn) {
        std::memset(out + 2, 0, 6);
        return;
    }
    const i32 a0 = mx, range = a0 - mn, half = range >> 1, steps = six ? 5 : 7;
    u32 bits = 0;
    for (int i = 15; i >= 0; --i) {
        bits <<= 3;
        const u8 a = static_cast<u8>(px[i] >> 24);
        if (six && a == 0)
            bits |= 6;
        else if (six && a == 0xff)
            bits |= 7;
        else {
            const i32 t = ((a0 - a) * steps + half) / range;
            if (t >= steps)
                bits |= 1;
            else if (t > 0)
                bits |= static_cast<u32>(t + 1);
        }
        if ((i & 7) == 0) {
            u8* dst = out + (i == 8 ? 5 : 2);
            dst[0] = static_cast<u8>(bits);
            dst[1] = static_cast<u8>(bits >> 8);
            dst[2] = static_cast<u8>(bits >> 16);
            bits >>= 16;
        }
    }
}

// 0x00799d23 / 0x0079a853 / 0x0079a8ac: one block from 16 A8R8G8B8 pixels (modified in place).
template <class R>
bool encode_dxt_block(u32 format, u32 px[16], u8* out) {
    const u32 no_key_lo = 0x00ffffff, no_key_hi = 0;  // 0x0079a821: nothing is keyed
    if (format == kDXT1) {
        bool opaque = true;
        for (int i = 0; i < 16; ++i)
            if (px[i] < 0x80000000u) opaque = false;
        if (opaque) return ColorEncoder<R>::encode(px, out, no_key_lo, no_key_hi);
        u32 key = 0;
        for (;;) {
            bool used = false;
            for (int i = 0; i < 16; ++i)
                if (px[i] >= 0x80000000u && (px[i] & 0x00ffffff) == key) used = true;
            if (!used) break;
            ++key;
        }
        for (int i = 0; i < 16; ++i)
            if (px[i] < 0x80000000u) px[i] = key;
        return ColorEncoder<R>::encode(px, out, key, key);
    }
    if (format == kDXT3) {
        for (int row = 0; row < 4; ++row) {
            u32 w = 0;
            for (int j = 3; j >= 0; --j) w = (w << 4) | (px[row * 4 + j] >> 28);
            wr16(out + 2 * row, static_cast<u16>(w));
        }
    } else {
        encode_alpha_dxt5(px, out);
    }
    return ColorEncoder<R>::encode(px, out + 8, no_key_lo, no_key_hi);
}

// ---------------------------------------------------------------------------
// Rows of float pixels (r, g, b, a) - D3DX's blitter interface (codec vtable +4 / +8).

struct Pixel {
    float r, g, b, a;
};

struct PaletteFloats {
    Pixel e[256];
};

/// Source level -> rows of floats (0x00790acb R8G8B8, 0x00790b57 A8R8G8B8, 0x00790bf9 X8R8G8B8,
/// 0x00791223 P8, 0x0079226e DXT).
class RowSource {
   public:
    RowSource(u32 format, const ImageLevel& level, const PaletteFloats* palette)
        : format_(format), level_(level), palette_(palette) {}

    static bool supported(u32 format) {
        return format == kR8G8B8 || format == kA8R8G8B8 || format == kX8R8G8B8 || format == kP8 || format == kDXT1 ||
               format == kDXT3 || format == kDXT5;
    }

    void row(u32 y, Pixel* out) {
        const u32 w = level_.width;
        if (is_dxt(format_)) {
            const u32 by = y / 4;
            if (cached_ != static_cast<i64>(by)) decode_block_row(by);
            const u32* p = block_row_.data() + static_cast<std::size_t>(y & 3) * stride_;
            for (u32 x = 0; x < w; ++x) {
                const u32 c = p[x];
                out[x] = {unorm8((c >> 16) & 0xff), unorm8((c >> 8) & 0xff), unorm8(c & 0xff), unorm8(c >> 24)};
            }
            return;
        }
        const u8* s = level_.data.data() + static_cast<std::size_t>(y) * level_.pitch;
        switch (format_) {
            case kR8G8B8:
                for (u32 x = 0; x < w; ++x, s += 3) out[x] = {unorm8(s[2]), unorm8(s[1]), unorm8(s[0]), 1.0f};
                break;
            case kA8R8G8B8:
                for (u32 x = 0; x < w; ++x, s += 4) out[x] = {unorm8(s[2]), unorm8(s[1]), unorm8(s[0]), unorm8(s[3])};
                break;
            case kX8R8G8B8:
                for (u32 x = 0; x < w; ++x, s += 4) out[x] = {unorm8(s[2]), unorm8(s[1]), unorm8(s[0]), 1.0f};
                break;
            case kP8:
                for (u32 x = 0; x < w; ++x) out[x] = palette_->e[s[x]];
                break;
            default: break;
        }
    }

   private:
    void decode_block_row(u32 by) {
        const u32 blocks = (level_.width + 3) / 4;
        stride_ = blocks * 4;
        block_row_.assign(static_cast<std::size_t>(stride_) * 4, 0);
        const u8* src = level_.data.data() + static_cast<std::size_t>(by) * level_.pitch;
        const u32 block_bytes = format_ == kDXT1 ? 8 : 16;
        for (u32 bx = 0; bx < blocks; ++bx) {
            u32 px[16];
            decode_dxt_block(format_, src + bx * block_bytes, px);
            for (int i = 0; i < 16; ++i) block_row_[static_cast<std::size_t>(i / 4) * stride_ + bx * 4 + (i & 3)] = px[i];
        }
        cached_ = by;
    }

    u32 format_;
    const ImageLevel& level_;
    const PaletteFloats* palette_;
    i64 cached_ = -1;
    u32 stride_ = 0;
    std::vector<u32> block_row_;
};

/// Rows of floats -> destination level (0x0078f92d A8R8G8B8, 0x0078fa26 X8R8G8B8, 0x007921ab DXT
/// with its block buffer flushed by 0x00791b9a).
template <class R>
class RowSink {
   public:
    RowSink(u32 format, ImageLevel& level, bool dither) : format_(format), level_(level), dither_(dither) {
        if (is_dxt(format_)) {
            blocks_ = (level_.width + 3) / 4;
            buffer_.assign(static_cast<std::size_t>(blocks_) * 16, Pixel{0, 0, 0, 0});
        }
    }
    static bool supported(u32 format) {
        return format == kA8R8G8B8 || format == kX8R8G8B8 || format == kDXT1 || format == kDXT3 || format == kDXT5;
    }

    void row(u32 y, const Pixel* in) {
        const u32 w = level_.width;
        if (is_dxt(format_)) {
            if (static_cast<i64>(y / 4) != block_y_) {
                flush();
                block_y_ = y / 4;
            }
            for (u32 x = 0; x < w; ++x) buffer_[static_cast<std::size_t>(y & 3) * blocks_ * 4 + x] = in[x];
            dirty_ = true;
            return;
        }
        u8* d = level_.data.data() + static_cast<std::size_t>(y) * level_.pitch;
        const float* table = dither_ ? kDither + (y & 3) * 8 : nullptr;
        for (u32 x = 0; x < w; ++x, d += 4) {
            const float k = table ? table[x & 3] : 0.5f;
            const u32 r = encode_channel<R>(in[x].r, k), g = encode_channel<R>(in[x].g, k),
                      b = encode_channel<R>(in[x].b, k);
            u32 v;
            if (format_ == kA8R8G8B8) {
                const u32 a = encode_channel<R>(in[x].a, k);
                v = ((a << 8 | r) << 8 | g) << 8 | b;
            } else {
                v = (r << 8 | g) << 8 | b;
            }
            wr32(d, v);
        }
    }

    /// Encodes the buffered row of blocks (also called once at the end).
    void flush() {
        if (!dirty_) return;
        dirty_ = false;
        const u32 w = level_.width, h = level_.height;
        const u32 xmask = w == 1 ? 0 : w == 2 ? 1 : 3, ymask = h == 1 ? 0 : h == 2 ? 1 : 3;
        const bool replicate = w <= 2 || h <= 2;
        const u32 block_bytes = format_ == kDXT1 ? 8 : 16;
        u8* dst = level_.data.data() + static_cast<std::size_t>(block_y_) * level_.pitch;
        for (u32 bx = 0; bx < blocks_; ++bx) {
            u32 px[16];
            for (u32 y = 0; y < 4; ++y)
                for (u32 x = 0; x < 4; ++x) {
                    const Pixel& p = buffer_[static_cast<std::size_t>(y) * blocks_ * 4 + bx * 4 + x];
                    const u32 r = encode_channel<R>(p.r, 0.5f), g = encode_channel<R>(p.g, 0.5f),
                              b = encode_channel<R>(p.b, 0.5f), a = encode_channel<R>(p.a, 0.5f);
                    px[y * 4 + x] = ((a << 8 | r) << 8 | g) << 8 | b;
                }
            if (replicate)
                for (u32 y = 0; y < 4; ++y)
                    for (u32 x = 0; x < 4; ++x) px[y * 4 + x] = px[(y & ymask) * 4 + (x & xmask)];
            if (!encode_dxt_block<R>(format_, px, dst + bx * block_bytes)) undefined_ = true;
        }
    }

   private:
    u32 format_;
    ImageLevel& level_;
    bool dither_;
    u32 blocks_ = 0;
    i64 block_y_ = -1;
    bool dirty_ = false;
    std::vector<Pixel> buffer_;

   public:
    bool undefined_ = false;  // the compressor reached a case D3DX computes from uninitialised data
};

// ---------------------------------------------------------------------------
// Blits (0x00786b38 strategies) between same-size levels and box-filtered halves.

void copy_rows(const ImageLevel& src, ImageLevel& dst, u32 bytes, u32 rows) {
    for (u32 y = 0; y < rows; ++y)
        std::memcpy(dst.data.data() + static_cast<std::size_t>(y) * dst.pitch,
                    src.data.data() + static_cast<std::size_t>(y) * src.pitch, bytes);
}

/// D3DXLoadSurfaceFromMemory of a same-size level: 0x007854bc (same format: row copy, or 0x00784485
/// block copy for 4-aligned DXT) else 0x007855ee (decode rows, encode rows).
template <class R>
bool load_level(u32 src_format, const ImageLevel& src, u32 dst_format, ImageLevel& dst, bool dither,
                const PaletteFloats& palette, std::string* why) {
    if (src.width != dst.width || src.height != dst.height) return fail(why, "level size differs (resampling)");
    if (src_format == dst_format && src_format != kP8 && src_format != kA8P8) {
        if (!is_dxt(src_format)) {
            const u32 bytes = row_bytes(src_format, src.width);
            if (!bytes) return fail(why, "unknown format");
            copy_rows(src, dst, bytes, src.height);
            return true;
        }
        if ((src.width & 3) == 0 && (src.height & 3) == 0) {
            copy_rows(src, dst, row_bytes(src_format, src.width), (src.height + 3) / 4);
            return true;
        }
    }
    if (!RowSource::supported(src_format)) return fail(why, "source format not converted natively");
    if (!RowSink<R>::supported(dst_format)) return fail(why, "target format not converted natively");
    if (is_dxt(dst_format)) {
        const auto ok = [](u32 d) { return d <= 2 || (d & 3) == 0; };
        if (!ok(dst.width) || !ok(dst.height)) return fail(why, "DXT level size not 1, 2 or a multiple of 4");
    }
    RowSource in(src_format, src, &palette);
    RowSink<R> out(dst_format, dst, dither);
    std::vector<Pixel> row(src.width);
    for (u32 y = 0; y < src.height; ++y) {
        in.row(y, row.data());
        out.row(y, row.data());
    }
    out.flush();
    return !out.undefined_ || fail(why, "DXT compressor reached an undefined case");
}

// 0x0078460c: A8R8G8B8 2x2 box, integer.
void box_a8r8g8b8(const ImageLevel& src, ImageLevel& dst) {
    const u32 m = 0xffc03fc0u, m2 = 0x3fc03fc0u;
    for (u32 y = 0; y < dst.height; ++y) {
        const u8* r0 = src.data.data() + static_cast<std::size_t>(2 * y) * src.pitch;
        const u8* r1 = r0 + src.pitch;
        u8* d = dst.data.data() + static_cast<std::size_t>(y) * dst.pitch;
        for (u32 x = 0; x < src.width / 2; ++x) {
            const u32 p0 = rd32(r0 + 8 * x), p1 = rd32(r0 + 8 * x + 4), p2 = rd32(r1 + 8 * x), p3 = rd32(r1 + 8 * x + 4);
            const u32 t1 = ((p0 >> 2) & m) + ((p2 >> 2) & m) + ((p1 >> 2) & m) + ((p3 >> 2) & m) - 0x7fff80u;
            const u32 t2 = ((p0 & 0xff00ffu) + (p2 & 0xff00ffu) + (p1 & 0xff00ffu) + (p3 & 0xff00ffu) + 0x20002u) >> 2;
            const u32 t3 = ((p0 >> 2) & m2) + ((p2 >> 2) & m2) + ((p1 >> 2) & m2) + ((p3 >> 2) & m2) + 0x800080u;
            wr32(d + 4 * x, ((t1 ^ t2) & 0xff00ffu) ^ t3);
        }
    }
}
// 0x0078476a: X8R8G8B8 2x2 box, integer (the X byte becomes 0).
void box_x8r8g8b8(const ImageLevel& src, ImageLevel& dst) {
    for (u32 y = 0; y < dst.height; ++y) {
        const u8* r0 = src.data.data() + static_cast<std::size_t>(2 * y) * src.pitch;
        const u8* r1 = r0 + src.pitch;
        u8* d = dst.data.data() + static_cast<std::size_t>(y) * dst.pitch;
        for (u32 x = 0; x < src.width / 2; ++x) {
            const u32 p0 = rd32(r0 + 8 * x), p1 = rd32(r0 + 8 * x + 4), p2 = rd32(r1 + 8 * x), p3 = rd32(r1 + 8 * x + 4);
            const u32 rb = (p0 & 0xff00ffu) + (p2 & 0xff00ffu) + (p1 & 0xff00ffu) + (p3 & 0xff00ffu) + 0x20002u;
            const u32 g = (p0 & 0xff00u) + (p2 & 0xff00u) + (p1 & 0xff00u) + (p3 & 0xff00u) + 0x200u;
            wr32(d + 4 * x, ((rb & 0x3fc03fcu) | (g & 0x3fc00u)) >> 2);
        }
    }
}

/// D3DXFilterTexture step with D3DX_FILTER_BOX: level `src` -> `dst` (0x0078599b).
template <class R>
bool box_level(u32 format, const ImageLevel& src, ImageLevel& dst, u32 filter, std::string* why) {
    const bool half_w = dst.width == src.width / 2 || (dst.width == 1 && src.width == 1);
    const bool half_h = dst.height == src.height / 2 || (dst.height == 1 && src.height == 1);
    if (!half_w || !half_h) return fail(why, "mip level is not a 2:1 box step");
    const bool dither = (filter & kFilterDither) != 0;
    // Same-format integer fast paths (jump table at 0x00785a70) when not dithering; DXT has none.
    if (!dither && src.width >= 2 && src.height >= 2) {
        if (format == kA8R8G8B8) return box_a8r8g8b8(src, dst), true;
        if (format == kX8R8G8B8) return box_x8r8g8b8(src, dst), true;
    }
    if (!RowSource::supported(format) || !RowSink<R>::supported(format) || format == kR8G8B8)
        return fail(why, "box filter for this format not reproduced");
    if (is_dxt(format)) {
        const auto ok = [](u32 d) { return d <= 2 || (d & 3) == 0; };
        if (!ok(dst.width) || !ok(dst.height)) return fail(why, "DXT level size not 1, 2 or a multiple of 4");
    }
    PaletteFloats unused{};
    RowSource in(format, src, &unused);
    RowSink<R> out(format, dst, dither);
    std::vector<Pixel> row0(src.width), row1(src.width), result(dst.width);
    const bool one_row = src.height == 1, one_col = src.width == 1;
    for (u32 y = 0; y < dst.height; ++y) {
        in.row(2 * y, row0.data());
        if (!one_row) in.row(2 * y + 1, row1.data());
        const Pixel* a = row0.data();
        const Pixel* b = one_row ? row0.data() : row1.data();
        for (u32 x = 0; x < dst.width; ++x) {
            const Pixel& p00 = a[2 * x];
            const Pixel& p01 = a[one_col ? 2 * x : 2 * x + 1];
            const Pixel& p10 = b[2 * x];
            const Pixel& p11 = b[one_col ? 2 * x : 2 * x + 1];
            // register / memory split of 0x00785c18..0x00785cb7
            const R ar = static_cast<R>(p01.r) + static_cast<R>(p00.r);
            const R ag = static_cast<R>(p01.g) + static_cast<R>(p00.g);
            const float ab = static_cast<float>(static_cast<R>(p01.b) + static_cast<R>(p00.b));
            const float aa = static_cast<float>(static_cast<R>(p01.a) + static_cast<R>(p00.a));
            const float br = static_cast<float>(ar + static_cast<R>(p10.r));
            const R bg = ag + static_cast<R>(p10.g);
            const R bb = static_cast<R>(ab) + static_cast<R>(p10.b);
            const float ba = static_cast<float>(static_cast<R>(aa) + static_cast<R>(p10.a));
            const float cr = static_cast<float>(static_cast<R>(br) + static_cast<R>(p11.r));
            const float cg = static_cast<float>(bg + static_cast<R>(p11.g));
            const R cb = bb + static_cast<R>(p11.b);
            const R ca = static_cast<R>(ba) + static_cast<R>(p11.a);
            const R quarter = static_cast<R>(0.25f);
            result[x] = {static_cast<float>(static_cast<R>(cr) * quarter), static_cast<float>(static_cast<R>(cg) * quarter),
                         static_cast<float>(cb * quarter), static_cast<float>(ca * quarter)};
        }
        out.row(y, result.data());
    }
    out.flush();
    return !out.undefined_ || fail(why, "DXT compressor reached an undefined case");
}

template <class R>
bool build_levels(const Image& image, const TexturePlan& plan, std::vector<ImageLevel>& out, std::string* why) {
    PaletteFloats palette;
    for (int i = 0; i < 256; ++i) {
        const u32 e = plan.palette[i];
        palette.e[i] = {unorm8(e & 0xff), unorm8((e >> 8) & 0xff), unorm8((e >> 16) & 0xff), unorm8(e >> 24)};
    }
    const u32 n = std::min<u32>(static_cast<u32>(image.levels.size()), plan.levels);
    for (u32 l = 0; l < n; ++l)
        if (!load_level<R>(image.format, image.levels[l], plan.format, out[l], (plan.filter & kFilterDither) != 0,
                           palette, why))
            return false;
    if (n < plan.levels) {
        const bool generate = (plan.filter & 0xff) == 1 || (plan.mip_filter & 0xff) == 2 || (plan.mip_filter & 0xff) == 5;
        if (!generate) return fail(why, "missing levels loaded with a resampling mip filter");
        if ((plan.mip_filter & 0xffff) != 5 || (plan.mip_filter & 0xfff00000u)) return fail(why, "mip filter not box");
        for (u32 l = n; l < plan.levels; ++l)
            if (!box_level<R>(plan.format, out[l - 1], out[l], plan.mip_filter, why)) return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Loaders. Rejected: D3DX's loader fails too (and D3DX tries its next loader); Unsupported: D3DX
// would read the file in a way not reproduced here (undefined heap contents, out-of-bounds access).

enum class Load { Ok, Rejected, Unsupported };
Load reject(std::string* why, const char* text) {
    if (why) *why = text;
    return Load::Rejected;
}
Load unsupported(std::string* why, const char* text) {
    if (why) *why = text;
    return Load::Unsupported;
}

/// Byte buffer with bounds-checked writes and a record of written bytes (RLE decoders).
struct Canvas {
    std::vector<u8> bytes;
    std::vector<u8> written;
    bool ok = true;
    explicit Canvas(std::size_t n) : bytes(n, 0), written(n, 0) {}
    void put(i64 at, u8 v) {
        if (at < 0 || at >= static_cast<i64>(bytes.size())) {
            ok = false;
            return;
        }
        bytes[static_cast<std::size_t>(at)] = v;
        written[static_cast<std::size_t>(at)] = 1;
    }
};

// 0x00786cd8: BITMAPINFOHEADER + palette + pixels.
Load load_dib(const u8* p, std::size_t size, Image& img, std::string* why) {
    if (size < 0x28 || rd32(p) < 0x28) return reject(why, "DIB: header");
    const u32 header = rd32(p);
    const u32 width = rd32(p + 4);
    const i32 height_signed = static_cast<i32>(rd32(p + 8));
    const u32 height = height_signed > 0 ? static_cast<u32>(height_signed) : 0u - static_cast<u32>(height_signed);
    const u32 bits = rd16(p + 0xe);
    u32 colors = rd32(p + 0x20);
    if (bits <= 8 && colors == 0) colors = 1u << bits;
    const u32 pal_end = header + colors * 4;
    if (pal_end > size || pal_end < header) return reject(why, "DIB: palette beyond the data");
    if (rd16(p + 0xc) != 1) return reject(why, "DIB: planes");
    const u32 compression = rd32(p + 0x10);
    if (height_signed < 0 && compression != 0 && compression != 3) return reject(why, "DIB: top-down RLE");
    u32 format = 0;
    if (compression <= 2) {
        switch (bits) {
            case 1:
            case 4:
            case 8: format = kP8; break;
            case 16: format = kX1R5G5B5; break;
            case 24: format = kR8G8B8; break;
            case 32: format = kX8R8G8B8; break;
            default: return reject(why, "DIB: bit count");
        }
    } else if (compression == 3) {
        if (header < 0x6c) return reject(why, "DIB: bit fields without a V4 header");
        const u32 r = rd32(p + 0x28), g = rd32(p + 0x2c), b = rd32(p + 0x30), a = rd32(p + 0x34);
        if (bits == 32 && b == 0xff && g == 0xff00 && r == 0xff0000 && a == 0)
            format = kX8R8G8B8;
        else if (bits == 32 && b == 0xff && g == 0xff00 && r == 0xff0000 && a == 0xff000000u)
            format = kA8R8G8B8;
        else if (bits == 24 && b == 0xff && g == 0xff00 && r == 0xff0000 && a == 0)
            format = kR8G8B8;
        else if (bits == 16 && b == 0xff && g == 0xff && r == 0xff && a == 0xff00)
            format = kA8L8;
        else if (bits == 16 && b == 0x1f && g == 0x7e0 && r == 0xf800 && a == 0)
            format = kR5G6B5;
        else if (bits == 16 && b == 0x1f && g == 0x3e0 && r == 0x7c00 && a == 0)
            format = kX1R5G5B5;
        else if (bits == 16 && b == 0x1f && g == 0x3e0 && r == 0x7c00 && a == 0x8000)
            format = kA1R5G5B5;
        else if (bits == 16 && b == 0xf && g == 0xf0 && r == 0xf00 && a == 0xf000)
            format = kA4R4G4B4;
        else if (bits == 16 && b == 3 && g == 0x1c && r == 0xe0 && a == 0xff00)
            format = kA8R3G3B2;
        else
            return reject(why, "DIB: bit fields");
    } else {
        return reject(why, "DIB: compression");
    }
    if (width == 0 || height == 0 || width > 0x8000 || height > 0x8000) return unsupported(why, "DIB: size");
    if (format == kP8) {
        if (colors > 256) return unsupported(why, "DIB: more than 256 colours");
        img.has_palette = true;
        img.palette.fill(0xffffffffu);
        const u8* e = p + header;
        for (u32 i = 0; i < colors; ++i, e += 4) img.palette[i] = u32(e[2]) | u32(e[1]) << 8 | u32(e[0]) << 16 | 0xff000000u;
    }
    u32 src_stride, row_len;
    if (bits == 1) {
        src_stride = ((width >> 3) + 3) & ~3u;
        row_len = width;
    } else if (bits == 4) {
        src_stride = ((width >> 1) + 3) & ~3u;
        row_len = width;
    } else {
        row_len = (bits >> 3) * width;
        src_stride = (row_len + 3) & ~3u;
    }
    const u32 pitch = (row_len + 3) & ~3u;
    ImageLevel level;
    level.width = width;
    level.height = height;
    level.pitch = pitch;
    const u8* src = p + pal_end;
    const u8* end = p + size;
    if (static_cast<std::uint64_t>(pitch) * height > kMaxBytes) return unsupported(why, "DIB: image too large");
    const std::size_t total = static_cast<std::size_t>(pitch) * height;
    img.type = ImageFileType::Dib;
    img.format = format;
    if (height_signed < 0 && bits >= 8) {
        if (static_cast<std::size_t>(end - src) < total) return unsupported(why, "DIB: pixels beyond the data");
        level.data.assign(src, src + total);
        img.levels.push_back(std::move(level));
        return Load::Ok;
    }
    Canvas c(total);
    const i64 step = height_signed < 0 ? static_cast<i64>(pitch) : -static_cast<i64>(pitch);
    const i64 first = height_signed < 0 ? 0 : static_cast<i64>(height - 1) * pitch;
    if (compression == 1 || compression == 2) {
        const bool rle4 = compression == 2;
        i64 at = first, row_start = first;
        const u8* s = src;
        while (at >= 0) {
            if (end - s < 2) return unsupported(why, "DIB: RLE beyond the data");
            const u32 n = s[0];
            if (n != 0) {
                for (u32 i = 0; i < n; ++i)
                    c.put(at + i, rle4 ? static_cast<u8>((i & 1) ? s[1] & 0xf : s[1] >> 4) : s[1]);
                at += n;
                s += 2;
                continue;
            }
            const u32 code = s[1];
            if (code == 0) {
                row_start -= pitch;
                at = row_start;
                s += 2;
            } else if (code == 1) {
                at = -static_cast<i64>(pitch);
                s += 2;
            } else if (code == 2) {
                if (end - s < 4) return unsupported(why, "DIB: RLE beyond the data");
                at += static_cast<i64>(s[2]) - static_cast<i64>(s[3]) * pitch;
                s += 4;
            } else {
                const u32 bytes = rle4 ? (((code >> 1) + 1) & ~1u) : ((code + 1) & ~1u);
                const u32 need = rle4 ? (code + 1) / 2 : code;
                if (static_cast<u32>(end - s) < 2 + std::max(bytes, need)) return unsupported(why, "DIB: RLE beyond the data");
                for (u32 i = 0; i < code; ++i)
                    c.put(at + i, rle4 ? static_cast<u8>((i & 1) ? s[2 + (i >> 1)] & 0xf : s[2 + (i >> 1)] >> 4) : s[2 + i]);
                at += code;
                s += 2 + bytes;
            }
            if (!c.ok) return unsupported(why, "DIB: RLE writes outside the image");
        }
        if (!c.ok) return unsupported(why, "DIB: RLE writes outside the image");
        for (u32 y = 0; y < height; ++y)
            for (u32 x = 0; x < row_len; ++x)
                if (!c.written[static_cast<std::size_t>(y) * pitch + x]) return unsupported(why, "DIB: RLE leaves pixels undefined");
    } else {
        i64 at = first;
        const std::size_t avail = static_cast<std::size_t>(end - src);
        std::uint64_t offset = 0;  // of row y in the pixel data (rows can step past the end)
        for (u32 y = 0; y < height; ++y, at += step, offset += src_stride) {
            const u32 need = bits == 1 ? (width + 7) / 8 : bits == 4 ? (width + 1) / 2 : row_len;
            if (offset > avail || avail - offset < need) return unsupported(why, "DIB: pixels beyond the data");
            const u8* s = src + offset;
            if (bits == 1) {
                for (u32 i = 0; i < width; ++i) c.put(at + i, static_cast<u8>((s[i >> 3] >> (7 - (i & 7))) & 1));
            } else if (bits == 4) {
                for (u32 i = 0; i < width; ++i) c.put(at + i, static_cast<u8>((i & 1) ? s[i >> 1] & 0xf : s[i >> 1] >> 4));
            } else {
                std::memcpy(c.bytes.data() + at, s, row_len);
            }
        }
    }
    level.data = std::move(c.bytes);
    img.levels.push_back(std::move(level));
    return Load::Ok;
}

// 0x00788229: DDS (DXT1-5 and six uncompressed RGB layouts; cube maps keep face 0).
Load load_dds(const u8* p, std::size_t size, Image& img, std::string* why) {
    if (size < 4 + 0x7c) return reject(why, "DDS: header");
    const u8* h = p + 4;
    const u32 width = rd32(h + 0xc), height = rd32(h + 8);
    const u32 caps2 = rd32(h + 0x6c) & 0xfc00;
    u32 faces;
    if (caps2 == 0xfc00)
        faces = 6;
    else if (caps2 == 0)
        faces = 1;
    else
        return reject(why, "DDS: partial cube map");
    u32 mips = rd32(h + 0x18);
    if (mips == 0) mips = 1;
    const u32 fourcc = rd32(h + 0x50), flags = rd32(h + 0x4c), bits = rd32(h + 0x54);
    const u32 gmask = rd32(h + 0x5c), amask = rd32(h + 0x64);
    u32 format;
    if (is_dxt(fourcc))
        format = fourcc;
    else if (flags == 0x41 && bits == 32 && amask == 0xff000000u)
        format = kA8R8G8B8;
    else if (flags == 0x40 && bits == 32)
        format = kX8R8G8B8;
    else if (flags == 0x40 && bits == 16 && gmask == 0x7e0)
        format = kR5G6B5;
    else if (flags == 0x40 && bits == 16 && gmask == 0x3e0)
        format = kX1R5G5B5;
    else if (flags == 0x41 && bits == 16 && amask == 0x8000)
        format = kA1R5G5B5;
    else if (flags == 0x41 && bits == 16 && amask == 0xf000)
        format = kA4R4G4B4;
    else
        return reject(why, "DDS: pixel format");
    if (width == 0 || height == 0 || width > 0x8000 || height > 0x8000 || mips > 32) return unsupported(why, "DDS: size");
    std::size_t at = 4 + 0x7c;
    img.type = ImageFileType::Dds;
    img.format = format;
    img.cube = faces == 6;
    for (u32 face = 0; face < faces; ++face) {
        u32 w = width, hh = height;
        for (u32 l = 0; l < mips; ++l) {
            u32 pitch, rows;
            if (is_dxt(format)) {
                pitch = ((w + 3) >> 2) * (format == kDXT1 ? 8u : 16u);
                rows = (hh + 3) >> 2;
            } else {
                pitch = w * (format == kA8R8G8B8 || format == kX8R8G8B8 ? 4u : 2u);
                rows = hh;
            }
            const std::uint64_t bytes64 = static_cast<std::uint64_t>(pitch) * rows;
            if (size - at < bytes64) return reject(why, "DDS: levels beyond the data");
            const auto bytes = static_cast<std::size_t>(bytes64);
            if (face == 0) {
                ImageLevel level;
                level.width = w;
                level.height = hh;
                level.pitch = pitch;
                level.data.assign(p + at, p + at + bytes);
                img.levels.push_back(std::move(level));
            }
            at += bytes;
            w = w == 1 ? 1 : w >> 1;
            hh = hh == 1 ? 1 : hh >> 1;
        }
    }
    return Load::Ok;
}

// 0x007875ca: TGA (colour-mapped, true-colour, grey; raw or RLE; any origin).
bool tga_header_ok(const u8* p, std::size_t size) {
    return size >= 0x12 && (p[1] & 0xfe) == 0 && (p[2] & 0xf4) == 0 && rd16(p + 0xc) != 0 && rd16(p + 0xe) != 0;
}
Load load_tga(const u8* p, std::size_t size, Image& img, std::string* why) {
    if (!tga_header_ok(p, size)) return reject(why, "TGA: header");
    const u32 cmap_type = p[1], type = p[2];
    const u32 width = rd16(p + 0xc), height = rd16(p + 0xe);
    const u32 entry_bytes = (p[7] + 7u) >> 3;
    u32 cmap_format = 0;
    if (cmap_type) {
        switch (p[7]) {
            case 15: cmap_format = kX1R5G5B5; break;
            case 16: cmap_format = kA1R5G5B5; break;
            case 24: cmap_format = kR8G8B8; break;
            case 32: cmap_format = kA8R8G8B8; break;
            default: return reject(why, "TGA: colour map entry size");
        }
    }
    const u32 depth = p[0x10];
    const u32 pixel_bytes = (depth + 7) >> 3;
    u32 format;
    switch (type & 3) {
        case 1:
            if (!cmap_type || depth != 8) return reject(why, "TGA: colour-mapped image");
            format = kP8;
            break;
        case 2:
            switch (depth) {
                case 15: format = kX1R5G5B5; break;
                case 16: format = kA1R5G5B5; break;
                case 24: format = kR8G8B8; break;
                case 32: format = kA8R8G8B8; break;
                default: return reject(why, "TGA: pixel depth");
            }
            break;
        case 3:
            if (depth != 8) return reject(why, "TGA: grey depth");
            format = kL8;
            break;
        default: return reject(why, "TGA: image type");
    }
    const bool top_down = (p[0x11] & 0x20) != 0, left_to_right = (p[0x11] & 0x10) == 0, rle = (type & 8) != 0;
    std::size_t remaining = size - 0x12;
    if (remaining < p[0]) return reject(why, "TGA: id");
    remaining -= p[0];
    const u8* s = p + 0x12 + p[0];
    const u32 cmap_len = rd16(p + 5);
    const std::size_t cmap_size = static_cast<std::size_t>(cmap_len) * entry_bytes;
    if (remaining < cmap_size) return reject(why, "TGA: colour map");
    if (format == kP8) {
        const u32 first = rd16(p + 3);
        if (first + cmap_len > 256) return reject(why, "TGA: colour map range");
        img.has_palette = true;
        img.palette.fill(0xffffffffu);
        const u8* e = s;
        for (u32 i = 0; i < cmap_len; ++i) {
            u32 r, g, b, a;
            if (cmap_format == kR8G8B8) {
                b = e[0], g = e[1], r = e[2], a = 0xff;
                e += 3;
            } else if (cmap_format == kA8R8G8B8) {
                b = e[0], g = e[1], r = e[2], a = e[3];
                e += 4;
            } else {
                const u32 v = rd16(e);
                const u32 r5 = (v >> 10) & 31, g5 = (v >> 5) & 31, b5 = v & 31;
                r = ((r5 >> 2) | (r5 << 3)) & 0xff;
                g = ((g5 >> 2) | (g5 << 3)) & 0xff;
                b = ((b5 >> 2) | (b5 << 3)) & 0xff;
                a = cmap_format == kA1R5G5B5 ? ((v >> 15) * 0xff) & 0xff : 0xff;
                e += 2;
            }
            img.palette[first + i] = r | g << 8 | b << 16 | a << 24;
        }
    }
    s += cmap_size;
    remaining -= cmap_size;
    const u32 pitch = width * pixel_bytes;
    if (static_cast<std::uint64_t>(pitch) * height > kMaxBytes) return unsupported(why, "TGA: image too large");
    const std::size_t total = static_cast<std::size_t>(pitch) * height;
    ImageLevel level;
    level.width = width;
    level.height = height;
    level.pitch = pitch;
    img.type = ImageFileType::Tga;
    img.format = format;
    if (!rle && top_down && left_to_right) {
        if (remaining < total) return reject(why, "TGA: pixels beyond the data");
        level.data.assign(s, s + total);
        img.levels.push_back(std::move(level));
        return Load::Ok;
    }
    Canvas c(total);
    i64 row = top_down ? 0 : static_cast<i64>(height - 1) * pitch;
    const i64 row_step = top_down ? static_cast<i64>(pitch) : -static_cast<i64>(pitch);
    const i64 px_step = left_to_right ? static_cast<i64>(pixel_bytes) : -static_cast<i64>(pixel_bytes);
    for (u32 y = 0; y < height; ++y, row += row_step) {
        i64 at = left_to_right ? row : row + pitch - pixel_bytes;
        u32 done = 0;
        while (done < width) {
            u32 count;
            bool run = false;
            if (rle) {
                if (remaining < 1) return reject(why, "TGA: RLE beyond the data");
                const u8 b = *s++;
                --remaining;
                count = (b & 0x7fu) + 1;
                run = (b & 0x80) != 0;
            } else {
                count = width;
            }
            done += count;
            for (u32 i = 0; i < count; ++i) {
                if (remaining < pixel_bytes) return reject(why, "TGA: pixels beyond the data");
                for (u32 k = 0; k < pixel_bytes; ++k) c.put(at + k, s[k]);
                if (!run) {
                    s += pixel_bytes;
                    remaining -= pixel_bytes;
                }
                at += px_step;
            }
            if (run) {
                s += pixel_bytes;
                remaining -= pixel_bytes;
            }
            if (!c.ok) return unsupported(why, "TGA: RLE writes outside the image");
        }
    }
    level.data = std::move(c.bytes);
    img.levels.push_back(std::move(level));
    return Load::Ok;
}

}  // namespace

// ---------------------------------------------------------------------------

std::uint32_t row_bytes(std::uint32_t format, std::uint32_t width) {
    if (format == kDXT1) return ((width + 3) / 4) * 8;
    if (is_dxt(format)) return ((width + 3) / 4) * 16;
    const FormatInfo& f = format_info(format);
    if (f.format == 0 || f.format == 0x59565955 || f.format == 0x32595559) return 0;
    return (f.bpp / 8) * width;
}

std::uint32_t row_count(std::uint32_t format, std::uint32_t height) { return is_dxt(format) ? (height + 3) / 4 : height; }

bool decode_image(const void* data, std::size_t size, Image& out, std::string* why) {
    out = Image{};
    const auto* p = static_cast<const u8*>(data);
    if (!p) return fail(why, "no data");
    // D3DX tries BMP, PPM, DDS, JPG, PNG, TGA, DIB in that order (0x0078856c) and takes the first
    // loader that succeeds. Every BMP / PPM / DDS / JPEG / PNG signature has a second byte that the
    // TGA loader rejects, so a file that one of those claims never reaches TGA or DIB as valid data.
    if (size >= 14 && p[0] == 'B' && p[1] == 'M' && rd32(p + 2) <= size) {
        if (load_dib(p + 14, size - 14, out, why) != Load::Ok) return false;
        out.type = ImageFileType::Bmp;
        return true;
    }
    if (size >= 2 && p[0] == 'P' && (p[1] == '3' || p[1] == '6')) return fail(why, "PPM file");
    if (size >= 4 && rd32(p) == 0x20534444) return load_dds(p, size, out, why) == Load::Ok;
    if (size >= 2 && p[0] == 0xff && p[1] == 0xd8) return fail(why, "JPEG file");
    if (size >= 4 && p[0] == 0x89 && p[1] == 'P' && p[2] == 'N' && p[3] == 'G') return fail(why, "PNG file");
    const Load tga = load_tga(p, size, out, why);
    if (tga == Load::Ok) return true;
    if (tga == Load::Unsupported) return false;
    out = Image{};
    return load_dib(p, size, out, why) == Load::Ok;
}

bool plan_texture(const Image& image, const LoadArgs& args, const DeviceQuery& device, TexturePlan& plan,
                  std::string* why) {
    plan = TexturePlan{};
    if (image.levels.empty()) return fail(why, "no image");
    if (args.color_key != 0) return fail(why, "colour key");
    // 0x00765258: defaults.
    const u32 src_format = image.format;
    u32 width = args.width == 0 || args.width == 0xffffffffu ? image.width() : args.width;
    u32 height = args.height == 0 || args.height == 0xffffffffu ? image.height() : args.height;
    u32 format = args.format;
    if (format == kUnknown) format = src_format == kR8G8B8 ? kX8R8G8B8 : src_format;
    const bool bump = format_info(src_format).type == 3;
    plan.filter = args.filter == 0xffffffffu ? (bump ? 0x80002u : 0x80004u) : args.filter;
    plan.mip_filter = args.mip_filter == 0xffffffffu ? (bump ? 2u : 5u) : args.mip_filter;
    if (image.has_palette)
        plan.palette = image.palette;  // colour key 0: entries equal to 0 are set to 0
    else
        plan.palette.fill(0xffffffffu);
    if (format == kA8P8) format = kA8R8G8B8;
    if (format == kP8) {
        if (!image.has_palette) return fail(why, "P8 requested for an image without a palette");
        bool opaque = true;
        for (u32 e : image.palette)
            if ((e >> 24) != 0xff) opaque = false;
        format = opaque ? kX8R8G8B8 : kA8R8G8B8;
    }

    // 0x00764064: D3DXCheckTextureRequirements(width, height, NULL depth, levels, usage, format, pool).
    u32 usage = args.usage == 0xffffffffu ? 0 : args.usage;
    if (usage & ~3u) return fail(why, "usage");
    if (args.pool > 2) return fail(why, "pool");
    if (width == 0xffffffffu) {
        if (height == 0xffffffffu)
            width = height = 256;
        else
            width = height;
    } else if (height == 0xffffffffu) {
        height = width;
    }
    if (width == 0) width = 1;
    if (height == 0) height = 1;
    u32 depth = 1;
    if (width > device.max_texture_width) width = device.max_texture_width;
    if (height > device.max_texture_height) height = device.max_texture_height;
    const u32 aspect = (device.texture_caps & 0x20) ? 1u : device.max_texture_aspect_ratio;  // SQUAREONLY
    if (aspect) {
        if (width > aspect * height) width = aspect * height;
        if (height > aspect * width) height = aspect * width;
    }
    if (device.texture_caps & 2) {  // POW2
        u32 v = 1;
        while (v < width) v <<= 1;
        width = v;
        v = 1;
        while (v < height) v <<= 1;
        height = v;
        v = 1;
        while (v < depth) v <<= 1;
        depth = v;
    }
    if (is_dxt(format)) {
        width = (width + 3) & ~3u;
        height = (height + 3) & ~3u;
    }
    u32 levels = args.mip_levels;
    const auto pow2 = [](u32 v) { return (v & (v - 1)) == 0; };
    if ((device.texture_caps & 0x4000) && pow2(width) && pow2(height) && pow2(depth)) {  // MIPMAP
        const auto count = [](u32 v) {
            u32 n = 1;
            while (v && !(v & 1)) v >>= 1, ++n;
            return n;
        };
        const u32 most = std::max(count(width), count(height));
        if (levels > most || levels == 0) levels = most;
    } else {
        levels = 1;
    }
    if (width == 0 || height == 0) return fail(why, "size");
    // Format: the first device-supported format of D3DX's table that is the requested one, else
    // the closest convertible one.
    const FormatInfo& want = format_info(format);
    u32 chosen = 0, best = ~0u, best_bpp = 0;
    for (const FormatInfo& f : kFormatTable) {
        if (!device.texture_format_ok || !device.texture_format_ok(f.format)) continue;
        if (f.format == format) {
            chosen = format;
            break;
        }
        if (!f.convertible) continue;
        const u32 d = format_distance(f, want);
        if (d == ~0u) {
            if (chosen == 0) return fail(why, "format search compares uninitialised data");
            continue;
        }
        if (d < best || (d == best && f.bpp < best_bpp)) {
            chosen = f.format;
            best = d;
            best_bpp = f.bpp;
        }
    }
    if (chosen == 0) return fail(why, "no texture format available");
    plan.width = width;
    plan.height = height;
    plan.levels = levels;
    plan.usage = args.usage;  // CreateTexture gets the caller's value (0x00765490), not the normalised one
    plan.format = chosen;
    plan.pool = args.pool;
    // D3DX's blitter rejects these filters (0x00786b38): the load would fail.
    const auto valid_filter = [](u32 f) { return (f & 0xffff) >= 1 && (f & 0xffff) <= 5 && !(f & 0xfff00000u); };
    if (!valid_filter(plan.filter)) return fail(why, "filter");
    return true;
}

bool build_texture(const Image& image, const TexturePlan& plan, Precision precision, std::vector<ImageLevel>& out,
                   std::string* why) {
    out.assign(plan.levels, ImageLevel{});
    for (u32 l = 0; l < plan.levels; ++l) {
        ImageLevel& level = out[l];
        level.width = std::max(1u, plan.width >> l);
        level.height = std::max(1u, plan.height >> l);
        level.pitch = row_bytes(plan.format, level.width);
        if (!level.pitch) return fail(why, "unknown target format");
        if (static_cast<std::uint64_t>(level.pitch) * row_count(plan.format, level.height) > kMaxBytes)
            return fail(why, "texture too large");
        level.data.assign(static_cast<std::size_t>(level.pitch) * row_count(plan.format, level.height), 0);
    }
    if (precision == Precision::Single) return build_levels<Single>(image, plan, out, why);
    return build_levels<double>(image, plan, out, why);
}

}  // namespace joan::render
