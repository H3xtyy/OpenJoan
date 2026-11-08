// v3d_87.dll reimplementation - see v3d.hpp. Addresses are v3d_87.dll / v3d_sse.dll (image base
// 0x10000000; the two shipped files are identical).
#include "v3d.hpp"

#include <cstring>

#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
#define JOAN_V3D_X86 1
#include <emmintrin.h>
#if defined(_MSC_VER)
#include "../compiler.hpp"
#else
#include <cpuid.h>
#endif
#else
#include <cmath>
#endif

// Float steps of the SSE functions must be single SSE operations, not x87 ones.
#if defined(_M_IX86) && !defined(_M_IX86_FP)
#error "v3d.cpp needs SSE2 float arithmetic (/arch:SSE2)"
#elif defined(_M_IX86) && _M_IX86_FP < 2
#error "v3d.cpp needs SSE2 float arithmetic (/arch:SSE2)"
#elif defined(__FLT_EVAL_METHOD__) && __FLT_EVAL_METHOD__ != 0
#error "v3d.cpp needs SSE float arithmetic (-msse2 -mfpmath=sse)"
#endif

// No fused multiply-add: every product and sum is rounded on its own, as in the original.
#if defined(_MSC_VER) && !defined(__clang__)
#pragma fp_contract(off)
#elif defined(__clang__)
#pragma STDC FP_CONTRACT OFF
#endif

namespace joan::render::v3d {
namespace {

std::uint64_t bits(double v) {
    std::uint64_t u;
    std::memcpy(&u, &v, sizeof u);
    return u;
}
double from_bits(std::uint64_t u) {
    double v;
    std::memcpy(&v, &u, sizeof v);
    return v;
}
std::uint32_t fbits(float v) {
    std::uint32_t u;
    std::memcpy(&u, &v, sizeof u);
    return u;
}

bool is_nan(double v) { return v != v; }

JOAN_V3D_INLINE X87Mem mem(float f) { return x87_mem(f); }

// SSE single-precision operations with the SSE NaN rule made explicit: with two NaN operands
// the first source operand (quietened) - a compiler may commute the operands of `a op b`.
float sse_nan(float a, float b, float r) {
    if (a != a) {
        const std::uint32_t u = fbits(a) | 0x00400000u;
        std::memcpy(&r, &u, 4);
    } else if (b != b) {
        const std::uint32_t u = fbits(b) | 0x00400000u;
        std::memcpy(&r, &u, 4);
    }
    return r;
}
inline float add(float a, float b) {
    const float r = a + b;
    return r == r ? r : sse_nan(a, b, r);
}
inline float sub(float a, float b) {
    const float r = a - b;
    return r == r ? r : sse_nan(a, b, r);
}
inline float mul(float a, float b) {
    const float r = a * b;
    return r == r ? r : sse_nan(a, b, r);
}

/// x87 result of an operation whose double result `r` is a NaN (Intel SDM vol. 1, table 4-7):
/// SNaN + QNaN -> the QNaN; two SNaNs or two QNaNs -> the one with the larger significand,
/// quieted, the positive one if equal; one NaN -> that NaN quieted (also SSE2's r). SNaN operands only come
/// from x87 memory operands (`mem()`); fld quiets them.
double x87_nan(double a, double b, double r) {
    if (!is_nan(a) || !is_nan(b)) return r;  // one NaN operand (or invalid operation): same on both
    constexpr std::uint64_t kQuiet = 0x0008000000000000ull, kSig = 0x000fffffffffffffull;
    const std::uint64_t ua = bits(a), ub = bits(b);
    const bool qa = (ua & kQuiet) != 0, qb = (ub & kQuiet) != 0;
    if (qa != qb) return from_bits(qa ? ua : ub);
    if ((ua & kSig) != (ub & kSig)) return from_bits(((ua & kSig) > (ub & kSig) ? ua : ub) | kQuiet);
    return from_bits((ua & ub) | kQuiet);  // equal significands: the positive one (measured)
}

double sqrt_rn(double v) {
#ifdef JOAN_V3D_X86
    return _mm_cvtsd_f64(_mm_sqrt_sd(_mm_setzero_pd(), _mm_set_sd(v)));
#else
    return std::sqrt(v);
#endif
}

float rsqrt(float v) {
#ifdef JOAN_V3D_X86
    return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(v)));  // rsqrtss: the CPU's own approximation
#else
    return 1.0f / std::sqrt(v);  // not bit-exact (rsqrtss is a hardware table lookup)
#endif
}

