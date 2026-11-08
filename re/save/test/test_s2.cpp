// Offline proof of batch S2 (re/a6/SAVES_PLAN.md 4 / 5.1, map records): the generated records of section s2
// (src/save/gen/records_s2.hpp) on the game's real typed structs (Place, Facility + Building / Siege / Shop /
// FlagFacility tails, ShopStock, RecruitRow, MapObject + its tails, WorldItem, Missile = Order 0x6c), against the old
// raw path (file_write(obj + from, size) / file_read), through a memory file with File's vary-size framing.
//
// Built twice by build_s2.sh: freestanding i686 Linux (x86 layout, like joan_re.exe) and native x86_64 Linux.
//   i686:   every chunk == the object's raw bytes [from, from + size), the field-wise stream == the raw stream, every
//           load == the raw read (whole object compared, prefilled), SaveCompare silent; --write-golden FILE.
//   x86_64: put -> get -> put reproduces the chunk; --check-golden FILE decodes the i686 stream into the x64 structs
//           and re-encodes it: byte for byte the same except pointer fields, which keep only null / non-null (the
//           token, SAVES_PLAN 3.1), as every reader of these records tests them.
//   both:   the empty Order00 tail and the schema-less 0x18 tail: same framing as the raw write.
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "original/gen/building.hpp"
#include "original/gen/facility.hpp"
#include "original/gen/flag_facility.hpp"
#include "original/gen/gate.hpp"
#include "original/gen/map_object.hpp"
#include "original/gen/missile.hpp"
#include "original/gen/order0b.hpp"
#include "original/gen/order0c_a.hpp"
#include "original/gen/order45.hpp"
#include "original/gen/place.hpp"
#include "original/gen/shop.hpp"
#include "original/gen/shop_stock.hpp"
#include "original/gen/siege.hpp"
#include "original/gen/wall.hpp"
#include "original/gen/world_item.hpp"
#include "original/records.hpp"
#include "save/gen/records_s2.hpp"
#include "save/io.hpp"

namespace jo = joan::orig;
namespace sv = joan::save;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

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
void check(bool ok, const char* what, const char* rec = "", u32 a = 0, u32 b = 0) {
    ++g_checks;
    if (ok) return;
    if (++g_failed > 30) return;
    out("FAIL ");
    out(rec);
    out(": ");
    out(what);
    out(" ");
    hex(a);
    out(" ");
    hex(b);
    out("\n");
}

u32 g_rng = 0x2545f491u;
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
    u8 data[1u << 23];
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
bool yes() { return true; }
bool no() { return false; }
void on_event(const sv::Event& e) {
    if (e.kind == sv::Event::Compared) ++g_compared;
    if (e.kind == sv::Event::Mismatch) ++g_mismatch;
    if (e.kind == sv::Event::Misuse) ++g_misuse;
}
void install() {
    sv::Hooks& h = sv::hooks();
    h.fieldwise = &yes;
    h.compare = &yes;
    h.audit = &no;
    h.event = &on_event;
}

// ------------------------------------------------------------------------------------- test objects
// The object storage (the biggest record object is Siege, 0x2f14 bytes on x86).
alignas(16) u8 g_obj[4][0x4000];

// x86: random bytes, with special floats in F32 members and null in a third of the pointer members (chunk offsets
// from the field table). x64: random bytes only (the field table holds x86 offsets).
void random_object(void* obj, u32 size, const sv::Record& r, u32 from) {
    fill(obj, size);
    if constexpr (!sv::kX86Layout) return;
    static const u32 kFloats[] = {0x7f800001u, 0xff800001u, 0x7fc00000u, 0x7f800000u, 0x00000001u, 0x80000000u};
    auto* b = static_cast<u8*>(obj) + from;
    for (u32 k = 0; k < r.count; ++k) {
        const sv::Field& f = r.fields[k];
        for (u32 e = 0; e + 4 <= f.size; e += 4) {
            if (f.type == sv::Type::F32 && rnd() % 3 == 0) put32(b, f.off + e, kFloats[rnd() % 6]);
            if (f.type == sv::Type::Pointer && rnd() % 3 == 0) put32(b, f.off + e, 0);
        }
    }
}

