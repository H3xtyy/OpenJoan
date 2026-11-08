// v3d_87.dll - the game's vector maths library - reimplemented bit for bit.
//
// joan.exe imports 23 functions from v3d_87.dll (IAT 0x00a6ddec..0x00a6de44). At start-up the
// original game picks a CPU-specific build (0x00447150: V3D_SSE.DLL, V3D_K3D.DLL, V3D_X87.DLL, the
// first whose vertexf_check_cpu() returns non-zero) and copies it over V3D_87.DLL; the shipped
// v3d_87.dll is byte-identical to v3d_sse.dll, so the code that runs is the SSE build - the one
// reimplemented here. openjoan.exe never loads the DLL: the loader's import override table
// (loader/import_overrides.hpp) binds the 23 IAT slots to the wrappers in lib/v3d/v3d87.cpp, and our
// 0x00447150 (modules/game_b108_05.cpp) only asks vertexf_check_cpu (no file is probed). It is an
// MSVC debug build (/RTC stack fill, no optimisation): most functions are hand-written SSE
// (packed single precision, MXCSR rounding), a few are plain x87 C code, and matrix_inverse calls
// the statically linked x87 D3DXMatrixInverse. Details: notes V3D.md.
//
//  * SSE functions: float arithmetic in the original lane order. With SSE float arithmetic
//    (MSVC /arch:SSE2, or -msse2 -mfpmath=sse) every result is the same IEEE single operation
//    under the same MXCSR, so plain float code is exact. rsqrtss (vector_normalize) is a
//    vendor-specific approximation and is executed with the same instruction on x86.
//  * x87 functions (length, length_sq, dot_product, matrix_multiply_translation, matrix_inverse,
//    SkinVertex::normalize): each step follows the x87 code under the precision control in
//    force (the game runs at 24 bits; 53 bits supported too) - see X87 below. Values the
//    originals return in st0 (length, length_sq, dot_product) are returned as double: at 24-bit
//    precision they are 24-bit significands with the x87's unbounded exponent range, which a
//    float would clamp (callers that keep st0 see the difference for |v| beyond ~1.8e19).
//
// NaN payloads are reproduced too: x87 steps apply the x87 rule (QNaN over SNaN, else the larger
// significand, SNaNs only from memory operands - X87Mem), SSE steps the SSE rule (first operand),
// made explicit because a compiler may commute `a * b`. rsqrtss is the CPU's own approximation:
// equal to the DLL on the same CPU (the DLL runs on the player's CPU too).
//
// Portable C++ (no Windows headers); x86 only for rsqrtss and cpuid.
#pragma once

#include <cstdint>
#include <cstring>

#if defined(_MSC_VER)
#define JOAN_V3D_INLINE __forceinline
#else
#define JOAN_V3D_INLINE inline __attribute__((always_inline))
#endif