void copy_dwords(void* dst, const void* src, std::size_t n) { std::memcpy(dst, src, n * 4); }

const Matrix* row_matrix(const Matrix* bones, std::int32_t bone, std::uint32_t stride) {
    // mul dword: low 32 bits of bone * stride
    return reinterpret_cast<const Matrix*>(reinterpret_cast<const std::uint8_t*>(bones) +
                                           static_cast<std::uint32_t>(static_cast<std::uint32_t>(bone) * stride));
}

#ifdef JOAN_V3D_X86
// Packed forms of the steps below: each lane is the same IEEE single operation under the same MXCSR as the scalar
// step (the original's own mulps / addps). A NaN operand or result carries through every later step, so when no
// lane used is a NaN no step had one and the scalar NaN rules (sse_nan) never applied; otherwise the scalar code
// runs again.
bool any_nan(__m128 v, int lanes) { return (_mm_movemask_ps(_mm_cmpunord_ps(v, v)) & lanes) != 0; }
#endif

Vec3 point_xf_scalar(float x, float y, float z, const float* m);
Vec3 vector_xf_scalar(float x, float y, float z, const float* m);

/// (x * r0 + y * r1) + (z * r2 + r3): point_transform's lane order (0x100018a0).
Vec3 point_xf(float x, float y, float z, const float* m) {
#ifdef JOAN_V3D_X86
    const __m128 v = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(x), _mm_loadu_ps(m)), _mm_mul_ps(_mm_set1_ps(y), _mm_loadu_ps(m + 4))),
                                _mm_add_ps(_mm_mul_ps(_mm_set1_ps(z), _mm_loadu_ps(m + 8)), _mm_loadu_ps(m + 12)));
    if (!any_nan(v, 7)) {
        alignas(16) float o[4];
        _mm_store_ps(o, v);
        return Vec3{o[0], o[1], o[2]};
    }
#endif
    return point_xf_scalar(x, y, z, m);
}
/// (x * r0 + y * r1) + z * r2: vector_transform's lane order (0x10001b50).
Vec3 vector_xf(float x, float y, float z, const float* m) {
#ifdef JOAN_V3D_X86
    const __m128 v = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(x), _mm_loadu_ps(m)), _mm_mul_ps(_mm_set1_ps(y), _mm_loadu_ps(m + 4))),
                                _mm_mul_ps(_mm_set1_ps(z), _mm_loadu_ps(m + 8)));
    if (!any_nan(v, 7)) {
        alignas(16) float o[4];
        _mm_store_ps(o, v);
        return Vec3{o[0], o[1], o[2]};
    }
#endif
    return vector_xf_scalar(x, y, z, m);
}

Vec3 point_xf_scalar(float x, float y, float z, const float* m) {
    Vec3 r;
    r.x = add(add(mul(x, m[0]), mul(y, m[4])), add(mul(z, m[8]), m[12]));
    r.y = add(add(mul(x, m[1]), mul(y, m[5])), add(mul(z, m[9]), m[13]));
    r.z = add(add(mul(x, m[2]), mul(y, m[6])), add(mul(z, m[10]), m[14]));
    return r;
}
Vec3 vector_xf_scalar(float x, float y, float z, const float* m) {
    Vec3 r;
    r.x = add(add(mul(x, m[0]), mul(y, m[4])), mul(z, m[8]));
    r.y = add(add(mul(x, m[1]), mul(y, m[5])), mul(z, m[9]));
    r.z = add(add(mul(x, m[2]), mul(y, m[6])), mul(z, m[10]));
    return r;
}
Vec4 v4d_xf(const Vec4& v, const float* m) {  // (x * r0 + y * r1) + (z * r2 + w * r3)
    Vec4 r;
    r.x = add(add(mul(v.x, m[0]), mul(v.y, m[4])), add(mul(v.z, m[8]), mul(v.w, m[12])));
    r.y = add(add(mul(v.x, m[1]), mul(v.y, m[5])), add(mul(v.z, m[9]), mul(v.w, m[13])));
    r.z = add(add(mul(v.x, m[2]), mul(v.y, m[6])), add(mul(v.z, m[10]), mul(v.w, m[14])));
    r.w = add(add(mul(v.x, m[3]), mul(v.y, m[7])), add(mul(v.z, m[11]), mul(v.w, m[15])));
    return r;
}

