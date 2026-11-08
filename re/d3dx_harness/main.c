// D3DX8 texture-loader harness: maps joan.exe at its image base (freestanding i386 Linux) and runs
// D3DXCreateTextureFromFileInMemoryEx (0x00765258) against a fake IDirect3DDevice8, then dumps the
// created texture levels. Ground truth for src/render/image.cpp.
//
// usage: d3dx_run <joan.exe> <image file> <out dump> [key=value ...]
//   pc=24|53            x87 precision control (default 24, what Direct3D 8 sets)
//   caps=0x...          D3DCAPS8::TextureCaps (default 0x4000 = MIPMAP)
//   maxw= maxh= aspect= MaxTextureWidth / Height / AspectRatio (default 4096 4096 0)
//   w= h= mips= usage= fmt= pool= filter= mipfilter= colorkey=   D3DX arguments (hex or dec)
//   fmts=gl|nodxt|no8888x|all                                     formats CheckDeviceFormat accepts
//   fill=0x..           initial byte of fresh texture memory (default 0xcd)
#include "rt.h"
void rt_flush(void);

static u8* read_file(const char* path, u32* size) {
    long fd = sys_open(path, 0);
    if (fd < 0) { printf_("open %s failed\n", path); die("open"); }
    u32 cap = 256u << 20; u8* buf = sys_mmap(0, cap, 3, 0x22); if ((long)buf < 0 && (long)buf > -4096) die("mmap file"); u32 n = 0;
    for (;;) { long r = sys_read(fd, buf + n, cap - n); if (r <= 0) break; n += r; if (n == cap) die("file too big"); }
    sys_close(fd); *size = n; return buf;
}
static u32 rd32(const u8* p) { return *(const u32*)p; }

// --- heap: bump allocator ---
static u8* arena; static u32 arena_used, arena_size;
static void* halloc(u32 n) {
    if (!arena) { arena_size = 1800u << 20; arena = sys_mmap(0, arena_size, 3, 0x22); if ((long)arena < 0 && (long)arena > -4096) die("mmap arena"); }
    u32 need = (n + 8 + 15) & ~15u;
    if (arena_used + need > arena_size) die("arena full");
    u8* p = arena + arena_used; arena_used += need;
    *(u32*)p = n; p += 8;
    memset(p, 0xa5, n);  // like a debug heap: never zero, so reads of uninitialised memory show up
    return p;
}
static void* game_new(u32 n) { return halloc(n); }
static void game_delete(void* p) { (void)p; }
static u32 iat_slot[4096]; static const char* iat_name[4096]; static u32 iat_n;
static void import_trap(void) {
    u32 ret = (u32)__builtin_return_address(0); u32 slot = *(u32*)(ret - 4);
    const char* n = "?";
    for (u32 i = 0; i < iat_n; ++i) if (iat_slot[i] == slot) n = iat_name[i];
    printf_("import %s called from %x\n", n, ret);
    u32* fp = (u32*)__builtin_frame_address(0);
    for (int i = 0; i < 12 && fp; ++i) { printf_("  frame ret %x\n", fp[1]); fp = (u32*)fp[0]; }
    die("import called");
}
static STDCALL void cs_nop(void* c) { (void)c; }

