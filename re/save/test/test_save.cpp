// Offline proof of the S0 serialiser (re/a6/SAVES_PLAN.md 5.1): src/save/io.cpp + the generated records of section s0
// (src/save/gen/records_s0.hpp) on the game's real typed structs (src/original/gen/calendar.hpp, path_point.hpp),
// against the old raw path, through a memory file with File's vary-size framing (src/ilib/file.cpp file_write /
// file_read semantics, transcribed below).
//
// Built twice by build.sh: freestanding i686 Linux (the x86 layout, like joan_re.exe) and native x86_64 Linux.
//   i686:   every chunk == the old raw bytes (memcpy, or img::save_words for PathPoint), every load == the raw read
//           (+ img::load_words), SaveCompare silent; --write-golden DIR writes the chunk stream it produced.
//   x86_64: same checks where the layout allows; --check-golden DIR decodes the i686 stream, re-encodes it and must
//           reproduce it byte for byte (the x64 path writes x86 bytes without a game).
//   both:   pointer policies, framing (truncation / zero fill / u16 prefix), misuse detection, mismatch naming.
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "original/gen/calendar.hpp"
#include "original/gen/path_point.hpp"
#include "save/gen/records_s0.hpp"
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

// ------------------------------------------------------------------------- memory File (src/ilib/file.cpp)
struct MemFile {
    u8 data[1u << 20];
    u32 len, pos;
};
MemFile g_files[4];

void raw_put(MemFile& f, const void* p, u32 n) {  // WriteFile at the position
    std::memcpy(f.data + f.pos, p, n);
    f.pos += n;
    if (f.pos > f.len) f.len = f.pos;
}
u32 raw_get(MemFile& f, void* p, u32 n) {  // ReadFile: TRUE also at EOF (short read)
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
    if (raw_get(f, &prefix, 2) != 2) prefix = 0;  // read_raw: T{} unless ReadFile fails (it does not at EOF)
    if (prefix != 0) read_size = size < prefix ? size : prefix, rec_size = prefix;
    raw_get(f, buf, read_size);
    if (read_size < rec_size) f.pos += rec_size - read_size;  // file_seek(rec_size - read_size, FILE_CURRENT)
    if (read_size < size) std::memset(static_cast<u8*>(buf) + read_size, 0, size - read_size);
    return 1;
}
const char* mf_name(void*) { return "mem"; }
sv::Port port(MemFile& f) { return sv::Port{&f, &mf_write, &mf_read, &mf_name}; }
void reset(MemFile& f) { f.len = f.pos = 0; }

// ----------------------------------------------------------------- image / code addresses (src/image.hpp)
constexpr u32 kImgVa = 0x009f4000, kImgSize = 0x1000, kCodeVa = 0x00401230;
alignas(16) u8 g_img[kImgSize];
void fake_code() {}
uptr img_base() { return reinterpret_cast<uptr>(g_img); }
uptr code_ptr() { return reinterpret_cast<uptr>(&fake_code); }
bool in_image(uptr v) { return v - img_base() < kImgSize; }
bool in_old(u32 v) { return v - kImgVa < kImgSize; }
u32 image_out(uptr v) {
    if (in_image(v)) return kImgVa + static_cast<u32>(v - img_base());
    if (v == code_ptr()) return kCodeVa;
    return static_cast<u32>(v);
}
uptr image_in(u32 v) {
    if (in_old(v)) return img_base() + (v - kImgVa);
    if (v == kCodeVa) return code_ptr();
    return v;
}
// Transcriptions of img::save_words / img::load_words (src/image.hpp) over the fake image: the old raw path.
void save_words(void* dst, const void* src, u32 n) {
    std::memcpy(dst, src, n);
    auto* d = static_cast<u8*>(dst);
    for (u32 i = 0; i + 4 <= n; i += 4) {
        u32 v;
        std::memcpy(&v, d + i, 4);
        if (in_image(v)) {
            v = kImgVa + static_cast<u32>(v - img_base());
            std::memcpy(d + i, &v, 4);
        } else if (v == code_ptr()) {
            const u32 c = kCodeVa;
            std::memcpy(d + i, &c, 4);
        }
    }
}
void load_words(void* p, u32 n) {
    auto* d = static_cast<u8*>(p);
    for (u32 i = 0; i + 4 <= n; i += 4) {
        u32 v;
        std::memcpy(&v, d + i, 4);
        if (in_old(v)) {
            v = static_cast<u32>(img_base() + (v - kImgVa));
            std::memcpy(d + i, &v, 4);
        } else if (v == kCodeVa) {
            v = static_cast<u32>(code_ptr());
            std::memcpy(d + i, &v, 4);
        }
    }
}