/// lensq = (y*y + 0) + (x*x + z*z) (0x100019e0: mulps / movhlps / addps / shufps / addss),
/// compared as an int32 with 0x2edbe6ff (1e-10f): below (also negative / sign-set NaN) -> no
/// normalisation; otherwise v * rsqrtss(lensq).
bool normalized(float x, float y, float z, Vec3* r) {
    const float xx = mul(x, x), yy = mul(y, y), zz = mul(z, z);
    const float lsq = add(add(yy, 0.0f), add(xx, zz));
    if (static_cast<std::int32_t>(fbits(lsq)) < 0x2edbe6ff) return false;
    const float s = rsqrt(lsq);
    r->x = mul(s, x);
    r->y = mul(s, y);
    r->z = mul(s, z);
    return true;
}

}  // namespace

// --- X87 ------------------------------------------------------------------------------------
double X87::nan_result(double a, double b, double r) { return x87_nan(a, b, r); }
double X87::sqrt(double a) const {
    const double r = sqrt_rn(a);
    return single ? round24(r) : r;
}

// --- SSE functions --------------------------------------------------------------------------
Vec3* vector_normalize(Vec3* v) {  // 0x100019e0
    Vec3 r;
    if (normalized(v->x, v->y, v->z, &r)) *v = r;
    return v;
}

Vec3* vector_normalize(Vec3* out, const Vec3* v) {  // 0x10001a90
    Vec3 r;
    if (normalized(v->x, v->y, v->z, &r))
        *out = r;
    else
        copy_dwords(out, v, 3);  // movlps / movss of the loaded vector
    return out;
}

Vec3* cross_product(Vec3* out, const Vec3* a, const Vec3* b) {  // 0x10001800
    const float ax = a->x, ay = a->y, az = a->z, bx = b->x, by = b->y, bz = b->z;
    out->x = sub(mul(ay, bz), mul(az, by));
    out->y = sub(mul(az, bx), mul(ax, bz));
    out->z = sub(mul(ax, by), mul(ay, bx));
    return out;
}

Vec3* point_transform(Vec3* v, const Matrix* m) {  // 0x100018a0
    *v = point_xf(v->x, v->y, v->z, m->m);
    return v;
}
Vec3* point_transform(Vec3* out, const Vec3* v, const Matrix* m) {  // 0x10001940
    *out = point_xf(v->x, v->y, v->z, m->m);
    return out;
}
Vec3* vector_transform(Vec3* v, const Matrix* m) {  // 0x10001b50
    *v = vector_xf(v->x, v->y, v->z, m->m);
    return v;
}
Vec3* vector_transform(Vec3* out, const Vec3* v, const Matrix* m) {  // 0x10001be0
    *out = vector_xf(v->x, v->y, v->z, m->m);
    return out;
}
Vec4* v4d_transform(Vec4* v, const Matrix* m) {  // 0x10001c80
    *v = v4d_xf(*v, m->m);
    return v;
}
Vec4* v4d_transform(Vec4* out, const Vec4* v, const Matrix* m) {  // 0x10001d20
    *out = v4d_xf(*v, m->m);
    return out;
}

Matrix* matrix_multiply(Matrix* out, const Matrix* a, const Matrix* b) {  // 0x100021e0
    float bm[16];
    copy_dwords(bm, b->m, 16);  // b's rows are loaded first (xmm4..7)
#ifdef JOAN_V3D_X86
    const __m128 b0 = _mm_loadu_ps(bm), b1 = _mm_loadu_ps(bm + 4), b2 = _mm_loadu_ps(bm + 8), b3 = _mm_loadu_ps(bm + 12);
#endif
    for (int r = 0; r < 4; ++r) {
        const float* ar = a->m + r * 4;
        const float x = ar[0], y = ar[1], z = ar[2], w = ar[3];
#ifdef JOAN_V3D_X86
        const __m128 v = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(x), b0), _mm_mul_ps(_mm_set1_ps(y), b1)), _mm_mul_ps(_mm_set1_ps(z), b2)),
                                    _mm_mul_ps(_mm_set1_ps(w), b3));
        if (!any_nan(v, 15)) {
            _mm_storeu_ps(out->m + r * 4, v);  // written before the next row of a is read
            continue;
        }
