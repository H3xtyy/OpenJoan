// v3d_87.dll harness: maps the real DLL at its image base (freestanding i386 Linux, no DllMain)
// and compares every function joan.exe imports with src/render/v3d.cpp, bit for bit, on random
// and edge-case inputs (zeros, denormals, huge values, inf, NaN payloads), at x87 precision
// control 24 and 53 bits. Ground truth for N5 (notes V3D.md).
//
// usage: v3d_run <v3d_87.dll> [iterations per test (default 200000)] [seed]
#include <stddef.h>
#include <stdint.h>

#include "../../src/render/v3d.hpp"

extern "C" {
long sys_raw(long n, long a, long b, long c, long d, long e);
void printf_(const char* fmt, ...);
void die(const char* s);
void rt_flush(void);
void* memcpy(void* d, const void* s, size_t n);
void* memset(void* d, int c, size_t n);
int memcmp(const void* a, const void* b, size_t n);
int strcmp(const char* a, const char* b);
}

namespace v3d = joan::render::v3d;
using v3d::Matrix;
using v3d::SkinVertex;
using v3d::Vec3;
using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

static inline long sys(long n, long a, long b, long c, long d, long e) {
    long r;
    __asm__ volatile("push %%ebp\n xor %%ebp,%%ebp\n int $0x80\n pop %%ebp"
                     : "=a"(r)
                     : "a"(n), "b"(a), "c"(b), "d"(c), "S"(d), "D"(e)
                     : "memory");
    return r;
}
extern "C" long sys_raw(long n, long a, long b, long c, long d, long e) { return sys(n, a, b, c, d, e); }

static u8* read_file(const char* path, u32* size) {
    long fd = sys(5, (long)path, 0, 0, 0, 0);
    if (fd < 0) die("open dll");
    u32 cap = 4u << 20;
    u8* buf = (u8*)sys(192, 0, cap, 3, 0x22, -1);
    u32 n = 0;
    for (;;) {
        long r = sys(3, fd, (long)(buf + n), cap - n, 0, 0);
        if (r <= 0) break;
        n += r;
    }
    sys(6, fd, 0, 0, 0, 0);
    *size = n;
    return buf;
}
static u32 rd32(const u8* p) { return *(const u32*)p; }