/// The vector maths library of the original game, reimplemented bit for bit.
namespace joan::render::v3d {

/// Three floats (D3DXVECTOR3 layout, 12 bytes).
struct Vec3 {
    float x;  ///< x component
    float y;  ///< y component
    float z;  ///< z component
};
/// Four floats (D3DXVECTOR4 layout, 16 bytes).
struct Vec4 {
    float x;  ///< x component
    float y;  ///< y component
    float z;  ///< z component
    float w;  ///< w component
};
/// 4x4 matrix of floats.
struct Matrix {  // D3DMATRIX, row-major m[row * 4 + column]
    float m[16];  ///< elements, row-major m[row * 4 + column], row-vector convention
};

/// SkinVertex (0x84 bytes): up to 4 bone influences sorted by decreasing weight.
struct SkinBone {
    std::int32_t bone;  ///< index into the bone matrix array
    Vec3 pos;           ///< position in the bone's space
    Vec3 normal;        ///< normal in the bone's space
    float weight;       ///< influence of this bone (the bones of a vertex sum to 1 once skin_normalize has run)
};
/// A vertex's skinning data: `count` influences, heaviest first (the layout the game's meshes use).
struct SkinVertex {
    std::int32_t count;  ///< number of valid entries of `bones` (1..4 once a bone was added)
    SkinBone bones[4];   ///< the influences
};
static_assert(sizeof(SkinBone) == 0x20 && sizeof(SkinVertex) == 0x84, "v3d_87.dll layout");

/// One x87 operation of the original at the current precision control (round to nearest):
/// `single` = 24-bit significands with the x87's (unbounded) exponent range, otherwise 53 bits.
/// Operands are doubles holding x87 register values exactly. Each step is the double result
/// rounded to 24 bits - exact, because double rounding is innocuous for +, -, *, / and sqrt of
/// 24-bit operands at 53 bits (53 >= 2 * 24 + 2) and no game value leaves the double range.
/// An x87 memory operand (fadd / fmul / fdiv m32): the float's bits as a double, an SNaN kept
/// signalling (the x87 prefers a QNaN to it). Carried as bits: returning a signalling double
/// through st0 (x86 ABI, unoptimised builds) would quieten it.
struct X87Mem {
    std::uint64_t bits;  ///< the float widened to a double, as IEEE-754 binary64 bits (SNaN payload kept)
};
/// The float `f` as an X87Mem (SNaN kept signalling; see the comment above).
JOAN_V3D_INLINE X87Mem x87_mem(float f) {
    std::uint32_t u;
    std::memcpy(&u, &f, 4);
    X87Mem m;
    if ((u & 0x7f800000u) == 0x7f800000u && (u & 0x007fffffu) != 0) {  // NaN: widen the bits
        m.bits = (static_cast<std::uint64_t>(u & 0x80000000u) << 32) | 0x7ff0000000000000ull |
                 (static_cast<std::uint64_t>(u & 0x007fffffu) << 29);
    } else {
        const double d = f;
        std::memcpy(&m.bits, &d, 8);
    }
    return m;
}

/// Rounds a double to a 24-bit significand, ties to even, keeping the exponent (no overflow /
/// denormalisation: the x87 register format at 24-bit precision). NaN / inf / 0 unchanged.
JOAN_V3D_INLINE double round24(double v) {  // (MSVC kept it out of line: ~3% of a frame in calls)
    std::uint64_t u;
    std::memcpy(&u, &v, 8);
    if (((u >> 52) & 0x7ff) == 0x7ff) return v;  // NaN / inf
    // 52 -> 23 stored significand bits, ties to even; a carry moves into the exponent.
    const std::uint64_t lsb = (u >> 29) & 1;
    u += 0x0fffffffull + lsb;
    u &= ~0x1fffffffull;
    std::memcpy(&v, &u, 8);
    return v;
}

/// The x87 arithmetic of the original at one precision control: `single` (24-bit significands) or
/// double (53). The four basic steps are inline (the game's collision and AI maths call them for every
/// vector operation); only a NaN result goes out of line.
struct X87 {
    bool single = true;  ///< true: 24-bit precision control (what Direct3D 8 sets), false: 53-bit
    /// a + b as the x87 computes it at this precision.
    JOAN_V3D_INLINE double add(double a, double b) const { return finish(a + b, a, b); }
    /// a - b as the x87 computes it at this precision.
    JOAN_V3D_INLINE double sub(double a, double b) const { return finish(a - b, a, b); }
    /// a * b as the x87 computes it at this precision.
    JOAN_V3D_INLINE double mul(double a, double b) const { return finish(a * b, a, b); }
    /// a / b as the x87 computes it at this precision.
    JOAN_V3D_INLINE double div(double a, double b) const { return finish(a / b, a, b); }
    /// Rounds the double result `r` of an operation on `a` and `b` to the precision; a NaN `r` takes nan_result.
    JOAN_V3D_INLINE double finish(double r, double a, double b) const {
        if (r != r) return nan_result(a, b, r);
        return single ? round24(r) : r;
    }
    /// Which operand's NaN (or the default NaN) the x87 returns for operands `a`, `b` whose result `r` is a NaN.
    static double nan_result(double a, double b, double r);  // the x87's NaN choice
    // Memory operand forms (the operand's bits as a double: an SNaN stays signalling).
    /// a + b with `b` a memory operand (m32).
    JOAN_V3D_INLINE double add(double a, X87Mem b) const { return add(a, mem_value(b)); }
    /// a * b with `b` a memory operand (m32).
    JOAN_V3D_INLINE double mul(double a, X87Mem b) const { return mul(a, mem_value(b)); }
    /// a / b with `b` a memory operand (m32).
    JOAN_V3D_INLINE double div(double a, X87Mem b) const { return div(a, mem_value(b)); }
    /// The memory operand's bits as the double the x87 register would hold (an SNaN stays signalling).
    static JOAN_V3D_INLINE double mem_value(X87Mem b) {
        double d;
        std::memcpy(&d, &b.bits, 8);
        return d;
    }
    /// fsqrt of `a` at this precision.
    double sqrt(double a) const;  // fsqrt (the CRT's sqrt keeps the precision control)
};

// --- SSE functions (bit-exact under any x87 precision; follow MXCSR like the original) ---
Vec3* vector_normalize(Vec3* v);  ///< in place; unchanged if |v|^2 < 1e-10
Vec3* vector_normalize(Vec3* out, const Vec3* v);  ///< out = v if |v|^2 < 1e-10
/// out = a x b (cross product).
Vec3* cross_product(Vec3* out, const Vec3* a, const Vec3* b);
Vec3* point_transform(Vec3* v, const Matrix* m);  ///< v = v * m (w = 1), in place
/// out = v * m (w = 1).
Vec3* point_transform(Vec3* out, const Vec3* v, const Matrix* m);
Vec3* vector_transform(Vec3* v, const Matrix* m);  ///< v = v * m (w = 0), in place
/// out = v * m (w = 0).
Vec3* vector_transform(Vec3* out, const Vec3* v, const Matrix* m);
Vec4* v4d_transform(Vec4* v, const Matrix* m);  ///< not imported by joan.exe
/// out = v * m (four components).
Vec4* v4d_transform(Vec4* out, const Vec4* v, const Matrix* m);
Matrix* matrix_multiply(Matrix* a, const Matrix* b);  ///< a = a * b
/// out = a * b.
Matrix* matrix_multiply(Matrix* out, const Matrix* a, const Matrix* b);
Matrix* matrix_transpose(Matrix* m);  ///< in place (integer moves)
Matrix* matrix_transpose(Matrix* out, const Matrix* in);  ///< element order of the original (out == in scrambles)
/// Skinning: for each of `n` vertices, position = weighted sum of point_transform(bone pos) over
/// `count` bones (count 1: unweighted), normal = vector_transform(bones[0].normal) unweighted.
/// Pointers advance by their strides; bone matrix = bones + bone * bone_stride. count <= 0 with
/// n > 0 loops 2^32 times like the original (never happens: add_bone keeps 1..4).
void skin_deform(std::int32_t n, Vec3* pos, std::uint32_t pos_stride, Vec3* normal, std::uint32_t normal_stride,
                 const SkinVertex* sv, std::uint32_t sv_stride, const Matrix* bones, std::uint32_t bone_stride);

// --- x87 functions ---
double length_sq(const Vec3* v, X87 x);  ///< st0 of the original
double length(const Vec3* v, X87 x);  ///< st0 of the original (CRT sqrt)
/// The dot product of a and b, as the x87 result st0 of the original.
double dot_product(const Vec3* a, const Vec3* b, X87 x);
Matrix* matrix_multiply_translation(Matrix* m, const Vec3* v, X87 x);  ///< m._41.._43 += v
/// D3DXMatrixInverse (static D3DX8 x87 code) into a copy, then copied to out. A singular matrix
/// (1/det not finite as a float) leaves the copy unwritten: out = 16 x 0xcccccccc (the debug
/// build's stack fill, -4.316e8f).
Matrix* matrix_inverse(Matrix* out, const Matrix* in, X87 x);
/// The same as matrix_inverse(out, in, x) with out = in (v3d_87.dll 0x10001f20).
Matrix* matrix_inverse(Matrix* m, X87 x);

// --- SkinVertex members ---
void skin_init(SkinVertex* sv);  ///< count = 0
void skin_deinit(SkinVertex* sv);  ///< count = 0
/// Inserts a bone before the first one with a smaller weight (keeps the 3 heaviest + the new
/// one); returns its index or -1 if it would land at index 4.
std::int32_t skin_add_bone(SkinVertex* sv, std::int32_t bone, const Vec3* pos, const Vec3* normal, float weight);
/// Weights /= their sum (sum > 0) or 1 / count each; then vector_normalize(bones[0].normal).
void skin_normalize(SkinVertex* sv, X87 x);

/// 1 if the CPU has SSE (CPUID.1:EDX bit 25).
std::int32_t vertexf_check_cpu();

}  // namespace joan::render::v3d