#endif
        float row[4];
        for (int c = 0; c < 4; ++c)
            row[c] = add(add(add(mul(x, bm[c]), mul(y, bm[4 + c])), mul(z, bm[8 + c])), mul(w, bm[12 + c]));
        copy_dwords(out->m + r * 4, row, 4);  // written before the next row of a is read
    }
    return out;
}
Matrix* matrix_multiply(Matrix* a, const Matrix* b) {  // 0x10002050
    return matrix_multiply(a, a, b);
}

Matrix* matrix_transpose(Matrix* m) {  // 0x10001dc0: swaps as integers
    auto* u = reinterpret_cast<std::uint32_t*>(m->m);
    static const int kPairs[6][2] = {{1, 4}, {2, 8}, {3, 12}, {6, 9}, {7, 13}, {11, 14}};
    for (const auto& p : kPairs) {
        const std::uint32_t t = u[p[0]];
        u[p[0]] = u[p[1]];
        u[p[1]] = t;
    }
    return m;
}
Matrix* matrix_transpose(Matrix* out, const Matrix* in) {  // 0x10001e60: in[k] -> out[transposed k], k = 0..15
    auto* o = reinterpret_cast<std::uint32_t*>(out->m);
    const auto* i = reinterpret_cast<const std::uint32_t*>(in->m);
    for (int k = 0; k < 16; ++k) o[(k & 3) * 4 + (k >> 2)] = i[k];
    return out;
}

void skin_deform(std::int32_t n, Vec3* pos, std::uint32_t pos_stride, Vec3* normal, std::uint32_t normal_stride,
                 const SkinVertex* sv, std::uint32_t sv_stride, const Matrix* bones, std::uint32_t bone_stride) {  // 0x100023e0
    auto* p = reinterpret_cast<std::uint8_t*>(pos);
    auto* nm = reinterpret_cast<std::uint8_t*>(normal);
    const auto* s = reinterpret_cast<const std::uint8_t*>(sv);
    for (std::int32_t i = 0; i < n; ++i, p += pos_stride, nm += normal_stride, s += sv_stride) {
        const auto* v = reinterpret_cast<const SkinVertex*>(s);
        const SkinBone& b0 = v->bones[0];
        if (v->count == 1) {
            const float* m = row_matrix(bones, b0.bone, bone_stride)->m;
            *reinterpret_cast<Vec3*>(p) = point_xf(b0.pos.x, b0.pos.y, b0.pos.z, m);
            *reinterpret_cast<Vec3*>(nm) = vector_xf(b0.normal.x, b0.normal.y, b0.normal.z, m);
            continue;
        }
        float ax = 0.0f, ay = 0.0f, az = 0.0f;  // xorps xmm6
        std::uint32_t left = static_cast<std::uint32_t>(v->count);
        const SkinBone* b = v->bones;
        do {  // dec ecx / jne
            const Vec3 t = point_xf(b->pos.x, b->pos.y, b->pos.z, row_matrix(bones, b->bone, bone_stride)->m);
            const float w = b->weight;
            ax = add(ax, mul(t.x, w));
            ay = add(ay, mul(t.y, w));
            az = add(az, mul(t.z, w));
            ++b;
        } while (--left != 0);
        Vec3 acc;
        acc.x = ax;
        acc.y = ay;
        acc.z = az;
        *reinterpret_cast<Vec3*>(p) = acc;
        // The normal comes from the first bone only, unweighted.
        *reinterpret_cast<Vec3*>(nm) =
            vector_xf(b0.normal.x, b0.normal.y, b0.normal.z, row_matrix(bones, b0.bone, bone_stride)->m);
    }
}