// --- imports: every KERNEL32 function traps (DllMain is not run; the maths must not need them) ---
static u32 iat_slot[512];
static const char* iat_name[512];
static u32 iat_n;
static void import_trap(void) {
    u32 ret = (u32)__builtin_return_address(0);
    u32 slot = *(u32*)(ret - 4);
    const char* n = "?";
    for (u32 i = 0; i < iat_n; ++i)
        if (iat_slot[i] == slot) n = iat_name[i];
    printf_("import %s called from %x\n", n, ret);
    die("import called");
}
// The CRT's math error path (_87except -> errno through _getptd) needs a per-thread data block.
static u32 ptd[0x100];
static __attribute__((stdcall)) u32 k_GetLastError() { return 0; }
static __attribute__((stdcall)) void k_SetLastError(u32) {}
static __attribute__((stdcall)) void* k_TlsGetValue(u32) { return ptd; }
static u32 base;
static u32 exports[64];
static const char* export_names[64];
static u32 nexports;
static void load_pe(const char* path) {
    u32 size;
    u8* f = read_file(path, &size);
    u32 pe = rd32(f + 0x3c);
    u32 nsec = *(uint16_t*)(f + pe + 6), optsz = *(uint16_t*)(f + pe + 20);
    u8* opt = f + pe + 24;
    base = rd32(opt + 28);
    u32 image = rd32(opt + 56), hdrs = rd32(opt + 60);
    u8* m = (u8*)sys(192, (long)base, image, 7, 0x32, -1);
    if ((u32)m != base) die("map image");
    memcpy(m, f, hdrs);
    u8* sec = opt + optsz;
    for (u32 i = 0; i < nsec; ++i, sec += 40) {
        u32 va = rd32(sec + 12), rawsz = rd32(sec + 16), raw = rd32(sec + 20), vsz = rd32(sec + 8);
        memcpy(m + va, f + raw, rawsz < vsz || !vsz ? rawsz : vsz);
    }
    u32 imp = rd32(opt + 96 + 8);
    for (u8* d = m + imp; rd32(d + 12); d += 20) {
        u32* iat = (u32*)(m + rd32(d + 16));
        u32* names = (u32*)(m + (rd32(d) ? rd32(d) : rd32(d + 16)));
        for (; *iat; ++iat, ++names) {
            const char* n = (*names & 0x80000000) ? "(ordinal)" : (const char*)(m + *names + 2);
            if (iat_n < 512) {
                iat_slot[iat_n] = (u32)iat;
                iat_name[iat_n++] = n;
            }
            if (!strcmp(n, "GetLastError")) *iat = (u32)k_GetLastError;
            else if (!strcmp(n, "SetLastError")) *iat = (u32)k_SetLastError;
            else if (!strcmp(n, "TlsGetValue")) *iat = (u32)k_TlsGetValue;
            else *iat = (u32)import_trap;
        }
    }
    u32 exp = rd32(opt + 96);
    u8* e = m + exp;
    u32 n = rd32(e + 24), eat = rd32(e + 28), nt = rd32(e + 32), ot = rd32(e + 36);
    for (u32 i = 0; i < n && i < 64; ++i) {
        export_names[i] = (const char*)(m + rd32(m + nt + 4 * i));
        exports[i] = base + rd32(m + eat + 4 * *(uint16_t*)(m + ot + 2 * i));
    }
    nexports = n;
}
static u32 find_export(const char* name) {
    for (u32 i = 0; i < nexports; ++i)
        if (!strcmp(export_names[i], name)) return exports[i];
    printf_("export %s\n", name);
    die("export not found");
    return 0;
}
// fs:0 -> a TIB for the SEH frame of vertexf_check_cpu
struct user_desc {
    u32 entry, base, limit, flags;
};
static u32 tib[0x400];
static void setup_fs(void) {
    tib[0] = 0xffffffff;
    tib[1] = 0xc0000000;
    tib[6] = (u32)tib;
    user_desc d = {0xffffffff, (u32)tib, 0xfffff, 1 | (1 << 4) | (1 << 6)};
    if (sys(243, (long)&d, 0, 0, 0, 0) < 0) die("set_thread_area");
    uint16_t sel = (uint16_t)((d.entry << 3) | 3);
    __asm__ volatile("mov %0, %%fs" ::"r"(sel));
}

// --- x87 control ---
static void set_cw(uint16_t cw) {
    __asm__ volatile("fninit\n fldcw %0" ::"m"(cw));
}
static uint16_t get_cw() {
    uint16_t cw;
    __asm__ volatile("fnstcw %0" : "=m"(cw));
    return cw;
}