// Two chunks equal except at Pointer fields, which must agree on null / non-null. Returns the pointer fields whose
// token differs from the stored value (x64 re-encoding of x86 values).
bool same_but_tokens(const sv::Record& r, const u8* a, const u8* b, u32* token_diffs) {
    u32 off = 0;
    for (u32 k = 0; k < r.count; ++k) {
        const sv::Field& f = r.fields[k];
        if (std::memcmp(a + off, b + off, f.off - off) != 0) return false;  // (tables tile the chunk)
        if (f.type == sv::Type::Pointer) {
            for (u32 e = 0; e < f.size; e += 4) {
                const u32 x = get32(a, f.off + e), y = get32(b, f.off + e);
                if ((x == 0) != (y == 0)) return false;
                *token_diffs += x != y;
            }
        } else if (std::memcmp(a + f.off, b + f.off, f.size) != 0) {
            return false;
        }
        off = f.off + f.size;
    }
    return std::memcmp(a + off, b + off, r.size - off) == 0;
}

enum class Mode { Local, WriteGolden, CheckGolden };
Mode g_mode = Mode::Local;
MemFile& g_golden = g_files[2];
MemFile& g_regolden = g_files[3];
u32 g_golden_records = 0, g_token_diffs = 0;

// One record: n random objects of class T, chunk = [from, from + r.size) on x86.
template <class T, class Put, class Get>
void run(const sv::Record& r, u32 from, int n, Put put, Get get) {
    static_assert(sizeof(T) <= sizeof g_obj[0]);
    T& obj = *reinterpret_cast<T*>(g_obj[0]);
    T& raw = *reinterpret_cast<T*>(g_obj[1]);
    T& fw = *reinterpret_cast<T*>(g_obj[2]);
    if (g_mode == Mode::CheckGolden) {
        for (int i = 0; i < n; ++i) {
            const u32 at = g_golden.pos;
            {
                sv::In in(port(g_golden), r);
                get(in, obj);
                check(in.end() == 1, "golden read", r.name);
            }
            const u32 at2 = g_regolden.len;
            {
                sv::Out o(port(g_regolden), r);
                put(o, obj);
                check(o.end() == 1, "golden re-encode", r.name);
            }
            check(g_golden.pos - at == r.size + 2 && g_regolden.len - at2 == r.size + 2, "golden framing", r.name, at);
            check(same_but_tokens(r, g_golden.data + at + 2, g_regolden.data + at2 + 2, &g_token_diffs),
                  "golden re-encoded == golden (pointers: null iff null)", r.name, static_cast<u32>(i));
            ++g_golden_records;
        }
        return;
    }
    MemFile& a = g_files[0];
    MemFile& b = g_files[1];
    reset(a);
    reset(b);
    g_compared = g_mismatch = 0;
    const u32 seed = g_rng;
    u32 nulls = 0;
    for (int i = 0; i < n; ++i) {
        random_object(&obj, sizeof(T), r, from);
        const u8* const range = reinterpret_cast<const u8*>(&obj) + from;
        if constexpr (sv::kX86Layout) {
            mf_write(&a, range, r.size);  // old: file_write(file, obj + from, size)
            for (u32 k = 0; k < r.count; ++k)
                if (r.fields[k].type == sv::Type::Pointer) nulls += get32(range, r.fields[k].off) == 0;
        }
        sv::Out o(port(b), r, range);
        put(o, obj);
        if constexpr (sv::kX86Layout)
            check(std::memcmp(o.data(), range, r.size) == 0, "chunk == object bytes", r.name, static_cast<u32>(i));
        if (g_mode == Mode::WriteGolden) {
            sv::Out g(port(g_golden), r);
            put(g, obj);
            g.end();
            ++g_golden_records;
        }
        check(o.end() == 1, "end", r.name);
    }
    if constexpr (sv::kX86Layout) {
        check(a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0, "stream == raw stream", r.name, a.len, b.len);
        check(g_compared == n && g_mismatch == 0, "SaveCompare silent (write)", r.name, static_cast<u32>(g_compared),
              static_cast<u32>(g_mismatch));
        bool has_ptr = false;
        for (u32 k = 0; k < r.count; ++k) has_ptr = has_ptr || r.fields[k].type == sv::Type::Pointer;
        check(!has_ptr || nulls > 0, "test has null pointers", r.name);
    }
    // Reads: x86 against the raw read (whole object, prefilled with the same random bytes); both: the round trip.
    a.pos = b.pos = 0;
    g_compared = 0;
    u32 obj_rng = seed, fill_rng = g_rng ^ 0x9e3779b9u;
    for (int i = 0; i < n; ++i) {
        g_rng = obj_rng;
        random_object(&obj, sizeof(T), r, from);  // the object written above
        obj_rng = g_rng;
        g_rng = fill_rng;
        fill(&raw, sizeof(T));
        fill_rng = g_rng;
        std::memcpy(&fw, &raw, sizeof(T));
        u8* const fw_range = reinterpret_cast<u8*>(&fw) + from;
        if constexpr (sv::kX86Layout) mf_read(&a, reinterpret_cast<u8*>(&raw) + from, r.size);  // old: file_read
        {
            sv::In in(port(b), r, fw_range);
            get(in, fw);
            check(in.end() == 1, "read result", r.name);
        }
        if constexpr (sv::kX86Layout) {
            check(std::memcmp(&raw, &fw, sizeof(T)) == 0, "load == raw read", r.name, static_cast<u32>(i));
            check(std::memcmp(fw_range, reinterpret_cast<const u8*>(&obj) + from, r.size) == 0, "round trip", r.name,
                  static_cast<u32>(i));
        }
        // put(get(chunk)) == chunk (x64: the tokens are stable)
        MemFile& c = g_files[3];
        reset(c);
        sv::Out o(port(c), r);
        put(o, fw);
        check(o.end() == 1, "re-put", r.name);
        check(c.len == r.size + 2 && std::memcmp(c.data, b.data + b.pos - r.size - 2, c.len) == 0, "put(get(x)) == x",
              r.name, static_cast<u32>(i));
    }
    if constexpr (sv::kX86Layout)
        check(g_compared == n && g_mismatch == 0, "SaveCompare silent (read)", r.name, static_cast<u32>(g_compared),
              static_cast<u32>(g_mismatch));
    out(r.name);
    out(": ");
    dec(static_cast<u32>(n));
    out(" x ");
    dec(r.size);
    out(" bytes");
    if constexpr (sv::kX86Layout) {
        out(" == raw (");
        dec(nulls);
        out(" null pointers)");
    }
    out(", round trip ok\n");
}