static u32 base;
static void patch_jmp(u32 at, void* to) {
    u8* p = (u8*)at; p[0] = 0xe9; *(u32*)(p + 1) = (u32)to - (at + 5);
}
static void load_pe(const char* path) {
    u32 size; u8* f = read_file(path, &size);
    u32 pe = rd32(f + 0x3c);
    u32 nsec = *(u16*)(f + pe + 6), optsz = *(u16*)(f + pe + 20);
    u8* opt = f + pe + 24;
    base = rd32(opt + 28); u32 image = rd32(opt + 56), hdrs = rd32(opt + 60);
    u8* m = sys_mmap((void*)base, image, 7, 0x32);  // MAP_FIXED|PRIVATE|ANON
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
            if (iat_n < 4096) { iat_slot[iat_n] = (u32)iat; iat_name[iat_n++] = n; }
            if (!strcmp(n, "InitializeCriticalSection") || !strcmp(n, "EnterCriticalSection") ||
                !strcmp(n, "LeaveCriticalSection") || !strcmp(n, "DeleteCriticalSection"))
                *iat = (u32)cs_nop;
            else
                *iat = (u32)import_trap;
        }
    }
    patch_jmp(0x007b1d35, (void*)game_new);
    patch_jmp(0x007b1d2a, (void*)game_delete);
    patch_jmp(0x007b6c66, (void*)game_new);   // _nh_malloc(size, flag)
    patch_jmp(0x007b4e1d, (void*)game_delete);  // free
}
// --- fs:0 TIB ---
struct user_desc { u32 entry, base, limit; u32 flags; };
static u32 tib[0x400];
static void setup_fs(void) {
    tib[0] = 0xffffffff; tib[1] = 0xc0000000; tib[2] = 0; tib[6] = (u32)tib;
    struct user_desc d = {0xffffffff, (u32)tib, 0xfffff, 0};
    d.flags = 1 | (1 << 4) | (1 << 6);
    long r = sys(243, (long)&d, 0, 0, 0, 0, 0);
    if (r < 0) die("set_thread_area");
    u16 sel = (u16)((d.entry << 3) | 3);
    __asm__ volatile("mov %0, %%fs" ::"r"(sel));
}

// --- configuration ---
static u32 cfg_pc = 24, cfg_caps = 0x4000, cfg_maxw = 4096, cfg_maxh = 4096, cfg_aspect = 0, cfg_fill = 0xcd;
static u32 arg_w = 0xffffffff, arg_h = 0xffffffff, arg_mips = 0, arg_usage = 0, arg_fmt = 0, arg_pool = 1;
static u32 arg_filter = 0xffffffff, arg_mipfilter = 0xffffffff, arg_colorkey = 0;
static const char* cfg_fmts = "gl";

static int fmt_ok(u32 f) {
    static const u32 gl[] = {21, 22, 23, 24, 25, 26, 28, 30, 50, 51, 60, 0x31545844, 0x32545844, 0x33545844, 0x34545844, 0x35545844, 0};
    static const u32 all[] = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 40, 41, 50, 51, 52, 60, 61, 62, 63, 64, 65,
                              0x31545844, 0x32545844, 0x33545844, 0x34545844, 0x35545844, 0};
    const u32* list = !strcmp(cfg_fmts, "all") ? all : gl;
    if (!strcmp(cfg_fmts, "nodxt") && (f >> 24) == 0x31 + 0) return 0;
    if (!strcmp(cfg_fmts, "nodxt") && (f & 0xffffff) == 0x545844) return 0;
    if (!strcmp(cfg_fmts, "no8888x") && f == 22) return 0;
    for (; *list; ++list) if (*list == f) return 1;
    return 0;
}
static int is_dxt(u32 f) { return (f & 0xffffff) == 0x545844; }
static u32 fmt_bytes(u32 f) {
    if (f == 0x31545844) return 8;
    if (is_dxt(f)) return 16;
    switch (f) {
        case 20: return 3;
        case 21: case 22: return 4;
        case 23: case 24: case 25: case 26: case 30: case 51: case 60: case 29: return 2;
        default: return 1;
    }
}