// ---------------------------------------------------------------------------------------------- hooks
bool g_compare = true;
int g_compared = 0, g_mismatch = 0, g_misuse = 0;
const sv::Field* g_last_field = nullptr;
u32 g_last_off = 0;
bool fieldwise_on() { return true; }
bool compare_on() { return g_compare; }
bool audit_off() { return false; }
void on_event(const sv::Event& e) {
    if (e.kind == sv::Event::Compared) ++g_compared;
    if (e.kind == sv::Event::Mismatch) {
        ++g_mismatch;
        g_last_field = e.field;
        g_last_off = e.off;
    }
    if (e.kind == sv::Event::Misuse) ++g_misuse;
}
void install() {
    sv::Hooks& h = sv::hooks();
    h.fieldwise = &fieldwise_on;
    h.compare = &compare_on;
    h.audit = &audit_off;
    h.image_out = &image_out;
    h.image_in = &image_in;
    h.legacy_out = &save_words;
    h.legacy_in = &load_words;
    h.event = &on_event;
}

// ------------------------------------------------------------------------------------- test objects
// Random bytes, with signalling / quiet NaNs, infinities and denormals in the float members now and then.
void random_calendar(jo::Calendar& c) {
    fill(&c, sizeof c);
    static const u32 kFloats[] = {0x7f800001u, 0xff800001u, 0x7fc00000u, 0x7f800000u, 0x00000001u, 0x80000000u};
    if (rnd() % 3 == 0) put32(&c, 0x0c, kFloats[rnd() % 6]);
    if (rnd() % 3 == 0) put32(&c, 0x1c, kFloats[rnd() % 6]);
    if (rnd() % 3 == 0) put32(&c, 0x20, kFloats[rnd() % 6]);
}
// Random dwords, some of them addresses inside the fake image / the fake code pointer (x86: they fit a dword), some
// original addresses (as a save of joan.exe holds them).
void random_point(jo::PathPoint& p) {
    fill(&p, sizeof p);
    for (u32 off = 0; off < 0x24; off += 4) {
        switch (rnd() % 8) {
        case 0:
            if (sv::kX86Layout) put32(&p, off, static_cast<u32>(img_base() + rnd() % kImgSize));
            break;
        case 1:
            if (sv::kX86Layout) put32(&p, off, static_cast<u32>(code_ptr()));
            break;
        case 2: put32(&p, off, kImgVa + rnd() % kImgSize); break;
        case 3: put32(&p, off, kCodeVa); break;
        case 4: put32(&p, off, 0x7f800001u); break;
        default: break;
        }
    }
}

constexpr int kN = 3000;
jo::Calendar g_cal[kN];
jo::PathPoint g_pts[kN];

// --------------------------------------------------------------------------------------------- tests
// Calendar (Legacy::Raw): chunk == the object's bytes, file == the raw write's file, load == the raw read.
void test_calendar() {
    MemFile& a = g_files[0];
    MemFile& b = g_files[1];
    reset(a);
    reset(b);
    g_compared = g_mismatch = 0;
    for (int i = 0; i < kN; ++i) {
        random_calendar(g_cal[i]);
        mf_write(&a, &g_cal[i], 0x50);  // old: file_write(file, this, 0x50)
        sv::Out o(port(b), sv::kCalendar, &g_cal[i]);
        sv::put_Calendar(o, g_cal[i]);
        check(std::memcmp(o.data(), &g_cal[i], 0x50) == 0, "calendar chunk == object bytes", static_cast<u32>(i));
        check(o.end() == 1, "calendar end");
    }
    check(a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0, "calendar stream == raw stream", a.len, b.len);
    check(g_compared == (sv::kX86Layout ? kN : 0) && g_mismatch == 0, "calendar SaveCompare silent", static_cast<u32>(g_compared),
          static_cast<u32>(g_mismatch));
    a.pos = b.pos = 0;
    g_compared = 0;
    for (int i = 0; i < kN; ++i) {
        jo::Calendar raw, fw;
        fill(&raw, sizeof raw);
        std::memcpy(&fw, &raw, sizeof fw);
        mf_read(&a, &raw, 0x50);  // old: file_read(file, this, 0x50)
        sv::In in(port(b), sv::kCalendar, &fw);
        sv::get_Calendar(in, fw);
        check(in.end() == 1, "calendar read result");
        check(std::memcmp(&raw, &fw, sizeof raw) == 0, "calendar load == raw read", static_cast<u32>(i));
        check(std::memcmp(&fw, &g_cal[i], sizeof fw) == 0, "calendar round trip", static_cast<u32>(i));
    }
    check(g_compared == (sv::kX86Layout ? kN : 0) && g_mismatch == 0, "calendar load SaveCompare silent",
          static_cast<u32>(g_compared), static_cast<u32>(g_mismatch));
    out("calendar: ");
    dec(kN);
    out(" records written / read, stream ");
    dec(b.len);
    out(" bytes == raw\n");
}