// --- random inputs ---
static u64 rng_state = 0x9e3779b97f4a7c15ull;
static u32 rnd() {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 7;
    rng_state ^= rng_state << 17;
    return (u32)(rng_state >> 32);
}
static float from_bits(u32 u) {
    float f;
    memcpy(&f, &u, 4);
    return f;
}
[[maybe_unused]] static u32 fbits(float f) {
    u32 u;
    memcpy(&u, &f, 4);
    return u;
}
static const u32 kSpecials[] = {
    0x00000000, 0x80000000, 0x3f800000, 0xbf800000, 0x00000001, 0x807fffff, 0x00800000, 0x7f7fffff, 0xff7fffff,
    0x7f800000, 0xff800000, 0x7fc00000, 0xffc00000, 0x7f800001, 0xffa00005, 0x7fc12345, 0x1e3ce508 /*1e-20*/,
    0x60ad78ec /*1e20*/, 0x5f8ac723 /*2e19*/, 0x5f0ac723, 0x2edbe6ff, 0x2edbe700, 0x2edbe6fe, 0x3727c5ac /*1e-5*/,
    0x40000000, 0x3effffff, 0x5f800000 /*2^64*/, 0x1f800000 /*2^-64*/, 0x0c000000, 0x73000000,
};
static int mode_mix;  // 0 mixed, 1 nice only, 2 wide finite
static float nice() {
    static const float ranges[] = {1.0f, 100.0f, 10000.0f, 0.01f};
    const float r = ranges[rnd() & 3];
    return ((float)(rnd() >> 8) / 16777216.0f * 2.0f - 1.0f) * r;
}
static float wide_finite() {  // any finite magnitude, random sign
    u32 e = rnd() % 255, m = rnd() & 0x7fffff, s = rnd() & 0x80000000u;
    return from_bits(s | (e << 23) | m);
}
static float rf() {
    if (mode_mix == 1) return nice();
    if (mode_mix == 2) return wide_finite();
    u32 r = rnd() % 100;
    if (r < 55) return nice();
    if (r < 70) return wide_finite();
    if (r < 80) return from_bits(kSpecials[rnd() % (sizeof kSpecials / 4)]);
    if (r < 85) return from_bits(rnd());
    if (r < 92) return from_bits((rnd() & 0x80000000u) | (rnd() % 0x01000000u));  // denormal / tiny
    return from_bits((rnd() & 0x80000000u) | (0x5e000000u + rnd() % 0x21800000u));  // huge (>~2^61)
}
static void rvec(Vec3* v) {
    v->x = rf();
    v->y = rf();
    v->z = rf();
}
static void rmat(Matrix* m) {
    u32 kind = rnd() % 10;
    for (int i = 0; i < 16; ++i) m->m[i] = rf();
    if (kind == 0) {  // affine
        m->m[3] = m->m[7] = m->m[11] = 0.0f;
        m->m[15] = 1.0f;
    } else if (kind == 1) {  // singular: duplicate row
        for (int i = 0; i < 4; ++i) m->m[4 + i] = m->m[i];
    } else if (kind == 2) {  // scaled
        const float s = rf();
        for (int i = 0; i < 16; ++i) m->m[i] *= s;
    }
}

// --- comparison bookkeeping ---
struct Stat {
    const char* name;
    u32 runs, bad;
};
static Stat stats[64];
static u32 nstats;
static Stat* stat(const char* name) {
    for (u32 i = 0; i < nstats; ++i)
        if (stats[i].name == name) return &stats[i];
    stats[nstats] = {name, 0, 0};
    return &stats[nstats++];
}
static void dump(const char* what, const void* p, u32 n) {
    printf_("    %s:", what);
    for (u32 i = 0; i < n / 4; ++i) printf_(" %08x", ((const u32*)p)[i]);
    printf_("\n");
}
static bool check(const char* name, const void* theirs, const void* mine, u32 n, const void* in1, u32 n1,
                  const void* in2 = nullptr, u32 n2 = 0, const void* in3 = nullptr, u32 n3 = 0) {
    Stat* s = stat(name);
    ++s->runs;
    if (!memcmp(theirs, mine, n)) return true;
    if (++s->bad <= 6) {
        printf_("MISMATCH %s (cw %x)\n", name, get_cw());
        dump("in1", in1, n1);
        if (in2) dump("in2", in2, n2);
        if (in3) dump("in3", in3, n3);
        dump("dll", theirs, n);
        dump("ours", mine, n);
    }
    return false;
}

// --- DLL entry points ---
#define CD __attribute__((cdecl))
#define TC __attribute__((thiscall))
struct Dll {
    Vec3*(CD* vector_transform1)(Vec3*, const Matrix*);
    Matrix*(CD* multiply_translation)(Matrix*, const Vec3*);
    int(CD* check_cpu)();
    double(CD* length_sq)(const Vec3*);
    double(CD* length)(const Vec3*);
    Matrix*(CD* multiply2)(Matrix*, const Matrix*);
    Vec3*(CD* cross)(Vec3*, const Vec3*, const Vec3*);
    Matrix*(CD* inverse1)(Matrix*);
    void(TC* sv_deinit)(void*);
    void(TC* sv_init)(void*);
    int(TC* sv_add_bone)(void*, int, const Vec3*, const Vec3*, float);
    void(TC* sv_normalize)(void*);
    void(CD* skin_deform)(int, Vec3*, u32, Vec3*, u32, const SkinVertex*, u32, const Matrix*, u32);
    Matrix*(CD* transpose1)(Matrix*);
    Matrix*(CD* transpose2)(Matrix*, const Matrix*);
    Matrix*(CD* multiply3)(Matrix*, const Matrix*, const Matrix*);
    Vec3*(CD* vector_transform2)(Vec3*, const Vec3*, const Matrix*);
    Vec3*(CD* point_transform1)(Vec3*, const Matrix*);
    Vec3*(CD* point_transform2)(Vec3*, const Vec3*, const Matrix*);
    Vec3*(CD* normalize2)(Vec3*, const Vec3*);
    Matrix*(CD* inverse2)(Matrix*, const Matrix*);
    Vec3*(CD* normalize1)(Vec3*);
    double(CD* dot)(const Vec3*, const Vec3*);
} dll;
template <class F>
static void bind(F& f, const char* name) {
    f = (F)find_export(name);
}

