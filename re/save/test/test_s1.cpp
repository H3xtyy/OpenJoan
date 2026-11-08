// Offline proof of the S1 records (re/a6/SAVES_PLAN.md 5.1, batch S1: Unit, ProfileHero, Group, UnitStats,
// UpgradeList, UpgradeListEntry, UnitData, ItemStack, InvItem, OrderEntry; src/save/gen/records_s1.hpp) on the game's
// real typed structs, through a memory file with File's vary-size framing (as test_save.cpp, whose S0 checks of the
// serialiser itself are not repeated here).
//
// Built twice by build_s1.sh: freestanding i686 Linux (the x86 layout, like joan_re.exe) and native x86_64 Linux.
//   i686:   per record, chunk == the object's bytes [0, size) (the old raw file_write), stream == the raw stream,
//           load == the raw file_read (the whole object), SaveCompare silent; --write-golden FILE writes the stream.
//   x86_64: put -> get -> put reproduces the chunk; --check-golden FILE decodes the i686 stream and re-encodes it:
//           every byte identical except pointer fields, which keep null / non-null (x64 token = low 32 bits | 1).
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "original/gen/group.hpp"
#include "original/gen/held_item.hpp"
#include "original/gen/inv_item.hpp"
#include "original/gen/order_entry.hpp"
#include "original/gen/unit_data.hpp"
#include "original/gen/unit_stats.hpp"
#include "original/gen/upgrade_list.hpp"
#include "original/gen/upgrade_list_entry.hpp"
#include "original/unit.hpp"
#include "save/gen/records_s1.hpp"
#include "save/io.hpp"

namespace jo = joan::orig;
namespace sv = joan::save;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using uptr = std::uintptr_t;

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
    const long w = sys(4, fd, reinterpret_cast<long>(p), static_cast<long>(n));
    sys(6, fd, 0, 0);
    return w == static_cast<long>(n);
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

u32 g_rng = 0x6c8e9cf5u;
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
u32 get32(const void* p, u32 off) {
    u32 v;
    std::memcpy(&v, static_cast<const u8*>(p) + off, 4);
    return v;
}

// ------------------------------------------------------------------------- memory File (src/ilib/file.cpp)
struct MemFile {
    u8 data[1u << 21];
    u32 len, pos;
};
MemFile g_files[4];

void raw_put(MemFile& f, const void* p, u32 n) {
    std::memcpy(f.data + f.pos, p, n);
    f.pos += n;
    if (f.pos > f.len) f.len = f.pos;
}
u32 raw_get(MemFile& f, void* p, u32 n) {
    const u32 avail = f.pos < f.len ? f.len - f.pos : 0;
    const u32 got = n < avail ? n : avail;
    std::memcpy(p, f.data + f.pos, got);
    f.pos += got;
    return got;
}
// File::file_write 0x005872e0 with allow_vary_size = 1: u16 size (0 if > 0xffff), then the bytes.
std::int32_t mf_write(void* o, const void* buf, u32 n) {
    MemFile& f = *static_cast<MemFile*>(o);
    const u16 prefix = n > 0xffff ? 0 : static_cast<u16>(n);
    raw_put(f, &prefix, 2);
    raw_put(f, buf, n);
    return 1;
}
// File::file_read 0x005873a0 with allow_vary_size = 1.
std::int32_t mf_read(void* o, void* buf, u32 size) {
    MemFile& f = *static_cast<MemFile*>(o);
    u32 read_size = size, rec_size = size;
    u16 prefix = 0;
    if (raw_get(f, &prefix, 2) != 2) prefix = 0;
    if (prefix != 0) read_size = size < prefix ? size : prefix, rec_size = prefix;
    raw_get(f, buf, read_size);
    if (read_size < rec_size) f.pos += rec_size - read_size;
    if (read_size < size) std::memset(static_cast<u8*>(buf) + read_size, 0, size - read_size);
    return 1;
}
const char* mf_name(void*) { return "mem"; }
sv::Port port(MemFile& f) { return sv::Port{&f, &mf_write, &mf_read, &mf_name}; }
void reset(MemFile& f) { f.len = f.pos = 0; }

// ---------------------------------------------------------------------------------------------- hooks
int g_compared = 0, g_mismatch = 0, g_misuse = 0;
bool on() { return true; }
bool off() { return false; }
void on_event(const sv::Event& e) {
    if (e.kind == sv::Event::Compared) ++g_compared;
    if (e.kind == sv::Event::Mismatch) ++g_mismatch;
    if (e.kind == sv::Event::Misuse) ++g_misuse;
}
void install() {
    sv::Hooks& h = sv::hooks();
    h.fieldwise = &on;
    h.compare = &on;
    h.audit = &off;
    h.event = &on_event;  // image_out / image_in: the defaults (S1 records have no Ptr::Image field)
}