// PathPoint (Legacy::ImageWords): chunk == img::save_words(object), load == raw read + img::load_words (x86).
void test_pathpoint() {
    MemFile& a = g_files[0];
    MemFile& b = g_files[1];
    reset(a);
    reset(b);
    g_compared = g_mismatch = 0;
    for (int i = 0; i < kN; ++i) {
        random_point(g_pts[i]);
        alignas(4) u8 rec[0x24];
        save_words(rec, &g_pts[i], 0x24);  // old: img::save_words(rec, e, 0x24); file_write(file, rec, 0x24)
        mf_write(&a, rec, 0x24);
        sv::Out o(port(b), sv::kPathPoint, &g_pts[i]);
        sv::put_PathPoint(o, g_pts[i]);
        check(std::memcmp(o.data(), rec, 0x24) == 0, "pathpoint chunk == save_words", static_cast<u32>(i));
        check(o.end() == 1, "pathpoint end");
    }
    check(a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0, "pathpoint stream == raw stream", a.len, b.len);
    check(g_compared == (sv::kX86Layout ? kN : 0) && g_mismatch == 0, "pathpoint SaveCompare silent",
          static_cast<u32>(g_compared), static_cast<u32>(g_mismatch));
    a.pos = b.pos = 0;
    g_compared = 0;
    int image_values = 0;
    for (int i = 0; i < kN; ++i) {
        jo::PathPoint raw, fw;
        fill(&raw, sizeof raw);
        std::memcpy(&fw, &raw, sizeof fw);
        mf_read(&a, &raw, 0x24);  // old: file_read(file, e, 0x24); img::load_words(e, 0x24)
        u8 file_bytes[0x24];
        std::memcpy(file_bytes, &raw, 0x24);
        load_words(&raw, 0x24);
        sv::In in(port(b), sv::kPathPoint, &fw);
        sv::get_PathPoint(in, fw);
        check(in.end() == 1, "pathpoint read result");
        if (sv::kX86Layout) {
            check(std::memcmp(&raw, &fw, sizeof raw) == 0, "pathpoint load == raw read + load_words", static_cast<u32>(i));
        } else {
            // x64: an original address cannot be relocated into a dword: the file bytes stay (load_words truncated).
            check(std::memcmp(&fw, file_bytes, sizeof fw) == 0, "pathpoint x64 load keeps the file bytes", static_cast<u32>(i));
        }
        for (u32 off = 0; off < 0x24; off += 4) {
            u32 v;
            std::memcpy(&v, file_bytes + off, 4);
            image_values += in_old(v) || v == kCodeVa;
        }
    }
    check(g_compared == (sv::kX86Layout ? kN : 0) && g_mismatch == 0, "pathpoint load SaveCompare silent",
          static_cast<u32>(g_compared), static_cast<u32>(g_mismatch));
    check(image_values > kN / 4, "pathpoint test has image values", static_cast<u32>(image_values));
    out("pathpoint: ");
    dec(kN);
    out(" records, ");
    dec(static_cast<u32>(image_values));
    out(" image / code dwords, stream ");
    dec(b.len);
    out(" bytes == raw\n");
}

// The golden stream: the Calendar and PathPoint chunks of the two tests above, as written field-wise.
u32 build_golden(MemFile& g) {
    reset(g);
    for (int i = 0; i < kN; ++i) {
        sv::Out o(port(g), sv::kCalendar);
        sv::put_Calendar(o, g_cal[i]);
        o.end();
        sv::Out p(port(g), sv::kPathPoint);
        sv::put_PathPoint(p, g_pts[i]);
        p.end();
    }
    return g.len;
}