// Each test runs the DLL and ours on identical copies of every buffer (aliasing reproduced by
// building the same layout in two scratch areas).
struct Area {
    alignas(16) u8 b[4096];
};
static Area A, B;

static void test_vectors(v3d::X87 x) {
    Vec3 a, b, ra, rb;
    Matrix m;
    rvec(&a);
    rvec(&b);
    rmat(&m);
    {  // length_sq / length / dot: st0
        double t = dll.length_sq(&a), o = v3d::length_sq(&a, x);
        check("length_sq", &t, &o, 8, &a, 12);
        t = dll.length(&a);
        o = v3d::length(&a, x);
        check("length", &t, &o, 8, &a, 12);
        t = dll.dot(&a, &b);
        o = v3d::dot_product(&a, &b, x);
        check("dot_product", &t, &o, 8, &a, 12, &b, 12);
    }
    {  // cross: separate, out = a, out = b
        void* r1 = dll.cross(&ra, &a, &b);
        void* r2 = v3d::cross_product(&rb, &a, &b);
        check("cross_product", &ra, &rb, 12, &a, 12, &b, 12);
        if (r1 != &ra || r2 != &rb) die("cross return");
        Vec3 a1 = a, a2 = a;
        dll.cross(&a1, &a1, &b);
        v3d::cross_product(&a2, &a2, &b);
        check("cross_product(out=a)", &a1, &a2, 12, &a, 12, &b, 12);
        Vec3 b1 = b, b2 = b;
        dll.cross(&b1, &a, &b1);
        v3d::cross_product(&b2, &a, &b2);
        check("cross_product(out=b)", &b1, &b2, 12, &a, 12, &b, 12);
    }
    {  // transforms
        dll.point_transform2(&ra, &a, &m);
        v3d::point_transform(&rb, &a, &m);
        check("point_transform(out, v, m)", &ra, &rb, 12, &a, 12, &m, 64);
        ra = a;
        rb = a;
        dll.point_transform1(&ra, &m);
        v3d::point_transform(&rb, &m);
        check("point_transform(v, m)", &ra, &rb, 12, &a, 12, &m, 64);
        ra = a;
        rb = a;
        dll.point_transform2(&ra, &ra, &m);
        v3d::point_transform(&rb, &rb, &m);
        check("point_transform(out=v)", &ra, &rb, 12, &a, 12, &m, 64);
        dll.vector_transform2(&ra, &a, &m);
        v3d::vector_transform(&rb, &a, &m);
        check("vector_transform(out, v, m)", &ra, &rb, 12, &a, 12, &m, 64);
        ra = a;
        rb = a;
        dll.vector_transform1(&ra, &m);
        v3d::vector_transform(&rb, &m);
        check("vector_transform(v, m)", &ra, &rb, 12, &a, 12, &m, 64);
    }
    {  // normalize: random vectors and vectors near the 1e-10 threshold
        Vec3 v = a;
        if (rnd() % 4 == 0) {
            const float s = from_bits(0x36000000u + rnd() % 0x02000000u);  // ~2e-6 .. 3e-5
            v.x = s * nice();
            v.y = s * nice();
            v.z = s * nice();
        }
        ra = v;
        rb = v;
        dll.normalize1(&ra);
        v3d::vector_normalize(&rb);
        check("vector_normalize(v)", &ra, &rb, 12, &v, 12);
        memset(&ra, 0x5a, 12);
        memset(&rb, 0x5a, 12);
        dll.normalize2(&ra, &v);
        v3d::vector_normalize(&rb, &v);
        check("vector_normalize(out, v)", &ra, &rb, 12, &v, 12);
        ra = v;
        rb = v;
        dll.normalize2(&ra, &ra);
        v3d::vector_normalize(&rb, &rb);
        check("vector_normalize(out=v)", &ra, &rb, 12, &v, 12);
    }
    {  // matrix_multiply_translation
        Matrix m1 = m, m2 = m;
        dll.multiply_translation(&m1, &a);
        v3d::matrix_multiply_translation(&m2, &a, x);
        check("matrix_multiply_translation", &m1, &m2, 64, &m, 64, &a, 12);
    }
}

