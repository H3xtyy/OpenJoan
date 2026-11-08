// Offline proof of batch S6 (re/a6/SAVES_PLAN.md 4 / 5.1, the non-state files): the generated records of section s6
// (src/save/gen/records_s6.hpp) on the game's real typed structs - Material (mesh / ground caches, Material::write
// 0x005e2a90 / read 0x005e2b70), AnimKey = Track's KeyFrame (ANIM_TMP\*.amt, AnimDef::write_cache 0x005b7810 /
// read_cache 0x005b7930), DbfHeader (Database::write 0x00581250 and the DataBase rewriters) - plus the save preview's
// BITMAPFILEHEADER / BITMAPINFOHEADER chunks (Texture::save_bmp 0x00611e70), against the old raw path through a
// memory file with File's allow_vary_size = 0 semantics (plain bytes: every one of these files is created with 0).
//
// Built twice by build_s6.sh: freestanding i686 Linux (x86 layout, like joan_re.exe) and native x86_64 Linux.
//   i686:   every chunk == the object's raw bytes, the field-wise stream == the raw stream, every load == the raw read
//           (whole object compared, prefilled), SaveCompare silent; --write-golden FILE.
//   x86_64: AnimKey / KeyFrame / DbfHeader are pointer-free: chunk == object bytes there too (the .amt key arrays and
//           the DBF field table stay single raw array writes); Material: put -> get -> put is stable and its texture
//           pointers become 0-iff-null tokens; --check-golden FILE decodes the i686 stream and re-encodes it: byte
//           for byte the same except non-null pointer values (tokens).
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "original/database.hpp"
#include "original/en3d.hpp"
#include "original/gen/anim_key.hpp"
#include "original/gen/key_frame.hpp"
#include "save/gen/records_s6.hpp"
#include "save/io.hpp"

namespace jo = joan::orig;
namespace sv = joan::save;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

// The game code relies on these on every target (animdef.cpp, db.cpp, database.cpp).
static_assert(sizeof(jo::AnimKey) == 0x20 && sizeof(jo::KeyFrame) == 0x20);
static_assert(offsetof(jo::KeyFrame, rotation) == offsetof(jo::AnimKey, rot));
static_assert(offsetof(jo::KeyFrame, position) == offsetof(jo::AnimKey, pos));
static_assert(sizeof(jo::DbfHeader) == 0x20 && sizeof(jo::DbfFieldDesc) == 0x20);

// ------------------------------------------------------------------------------------------------- platform
#ifdef SAVE_TEST_FREESTANDING
extern "C" std::size_t strlen(const char* s);
namespace {
long sys(long n, long a, long b, long c) {
    long r;
    __asm__ volatile("int $0x80" : "=a"(r) : "a"(n), "b"(a), "c"(b), "d"(c) : "memory");
    return r;
}
void out(const char* s) { sys(4, 1, reinterpret_cast<long>(s), static_cast<long>(strlen(s))); }
bool save_file(const char* path, const void* p, u32 n) {
    const long fd = sys(5, reinterpret_cast<long>(path), 0x241, 0644);  // O_WRONLY | O_CREAT | O_TRUNC
    if (fd < 0) return false;
    long done = 0;
    while (done < static_cast<long>(n)) {
        const long w = sys(4, fd, reinterpret_cast<long>(static_cast<const u8*>(p) + done), static_cast<long>(n) - done);
        if (w <= 0) break;
        done += w;
    }
    sys(6, fd, 0, 0);
    return done == static_cast<long>(n);
}
long load_file(const char* path, void* p, u32 cap) {
    const long fd = sys(5, reinterpret_cast<long>(path), 0, 0);
    if (fd < 0) return -1;
    long got = 0;
    for (;;) {
        const long r = sys(3, fd, reinterpret_cast<long>(static_cast<u8*>(p) + got), static_cast<long>(cap) - got);
        if (r <= 0) break;
        got += r;
    }
    sys(6, fd, 0, 0);
    return got;
}
}  // namespace
#else
#include <cstdio>
namespace {
void out(const char* s) { std::fputs(s, stdout); }
bool save_file(const char* path, const void* p, u32 n) {
    std::FILE* f = std::fopen(path, "wb");
    if (f == nullptr) return false;
    const bool ok = std::fwrite(p, 1, n, f) == n;
    return std::fclose(f) == 0 && ok;
}
long load_file(const char* path, void* p, u32 cap) {
    std::FILE* f = std::fopen(path, "rb");
    if (f == nullptr) return -1;
    const long got = static_cast<long>(std::fread(p, 1, cap, f));
    std::fclose(f);
    return got;
}
}  // namespace
#endif