// x64 (or any build): decode a stream written by the i686 build and re-encode it: byte for byte the same.
void check_golden(const char* path) {
    MemFile& g = g_files[2];
    MemFile& r = g_files[3];
    const long n = load_file(path, g.data, sizeof g.data);
    check(n > 0, "golden file readable");
    if (n <= 0) return;
    g.len = static_cast<u32>(n);
    g.pos = 0;
    reset(r);
    int records = 0;
    while (g.pos < g.len) {
        jo::Calendar c;
        jo::PathPoint p;
        {
            sv::In in(port(g), sv::kCalendar);
            sv::get_Calendar(in, c);
            check(in.end() == 1, "golden calendar read");
        }
        {
            sv::In in(port(g), sv::kPathPoint);
            sv::get_PathPoint(in, p);
            check(in.end() == 1, "golden pathpoint read");
        }
        sv::Out o(port(r), sv::kCalendar);
        sv::put_Calendar(o, c);
        o.end();
        sv::Out q(port(r), sv::kPathPoint);
        sv::put_PathPoint(q, p);
        q.end();
        records += 2;
    }
    check(r.len == g.len && std::memcmp(r.data, g.data, g.len) == 0, "golden re-encoded == golden", r.len, g.len);
    out("golden: ");
    dec(static_cast<u32>(records));
    out(" records decoded and re-encoded, ");
    dec(g.len);
    out(" bytes identical\n");
}

// Pointer policies on a hand-made record (the generator emits the same calls for pointer members).
struct Ptrs {
    void* flag;
    void* cleared;
    void* kept;
    void* image;
    void* code;
    void* stale;
    void* null;
};
const sv::Field kPtrsFields[] = {
    {0x00, 4, 4, "flag", sv::Type::Pointer, sv::Ptr::Flag},     {0x04, 4, 4, "cleared", sv::Type::Pointer, sv::Ptr::Cleared},
    {0x08, 4, 4, "kept", sv::Type::Pointer, sv::Ptr::Kept},     {0x0c, 4, 4, "image", sv::Type::Pointer, sv::Ptr::Image},
    {0x10, 4, 4, "code", sv::Type::Pointer, sv::Ptr::Image},    {0x14, 4, 4, "stale", sv::Type::Pointer, sv::Ptr::Stale},
    {0x18, 4, 4, "null", sv::Type::Pointer, sv::Ptr::Flag},
};
const sv::Record kPtrs{"Ptrs", "Ptrs", 0x1c, sv::Legacy::Raw, kPtrsFields, 7};

void test_pointers() {
    static u8 heap_like[64];
    MemFile& f = g_files[0];
    reset(f);
    Ptrs p{heap_like, heap_like + 4, heap_like + 8, g_img + 0x40, reinterpret_cast<void*>(code_ptr()), heap_like + 12,
           nullptr};
    sv::Out o(port(f), kPtrs);
    o.ptr(p.flag, sv::Ptr::Flag);
    o.ptr(p.cleared, sv::Ptr::Cleared);
    o.ptr(p.kept, sv::Ptr::Kept);
    o.ptr(p.image, sv::Ptr::Image);
    o.ptr(p.code, sv::Ptr::Image);
    o.ptr(p.stale, sv::Ptr::Stale);
    o.ptr(p.null, sv::Ptr::Flag);
    u32 w[7];
    std::memcpy(w, o.data(), sizeof w);
    check(o.end() == 1, "ptrs end");
    const void* const vals[] = {p.flag, p.cleared, p.kept, nullptr, nullptr, p.stale};
    for (int i = 0; i < 6; ++i) {
        if (i == 3 || i == 4) continue;
        const u32 low = static_cast<u32>(reinterpret_cast<uptr>(vals[i]));
        check(w[i] == (sv::kX86Layout ? low : (low | 1u)), "ptr token", w[i], static_cast<u32>(i));
        check(w[i] != 0, "non-null ptr token != 0", static_cast<u32>(i));
    }
    check(w[3] == kImgVa + 0x40, "image ptr -> original VA", w[3]);
    check(w[4] == kCodeVa, "code ptr -> original code address", w[4]);
    check(w[6] == 0, "null ptr -> 0", w[6]);
    f.pos = 0;
    Ptrs q{};
    void* const mine = heap_like + 32;
    q.kept = mine;
    sv::In in(port(f), kPtrs);
    in.ptr(q.flag, sv::Ptr::Flag);
    in.ptr(q.cleared, sv::Ptr::Cleared);
    in.ptr(q.kept, sv::Ptr::Kept);
    in.ptr(q.image, sv::Ptr::Image);
    in.ptr(q.code, sv::Ptr::Image);
    in.ptr(q.stale, sv::Ptr::Stale);
    in.ptr(q.null, sv::Ptr::Flag);
    check(in.end() == 1, "ptrs read");
    check(q.flag != nullptr && q.cleared != nullptr && q.stale != nullptr && q.null == nullptr, "flag tokens: null iff 0");
    if (sv::kX86Layout) {
        check(q.flag == p.flag && q.cleared == p.cleared && q.stale == p.stale && q.kept == p.kept, "x86: raw values");
    } else {
        check(q.kept == mine, "x64: Kept leaves the field");
    }
    check(q.image == p.image, "image ptr relocated back");
    check(q.code == p.code, "code ptr resolved back");
    out("pointers: Flag / Cleared / Kept / Image (data, code) / Stale / null ok\n");
}

