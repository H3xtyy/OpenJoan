// Offline proof of batch S5 (re/a6/SAVES_PLAN.md 5.1): the records of section s5 (src/save/gen/records_s5.hpp) on the
// game's real typed structs, composed into chunks exactly as the converted call sites do (src/game/gamefile.cpp
// header, gamefil2.cpp step 2 + markers, src/save/s5.hpp Profile head / ProfileUnit name key, gfile3.cpp 0x483740 /
// 0x483800, src/ilib/dynarr.cpp empty rooms, src/game/_unplaced/config.cpp CONFIG.DAT), against the old raw calls,
// through a memory file with File's framing (vary-size .SAV files; CONFIG.DAT / profiles without the u16 prefix).
//
// Built twice by build_s5.sh: freestanding i686 Linux (the x86 layout, like joan_re.exe) and native x86_64 Linux.
//   i686:   every chunk == the old raw bytes at the x86 offsets, every load == the raw read, SaveCompare silent;
//           --write-golden FILE writes the chunk stream.
//   x86_64: the records without pointers (GameFile header, Config, KeyBinding, EmptyRoom) == the raw bytes too (same
//           layout); --check-golden FILE decodes the i686 stream, re-encodes it: byte for byte the same except the
//           pointer fields (x64 tokens: null iff null), and the decoded scalars land on the x64 members.
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "original/dynarray.hpp"
#include "original/gen/config.hpp"
#include "original/gen/game_file.hpp"
#include "original/gen/key_binding.hpp"
#include "original/gen/profile.hpp"
#include "original/gen/profile_unit.hpp"
#include "original/gen/wnd.hpp"
#include "save/gen/records_s5.hpp"
#include "save/io.hpp"

namespace jo = joan::orig;
namespace sv = joan::save;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
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

u32 g_rng = 0x51f15e5u;
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
const u8* bytes(const void* p) { return static_cast<const u8*>(p); }
u8* bytes(void* p) { return static_cast<u8*>(p); }

// ------------------------------------------------------------------------- memory File (src/ilib/file.cpp)
struct MemFile {
    u8 data[1u << 22];
    u32 len, pos;
    bool vary;  // allow_vary_size: .SAV files (u16 prefix); profiles and CONFIG.DAT: plain bytes
};
MemFile g_files[4];