// --- fake texture ---
struct Tex;
struct Surf { void** vtbl; struct Tex* tex; u32 level; };
struct Tex { void** vtbl; u32 refs, w, h, levels, usage, fmt, pool; u8* data[16]; u32 pitch[16], rows[16], lw[16], lh[16]; struct Surf surf[16]; int locked[16]; };
static void trap_generic(void) { die("unexpected interface method"); }
#define STD STDCALL
static STD i32 tex_qi(void* t, void* a, void* b) { (void)t; (void)a; (void)b; die("tex QI"); return 0; }
static STD u32 tex_addref(struct Tex* t) { return ++t->refs; }
static STD u32 tex_release(struct Tex* t) { return --t->refs; }
static STD u32 tex_levelcount(struct Tex* t) { return t->levels; }
static STD i32 tex_leveldesc(struct Tex* t, u32 l, u32* d) {
    if (l >= t->levels) return 0x8876086c;
    d[0] = t->fmt; d[1] = 1; d[2] = t->usage; d[3] = t->pool; d[4] = t->pitch[l] * t->rows[l]; d[5] = 0; d[6] = t->lw[l]; d[7] = t->lh[l];
    return 0;
}
static STD i32 tex_getsurf(struct Tex* t, u32 l, struct Surf** out) {
    if (l >= t->levels) return 0x8876086c;
    ++t->refs; *out = &t->surf[l]; return 0;
}
static u8* lock_ptr(struct Tex* t, u32 l, const i32* rect) {
    u8* p = t->data[l];
    if (rect) {
        u32 x = rect[0], y = rect[1];
        p += is_dxt(t->fmt) ? (y / 4) * t->pitch[l] + (x / 4) * fmt_bytes(t->fmt) : y * t->pitch[l] + x * fmt_bytes(t->fmt);
    }
    return p;
}
static STD i32 tex_lock(struct Tex* t, u32 l, i32* lr, const i32* rect, u32 flags) {
    (void)flags;
    if (l >= t->levels || t->locked[l]) die("bad tex lock");
    t->locked[l] = 1; lr[0] = t->pitch[l]; lr[1] = (i32)lock_ptr(t, l, rect); return 0;
}
static STD i32 tex_unlock(struct Tex* t, u32 l) { if (!t->locked[l]) die("bad unlock"); t->locked[l] = 0; return 0; }
static void* tex_vtbl[19];
static STD u32 surf_addref(struct Surf* s) { return ++s->tex->refs; }
static STD u32 surf_release(struct Surf* s) { return --s->tex->refs; }
static STD i32 surf_desc(struct Surf* s, u32* d) { return tex_leveldesc(s->tex, s->level, d); }
static STD i32 surf_lock(struct Surf* s, i32* lr, const i32* rect, u32 flags) { return tex_lock(s->tex, s->level, lr, rect, flags); }
static STD i32 surf_unlock(struct Surf* s) { return tex_unlock(s->tex, s->level); }
static void* surf_vtbl[11];