// Framing: the u16 prefix, a stored chunk longer / shorter than the reader's record (File::file_read semantics).
void test_framing() {
    MemFile& f = g_files[0];
    reset(f);
    jo::Calendar c;
    random_calendar(c);
    {
        sv::Out o(port(f), sv::kCalendar);
        sv::put_Calendar(o, c);
        o.end();
    }
    check(f.len == 0x52 && f.data[0] == 0x50 && f.data[1] == 0, "u16 size prefix", f.len);
    {
        sv::Out o(port(f), 4, "marker");
        o.u32(0x2facebee);
        o.end();
    }
    f.pos = 0;
    u8 part[0x40];
    {
        sv::In in(port(f), 0x40, "short");  // a reader of an older, shorter record: the excess is skipped
        in.bytes(part, 0x40);
        check(in.end() == 1, "short read");
    }
    check(std::memcmp(part, &c, 0x40) == 0, "short read = first bytes");
    {
        sv::In in(port(f), 4, "marker");
        check(in.u32() == 0x2facebee, "chunk after a skipped tail");
        in.end();
    }
    f.pos = 0;
    u8 longer[0x60];
    {
        sv::In in(port(f), 0x60, "long");  // a reader of a longer record: zero fill
        in.bytes(longer, 0x60);
        in.end();
    }
    bool zero = true;
    for (u32 i = 0x50; i < 0x60; ++i) zero = zero && longer[i] == 0;
    check(std::memcmp(longer, &c, 0x50) == 0 && zero, "long read = bytes + zero fill");
    out("framing: u16 prefix, truncation, zero fill ok\n");
}

// Misuse (a writer / reader that does not fill / consume the record exactly) and mismatch naming.
void test_misuse_and_compare() {
    MemFile& f = g_files[0];
    reset(f);
    g_misuse = 0;
    jo::Calendar c;
    random_calendar(c);
    {
        sv::Out o(port(f), sv::kCalendar);
        o.u32(c.start_julian);
        check(o.end() == 0, "short chunk refused");
    }
    check(f.len == 0 && g_misuse == 1, "short chunk: nothing written, reported", f.len, static_cast<u32>(g_misuse));
    {
        sv::Out o(port(f), 4, "four");
        o.u32(1);
        o.u32(2);
        check(o.end() == 0, "overflow refused");
    }
    check(f.len == 0 && g_misuse == 2, "overflow: nothing written, reported", f.len, static_cast<u32>(g_misuse));
    { sv::Out o(port(f), 4, "dropped"); }  // destroyed without end()
    check(g_misuse == 3, "missing end() reported", static_cast<u32>(g_misuse));
    if (sv::kX86Layout) {
        jo::Calendar old = c;
        old.month ^= 0x100;
        g_mismatch = 0;
        sv::Out o(port(f), sv::kCalendar, &old);
        sv::put_Calendar(o, c);
        o.end();
        check(g_mismatch == 1 && g_last_field != nullptr && std::strcmp(g_last_field->name, "month") == 0 &&
                  g_last_off == 0x15,
              "mismatch names the field", static_cast<u32>(g_mismatch), g_last_off);
    }
    out("misuse / mismatch reporting ok\n");
}

}  // namespace

int test_main(int argc, char** argv) {
    install();
    out(sv::kX86Layout ? "save test, x86 layout (4-byte pointers)\n" : "save test, x64 layout (8-byte pointers)\n");
    test_calendar();
    test_pathpoint();
    test_pointers();
    test_framing();
    test_misuse_and_compare();
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