#define REC(R, C, FROM, N) \
    run<jo::C>(sv::k##R, FROM, N, [](sv::Out& o, const jo::C& r) { sv::put_##R(o, r); }, \
               [](sv::In& i, jo::C& r) { sv::get_##R(i, r); })

void test_records() {
    REC(Place, Place, 0x0, 400);
    REC(FacilityBase, Facility, 0x4, 400);
    REC(FacilityBase, Building, 0x4, 50);  // the base part of a subclass object
    REC(BuildingTail, Building, 0x79, 400);
    REC(SiegeTail, Siege, 0x79, 60);
    REC(ShopTail, Shop, 0x79, 400);
    REC(ShopStock, ShopStock, 0x0, 400);
    REC(FlagFacilityTail, FlagFacility, 0x79, 400);
    REC(FlagRow, RecruitRow, 0x0, 400);
    REC(MapObjectBase, MapObject, 0x4, 400);
    REC(Order0bTail, Order0b, 0xc2, 400);
    REC(GateTail, Gate, 0xc2, 400);
    REC(Order0cTail, Order0c_a, 0xc2, 400);
    REC(Order45Tail, Order45, 0xc2, 400);
    REC(WallTail, Wall, 0xc2, 400);
    REC(WorldItem, WorldItem, 0x12, 400);
    REC(Order, Missile, 0x0, 400);
}

// The tails without schema (load_c2 / save_c2 N = 0: Order00, N = 0x18: the class of 0x00481890): Out / In with a
// size and a name give the raw write's framing.
void test_unschema_tails() {
    if (g_mode == Mode::CheckGolden) return;
    MemFile& a = g_files[0];
    MemFile& b = g_files[1];
    reset(a);
    reset(b);
    u8 tail[0x18];
    fill(tail, sizeof tail);
    mf_write(&a, tail, 0);
    mf_write(&a, tail, 0x18);
    {
        sv::Out o(port(b), 0, "Order00Tail", tail);
        check(o.end() == 1, "empty tail end", "Order00Tail");
    }
    {
        sv::Out o(port(b), 0x18, "MapObject18Tail", tail);
        o.bytes(tail, 0x18);
        check(o.end() == 1, "0x18 tail end", "MapObject18Tail");
    }
    check(a.len == 0x1c && a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0, "tails stream == raw", "tails",
          a.len, b.len);
    b.pos = 0;
    u8 back[0x18] = {};
    {
        sv::In in(port(b), 0, "Order00Tail", back);
        check(in.end() == 1, "empty tail read", "Order00Tail");
    }
    {
        sv::In in(port(b), 0x18, "MapObject18Tail", back);
        in.bytes(back, 0x18);
        check(in.end() == 1, "0x18 tail read", "MapObject18Tail");
    }
    check(std::memcmp(back, tail, 0x18) == 0 && b.pos == b.len, "tails read back", "tails");
    out("Order00Tail (empty chunk) / MapObject18Tail (0x18 bytes): framing == raw\n");
}

}  // namespace

int test_main(int argc, char** argv) {
    install();
    out(sv::kX86Layout ? "save test s2, x86 layout (4-byte pointers)\n" : "save test s2, x64 layout (8-byte pointers)\n");
    const char* golden_out = nullptr;
    const char* golden_in = nullptr;
    for (int i = 1; i + 1 < argc; i += 2) {
        if (std::strcmp(argv[i], "--write-golden") == 0) golden_out = argv[i + 1];
        if (std::strcmp(argv[i], "--check-golden") == 0) golden_in = argv[i + 1];
    }
    reset(g_golden);
    g_mode = golden_out != nullptr ? Mode::WriteGolden : Mode::Local;
    test_records();
    test_unschema_tails();
    check(g_misuse == 0, "no misuse events", "all", static_cast<u32>(g_misuse));
    if (golden_out != nullptr) {
        check(save_file(golden_out, g_golden.data, g_golden.len), "golden written", "all");
        out("golden written: ");
        dec(g_golden_records);
        out(" records, ");
        dec(g_golden.len);
        out(" bytes\n");
    }
    if (golden_in != nullptr) {
        const long n = load_file(golden_in, g_golden.data, sizeof g_golden.data);
        check(n > 0, "golden file readable", "all");
        if (n > 0) {
            g_golden.len = static_cast<u32>(n);
            g_golden.pos = 0;
            reset(g_regolden);
            g_golden_records = g_token_diffs = 0;
            g_mode = Mode::CheckGolden;
            test_records();
            check(g_golden.pos == g_golden.len, "golden consumed", "all", g_golden.pos, g_golden.len);
            if (sv::kX86Layout)
                check(g_regolden.len == g_golden.len && std::memcmp(g_regolden.data, g_golden.data, g_golden.len) == 0,
                      "x86: golden re-encoded byte-identical", "all");
            out("golden: ");
            dec(g_golden_records);
            out(" records decoded and re-encoded, ");
            dec(g_golden.len);
            out(" bytes identical except ");
            dec(g_token_diffs);
            out(" non-null pointer values re-encoded as tokens\n");
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