namespace {

void hex(u32 v) {
    char b[11] = "0x";
    for (int i = 0; i < 8; ++i) b[2 + i] = "0123456789abcdef"[(v >> (28 - 4 * i)) & 15];
    b[10] = 0;
    out(b);
}
void dec(u32 v) {
    char b[12];
    int i = 11;
    b[i] = 0;
    do {
        b[--i] = static_cast<char>('0' + v % 10);
        v /= 10;
    } while (v != 0);
    out(b + i);
}

int g_checks = 0, g_failed = 0;
void check(bool ok, const char* what, u32 a = 0, u32 b = 0) {
    ++g_checks;
    if (ok) return;
    if (++g_failed > 30) return;
    out("FAIL ");
    out(what);
    out(" ");
    hex(a);
    out(" ");
    hex(b);
    out("\n");
}

u32 g_rng = 0x6b8b4567u;
u32 rnd() {
    g_rng ^= g_rng << 13;
    g_rng ^= g_rng >> 17;
    g_rng ^= g_rng << 5;
    return g_rng;
}
void fill(void* p, u32 n) {
    auto* b = static_cast<u8*>(p);
    for (u32 i = 0; i < n; ++i) b[i] = static_cast<u8>(rnd());
}
void put32(void* p, u32 off, u32 v) { std::memcpy(static_cast<u8*>(p) + off, &v, 4); }

// ----------------------------------------------------- memory File, allow_vary_size = 0 (src/ilib/file.cpp)
struct MemFile {
    u8 data[1u << 20];
    u32 len, pos;
};
MemFile g_files[4];

// File::file_write 0x005872e0 with allow_vary_size = 0: the bytes, nothing else.
std::int32_t mf_write(void* o, const void* buf, u32 n) {
    MemFile& f = *static_cast<MemFile*>(o);
    std::memcpy(f.data + f.pos, buf, n);
    f.pos += n;
    if (f.pos > f.len) f.len = f.pos;
    return 1;
}
// File::file_read / FileMem::file_read with allow_vary_size = 0: the bytes there are, the rest zero-filled.
std::int32_t mf_read(void* o, void* buf, u32 n) {
    MemFile& f = *static_cast<MemFile*>(o);
    const u32 avail = f.pos < f.len ? f.len - f.pos : 0;
    const u32 got = n < avail ? n : avail;
    std::memcpy(buf, f.data + f.pos, got);
    f.pos += got;
    if (got < n) std::memset(static_cast<u8*>(buf) + got, 0, n - got);
    return 1;
}
const char* mf_name(void*) { return "mem"; }
sv::Port port(MemFile& f) { return sv::Port{&f, &mf_write, &mf_read, &mf_name}; }
void reset(MemFile& f) { f.len = f.pos = 0; }

// ---------------------------------------------------------------------------------------------- hooks
int g_compared = 0, g_mismatch = 0, g_misuse = 0;
bool fieldwise_on() { return true; }
bool compare_on() { return true; }
bool audit_off() { return false; }
void on_event(const sv::Event& e) {
    if (e.kind == sv::Event::Compared) ++g_compared;
    if (e.kind == sv::Event::Mismatch) ++g_mismatch;
    if (e.kind == sv::Event::Misuse) ++g_misuse;
}
void install() {
    sv::Hooks& h = sv::hooks();
    h.fieldwise = &fieldwise_on;
    h.compare = &compare_on;
    h.audit = &audit_off;
    h.event = &on_event;
}

// The non-null pointer fields of a record may differ between an x86 chunk and its x64 re-encoding (token); all other
// bytes must be equal, and a pointer field null in one is null in the other.
bool same_but_tokens(const sv::Record& r, const u8* a, const u8* b, u32* token_diffs) {
    for (u32 i = 0; i < r.count; ++i) {
        const sv::Field& f = r.fields[i];
        if (f.type != sv::Type::Pointer) {
            if (std::memcmp(a + f.off, b + f.off, f.size) != 0) return false;
            continue;
        }
        for (u32 e = 0; e < f.size; e += 4) {
            u32 x, y;
            std::memcpy(&x, a + f.off + e, 4);
            std::memcpy(&y, b + f.off + e, 4);
            if ((x == 0) != (y == 0)) return false;
            *token_diffs += x != y;
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------- test objects
u8 g_textures[8][0x60];  // stand-ins for Texture objects
void random_material(jo::Material& m) {
    fill(&m, sizeof m);
    for (int s = 0; s < 4; ++s) m.textures[s] = rnd() % 3 == 0 ? nullptr : g_textures[rnd() % 8] + rnd() % 0x40;
}
void random_key(jo::AnimKey& k) {
    fill(&k, sizeof k);
    static const u32 kFloats[] = {0x7f800001u, 0xff800001u, 0x7fc00000u, 0x7f800000u, 0x00000001u, 0x80000000u};
    for (u32 off = 0; off < 0x20; off += 4)
        if (rnd() % 5 == 0) put32(&k, off, kFloats[rnd() % 6]);
}

constexpr int kN = 2000;
jo::Material g_mat[kN];
jo::AnimKey g_key[kN];
jo::DbfHeader g_dbf[kN];

// --------------------------------------------------------------------------------------------- tests
// One record: chunks / stream / loads against the raw path (x86), layout identity (pointer-free records, both),
// put(get(chunk)) == chunk (both).
template <class T, class Put, class Get>
void test_record(const char* name, const sv::Record& r, T* objs, Put put, Get get, bool pointer_free) {
    MemFile& a = g_files[0];
    MemFile& b = g_files[1];
    reset(a);
    reset(b);
    g_compared = g_mismatch = g_misuse = 0;
    int same_bytes = 0;
    for (int i = 0; i < kN; ++i) {
        if constexpr (sv::kX86Layout) mf_write(&a, &objs[i], r.size);  // old: file_write(file, obj, size)
        sv::Out o(port(b), r, &objs[i]);
        put(o, objs[i]);
        if (sv::kX86Layout || pointer_free) {
            check(std::memcmp(o.data(), &objs[i], r.size) == 0, "chunk == object bytes", static_cast<u32>(i), r.size);
            ++same_bytes;
        }
        check(o.end() == 1, "end");
    }
    if constexpr (sv::kX86Layout)
        check(a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0, "stream == raw stream", a.len, b.len);
    check(b.len == static_cast<u32>(kN) * r.size, "stream length", b.len);
    check(g_compared == (sv::kX86Layout ? kN : 0) && g_mismatch == 0 && g_misuse == 0, "SaveCompare silent",
          static_cast<u32>(g_compared), static_cast<u32>(g_mismatch));
    a.pos = b.pos = 0;
    g_compared = 0;
    for (int i = 0; i < kN; ++i) {
        T raw, fw;
        fill(&raw, sizeof raw);
        std::memcpy(&fw, &raw, sizeof fw);
        if constexpr (sv::kX86Layout) mf_read(&a, &raw, r.size);  // old: file_read(file, obj, size)
        sv::In in(port(b), r, &fw);
        get(in, fw);
        check(in.end() == 1, "read result");
        if constexpr (sv::kX86Layout) {
            check(std::memcmp(&raw, &fw, sizeof raw) == 0, "load == raw read", static_cast<u32>(i));
            check(std::memcmp(&fw, &objs[i], sizeof fw) == 0, "round trip", static_cast<u32>(i));
        }
        // put(get(chunk)) == chunk
        sv::Out o(port(g_files[2]), r);
        put(o, fw);
        check(std::memcmp(o.data(), b.data + static_cast<u32>(i) * r.size, r.size) == 0, "put(get(chunk)) == chunk",
              static_cast<u32>(i));
        reset(g_files[2]);
        o.end();
        reset(g_files[2]);
    }
    check(g_compared == (sv::kX86Layout ? kN : 0) && g_mismatch == 0, "load SaveCompare silent",
          static_cast<u32>(g_compared), static_cast<u32>(g_mismatch));
    out(name);
    out(": ");
    dec(kN);
    out(" records written / read");
    if (same_bytes != 0) {
        out(", ");
        dec(static_cast<u32>(same_bytes));
        out(" chunks == object bytes");
    }
    out(sv::kX86Layout ? ", stream == raw\n" : "\n");
}

// Material on x64: the texture pointers are 0-iff-null tokens (Material::read clears them right after the read).
void test_material_tokens() {
    MemFile& f = g_files[0];
    reset(f);
    jo::Material m;
    random_material(m);
    m.textures[0] = nullptr;
    m.textures[1] = g_textures[2];
    sv::Out o(port(f), sv::kMaterial, &m);
    sv::put_Material(o, m);
    u32 t0, t1;
    std::memcpy(&t0, o.data() + 0x24, 4);
    std::memcpy(&t1, o.data() + 0x28, 4);
    check(o.end() == 1, "material end");
    check(t0 == 0 && t1 != 0, "texture null -> 0, non-null -> != 0", t0, t1);
    if constexpr (sv::kX86Layout) check(t1 == static_cast<u32>(reinterpret_cast<std::uintptr_t>(m.textures[1])), "x86: raw value", t1);
    // the bytes after the 4 stages are the record's +0x34.. on every target
    check(std::memcmp(o.data() + 0x34, &m.texture_mode, 0x20) == 0, "material +0x34.. == members", 0);
    out("material: texture pointers 0 iff null, members after the stages at +0x34\n");
}

// Texture::save_bmp 0x00611e70: the two headers as the frame holds them (ebp-0x140 / ebp-0x168) and as the field-wise
// chunks write them.
void test_bmp_headers() {
    for (int t = 0; t < 200; ++t) {
        u8 fh[0xe], ih[0x28];
        fill(fh, sizeof fh);
        fill(ih, sizeof ih);
        const u16 type = 0x4d42;
        std::memcpy(fh, &type, 2);
        auto u16_at = [](const u8* p, u32 off) {
            u16 v;
            std::memcpy(&v, p + off, 2);
            return v;
        };
        auto u32_at = [](const u8* p, u32 off) {
            u32 v;
            std::memcpy(&v, p + off, 4);
            return v;
        };
        MemFile& f = g_files[0];
        reset(f);
        {
            sv::Out o(port(f), 0xeu, "BITMAPFILEHEADER", fh);
            o.u16(u16_at(fh, 0));
            o.u32(u32_at(fh, 2));
            o.u16(u16_at(fh, 6));
            o.u16(u16_at(fh, 8));
            o.u32(u32_at(fh, 0xa));
            check(o.end() == 1, "bmp file header end");
        }
        {
            sv::Out o(port(f), 0x28u, "BITMAPINFOHEADER", ih);
            o.u32(u32_at(ih, 0));
            o.i32(static_cast<i32>(u32_at(ih, 4)));
            o.i32(static_cast<i32>(u32_at(ih, 8)));
            o.u16(u16_at(ih, 0xc));
            o.u16(u16_at(ih, 0xe));
            for (u32 off = 0x10; off < 0x28; off += 4) o.u32(u32_at(ih, off));
            check(o.end() == 1, "bmp info header end");
        }
        check(f.len == 0x36 && std::memcmp(f.data, fh, 0xe) == 0 && std::memcmp(f.data + 0xe, ih, 0x28) == 0,
              "bmp headers == frame bytes", f.len);
    }
    out("bmp: BITMAPFILEHEADER 0xe + BITMAPINFOHEADER 0x28 == the frame bytes (200 random frames)\n");
}

void put_mat(sv::Out& o, const jo::Material& m) { sv::put_Material(o, m); }
void get_mat(sv::In& i, jo::Material& m) { sv::get_Material(i, m); }
void put_key(sv::Out& o, const jo::AnimKey& k) { sv::put_AnimKey(o, k); }
void get_key(sv::In& i, jo::AnimKey& k) { sv::get_AnimKey(i, k); }
void put_dbf(sv::Out& o, const jo::DbfHeader& h) { sv::put_DbfHeader(o, h); }
void get_dbf(sv::In& i, jo::DbfHeader& h) { sv::get_DbfHeader(i, h); }

// The golden stream: the Material / AnimKey / DbfHeader chunks, as written field-wise.
u32 build_golden(MemFile& g) {
    reset(g);
    for (int i = 0; i < kN; ++i) {
        {
            sv::Out o(port(g), sv::kMaterial);
            put_mat(o, g_mat[i]);
            o.end();
        }
        {
            sv::Out o(port(g), sv::kAnimKey);
            put_key(o, g_key[i]);
            o.end();
        }
        sv::Out o(port(g), sv::kDbfHeader);
        put_dbf(o, g_dbf[i]);
        o.end();
    }
    return g.len;
}

void check_golden(const char* path) {
    MemFile& g = g_files[2];
    MemFile& r = g_files[3];
    const long n = load_file(path, g.data, sizeof g.data);
    check(n > 0, "golden file readable");
    if (n <= 0) return;
    g.len = static_cast<u32>(n);
    g.pos = 0;
    reset(r);
    u32 records = 0, token_diffs = 0;
    while (g.pos < g.len) {
        const u32 at = g.pos;
        jo::Material m;
        jo::AnimKey k;
        jo::DbfHeader h;
        {
            sv::In in(port(g), sv::kMaterial);
            get_mat(in, m);
            check(in.end() == 1, "golden material read");
        }
        {
            sv::In in(port(g), sv::kAnimKey);
            get_key(in, k);
            check(in.end() == 1, "golden key read");
        }
        {
            sv::In in(port(g), sv::kDbfHeader);
            get_dbf(in, h);
            check(in.end() == 1, "golden dbf read");
        }
        const u32 at2 = r.pos;
        {
            sv::Out o(port(r), sv::kMaterial);
            put_mat(o, m);
            o.end();
        }
        {
            sv::Out o(port(r), sv::kAnimKey);
            put_key(o, k);
            o.end();
        }
        sv::Out o(port(r), sv::kDbfHeader);
        put_dbf(o, h);
        o.end();
        check(same_but_tokens(sv::kMaterial, g.data + at, r.data + at2, &token_diffs), "golden material re-encoded",
              at);
        check(std::memcmp(g.data + at + 0x54, r.data + at2 + 0x54, 0x40) == 0, "golden key / dbf re-encoded", at);
        records += 3;
    }
    check(r.len == g.len, "golden re-encoded length", r.len, g.len);
    if constexpr (sv::kX86Layout)
        check(std::memcmp(r.data, g.data, g.len) == 0 && token_diffs == 0, "x86: golden re-encoded == golden");
    out("golden: ");
    dec(records);
    out(" records decoded and re-encoded, ");
    dec(g.len);
    out(" bytes identical but ");
    dec(token_diffs);
    out(" non-null texture pointers re-encoded as tokens\n");
}

}  // namespace

int test_main(int argc, char** argv) {
    install();
    out(sv::kX86Layout ? "save test s6, x86 layout (4-byte pointers)\n" : "save test s6, x64 layout (8-byte pointers)\n");
    for (int i = 0; i < kN; ++i) {
        random_material(g_mat[i]);
        random_key(g_key[i]);
        fill(&g_dbf[i], sizeof g_dbf[i]);
    }
    test_record("Material", sv::kMaterial, g_mat, &put_mat, &get_mat, false);
    test_record("AnimKey", sv::kAnimKey, g_key, &put_key, &get_key, true);
    test_record("DbfHeader", sv::kDbfHeader, g_dbf, &put_dbf, &get_dbf, true);
    test_material_tokens();
    test_bmp_headers();
    for (int i = 1; i + 1 < argc; i += 2) {
        if (std::strcmp(argv[i], "--write-golden") == 0) {
            const u32 n = build_golden(g_files[2]);
            check(save_file(argv[i + 1], g_files[2].data, n), "golden written");
            out("golden written: ");
            dec(n);
            out(" bytes\n");
        } else if (std::strcmp(argv[i], "--check-golden") == 0) {
            check_golden(argv[i + 1]);
        }
    }
    out(g_failed == 0 ? "ALL OK, " : "FAILED, ");
    dec(static_cast<u32>(g_checks));
    out(" checks, ");
    dec(static_cast<u32>(g_failed));
    out(" failed\n");
    return g_failed == 0 ? 0 : 1;
}

#ifdef SAVE_TEST_FREESTANDING
extern "C" int cmain(int argc, char** argv) { return test_main(argc, argv); }
__asm__(".globl _start\n_start:\n xor %ebp,%ebp\n mov (%esp),%eax\n lea 4(%esp),%ecx\n and $-16,%esp\n sub $8,%esp\n"
        " push %ecx\n push %eax\n call cmain\n mov %eax,%ebx\n mov $1,%eax\n int $0x80\n");
#else
int main(int argc, char** argv) { return test_main(argc, argv); }
#endif