// --- x87 functions --------------------------------------------------------------------------
double length_sq(const Vec3* v, X87 x) {  // 0x10001740: st0 = (x*x + y*y) + z*z
    if (x.single) {
        // The steps without their NaN rules: squares and sums of squares make a NaN only from a NaN operand, and
        // a NaN carries to the result - so a non-NaN result is the one the full steps give.
        const double a = v->x, b = v->y, c = v->z;
        const double r = round24(round24(round24(a * a) + round24(b * b)) + round24(c * c));
        if (r == r) return r;
    }
    return x.add(x.add(x.mul(v->x, mem(v->x)), x.mul(v->y, mem(v->y))), x.mul(v->z, mem(v->z)));
}

double length(const Vec3* v, X87 x) {  // 0x100016c0: CRT sqrt (0x10006424) of length_sq
    // The CRT loads the double argument, keeps the precision control (sets RC = nearest and masks
    // exceptions: cw = (cw & 0x300) | 0x7f) and runs fsqrt; +inf and NaN pass through, a negative
    // argument (impossible for a sum of squares) would give the indefinite NaN.
    return x.sqrt(length_sq(v, x));
}

double dot_product(const Vec3* a, const Vec3* b, X87 x) {  // 0x100017a0
    return x.add(x.add(x.mul(a->x, mem(b->x)), x.mul(a->y, mem(b->y))), x.mul(a->z, mem(b->z)));
}

Matrix* matrix_multiply_translation(Matrix* m, const Vec3* v, X87 x) {  // 0x10002370: fld/fadd/fstp
    m->m[12] = static_cast<float>(x.add(m->m[12], mem(v->x)));
    m->m[13] = static_cast<float>(x.add(m->m[13], mem(v->y)));
    m->m[14] = static_cast<float>(x.add(m->m[14], mem(v->z)));
    return m;
}

// The steps of matrix_inverse: X87 (the x87's, NaN rules included) or Plain24 (the same 24-bit roundings without
// the NaN rules, memory operands loaded as plain floats) - equal for every step whose operands are not NaN.
struct X87Steps {
    X87 x;
    JOAN_V3D_INLINE double mul(double a, double b) const { return x.mul(a, b); }
    JOAN_V3D_INLINE double mul(double a, X87Mem b) const { return x.mul(a, b); }
    JOAN_V3D_INLINE double add(double a, double b) const { return x.add(a, b); }
    JOAN_V3D_INLINE double add(double a, X87Mem b) const { return x.add(a, b); }
    JOAN_V3D_INLINE double sub(double a, double b) const { return x.sub(a, b); }
    JOAN_V3D_INLINE double div(double a, double b) const { return x.div(a, b); }
    JOAN_V3D_INLINE X87Mem mem(float f) const { return x87_mem(f); }
};
struct Plain24Steps {
    JOAN_V3D_INLINE double mul(double a, double b) const { return round24(a * b); }
    JOAN_V3D_INLINE double add(double a, double b) const { return round24(a + b); }
    JOAN_V3D_INLINE double sub(double a, double b) const { return round24(a - b); }
    JOAN_V3D_INLINE double div(double a, double b) const { return round24(a / b); }
    JOAN_V3D_INLINE double mem(float f) const { return f; }
};

// D3DXMatrixInverse's arithmetic into result[16] (left as given when 1/det is not finite). Returns false when the
// determinant or a written element is a NaN.
template <class Ops>
bool inverse_steps(const float* m, std::uint32_t* result, const Ops& o);

Matrix* matrix_inverse(Matrix* out, const Matrix* in, X87 x) {  // 0x10001fd0 -> D3DXMatrixInverse 0x1000339d
    float m[16];
    copy_dwords(m, in->m, 16);  // D3DXMATRIX copy constructor into a local
    std::uint32_t result[16];   // the local result matrix: debug stack fill until written
    for (auto& r : result) r = 0xccccccccu;
    // At 24 bits first without the NaN rules: a NaN carries through every later step into the determinant or the
    // element it feeds, so when neither is a NaN no step had a NaN operand and the result is the x87 steps' one.
    if (!(x.single && inverse_steps(m, result, Plain24Steps{}))) {
        for (auto& r : result) r = 0xccccccccu;
        inverse_steps(m, result, X87Steps{x});
    }
    copy_dwords(out->m, result, 16);  // rep movsd from the local
    return out;
}