static void test_matrices(v3d::X87 x) {
    Matrix a, b, r1, r2;
    rmat(&a);
    rmat(&b);
    dll.multiply3(&r1, &a, &b);
    v3d::matrix_multiply(&r2, &a, &b);
    check("matrix_multiply(out, a, b)", &r1, &r2, 64, &a, 64, &b, 64);
    r1 = a;
    r2 = a;
    dll.multiply2(&r1, &b);
    v3d::matrix_multiply(&r2, &b);
    check("matrix_multiply(a, b)", &r1, &r2, 64, &a, 64, &b, 64);
    r1 = b;
    r2 = b;
    dll.multiply3(&r1, &a, &r1);
    v3d::matrix_multiply(&r2, &a, &r2);
    check("matrix_multiply(out=b)", &r1, &r2, 64, &a, 64, &b, 64);
    r1 = a;
    r2 = a;
    dll.multiply3(&r1, &r1, &r1);
    v3d::matrix_multiply(&r2, &r2, &r2);
    check("matrix_multiply(out=a=b)", &r1, &r2, 64, &a, 64);
    r1 = a;
    r2 = a;
    dll.transpose1(&r1);
    v3d::matrix_transpose(&r2);
    check("matrix_transpose(m)", &r1, &r2, 64, &a, 64);
    dll.transpose2(&r1, &a);
    v3d::matrix_transpose(&r2, &a);
    check("matrix_transpose(out, in)", &r1, &r2, 64, &a, 64);
    r1 = a;
    r2 = a;
    dll.transpose2(&r1, &r1);
    v3d::matrix_transpose(&r2, &r2);
    check("matrix_transpose(out=in)", &r1, &r2, 64, &a, 64);
    memset(&r1, 0x77, 64);
    memset(&r2, 0x77, 64);
    void* p1 = dll.inverse2(&r1, &a);
    void* p2 = v3d::matrix_inverse(&r2, &a, x);
    check("matrix_inverse(out, m)", &r1, &r2, 64, &a, 64);
    if (((u32*)&r1)[0] == 0xccccccccu && ((u32*)&r1)[15] == 0xccccccccu) ++stat("matrix_inverse singular (coverage)")->runs;
    if (p1 != &r1 || p2 != &r2) die("inverse return");
    r1 = a;
    r2 = a;
    dll.inverse1(&r1);
    v3d::matrix_inverse(&r2, x);
    check("matrix_inverse(m)", &r1, &r2, 64, &a, 64);
}

static void fill_sv(SkinVertex* sv, u32 nbones) {
    memset(sv, 0, sizeof *sv);
    u32 k = rnd() % 8;
    sv->count = k < 5 ? 1 : (k == 5 ? 2 : (k == 6 ? 3 : 4));
    for (int i = 0; i < 4; ++i) {
        sv->bones[i].bone = (int32_t)(rnd() % nbones);
        rvec(&sv->bones[i].pos);
        rvec(&sv->bones[i].normal);
        sv->bones[i].weight = rnd() % 4 ? (float)(rnd() >> 8) / 16777216.0f : rf();
    }
}