// --- fake IDirect3D8 / device ---
static void* d3d_vtbl[16];
static void* d3d_obj[1] = {d3d_vtbl};
static STD u32 d3d_release(void* t) { (void)t; return 1; }
static u32 cdf_calls;
static STD i32 d3d_cdf(void* t, u32 adapter, u32 type, u32 afmt, u32 usage, u32 rtype, u32 fmt) {
    (void)t; ++cdf_calls;
    if (adapter != 0 || type != 1 || afmt != 22) die("CheckDeviceFormat args");
    (void)usage;
    if (rtype != 3) return 0x8876086a;
    return fmt_ok(fmt) ? 0 : 0x8876086a;
}
static void* dev_vtbl[100];
static void* dev_obj[1] = {dev_vtbl};
static STD i32 dev_getd3d(void* t, void** out) { (void)t; *out = d3d_obj; return 0; }
static STD i32 dev_caps(void* t, u32* c) {
    (void)t;
    memset(c, 0, 0xd4);
    c[0] = 1; c[1] = 0;
    c[0x3c / 4] = cfg_caps; c[0x58 / 4] = cfg_maxw; c[0x5c / 4] = cfg_maxh; c[0x60 / 4] = 512; c[0x68 / 4] = cfg_aspect;
    return 0;
}
static STD i32 dev_mode(void* t, u32* m) { (void)t; m[0] = 1024; m[1] = 768; m[2] = 60; m[3] = 22; return 0; }
static struct Tex* created[8]; static u32 ncreated;
static STD i32 dev_create_tex(void* t, u32 w, u32 h, u32 levels, u32 usage, u32 fmt, u32 pool, struct Tex** out) {
    (void)t;
    printf_("CreateTexture %ux%u levels=%u usage=%x fmt=%x pool=%u\n", w, h, levels, usage, fmt, pool);
    if (!fmt_ok(fmt) || !w || !h) return 0x8876086c;
    u32 full = 1; for (u32 d = w > h ? w : h; d > 1; d >>= 1) ++full;
    if (levels == 0 || levels > full) levels = full;
    struct Tex* x = halloc(sizeof *x); memset(x, 0, sizeof *x);
    x->vtbl = tex_vtbl; x->refs = 1; x->w = w; x->h = h; x->levels = levels; x->usage = usage; x->fmt = fmt; x->pool = pool;
    for (u32 l = 0; l < levels; ++l) {
        u32 lw = w >> l, lh = h >> l; if (!lw) lw = 1; if (!lh) lh = 1;
        x->lw[l] = lw; x->lh[l] = lh;
        if (is_dxt(fmt)) { x->pitch[l] = ((lw + 3) / 4) * fmt_bytes(fmt); x->rows[l] = (lh + 3) / 4; }
        else { x->pitch[l] = lw * fmt_bytes(fmt); x->rows[l] = lh; }
        x->data[l] = halloc(x->pitch[l] * x->rows[l]);
        memset(x->data[l], (int)cfg_fill, x->pitch[l] * x->rows[l]);
        x->surf[l].vtbl = surf_vtbl; x->surf[l].tex = x; x->surf[l].level = l;
    }
    if (ncreated < 8) created[ncreated++] = x;
    *out = x;
    return 0;
}
static STD i32 dev_update_tex(void* t, struct Tex* src, struct Tex* dst) {
    (void)t;
    for (u32 l = 0; l < dst->levels && l < src->levels; ++l) memcpy(dst->data[l], src->data[l], dst->pitch[l] * dst->rows[l]);
    return 0;
}

static u32 num(const char* s) {
    u32 v = 0;
    if (s[0] == '-' && s[1] == '1') return 0xffffffff;
    if (s[0] == '0' && s[1] == 'x') { for (s += 2; *s; ++s) v = v * 16 + (*s <= '9' ? *s - '0' : (*s | 32) - 'a' + 10); return v; }
    for (; *s; ++s) v = v * 10 + (*s - '0');
    return v;
}
static int key(const char* a, const char* k, const char** v) {
    u32 n = strlen(k);
    if (memcmp(a, k, n) || a[n] != '=') return 0;
    *v = a + n + 1; return 1;
}

typedef STD i32 (*CreateFn)(void*, const void*, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, void*, void*, struct Tex**);