void raw_put(MemFile& f, const void* p, u32 n) {
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
// File::file_write 0x005872e0.
std::int32_t mf_write(void* o, const void* buf, u32 n) {
    MemFile& f = *static_cast<MemFile*>(o);
    if (f.vary) {
        const u16 prefix = n > 0xffff ? 0 : static_cast<u16>(n);
        raw_put(f, &prefix, 2);
    }
    raw_put(f, buf, n);
    return 1;
}
// File::file_read 0x005873a0.
std::int32_t mf_read(void* o, void* buf, u32 size) {
    MemFile& f = *static_cast<MemFile*>(o);
    if (!f.vary) {
        raw_get(f, buf, size);
        return 1;
    }
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
void reset(MemFile& f, bool vary) {
    f.len = f.pos = 0;
    f.vary = vary;
}
bool same_stream(const MemFile& a, const MemFile& b) { return a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0; }

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

// ------------------------------------------------------------------- the converted call sites, transcribed
// gamefile.cpp kHeaderChunks / read_header: {x86 offset, written size, read size, record, put, get}.
struct HeaderChunk {
    u32 off, wsize, rsize;
    const sv::Record* rec;
    void (*put)(sv::Out&, const jo::GameFile&);
    void (*get)(sv::In&, jo::GameFile&);
};
const HeaderChunk kHeader[10] = {
    {0x002, 4, 1, &sv::kGameFileFlags, &sv::put_GameFileFlags<jo::GameFile>, nullptr},
    {0x108, 8, 8, &sv::kGameFileTime, &sv::put_GameFileTime<jo::GameFile>, &sv::get_GameFileTime<jo::GameFile>},
    {0x110, 8, 8, &sv::kGameFileLocalTime, &sv::put_GameFileLocalTime<jo::GameFile>, &sv::get_GameFileLocalTime<jo::GameFile>},
    {0x118, 1, 1, &sv::kGameFileMode, &sv::put_GameFileMode<jo::GameFile>, &sv::get_GameFileMode<jo::GameFile>},
    {0x119, 1, 1, &sv::kGameFile119, &sv::put_GameFile119<jo::GameFile>, &sv::get_GameFile119<jo::GameFile>},
    {0x19f, 4, 4, &sv::kGameFileGameTime, &sv::put_GameFileGameTime<jo::GameFile>, &sv::get_GameFileGameTime<jo::GameFile>},
    {0x1a3, 4, 4, &sv::kGameFile1a3, &sv::put_GameFile1a3<jo::GameFile>, &sv::get_GameFile1a3<jo::GameFile>},
    {0x11a, 0x80, 0x80, &sv::kGameFileName, &sv::put_GameFileName<jo::GameFile>, &sv::get_GameFileName<jo::GameFile>},
    {0x19a, 4, 4, &sv::kGameFile19a, &sv::put_GameFile19a<jo::GameFile>, &sv::get_GameFile19a<jo::GameFile>},
    {0x19e, 1, 1, &sv::kGameFile19e, &sv::put_GameFile19e<jo::GameFile>, &sv::get_GameFile19e<jo::GameFile>},
};
void put_header(MemFile& f, jo::GameFile& g, bool legacy) {
    for (const HeaderChunk& c : kHeader) {
        sv::Out o(port(f), *c.rec, legacy ? bytes(&g) + c.off : nullptr);
        c.put(o, g);
        check(o.end() == 1, "header chunk end", c.off);
    }
}
void get_header(MemFile& f, jo::GameFile& g, bool legacy) {
    for (const HeaderChunk& c : kHeader) {
        if (c.get == nullptr) {
            sv::In in(port(f), 1, "GameFileFlags", legacy ? bytes(&g) + c.off : nullptr);
            g.flags = (g.flags & ~0xffu) | in.u8();
            check(in.end() == 1, "header flags read");
        } else {
            sv::In in(port(f), *c.rec, legacy ? bytes(&g) + c.off : nullptr);
            c.get(in, g);
            check(in.end() == 1, "header chunk read", c.off);
        }
    }
}

// gamefil2.cpp step 2 (0x476c40 / 0x476990) on the Wnd.
struct WndChunk {
    u32 off;
    const sv::Record* rec;
    void (*put)(sv::Out&, const jo::Wnd&);
    void (*get)(sv::In&, jo::Wnd&);
};
const WndChunk kWnd[4] = {
    {0x44, &sv::kWndSpeed, &sv::put_WndSpeed<jo::Wnd>, &sv::get_WndSpeed<jo::Wnd>},
    {0x48, &sv::kWndI16_48, &sv::put_WndI16_48<jo::Wnd>, &sv::get_WndI16_48<jo::Wnd>},
    {0x4a, &sv::kWndU8_4a, &sv::put_WndU8_4a<jo::Wnd>, &sv::get_WndU8_4a<jo::Wnd>},
    {0x32, &sv::kWndGameTime, &sv::put_WndGameTime<jo::Wnd>, &sv::get_WndGameTime<jo::Wnd>},
};
void put_wnd(MemFile& f, const jo::Wnd& w, bool legacy) {
    for (const WndChunk& c : kWnd) {
        sv::Out o(port(f), *c.rec, legacy ? bytes(&w) + c.off : nullptr);
        c.put(o, w);
        check(o.end() == 1, "wnd chunk end", c.off);
    }
}
void get_wnd(MemFile& f, jo::Wnd& w, bool legacy) {
    for (const WndChunk& c : kWnd) {
        sv::In in(port(f), *c.rec, legacy ? bytes(&w) + c.off : nullptr);
        c.get(in, w);
        check(in.end() == 1, "wnd chunk read", c.off);
        if (c.off == 0x4a) w.u8_4a = 1;  // as 0x476990
    }
}

// src/save/s5.hpp write_profile_head / read_profile_head / write_unit_key / read_unit_key.
void put_profile_head(MemFile& f, const jo::Profile& p, bool legacy) {
    {
        sv::Out o(port(f), sv::kProfile, legacy ? &p : nullptr);
        sv::put_Profile(o, p);
        check(o.end() == 1, "profile end");
    }
    sv::Out o(port(f), sv::kProfileName, legacy ? &p : nullptr);
    sv::put_ProfileName(o, p);
    check(o.end() == 1, "profile name end");
}
void get_profile_head(MemFile& f, jo::Profile& p, bool legacy) {
    {
        sv::In in(port(f), sv::kProfile, legacy ? &p : nullptr);
        sv::get_Profile(in, p);
        check(in.end() == 1, "profile read");
    }
    sv::In in(port(f), sv::kProfileName, legacy ? &p : nullptr);
    sv::get_ProfileName(in, p);
    check(in.end() == 1, "profile name read");
}
void put_unit_key(MemFile& f, const jo::ProfileUnit& u, bool legacy) {
    sv::Out o(port(f), 0x20, "ProfileUnit.name", legacy ? &u : nullptr);
    o.bytes(u.name, 0x20);
    check(o.end() == 1, "unit key end");
}
void get_unit_key(MemFile& f, jo::ProfileUnit& u, bool legacy) {
    sv::In in(port(f), 0x20, "ProfileUnit.name", legacy ? &u : nullptr);
    in.bytes(u.name, 0x20);
    check(in.end() == 1, "unit key read");
}
void put_unit(MemFile& f, const jo::ProfileUnit& u, bool legacy) {
    sv::Out o(port(f), sv::kProfileUnit, legacy ? &u : nullptr);
    sv::put_ProfileUnit(o, u);
    check(o.end() == 1, "profile unit end");
}
void get_unit(MemFile& f, jo::ProfileUnit& u, bool legacy) {
    sv::In in(port(f), sv::kProfileUnit, legacy ? &u : nullptr);
    sv::get_ProfileUnit(in, u);
    check(in.end() == 1, "profile unit read");
}

// dynarr.cpp write_rooms / read_rooms.
void put_rooms(MemFile& f, const jo::EmptyRoom* r, i32 n, bool legacy) {
    sv::Out o(port(f), static_cast<u32>(n * 8), "EmptyRoom[]", legacy ? r : nullptr);
    for (i32 i = 0; i < n; ++i) sv::put_EmptyRoom(o, r[i]);
    check(o.end() == 1, "rooms end");
}
void get_rooms(MemFile& f, jo::EmptyRoom* r, i32 n, bool legacy) {
    sv::In in(port(f), static_cast<u32>(n * 8), "EmptyRoom[]", legacy ? r : nullptr);
    for (i32 i = 0; i < n; ++i) sv::get_EmptyRoom(in, r[i]);
    check(in.end() == 1, "rooms read");
}

// config.cpp write_b05 / save_restore_45adc0: Config +0..+0xb5 and KeyBinding[55] at +0xb5, one 0xb05 chunk.
constexpr u32 kConfigSize = 0xb05, kKeysOff = 0xb5, kKeys = 55;
struct alignas(4) ConfigBuf {
    u8 b[kConfigSize];
};
void put_config(MemFile& f, const ConfigBuf& c, bool legacy) {
    const auto* const keys = reinterpret_cast<const jo::KeyBinding*>(c.b + kKeysOff);
    sv::Out o(port(f), kConfigSize, "Config", legacy ? c.b : nullptr);
    sv::put_ConfigHead(o, *reinterpret_cast<const jo::Config*>(c.b));
    for (u32 i = 0; i < kKeys; ++i) sv::put_KeyBinding(o, keys[i]);
    check(o.end() == 1, "config end");
}
void get_config(MemFile& f, ConfigBuf& c, bool legacy) {
    auto* const keys = reinterpret_cast<jo::KeyBinding*>(c.b + kKeysOff);
    sv::In in(port(f), kConfigSize, "Config", legacy ? c.b : nullptr);
    sv::get_ConfigHead(in, *reinterpret_cast<jo::Config*>(c.b));
    for (u32 i = 0; i < kKeys; ++i) sv::get_KeyBinding(in, keys[i]);
    check(in.end() == 1, "config read");
}

// ------------------------------------------------------------------------------------- test objects
constexpr int kN = 400;
jo::GameFile g_gf[kN];
jo::Wnd g_wnd[kN];
jo::Profile g_prof[kN];
jo::ProfileUnit g_unit[kN];
jo::EmptyRoom g_rooms[kN][9];
i32 g_room_n[kN];
ConfigBuf g_cfg[kN];

// Random bytes; the pointer members null about a third of the time.
template <class P>
void maybe_null(P*& p) {
    if (rnd() % 3 == 0) p = nullptr;
}
void random_objects() {
    for (int i = 0; i < kN; ++i) {
        fill(&g_gf[i], sizeof g_gf[i]);
        fill(&g_wnd[i], sizeof g_wnd[i]);
        fill(&g_prof[i], sizeof g_prof[i]);
        maybe_null(g_prof[i].units);
        maybe_null(g_prof[i].list_a);
        maybe_null(g_prof[i].list_b);
        fill(&g_unit[i], sizeof g_unit[i]);
        maybe_null(g_unit[i].leader);
        for (auto*& m : g_unit[i].members) maybe_null(m);
        g_room_n[i] = static_cast<i32>(rnd() % 10);
        fill(g_rooms[i], sizeof g_rooms[i]);
        fill(&g_cfg[i], sizeof g_cfg[i]);
        static const u32 kFloats[] = {0x7f800001u, 0xff800001u, 0x7fc00000u, 0x00000001u};
        if (rnd() % 3 == 0) std::memcpy(g_cfg[i].b + 0x76, &kFloats[rnd() % 4], 4);  // fov
        if (rnd() % 3 == 0) std::memcpy(g_cfg[i].b + 0xa8, &kFloats[rnd() % 4], 4);  // brightness
        if (rnd() % 3 == 0) std::memcpy(&g_wnd[i].speed, &kFloats[rnd() % 4], 4);
    }
}

// ------------------------------------------------------------------------------- the two chunk streams
// .SAV (vary-size) per object: header, marker, step 2, marker, Profile head, unit key, ProfileUnit 0x77, u32 room
// count + rooms. Plain (profiles, CONFIG.DAT) per object: Config, Profile head, unit key.
void put_marker(MemFile& f) {
    const u32 m = 0x2facebee;
    sv::Out o(port(f), 4, "marker", &m);
    o.u32(m);
    check(o.end() == 1, "marker end");
}
void put_long(MemFile& f, i32 v) { raw_put(f, &v, 4); }  // File::file_put_long: no prefix
i32 get_long(MemFile& f) {
    i32 v = 0;
    raw_get(f, &v, 4);
    return v;
}
void build_sav(MemFile& f, bool legacy) {
    reset(f, true);
    for (int i = 0; i < kN; ++i) {
        put_header(f, g_gf[i], legacy);
        put_marker(f);
        put_wnd(f, g_wnd[i], legacy);
        put_marker(f);
        put_profile_head(f, g_prof[i], legacy);
        put_unit_key(f, g_unit[i], legacy);
        put_unit(f, g_unit[i], legacy);
        put_long(f, g_room_n[i]);
        if (g_room_n[i] > 0) put_rooms(f, g_rooms[i], g_room_n[i], legacy);
    }
}
void build_plain(MemFile& f, bool legacy) {
    reset(f, false);
    for (int i = 0; i < kN; ++i) {
        put_config(f, g_cfg[i], legacy);
        put_profile_head(f, g_prof[i], legacy);
        put_unit_key(f, g_unit[i], legacy);
    }
}
// The same streams by the old raw calls (x86 offsets).
void build_sav_raw(MemFile& f) {
    reset(f, true);
    const u32 m = 0x2facebee;
    for (int i = 0; i < kN; ++i) {
        for (const HeaderChunk& c : kHeader) mf_write(&f, bytes(&g_gf[i]) + c.off, c.wsize);
        mf_write(&f, &m, 4);
        for (const WndChunk& c : kWnd) mf_write(&f, bytes(&g_wnd[i]) + c.off, c.rec->size);
        mf_write(&f, &m, 4);
        mf_write(&f, &g_prof[i], 0x48);
        mf_write(&f, &g_prof[i], 0x20);
        mf_write(&f, &g_unit[i], 0x20);
        mf_write(&f, &g_unit[i], 0x77);
        put_long(f, g_room_n[i]);
        if (g_room_n[i] > 0) mf_write(&f, g_rooms[i], static_cast<u32>(g_room_n[i] * 8));
    }
}
void build_plain_raw(MemFile& f) {
    reset(f, false);
    for (int i = 0; i < kN; ++i) {
        mf_write(&f, &g_cfg[i], kConfigSize);
        mf_write(&f, &g_prof[i], 0x48);
        mf_write(&f, &g_prof[i], 0x20);
        mf_write(&f, &g_unit[i], 0x20);
    }
}

// ---------------------------------------------------------------------------- writes == the raw calls
void test_writes() {
    MemFile& a = g_files[0];
    MemFile& b = g_files[1];
    if (sv::kX86Layout) {
        g_compared = g_mismatch = 0;
        build_sav_raw(a);
        build_sav(b, true);
        check(same_stream(a, b), ".SAV parts == raw stream", a.len, b.len);
        build_plain_raw(a);
        build_plain(b, true);
        check(same_stream(a, b), "profile / CONFIG.DAT parts == raw stream", a.len, b.len);
        check(g_compared > 20 * kN && g_mismatch == 0, "SaveCompare silent", static_cast<u32>(g_compared),
              static_cast<u32>(g_mismatch));
        out("writes: .SAV header / markers / step 2 / Profile / ProfileUnit / rooms and CONFIG.DAT == raw (");
        dec(static_cast<u32>(g_compared));
        out(" chunks compared, ");
        dec(a.len);
        out(" plain bytes)\n");
        return;
    }
    // x64: the pointer-free records have the x86 layout too: their chunks equal the raw bytes
    reset(a, true);
    reset(b, true);
    for (int i = 0; i < kN; ++i) {
        for (const HeaderChunk& c : kHeader) mf_write(&a, bytes(&g_gf[i]) + c.off, c.wsize);
        put_header(b, g_gf[i], false);
        if (g_room_n[i] > 0) {
            mf_write(&a, g_rooms[i], static_cast<u32>(g_room_n[i] * 8));
            put_rooms(b, g_rooms[i], g_room_n[i], false);
        }
    }
    check(same_stream(a, b), "x64 header / rooms == raw", a.len, b.len);
    reset(a, false);
    reset(b, false);
    for (int i = 0; i < kN; ++i) {
        mf_write(&a, &g_cfg[i], kConfigSize);
        put_config(b, g_cfg[i], false);
    }
    check(same_stream(a, b), "x64 CONFIG.DAT == raw", a.len, b.len);
    out("writes (x64): header, rooms, CONFIG.DAT == raw bytes\n");
}

// ----------------------------------------------------------------------------- reads == the raw calls
// x86: each object read field-wise == read raw (same prefill), and == the object written (where read).
void read_markers(MemFile& f, MemFile& r) {
    u32 m = 0, mr = 0;
    sv::In in(port(f), 4, "marker", &m);
    m = in.u32();
    check(in.end() == 1, "marker read");
    mf_read(&r, &mr, 4);
    check(m == 0x2facebee && mr == m, "marker");
}
template <class T>
void prefill_pair(T& raw, T& fw) {
    fill(&raw, sizeof raw);
    std::memcpy(&fw, &raw, sizeof fw);
}
void test_reads() {
    if (!sv::kX86Layout) return;
    MemFile& f = g_files[1];
    MemFile& r = g_files[0];
    build_sav(f, false);
    build_sav_raw(r);
    f.pos = r.pos = 0;
    g_compared = g_mismatch = 0;
    for (int i = 0; i < kN; ++i) {
        jo::GameFile gr, gw;
        prefill_pair(gr, gw);
        for (const HeaderChunk& c : kHeader) mf_read(&r, bytes(&gr) + c.off, c.rsize);
        get_header(f, gw, true);
        check(std::memcmp(&gr, &gw, sizeof gr) == 0, "header read == raw", static_cast<u32>(i));
        check(std::memcmp(bytes(&gw) + 0x108, bytes(&g_gf[i]) + 0x108, 0x1a7 - 0x108) == 0 &&
                  bytes(&gw)[2] == bytes(&g_gf[i])[2],
              "header round trip", static_cast<u32>(i));
        read_markers(f, r);
        jo::Wnd wr, ww;
        prefill_pair(wr, ww);
        for (const WndChunk& c : kWnd) {
            mf_read(&r, bytes(&wr) + c.off, c.rec->size);
            if (c.off == 0x4a) wr.u8_4a = 1;
        }
        get_wnd(f, ww, true);
        check(std::memcmp(&wr, &ww, sizeof wr) == 0, "step 2 read == raw", static_cast<u32>(i));
        check(std::memcmp(&ww.speed, &g_wnd[i].speed, 4) == 0 && ww.i16_48 == g_wnd[i].i16_48 &&
                  ww.game_time == g_wnd[i].game_time && ww.u8_4a == 1,
              "step 2 round trip", static_cast<u32>(i));
        read_markers(f, r);
        jo::Profile pr, pw;
        prefill_pair(pr, pw);
        mf_read(&r, &pr, 0x48);
        mf_read(&r, &pr, 0x20);
        get_profile_head(f, pw, true);
        check(std::memcmp(&pr, &pw, sizeof pr) == 0, "profile read == raw", static_cast<u32>(i));
        check(std::memcmp(&pw, &g_prof[i], sizeof pw) == 0, "profile round trip", static_cast<u32>(i));
        jo::ProfileUnit ur, uw;
        prefill_pair(ur, uw);
        mf_read(&r, &ur, 0x20);
        get_unit_key(f, uw, true);
        check(std::memcmp(&ur, &uw, sizeof ur) == 0, "unit key read == raw", static_cast<u32>(i));
        mf_read(&r, &ur, 0x77);
        get_unit(f, uw, true);
        check(std::memcmp(&ur, &uw, sizeof ur) == 0 && std::memcmp(&uw, &g_unit[i], sizeof uw) == 0,
              "profile unit read == raw, round trip", static_cast<u32>(i));
        const i32 n = get_long(f);
        check(n == get_long(r) && n == g_room_n[i], "room count");
        if (n > 0) {
            jo::EmptyRoom rr[9], rw[9];
            prefill_pair(rr, rw);
            mf_read(&r, rr, static_cast<u32>(n * 8));
            get_rooms(f, rw, n, true);
            check(std::memcmp(rr, rw, sizeof rr) == 0 && std::memcmp(rw, g_rooms[i], static_cast<u32>(n * 8)) == 0,
                  "rooms read == raw, round trip", static_cast<u32>(i));
        }
    }
    check(f.pos == f.len, ".SAV stream consumed", f.pos, f.len);
    MemFile& p = g_files[1];
    MemFile& q = g_files[0];
    build_plain(p, false);
    build_plain_raw(q);
    p.pos = q.pos = 0;
    for (int i = 0; i < kN; ++i) {
        static ConfigBuf cr, cw;
        prefill_pair(cr, cw);
        mf_read(&q, &cr, kConfigSize);
        get_config(p, cw, true);
        check(std::memcmp(cr.b, cw.b, kConfigSize) == 0 && std::memcmp(cw.b, g_cfg[i].b, kConfigSize) == 0,
              "config read == raw, round trip", static_cast<u32>(i));
        jo::Profile pr, pw;
        prefill_pair(pr, pw);
        mf_read(&q, &pr, 0x48);
        mf_read(&q, &pr, 0x20);
        get_profile_head(p, pw, true);
        check(std::memcmp(&pr, &pw, sizeof pr) == 0, "plain profile read == raw", static_cast<u32>(i));
        jo::ProfileUnit ur, uw;
        prefill_pair(ur, uw);
        mf_read(&q, &ur, 0x20);
        get_unit_key(p, uw, true);
        check(std::memcmp(&ur, &uw, sizeof ur) == 0, "plain unit key read == raw", static_cast<u32>(i));
    }
    check(g_compared > 20 * kN && g_mismatch == 0, "read SaveCompare silent", static_cast<u32>(g_compared),
          static_cast<u32>(g_mismatch));
    out("reads: every object == the raw read (same prefill) and == the written object\n");
}

// --------------------------------------------------------------------------------------------- golden
// [u32 .SAV length][.SAV stream][plain stream], written by the i686 build.
u32 build_golden(MemFile& g) {
    MemFile& s = g_files[0];
    MemFile& p = g_files[1];
    build_sav(s, false);
    build_plain(p, false);
    reset(g, false);
    raw_put(g, &s.len, 4);
    raw_put(g, s.data, s.len);
    raw_put(g, p.data, p.len);
    return g.len;
}

// Decoded on x64 and re-encoded: the chunk bytes are the golden ones, pointer fields compared as null / non-null.
bool same_chunk(const sv::Record* r, const u8* g, const u8* n, u32 size) {
    if (r == nullptr) return std::memcmp(g, n, size) == 0;
    for (u32 off = 0; off < size; ++off) {
        const sv::Field* fl = sv::field_at(*r, off);
        if (fl != nullptr && fl->type == sv::Type::Pointer) {
            u32 a, b;
            std::memcpy(&a, g + off, 4);
            std::memcpy(&b, n + off, 4);
            if ((a == 0) != (b == 0)) return false;
            if (!sv::kX86Layout && b != 0 && (b & 1u) == 0) return false;  // x64 token
            off += 3;
            continue;
        }
        if (g[off] != n[off]) return false;
    }
    return true;
}
int g_tokens = 0;
struct Golden {
    MemFile& g;
    int chunks = 0;
    // The golden chunk at the current position (vary: after its u16 prefix).
    const u8* at() const { return g.data + g.pos + (g.vary ? 2 : 0); }
};
template <class Dec, class Enc>
void round(Golden& G, const sv::Record* r, u32 size, Dec dec, Enc enc) {
    const u8* const gold = G.at();
    dec();
    MemFile& tmp = g_files[3];
    reset(tmp, false);
    enc(tmp);
    check(tmp.len == size && same_chunk(r, gold, tmp.data, size), "golden chunk re-encoded", size,
          static_cast<u32>(G.chunks));
    if (r != nullptr)
        for (u32 i = 0; i < r->count; ++i)
            if (r->fields[i].type == sv::Type::Pointer) ++g_tokens;
    ++G.chunks;
}
void check_golden(const char* path) {
    MemFile& g = g_files[2];
    const long n = load_file(path, g.data, sizeof g.data);
    check(n > 4, "golden file readable");
    if (n <= 4) return;
    g.len = static_cast<u32>(n);
    u32 sav_len;
    std::memcpy(&sav_len, g.data, 4);
    // the .SAV part
    MemFile& s = g_files[0];
    reset(s, true);
    std::memcpy(s.data, g.data + 4, sav_len);
    s.len = sav_len;
    Golden G{s};
    auto one = [&](const sv::Record* r, u32 size, auto get, auto put) {
        round(G, r, size, [&] { get(s); }, [&](MemFile& t) { put(t); });
    };
    int objects = 0;
    while (s.pos < s.len) {
        jo::GameFile gf;
        jo::Wnd w;
        jo::Profile p;
        jo::ProfileUnit u;
        fill(&gf, sizeof gf);
        fill(&w, sizeof w);
        fill(&p, sizeof p);
        fill(&u, sizeof u);
        for (const HeaderChunk& c : kHeader) {
            if (c.get == nullptr) {  // written 4, read 1: re-encode the read byte, compare the first byte
                const u8 b0 = G.at()[0];
                sv::In in(port(s), 1, "GameFileFlags");
                gf.flags = (gf.flags & ~0xffu) | in.u8();
                in.end();
                check((gf.flags & 0xffu) == b0, "golden flags byte");
                continue;
            }
            one(c.rec, c.rec->size, [&](MemFile& m) {
                sv::In in(port(m), *c.rec);
                c.get(in, gf);
                in.end();
            }, [&](MemFile& t) {
                sv::Out o(port(t), *c.rec);
                c.put(o, gf);
                o.end();
            });
        }
        auto marker = [&] {
            u32 m = 0;
            sv::In in(port(s), 4, "marker");
            m = in.u32();
            in.end();
            check(m == 0x2facebee, "golden marker");
        };
        marker();
        for (const WndChunk& c : kWnd)
            one(c.rec, c.rec->size, [&](MemFile& m) {
                sv::In in(port(m), *c.rec);
                c.get(in, w);
                in.end();
            }, [&](MemFile& t) {
                sv::Out o(port(t), *c.rec);
                c.put(o, w);
                o.end();
            });
        marker();
        one(&sv::kProfile, 0x48, [&](MemFile& m) {
            sv::In in(port(m), sv::kProfile);
            sv::get_Profile(in, p);
            in.end();
        }, [&](MemFile& t) {
            sv::Out o(port(t), sv::kProfile);
            sv::put_Profile(o, p);
            o.end();
        });
        one(&sv::kProfileName, 0x20, [&](MemFile& m) {
            sv::In in(port(m), sv::kProfileName);
            sv::get_ProfileName(in, p);
            in.end();
        }, [&](MemFile& t) {
            sv::Out o(port(t), sv::kProfileName);
            sv::put_ProfileName(o, p);
            o.end();
        });
        one(nullptr, 0x20, [&](MemFile& m) { get_unit_key(m, u, false); }, [&](MemFile& t) { put_unit_key(t, u, false); });
        one(&sv::kProfileUnit, 0x77, [&](MemFile& m) { get_unit(m, u, false); }, [&](MemFile& t) { put_unit(t, u, false); });
        const i32 rooms = get_long(s);
        if (rooms > 0) {
            jo::EmptyRoom rr[9];
            one(nullptr, static_cast<u32>(rooms * 8), [&](MemFile& m) { get_rooms(m, rr, rooms, false); },
                [&](MemFile& t) { put_rooms(t, rr, rooms, false); });
        }
        ++objects;
    }
    // the plain part (profiles, CONFIG.DAT)
    MemFile& p = g_files[1];
    reset(p, false);
    std::memcpy(p.data, g.data + 4 + sav_len, g.len - 4 - sav_len);
    p.len = g.len - 4 - sav_len;
    Golden P{p};
    auto plain = [&](const sv::Record* r, u32 size, auto get, auto put) {
        round(P, r, size, [&] { get(p); }, [&](MemFile& t) { put(t); });
    };
    while (p.pos < p.len) {
        static ConfigBuf c;
        jo::Profile pr;
        jo::ProfileUnit u;
        plain(nullptr, kConfigSize, [&](MemFile& m) { get_config(m, c, false); }, [&](MemFile& t) { put_config(t, c, false); });
        plain(&sv::kProfile, 0x48, [&](MemFile& m) {
            sv::In in(port(m), sv::kProfile);
            sv::get_Profile(in, pr);
            in.end();
        }, [&](MemFile& t) {
            sv::Out o(port(t), sv::kProfile);
            sv::put_Profile(o, pr);
            o.end();
        });
        plain(&sv::kProfileName, 0x20, [&](MemFile& m) {
            sv::In in(port(m), sv::kProfileName);
            sv::get_ProfileName(in, pr);
            in.end();
        }, [&](MemFile& t) {
            sv::Out o(port(t), sv::kProfileName);
            sv::put_ProfileName(o, pr);
            o.end();
        });
        plain(nullptr, 0x20, [&](MemFile& m) { get_unit_key(m, u, false); }, [&](MemFile& t) { put_unit_key(t, u, false); });
    }
    check(objects == kN, "golden objects", static_cast<u32>(objects));
    out("golden: ");
    dec(static_cast<u32>(G.chunks + P.chunks));
    out(" chunks decoded and re-encoded (");
    dec(static_cast<u32>(g_tokens));
    out(" pointer fields as null / non-null tokens), ");
    dec(g.len);
    out(" bytes\n");
}

// Spot checks of the decoded values on the target's own members (x64: other offsets than the file's).
void test_members() {
    MemFile& f = g_files[0];
    reset(f, true);
    jo::Wnd w{};
    w.game_time = 0x11223344u;
    w.i16_48 = -2;
    w.u8_4a = 7;
    const u32 sp = 0x7f800001u;  // a signalling NaN: bits kept
    std::memcpy(&w.speed, &sp, 4);
    put_wnd(f, w, false);
    check(f.len == 4 * 2 + 4 + 2 + 1 + 4, "step 2 size", f.len);
    check(f.data[2] == 0x01 && f.data[5] == 0x7f && f.data[8] == 0xfe && f.data[9] == 0xff && f.data[12] == 7 &&
              f.data[15] == 0x44 && f.data[18] == 0x11,
          "step 2 bytes (x86 file order: speed, i16_48, u8_4a, game_time)");
    f.pos = 0;
    jo::Wnd r;
    fill(&r, sizeof r);
    get_wnd(f, r, false);
    check(r.game_time == w.game_time && r.i16_48 == -2 && r.u8_4a == 1 && std::memcmp(&r.speed, &sp, 4) == 0,
          "step 2 decoded into the members");
    reset(f, false);
    jo::Profile p{};
    std::memcpy(p.name, "Jeanne", 7);
    p.unit_count = 3;
    p.units = reinterpret_cast<jo::ProfileUnit*>(static_cast<uptr>(0x1000));
    p.list_a = nullptr;
    p.list_b_count = 5;
    p.i32_44 = -9;
    put_profile_head(f, p, false);
    check(f.len == 0x68, "profile head size", f.len);
    u32 v;
    std::memcpy(&v, f.data + 0x24, 4);
    check(v != 0 && (sv::kX86Layout ? v == 0x1000u : (v & 1u) == 1u), "units token at +0x24", v);
    std::memcpy(&v, f.data + 0x2c, 4);
    check(v == 0, "null list_a at +0x2c", v);
    std::memcpy(&v, f.data + 0x30, 4);
    check(v == 5, "list_b_count at +0x30", v);
    std::memcpy(&v, f.data + 0x44, 4);
    check(v == static_cast<u32>(-9), "i32_44 at +0x44", v);
    check(std::memcmp(f.data + 0x48, "Jeanne", 7) == 0, "name again at +0x48");
    f.pos = 0;
    jo::Profile q;
    fill(&q, sizeof q);
    get_profile_head(f, q, false);
    check(q.unit_count == 3 && q.units != nullptr && q.list_a == nullptr && q.list_b_count == 5 && q.i32_44 == -9 &&
              std::strcmp(q.name, "Jeanne") == 0,
          "profile decoded into the members");
    out("members: step 2 / Profile land on the target's members, pointers as tokens\n");
}

}  // namespace

int test_main(int argc, char** argv) {
    install();
    out(sv::kX86Layout ? "save test s5, x86 layout (4-byte pointers)\n" : "save test s5, x64 layout (8-byte pointers)\n");
    random_objects();
    test_writes();
    test_reads();
    test_members();
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
    check(g_misuse == 0, "no misuse", static_cast<u32>(g_misuse));
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