static void test_skin(v3d::X87 x) {
    // skin_deform: n vertices, strided outputs, a bone array with a stride
    const u32 nbones = 1 + rnd() % 6, bone_stride = rnd() % 2 ? 64 : 80;
    static Matrix bone_mem[16];
    static u8 bones_raw[16 * 80];
    for (u32 i = 0; i < nbones; ++i) {
        rmat(&bone_mem[i]);
        memcpy(bones_raw + i * bone_stride, &bone_mem[i], 64);
    }
    const int n = 1 + (int)(rnd() % 8);
    static SkinVertex svs[8];
    for (int i = 0; i < n; ++i) fill_sv(&svs[i], nbones);
    const u32 ps = rnd() % 2 ? 12 : 32, ns = rnd() % 2 ? 12 : 32;
    memset(A.b, 0xcd, sizeof A.b);
    memset(B.b, 0xcd, sizeof B.b);
    dll.skin_deform(n, (Vec3*)A.b, ps, (Vec3*)(A.b + 2048), ns, svs, sizeof(SkinVertex), (const Matrix*)bones_raw,
                    bone_stride);
    v3d::skin_deform(n, (Vec3*)B.b, ps, (Vec3*)(B.b + 2048), ns, svs, sizeof(SkinVertex), (const Matrix*)bones_raw,
                     bone_stride);
    check("skin_deform", A.b, B.b, sizeof A.b, svs, sizeof(SkinVertex));

    // SkinVertex: init, add_bone sequence, normalize, deinit
    SkinVertex s1, s2;
    memset(&s1, 0xcc, sizeof s1);
    memset(&s2, 0xcc, sizeof s2);
    dll.sv_init(&s1);
    v3d::skin_init(&s2);
    check("SkinVertex::init", &s1, &s2, sizeof s1, &s1, 4);
    const int adds = 1 + (int)(rnd() % 7);
    for (int i = 0; i < adds; ++i) {
        Vec3 p, nn;
        rvec(&p);
        rvec(&nn);
        float w = rnd() % 3 ? (float)(rnd() % 8) / 8.0f : rf();  // ties are common
        const int bone = (int)(rnd() % 40);
        const SkinVertex before = s1;
        int r1 = dll.sv_add_bone(&s1, bone, &p, &nn, w);
        int r2 = v3d::skin_add_bone(&s2, bone, &p, &nn, w);
        check("SkinVertex::add_bone", &s1, &s2, sizeof s1, &before, sizeof before, &w, 4);
        check("SkinVertex::add_bone ret", &r1, &r2, 4, &before, sizeof before, &w, 4);
    }
    if (rnd() % 4 == 0) {  // arbitrary weights (zero / negative / NaN sums)
        for (int i = 0; i < 4; ++i) s1.bones[i].weight = s2.bones[i].weight = rnd() % 2 ? rf() : 0.0f;
    }
    const SkinVertex before = s1;
    dll.sv_normalize(&s1);
    v3d::skin_normalize(&s2, x);
    check("SkinVertex::normalize", &s1, &s2, sizeof s1, &before, sizeof before);
    dll.sv_deinit(&s1);
    v3d::skin_deinit(&s2);
    check("SkinVertex::deinit", &s1, &s2, sizeof s1, &before, 4);
}

static u32 parse(const char* s) {
    u32 v = 0;
    for (; *s; ++s) v = v * 10 + (u32)(*s - '0');
    return v;
}