// ------------------------------------------------------------------------------------------- records
constexpr int kN = 200;
constexpr u32 kMaxObj = 0x600;  // >= sizeof of every record class on both targets

struct Rec {
    const sv::Record* k;
    u32 obj_size;  // sizeof(T)
    void (*put)(sv::Out&, const void*);
    void (*get)(sv::In&, void*);
};
#define REC(R, T)                                                                                   \
    Rec {                                                                                           \
        &sv::k##R, static_cast<u32>(sizeof(T)),                                                     \
            [](sv::Out& o, const void* p) { sv::put_##R(o, *static_cast<const T*>(p)); },           \
            [](sv::In& i, void* p) { sv::get_##R(i, *static_cast<T*>(p)); }                          \
    }
const Rec kRecs[] = {
    REC(Unit, jo::Unit),           REC(ProfileHero, jo::Unit),           REC(Group, jo::Group),
    REC(UnitStats, jo::UnitStats), REC(UpgradeList, jo::UpgradeList),    REC(UpgradeListEntry, jo::UpgradeListEntry),
    REC(UnitData, jo::UnitData),   REC(ItemStack, jo::HeldItem),         REC(InvItem, jo::InvItem),
    REC(OrderEntry, jo::OrderEntry),
};
constexpr int kRecCount = sizeof kRecs / sizeof kRecs[0];
alignas(8) u8 g_objs[kRecCount][kN][kMaxObj];

// Random bytes; on x86 (chunk offset == object offset, every S1 record starts at 0) some pointer fields null and
// some float fields signalling / quiet NaNs, infinities, denormals.
void random_obj(const Rec& r, void* p) {
    fill(p, r.obj_size);
    if (!sv::kX86Layout) return;
    static const u32 kFloats[] = {0x7f800001u, 0xff800001u, 0x7fc00000u, 0x7f800000u, 0x00000001u, 0x80000000u};
    for (u32 i = 0; i < r.k->count; ++i) {
        const sv::Field& f = r.k->fields[i];
        for (u32 e = 0; e + f.elem <= f.size && f.elem == 4; e += 4) {
            if (f.type == sv::Type::Pointer && rnd() % 3 == 0) put32(p, f.off + e, 0);
            if (f.type == sv::Type::F32 && rnd() % 4 == 0) put32(p, f.off + e, kFloats[rnd() % 6]);
        }
    }
}

u32 pointer_fields(const Rec& r) {
    u32 n = 0;
    for (u32 i = 0; i < r.k->count; ++i) n += r.k->fields[i].type == sv::Type::Pointer;
    return n;
}

// x86: chunk == raw bytes, stream == raw stream, load == raw read. x64: put -> get -> put is stable.
void test_record(int ri) {
    const Rec& r = kRecs[ri];
    const u32 n = r.k->size;
    check(r.obj_size <= kMaxObj && n <= r.obj_size, "record fits", n, r.obj_size);
    MemFile& a = g_files[0];
    MemFile& b = g_files[1];
    reset(a);
    reset(b);
    g_compared = g_mismatch = g_misuse = 0;
    for (int i = 0; i < kN; ++i) {
        void* const obj = g_objs[ri][i];
        random_obj(r, obj);
        mf_write(&a, obj, n);  // old: file_write(file, this, n)
        sv::Out o(port(b), *r.k, sv::kX86Layout ? obj : nullptr);
        r.put(o, obj);
        if (sv::kX86Layout) check(std::memcmp(o.data(), obj, n) == 0, "chunk == object bytes", static_cast<u32>(ri), static_cast<u32>(i));
        if (!sv::kX86Layout) {
            alignas(8) u8 back[kMaxObj];
            std::memset(back, 0, sizeof back);
            u8 c1[sv::kMaxChunk];
            std::memcpy(c1, o.data(), n);
            MemFile& t = g_files[3];
            reset(t);
            mf_write(&t, c1, n);
            t.pos = 0;
            sv::In in(port(t), *r.k);
            r.get(in, back);
            check(in.end() == 1, "x64 get", static_cast<u32>(ri));
            sv::Out o2(port(t), *r.k);
            r.put(o2, back);
            check(std::memcmp(o2.data(), c1, n) == 0, "x64 put(get(put(x))) == put(x)", static_cast<u32>(ri), static_cast<u32>(i));
            check(o2.end() == 1, "x64 re-put end");
        }
        check(o.end() == 1, "end", static_cast<u32>(ri));
    }
    if (sv::kX86Layout) {
        check(a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0, "stream == raw stream", a.len, b.len);
        check(g_compared == kN && g_mismatch == 0, "SaveCompare silent", static_cast<u32>(g_compared), static_cast<u32>(g_mismatch));
        a.pos = b.pos = 0;
        g_compared = 0;
        for (int i = 0; i < kN; ++i) {
            alignas(8) u8 raw[kMaxObj], fw[kMaxObj];
            fill(raw, r.obj_size);
            std::memcpy(fw, raw, r.obj_size);
            mf_read(&a, raw, n);  // old: file_read(file, this, n)
            sv::In in(port(b), *r.k, fw);
            r.get(in, fw);
            check(in.end() == 1, "read result", static_cast<u32>(ri));
            check(std::memcmp(raw, fw, r.obj_size) == 0, "load == raw read (whole object)", static_cast<u32>(ri), static_cast<u32>(i));
            check(std::memcmp(fw, g_objs[ri][i], n) == 0, "round trip", static_cast<u32>(ri), static_cast<u32>(i));
        }
        check(g_compared == kN && g_mismatch == 0, "load SaveCompare silent", static_cast<u32>(g_compared), static_cast<u32>(g_mismatch));
    }
    check(g_misuse == 0, "no misuse", static_cast<u32>(g_misuse));
    out(r.k->name);
    out(": ");
    dec(kN);
    out(" x ");
    hex(n);
    out(" bytes, ");
    dec(r.k->count);
    out(" fields, ");
    dec(pointer_fields(r));
    out(sv::kX86Layout ? " pointers: chunk == raw, load == raw read\n" : " pointers: put/get/put stable\n");
}

// The golden stream: every record's kN chunks, record by record, as written field-wise.
u32 build_golden(MemFile& g) {
    reset(g);
    for (int ri = 0; ri < kRecCount; ++ri) {
        for (int i = 0; i < kN; ++i) {
            sv::Out o(port(g), *kRecs[ri].k);
            kRecs[ri].put(o, g_objs[ri][i]);
            o.end();
        }
    }
    return g.len;
}

// Decode the i686 stream and re-encode it: identical except pointer tokens (x64: null iff 0, else low 32 bits | 1).
void check_golden(const char* path) {
    MemFile& g = g_files[2];
    MemFile& r = g_files[3];
    const long len = load_file(path, g.data, sizeof g.data);
    check(len > 0, "golden file readable");
    if (len <= 0) return;
    g.len = static_cast<u32>(len);
    g.pos = 0;
    reset(r);
    u32 records = 0, ptrs = 0, nulls = 0, token_diffs = 0;
    for (int ri = 0; ri < kRecCount; ++ri) {
        const Rec& rc = kRecs[ri];
        const u32 n = rc.k->size;
        for (int i = 0; i < kN; ++i) {
            const u8* const chunk = g.data + g.pos + 2;
            check(g.pos + 2 + n <= g.len && chunk[-2] == static_cast<u8>(n) && chunk[-1] == static_cast<u8>(n >> 8),
                  "golden chunk header", static_cast<u32>(ri), static_cast<u32>(i));
            alignas(8) u8 obj[kMaxObj];
            std::memset(obj, 0, sizeof obj);
            sv::In in(port(g), *rc.k);
            rc.get(in, obj);
            check(in.end() == 1, "golden read", static_cast<u32>(ri));
            sv::Out o(port(r), *rc.k);
            rc.put(o, obj);
            const u8* const now = o.data();
            for (u32 fi = 0; fi < rc.k->count; ++fi) {
                const sv::Field& f = rc.k->fields[fi];
                if (f.type != sv::Type::Pointer) {
                    check(std::memcmp(now + f.off, chunk + f.off, f.size) == 0, "golden field identical", static_cast<u32>(ri), f.off);
                    continue;
                }
                for (u32 e = 0; e < f.size; e += 4) {
                    const u32 v = get32(chunk, f.off + e), w = get32(now, f.off + e);
                    ++ptrs;
                    nulls += v == 0;
                    token_diffs += v != w;
                    check(sv::kX86Layout ? w == v : (v == 0 ? w == 0 : w == (v | 1u)), "golden pointer null iff null", v, w);
                }
            }
            check(o.end() == 1, "golden re-put");
            ++records;
        }
    }
    check(g.pos == g.len, "golden fully consumed", g.pos, g.len);
    check(nulls > 0 && nulls < ptrs, "golden has null and non-null pointers", nulls, ptrs);
    out("golden: ");
    dec(records);
    out(" records decoded and re-encoded (");
    dec(g.len);
    out(" bytes): non-pointer bytes identical, ");
    dec(ptrs);
    out(" pointer fields (");
    dec(nulls);
    out(" null) null iff null, ");
    dec(token_diffs);
    out(" tokens differ (even x86 values; x64 writes low 32 | 1)\n");
}

}  // namespace

int test_main(int argc, char** argv) {
    install();
    out(sv::kX86Layout ? "save test S1, x86 layout (4-byte pointers)\n" : "save test S1, x64 layout (8-byte pointers)\n");
    for (int ri = 0; ri < kRecCount; ++ri) test_record(ri);
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