template <class Ops>
bool inverse_steps(const float* m, std::uint32_t* result, const Ops& o) {

    // D3DXMatrixInverse (x87; generated from the disassembly by symbolic execution: tN are x87
    // register values, sN values stored to float locals; double(m[i]) = loaded by fld, o.mem(m[i]) =
    // fmul memory operand; t67 / t70 / t73 / t76 stay in registers; det = t123, never stored).
    const double t1 = o.mul(double(m[5]), double(m[0]));
    const double t2 = o.mul(double(m[4]), double(m[1]));
    const double t3 = o.sub(t1, t2);
    const float s1 = static_cast<float>(t3);
    const double t4 = o.mul(double(m[9]), double(m[0]));
    const double t5 = o.mul(double(m[8]), double(m[1]));
    const double t6 = o.sub(t4, t5);
    const float s2 = static_cast<float>(t6);
    const double t7 = o.mul(double(m[13]), double(m[0]));
    const double t8 = o.mul(double(m[12]), double(m[1]));
    const double t9 = o.sub(t7, t8);
    const float s3 = static_cast<float>(t9);
    const double t10 = o.mul(double(m[9]), o.mem(m[4]));
    const double t11 = o.mul(double(m[8]), o.mem(m[5]));
    const double t12 = o.sub(t10, t11);
    const float s4 = static_cast<float>(t12);
    const double t13 = o.mul(double(m[13]), o.mem(m[4]));
    const double t14 = o.mul(double(m[12]), o.mem(m[5]));
    const double t15 = o.sub(t13, t14);
    const float s5 = static_cast<float>(t15);
    const double t16 = o.mul(double(m[13]), o.mem(m[8]));
    const double t17 = o.mul(double(m[12]), o.mem(m[9]));
    const double t18 = o.sub(t16, t17);
    const float s6 = static_cast<float>(t18);
    const double t19 = o.mul(double(m[2]), double(s4));
    const double t20 = o.mul(double(m[6]), double(s2));
    const double t21 = o.sub(t19, t20);
    const double t22 = o.mul(double(m[10]), double(s1));
    const double t23 = o.add(t21, t22);
    const float s7 = static_cast<float>(t23);
    const double t24 = o.mul(double(m[6]), double(s3));
    const double t25 = o.mul(double(m[14]), double(s1));
    const double t26 = o.sub(t24, t25);
    const double t27 = o.mul(double(m[2]), double(s5));
    const double t28 = o.sub(t26, t27);
    const float s8 = static_cast<float>(t28);
    const double t29 = o.mul(double(m[2]), double(s6));
    const double t30 = o.mul(double(m[10]), double(s3));
    const double t31 = o.sub(t29, t30);
    const double t32 = o.mul(double(m[14]), double(s2));
    const double t33 = o.add(t31, t32);
    const float s9 = static_cast<float>(t33);
    const double t34 = o.mul(double(m[10]), double(s5));
    const double t35 = o.mul(double(m[14]), double(s4));
    const double t36 = o.sub(t34, t35);
    const double t37 = o.mul(double(m[6]), double(s6));
    const double t38 = o.sub(t36, t37);
    const float s10 = static_cast<float>(t38);
    const double t39 = o.mul(double(s2), double(m[7]));
    const double t40 = o.mul(double(s1), double(m[11]));
    const double t41 = o.sub(t39, t40);
    const double t42 = o.mul(double(s4), double(m[3]));
    const double t43 = o.sub(t41, t42);
    const float s11 = static_cast<float>(t43);
    const double t44 = o.mul(double(s5), double(m[3]));
    const double t45 = o.mul(double(s3), double(m[7]));
    const double t46 = o.sub(t44, t45);
    const double t47 = o.mul(double(s1), o.mem(m[15]));
    const double t48 = o.add(t46, t47);
    const float s12 = static_cast<float>(t48);
    const double t49 = o.mul(double(s3), double(m[11]));
    const double t50 = o.mul(double(s2), o.mem(m[15]));
    const double t51 = o.sub(t49, t50);
    const double t52 = o.mul(double(s6), double(m[3]));
    const double t53 = o.sub(t51, t52);
    const float s13 = static_cast<float>(t53);
    const double t54 = o.mul(double(s6), double(m[7]));
    const double t55 = o.mul(double(s5), double(m[11]));
    const double t56 = o.sub(t54, t55);
    const double t57 = o.mul(double(s4), o.mem(m[15]));
    const double t58 = o.add(t56, t57);
    const float s14 = static_cast<float>(t58);
    const double t59 = o.mul(double(m[2]), double(m[7]));
    const double t60 = o.mul(double(m[6]), double(m[3]));
    const double t61 = o.sub(t59, t60);
    const float s15 = static_cast<float>(t61);
    const double t62 = o.mul(double(m[2]), double(m[11]));
    const double t63 = o.mul(double(m[10]), double(m[3]));
    const double t64 = o.sub(t62, t63);
    const float s16 = static_cast<float>(t64);
    const double t65 = o.mul(double(m[2]), o.mem(m[15]));
    const double t66 = o.mul(double(m[14]), double(m[3]));
    const double t67 = o.sub(t65, t66);
    const double t68 = o.mul(double(m[6]), double(m[11]));
    const double t69 = o.mul(double(m[10]), double(m[7]));
    const double t70 = o.sub(t68, t69);
    const double t71 = o.mul(double(m[6]), o.mem(m[15]));
    const double t72 = o.mul(double(m[14]), double(m[7]));
    const double t73 = o.sub(t71, t72);
    const double t74 = o.mul(double(m[10]), o.mem(m[15]));
    const double t75 = o.mul(double(m[14]), double(m[11]));
    const double t76 = o.sub(t74, t75);
    const double t77 = o.mul(double(s16), double(m[5]));
    const double t78 = o.mul(double(s15), o.mem(m[9]));
    const double t79 = o.sub(t77, t78);
    const double t80 = o.mul(t70, double(m[1]));
    const double t81 = o.sub(t79, t80);
    const float s17 = static_cast<float>(t81);
    const double t82 = o.mul(t73, double(m[1]));
    const double t83 = o.mul(t67, double(m[5]));
    const double t84 = o.sub(t82, t83);
    const double t85 = o.mul(double(s15), o.mem(m[13]));
    const double t86 = o.add(t84, t85);
    const float s18 = static_cast<float>(t86);
    const double t87 = o.mul(t67, o.mem(m[9]));
    const double t88 = o.mul(double(s16), o.mem(m[13]));
    const double t89 = o.sub(t87, t88);
    const double t90 = o.mul(t76, double(m[1]));
    const double t91 = o.sub(t89, t90);
    const float s19 = static_cast<float>(t91);
    const double t92 = o.mul(t76, double(m[5]));
    const double t93 = o.mul(t73, o.mem(m[9]));
    const double t94 = o.sub(t92, t93);
    const double t95 = o.mul(t70, o.mem(m[13]));
    const double t96 = o.add(t94, t95);
    const float s20 = static_cast<float>(t96);
    const double t97 = o.mul(t70, o.mem(m[0]));
    const double t98 = o.mul(double(s16), o.mem(m[4]));
    const double t99 = o.sub(t97, t98);
    const double t100 = o.mul(double(s15), o.mem(m[8]));
    const double t101 = o.add(t99, t100);
    const float s21 = static_cast<float>(t101);
    const double t102 = o.mul(t67, o.mem(m[4]));
    const double t103 = o.mul(double(s15), o.mem(m[12]));
    const double t104 = o.sub(t102, t103);
    const double t105 = o.mul(t73, o.mem(m[0]));
    const double t106 = o.sub(t104, t105);
    const float s22 = static_cast<float>(t106);
    const double t107 = o.mul(double(m[0]), t76);
    const double t108 = o.mul(t67, o.mem(m[8]));
    const double t109 = o.sub(t107, t108);
    const double t110 = o.mul(double(s16), o.mem(m[12]));
    const double t111 = o.add(t109, t110);
    const float s23 = static_cast<float>(t111);
    const double t112 = o.mul(t73, o.mem(m[8]));
    const double t113 = o.mul(t70, o.mem(m[12]));
    const double t114 = o.sub(t112, t113);
    const double t115 = o.mul(t76, o.mem(m[4]));
    const double t116 = o.sub(t114, t115);
    const float s24 = static_cast<float>(t116);
    const double t117 = o.mul(double(s17), o.mem(m[12]));
    const double t118 = o.mul(double(s18), o.mem(m[8]));
    const double t119 = o.add(t117, t118);
    const double t120 = o.mul(double(s19), o.mem(m[4]));
    const double t121 = o.add(t119, t120);
    const double t122 = o.mul(double(s20), o.mem(m[0]));
    const double det = o.add(t121, t122);
    // fld1 / fdiv st,st(1) / fstp dword; _finite() of that float.
    const float inv = static_cast<float>(o.div(1.0, det));
    const std::uint32_t inv_bits = fbits(inv);
    bool ok = det == det;
    if ((inv_bits & 0x7f800000u) != 0x7f800000u) {
        const float cof[16] = {s20, s19, s18, s17, s24, s23, s22, s21, s14, s13, s12, s11, s10, s9, s8, s7};
        for (int i = 0; i < 16; ++i) {
            const double p = o.mul(double(cof[i]), double(inv));
            ok = ok && p == p;
            result[i] = fbits(static_cast<float>(p));
        }
    }
    return ok;
}