int main(int argc, char** argv) {
    if (argc < 4) die("usage: d3dx_run joan.exe image out [key=value...]");
    for (int i = 4; i < argc; ++i) {
        const char* v;
        if (key(argv[i], "pc", &v)) cfg_pc = num(v);
        else if (key(argv[i], "caps", &v)) cfg_caps = num(v);
        else if (key(argv[i], "maxw", &v)) cfg_maxw = num(v);
        else if (key(argv[i], "maxh", &v)) cfg_maxh = num(v);
        else if (key(argv[i], "aspect", &v)) cfg_aspect = num(v);
        else if (key(argv[i], "fill", &v)) cfg_fill = num(v);
        else if (key(argv[i], "w", &v)) arg_w = num(v);
        else if (key(argv[i], "h", &v)) arg_h = num(v);
        else if (key(argv[i], "mips", &v)) arg_mips = num(v);
        else if (key(argv[i], "usage", &v)) arg_usage = num(v);
        else if (key(argv[i], "fmt", &v)) arg_fmt = num(v);
        else if (key(argv[i], "pool", &v)) arg_pool = num(v);
        else if (key(argv[i], "filter", &v)) arg_filter = num(v);
        else if (key(argv[i], "mipfilter", &v)) arg_mipfilter = num(v);
        else if (key(argv[i], "colorkey", &v)) arg_colorkey = num(v);
        else if (key(argv[i], "fmts", &v)) cfg_fmts = v;
        else { printf_("bad arg %s\n", argv[i]); die("args"); }
    }
    setup_fs();
    load_pe(argv[1]);
    for (int i = 0; i < 100; ++i) dev_vtbl[i] = (void*)trap_generic;
    for (int i = 0; i < 16; ++i) d3d_vtbl[i] = (void*)trap_generic;
    for (int i = 0; i < 19; ++i) tex_vtbl[i] = (void*)trap_generic;
    for (int i = 0; i < 11; ++i) surf_vtbl[i] = (void*)trap_generic;
    dev_vtbl[6] = (void*)dev_getd3d; dev_vtbl[7] = (void*)dev_caps; dev_vtbl[8] = (void*)dev_mode;
    dev_vtbl[20] = (void*)dev_create_tex; dev_vtbl[30] = (void*)dev_update_tex;
    d3d_vtbl[2] = (void*)d3d_release; d3d_vtbl[10] = (void*)d3d_cdf;
    tex_vtbl[0] = (void*)tex_qi; tex_vtbl[1] = (void*)tex_addref; tex_vtbl[2] = (void*)tex_release;
    tex_vtbl[13] = (void*)tex_levelcount; tex_vtbl[14] = (void*)tex_leveldesc; tex_vtbl[15] = (void*)tex_getsurf;
    tex_vtbl[16] = (void*)tex_lock; tex_vtbl[17] = (void*)tex_unlock;
    surf_vtbl[1] = (void*)surf_addref; surf_vtbl[2] = (void*)surf_release; surf_vtbl[8] = (void*)surf_desc;
    surf_vtbl[9] = (void*)surf_lock; surf_vtbl[10] = (void*)surf_unlock;

    u32 size; u8* data = read_file(argv[2], &size);
    u16 cw = cfg_pc == 53 ? 0x027f : cfg_pc == 64 ? 0x037f : 0x007f;
    __asm__ volatile("fninit\n fldcw %0" ::"m"(cw));
    struct Tex* tex = 0;
    i32 hr = ((CreateFn)0x00765258)(dev_obj, data, size, arg_w, arg_h, arg_mips, arg_usage, arg_fmt, arg_pool, arg_filter,
                                    arg_mipfilter, arg_colorkey, 0, 0, &tex);
    u16 cw_after; __asm__ volatile("fnstcw %0" : "=m"(cw_after));
    if (cw_after != cw) printf_("WARNING: control word changed %x -> %x\n", cw, cw_after);
    long fd = sys(5, (long)argv[3], 0x241, 0644, 0, 0, 0);
    if (fd < 0) die("create out");
    u32 hdr[6] = {0x31445854, (u32)hr, 0, 0, 0, 0};
    if (hr == 0 && tex) { hdr[2] = tex->fmt; hdr[3] = tex->w; hdr[4] = tex->h; hdr[5] = tex->levels; }
    sys_write(fd, hdr, sizeof hdr);
    if (hr == 0 && tex) {
        for (u32 l = 0; l < tex->levels; ++l) {
            u32 lh[4] = {tex->lw[l], tex->lh[l], tex->pitch[l], tex->rows[l]};
            sys_write(fd, lh, sizeof lh);
            sys_write(fd, tex->data[l], tex->pitch[l] * tex->rows[l]);
        }
        if (tex->refs != 1) printf_("WARNING: refs=%u\n", tex->refs);
    }
    sys_close(fd);
    printf_("hr=%x fmt=%x %ux%u levels=%u textures=%u cdf=%u\n", (u32)hr, hdr[2], hdr[3], hdr[4], hdr[5], ncreated, cdf_calls);
    rt_flush();
    return 0;
}
__asm__(".globl _start\n_start:\n xor %ebp,%ebp\n mov (%esp),%eax\n lea 4(%esp),%ecx\n and $-16,%esp\n sub $8,%esp\n push %ecx\n push %eax\n call main\n mov %eax,%ebx\n mov $1,%eax\n int $0x80\n");