extern "C" int main(int argc, char** argv) {
    if (argc < 2) die("usage: v3d_run v3d_87.dll [iterations] [seed]");
    const u32 iters = argc > 2 ? parse(argv[2]) : 200000;
    if (argc > 3) rng_state ^= (u64)parse(argv[3]) * 0x2545f4914f6cdd1dull;
    setup_fs();
    load_pe(argv[1]);
    bind(dll.vector_transform1, "?vector_transform@@YAAAU_D3DVECTOR@@AAU1@AAU_D3DMATRIX@@@Z");
    bind(dll.multiply_translation, "?matrix_multiply_translation@@YAAAU_D3DMATRIX@@AAU1@AAU_D3DVECTOR@@@Z");
    bind(dll.check_cpu, "vertexf_check_cpu");
    bind(dll.length_sq, "?length_sq@@YAMABU_D3DVECTOR@@@Z");
    bind(dll.length, "?length@@YAMABU_D3DVECTOR@@@Z");
    bind(dll.multiply2, "?matrix_multiply@@YAAAU_D3DMATRIX@@AAU1@0@Z");
    bind(dll.cross, "?cross_product@@YAAAU_D3DVECTOR@@AAU1@ABU1@1@Z");
    bind(dll.inverse1, "?matrix_inverse@@YAAAU_D3DMATRIX@@AAU1@@Z");
    bind(dll.sv_deinit, "?deinit@SkinVertex@@QAEXXZ");
    bind(dll.sv_init, "?init@SkinVertex@@QAEXXZ");
    bind(dll.sv_add_bone, "?add_bone@SkinVertex@@QAEHHAAU_D3DVECTOR@@0M@Z");
    bind(dll.sv_normalize, "?normalize@SkinVertex@@QAEXXZ");
    bind(dll.skin_deform, "?skin_deform@@YAXHPAU_D3DVECTOR@@I0IPAUSkinVertex@@IPAU_D3DMATRIX@@I@Z");
    bind(dll.transpose1, "?matrix_transpose@@YAAAU_D3DMATRIX@@AAU1@@Z");
    bind(dll.transpose2, "?matrix_transpose@@YAAAU_D3DMATRIX@@AAU1@0@Z");
    bind(dll.multiply3, "?matrix_multiply@@YAAAU_D3DMATRIX@@AAU1@00@Z");
    bind(dll.vector_transform2, "?vector_transform@@YAAAU_D3DVECTOR@@AAU1@0AAU_D3DMATRIX@@@Z");
    bind(dll.point_transform1, "?point_transform@@YAAAU_D3DVECTOR@@AAU1@AAU_D3DMATRIX@@@Z");
    bind(dll.point_transform2, "?point_transform@@YAAAU_D3DVECTOR@@AAU1@0AAU_D3DMATRIX@@@Z");
    bind(dll.normalize2, "?vector_normalize@@YAAAU_D3DVECTOR@@AAU1@0@Z");
    bind(dll.inverse2, "?matrix_inverse@@YAAAU_D3DMATRIX@@AAU1@0@Z");
    bind(dll.normalize1, "?vector_normalize@@YAAAU_D3DVECTOR@@AAU1@@Z");
    bind(dll.dot, "?dot_product@@YAMABU_D3DVECTOR@@0@Z");

    {
        int t = dll.check_cpu(), o = v3d::vertexf_check_cpu();
        Vec3 dummy = {0, 0, 0};
        check("vertexf_check_cpu", &t, &o, 4, &dummy, 4);
        printf_("vertexf_check_cpu: dll %d ours %d\n", t, o);
    }
    static const uint16_t kCw[] = {0x007f, 0x027f};  // PC24 (Direct3D 8), PC53 (CRT default)
    static const char* kMix[] = {"mixed", "nice", "wide-finite"};
    for (uint16_t cw : kCw) {
        for (int mix = 0; mix < 3; ++mix) {
            mode_mix = mix;
            nstats = 0;
            set_cw(cw);
            v3d::X87 x;
            x.single = (cw & 0x300) == 0;
            for (u32 i = 0; i < iters; ++i) {
                test_vectors(x);
                test_matrices(x);
                if (i % 4 == 0) test_skin(x);
                if (get_cw() != cw) {
                    printf_("control word changed to %x\n", get_cw());
                    set_cw(cw);
                }
            }
            u32 runs = 0, bad = 0;
            printf_("== cw %04x, %s inputs\n", cw, kMix[mix]);
            for (u32 i = 0; i < nstats; ++i) {
                printf_("  %s: %u runs, %u mismatches\n", stats[i].name, stats[i].runs, stats[i].bad);
                runs += stats[i].runs;
                bad += stats[i].bad;
            }
            printf_("  total %u comparisons, %u mismatches\n", runs, bad);
            rt_flush();
        }
    }
    rt_flush();
    return 0;
}

__asm__(
    ".globl _start\n_start:\n xor %ebp,%ebp\n mov (%esp),%eax\n lea 4(%esp),%ecx\n and $-16,%esp\n sub $8,%esp\n push "
    "%ecx\n push %eax\n call main\n mov %eax,%ebx\n mov $1,%eax\n int $0x80\n");