Matrix* matrix_inverse(Matrix* m, X87 x) {  // 0x10001f20: the same with out = in
    return matrix_inverse(m, m, x);
}

// --- SkinVertex -----------------------------------------------------------------------------
void skin_init(SkinVertex* sv) { sv->count = 0; }    // 0x10001130
void skin_deinit(SkinVertex* sv) { sv->count = 0; }  // 0x10001170

std::int32_t skin_add_bone(SkinVertex* sv, std::int32_t bone, const Vec3* pos, const Vec3* normal, float weight) {  // 0x100011b0
    std::int32_t i = 0;
    for (; i < sv->count; ++i)
        if (weight > sv->bones[i].weight) break;  // fcomp: C0 | C3 clear (ordered, greater)
    if (i >= 4) return -1;
    while (sv->count >= 4) sv->count = sv->count - 1;
    // 0x10001450 insert(array, count, 0x20, i + 1, nullptr): shift up, zero the slot.
    SkinBone* p = &sv->bones[i];
    if (i + 1 <= sv->count)
        std::memmove(p + 1, p, static_cast<std::size_t>(sv->count - i) * sizeof(SkinBone));
    std::memset(p, 0, sizeof(SkinBone));
    sv->count = sv->count + 1;
    p->bone = bone;
    copy_dwords(&p->pos, pos, 3);
    copy_dwords(&p->normal, normal, 3);
    std::memcpy(&p->weight, &weight, 4);
    return i;
}

void skin_normalize(SkinVertex* sv, X87 x) {  // 0x10001300
    if (sv->count <= 0) return;
    float sum = 0.0f;
    for (std::int32_t i = 0; i < sv->count; ++i) sum = static_cast<float>(x.add(sum, mem(sv->bones[i].weight)));
    if (sum > 0.0f) {  // fcomp 0.0: C0 | C3 clear
        for (std::int32_t i = 0; i < sv->count; ++i)
            sv->bones[i].weight = static_cast<float>(x.div(sv->bones[i].weight, mem(sum)));
    } else {
        for (std::int32_t i = 0; i < sv->count; ++i)  // fild count / fdivr 1.0
            sv->bones[i].weight = static_cast<float>(x.div(1.0, static_cast<double>(sv->count)));
    }
    vector_normalize(&sv->bones[0].normal);  // through the DLL's own export thunk
}

std::int32_t vertexf_check_cpu() {  // 0x100015b0: cpuid 1, EDX bit 25 (SSE)
#ifdef JOAN_V3D_X86
#if defined(_MSC_VER)
    int r[4];
    joan::cpu::id(r, 1);
    return (static_cast<std::uint32_t>(r[3]) & 0x2000000u) ? 1 : 0;
#else
    unsigned a, b, c, d;
    if (!__get_cpuid(1, &a, &b, &c, &d)) return 0;
    return (d & 0x2000000u) ? 1 : 0;
#endif
#else
    return 0;
#endif
}

}  // namespace joan::render::v3d
