// Offline proof of batch S4 (re/a6/SAVES_PLAN.md 5.1, campaign records): the generated records of section s4
// (src/save/gen/records_s4.hpp) and the hand-made step 17 / 19 chunks (src/save/records_s4.hpp) on the game's real
// typed structs (src/original/gen: Game, Scenario, ScenarioEntry, EndCon, BattleState, Nation, Rel, NewsItem,
// ZoneDef, Trigger, MsgEntry, Hud), against the old raw path, through a memory file with File's vary-size framing
// (the transcription of test_save.cpp).
//
// Built twice by build_s4.sh: freestanding i686 Linux (the x86 layout, like joan_re.exe) and native x86_64 Linux.
//   i686:   every chunk == the object's raw bytes (memcpy of [0, size) / the Hud ranges), the stream == the raw
//           writes' stream, every load == the raw read (same prefilled object, same result), SaveCompare silent;
//           --write-golden FILE writes the chunk stream.
//   x86_64: put -> get -> put reproduces the chunk (pointer tokens are stable); --check-golden FILE decodes the i686
//           stream and re-encodes it: byte for byte equal except pointer fields, which keep their null-ness (a
//           non-null x86 value v comes back as the token v | 1, SAVES_PLAN 5.6).
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "original/gen/battle_state.hpp"
#include "original/gen/game.hpp"
#include "original/gen/hud.hpp"
#include "original/gen/msg_entry.hpp"
#include "original/gen/nation.hpp"
#include "original/gen/news_item.hpp"
#include "original/gen/trigger.hpp"
#include "original/gen/zone_def.hpp"
#include "save/io.hpp"
#include "save/records_s4.hpp"

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
void check(bool ok, const char* what, const char* rec = "", u32 a = 0, u32 b = 0) {
    ++g_checks;
    if (ok) return;
    if (++g_failed > 40) return;
    out("FAIL ");
    out(what);
    out(" ");
    out(rec);
    out(" ");
    hex(a);
    out(" ");
    hex(b);
    out("\n");
}

u32 g_rng = 0x9e3779b9u;
u32 rnd() {
    g_rng ^= g_rng << 13;
    g_rng ^= g_rng >> 17;
    g_rng ^= g_rng << 5;
    return g_rng;
}
// Random bytes; now and then a run of zeros (null pointers) or a special float pattern at a random dword.
void fill(void* p, u32 n) {
    auto* b = static_cast<u8*>(p);
    for (u32 i = 0; i < n; ++i) b[i] = static_cast<u8>(rnd());
    static const u32 kSpecial[] = {0, 0, 0, 0x7f800001u, 0xff800001u, 0x7fc00000u, 0x7f800000u, 0x00000001u, 0x80000000u};
    for (u32 k = 0; k < n / 16 && n >= 4; ++k) {
        const u32 off = rnd() % (n - 3);
        const u32 v = kSpecial[rnd() % 9];
        std::memcpy(b + off, &v, 4);
    }
}

// ------------------------------------------------------------------------- memory File (src/ilib/file.cpp)
struct MemFile {
    u8 data[1u << 22];
    u32 len, pos;
};
MemFile g_files[3];

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
// File::file_write 0x005872e0 with allow_vary_size = 1.
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

// ---------------------------------------------------------------------------------------------- records
// One chunk kind under test: a generated record (put_<R> / get_<R> on the real struct), a Hud chunk or the
// trigger list byte. obj_off: the chunk's first byte in the object (x86 layout).
struct Rt {
    const char* name;
    const sv::Record* rec;
    u32 obj_size;  // sizeof the object (this target)
    u32 obj_off;
    void (*put)(sv::Out&, const void*);
    void (*get)(sv::In&, void*);
};
template <class T, void (*P)(sv::Out&, const T&)>
void put_w(sv::Out& o, const void* p) {
    P(o, *static_cast<const T*>(p));
}
template <class T, void (*G)(sv::In&, T&)>
void get_w(sv::In& i, void* p) {
    G(i, *static_cast<T*>(p));
}
#define REC(R, T) {#R, &sv::k##R, sizeof(T), 0, &put_w<T, &sv::put_##R<T>>, &get_w<T, &sv::get_##R<T>>}
template <u32 K>
void put_hud(sv::Out& o, const void* p) {
    sv::put_HudLog(o, K, *static_cast<const jo::Hud*>(p));
}
template <u32 K>
void get_hud(sv::In& i, void* p) {
    sv::get_HudLog(i, K, *static_cast<jo::Hud*>(p));
}
#define HUD(K) {sv::kHudLog[K].rec.name, &sv::kHudLog[K].rec, sizeof(jo::Hud), sv::kHudLog[K].off, &put_hud<K>, &get_hud<K>}
void put_byte(sv::Out& o, const void* p) { o.u8(static_cast<const u8*>(p)[0x1d]); }
void get_byte(sv::In& i, void* p) { static_cast<u8*>(p)[0x1d] = i.u8(); }

const Rt kRecs[] = {
    REC(Game, jo::Game),
    REC(Scenario, jo::Scenario),
    REC(ScenarioEntry, jo::ScenarioEntry),
    REC(EndCon, jo::EndCon),
    REC(BattleState, jo::BattleState),
    REC(Nation, jo::Nation),
    REC(Rel, jo::Rel),
    REC(NewsItem, jo::NewsItem),
    REC(ZoneDef, jo::ZoneDef),
    REC(Trigger, jo::Trigger),
    REC(MsgEntry, jo::MsgEntry),
    HUD(0), HUD(1), HUD(2), HUD(3), HUD(4), HUD(5), HUD(6), HUD(7), HUD(8),
    {"Triggers+0x1d", &sv::kTriggersByte1d, 0x20, 0x1d, &put_byte, &get_byte},
};
constexpr int kNumRecs = sizeof kRecs / sizeof kRecs[0];
constexpr int kN = 48;            // objects per record kind
constexpr u32 kMaxObj = 0x4000;   // >= sizeof every object on both targets
alignas(8) u8 g_obj[kN][kMaxObj];
alignas(8) u8 g_a[kMaxObj];
alignas(8) u8 g_b[kMaxObj];

// The pointer fields of a record keep null-ness, everything else is exact.
bool same_chunk(const sv::Record& r, const u8* a, const u8* b, u32* bad) {
    for (u32 i = 0; i < r.count; ++i) {
        const sv::Field& f = r.fields[i];
        if (f.type == sv::Type::Pointer) {
            for (u32 e = 0; e < f.size; e += 4) {
                u32 x, y;
                std::memcpy(&x, a + f.off + e, 4);
                std::memcpy(&y, b + f.off + e, 4);
                if ((x == 0) != (y == 0)) return *bad = f.off + e, false;
            }
        } else if (std::memcmp(a + f.off, b + f.off, f.size) != 0) {
            return *bad = f.off, false;
        }
    }
    return true;
}
// The field tables tile the chunk (the generator guarantees it; the hand tables are checked here).
void test_tables() {
    for (const Rt& t : kRecs) {
        u32 pos = 0;
        for (u32 i = 0; i < t.rec->count; ++i) {
            check(t.rec->fields[i].off == pos, "field table tiles the chunk", t.name, pos, t.rec->fields[i].off);
            pos = t.rec->fields[i].off + t.rec->fields[i].size;
        }
        check(pos == t.rec->size, "field table ends at the chunk size", t.name, pos, t.rec->size);
        check(t.obj_size <= kMaxObj, "object fits the test buffer", t.name, t.obj_size);
    }
}

// i686: chunk == raw bytes, stream == raw stream, load == raw read; x86_64: put -> get -> put stable.
void test_records() {
    for (const Rt& t : kRecs) {
        MemFile& a = g_files[0];
        MemFile& b = g_files[1];
        reset(a);
        reset(b);
        g_compared = g_mismatch = g_misuse = 0;
        const u32 n = t.rec->size;
        for (int k = 0; k < kN; ++k) {
            fill(g_obj[k], t.obj_size);
            const u8* const raw = g_obj[k] + t.obj_off;
            mf_write(&a, raw, n);  // old: file_write(file, obj + off, n)
            sv::Out o(port(b), *t.rec, sv::kX86Layout ? raw : nullptr);
            t.put(o, g_obj[k]);
            if (sv::kX86Layout) check(std::memcmp(o.data(), raw, n) == 0, "chunk == object bytes", t.name, static_cast<u32>(k));
            check(o.end() == 1, "end", t.name);
        }
        if (sv::kX86Layout) {
            check(a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0, "stream == raw stream", t.name, a.len, b.len);
            check(g_compared == kN && g_mismatch == 0, "SaveCompare silent (write)", t.name, static_cast<u32>(g_compared),
                  static_cast<u32>(g_mismatch));
        }
        // read back: raw read and field-wise read into the same prefilled object
        a.pos = b.pos = 0;
        g_compared = 0;
        for (int k = 0; k < kN; ++k) {
            fill(g_a, t.obj_size);
            std::memcpy(g_b, g_a, t.obj_size);
            mf_read(&a, g_a + t.obj_off, n);  // old: file_read(file, obj + off, n)
            sv::In in(port(b), *t.rec, sv::kX86Layout ? g_b + t.obj_off : nullptr);
            t.get(in, g_b);
            check(in.end() == 1, "read result", t.name);
            if (sv::kX86Layout) {
                check(std::memcmp(g_a, g_b, t.obj_size) == 0, "load == raw read", t.name, static_cast<u32>(k));
            } else {  // x64: re-encode the loaded object, compare with the stored chunk
                sv::Out o(port(g_files[2]), *t.rec);
                t.put(o, g_b);
                u32 bad = 0;
                check(same_chunk(*t.rec, o.data(), b.data + b.pos - n, &bad), "x64 put(get(chunk)) == chunk", t.name, bad);
                // a token comes back as itself (x64 put -> get -> put is byte-stable)
                u8 first[sv::kMaxChunk];
                std::memcpy(first, o.data(), n);
                o.end();
                reset(g_files[2]);
                {
                    sv::Out p(port(g_files[2]), *t.rec);
                    t.put(p, g_b);
                    p.end();
                }
                g_files[2].pos = 0;
                fill(g_a, t.obj_size);
                sv::In again(port(g_files[2]), *t.rec);
                t.get(again, g_a);
                again.end();
                sv::Out q(port(g_files[2]), *t.rec);
                t.put(q, g_a);
                check(std::memcmp(q.data(), first, n) == 0, "x64 tokens stable", t.name);
                q.end();
                reset(g_files[2]);
            }
        }
        if (sv::kX86Layout)
            check(g_compared == kN && g_mismatch == 0, "SaveCompare silent (read)", t.name, static_cast<u32>(g_compared),
                  static_cast<u32>(g_mismatch));
        check(g_misuse == 0, "no misuse", t.name, static_cast<u32>(g_misuse));
    }
    out("records: ");
    dec(static_cast<u32>(kNumRecs));
    out(" chunk kinds x ");
    dec(kN);
    out(sv::kX86Layout ? " objects: chunks == raw bytes, loads == raw reads\n" : " objects: put/get round trips stable\n");
}

// Pointer policies of the records: a null pointer is written as 0 and read back as null, a non-null one as non-0.
void test_pointers() {
    jo::Game g;
    fill(&g, sizeof g);
    g.word_set = nullptr;
    g.scenario.local.name = nullptr;
    g.end_conditions[3].text = nullptr;
    alignas(4) static u8 heap_like[16];
    g.word_stack = reinterpret_cast<jo::ArrI16*>(heap_like + 2);  // even address: x64 token = low | 1
    g.end_conditions[3].text2 = reinterpret_cast<char*>(heap_like + 4);
    MemFile& f = g_files[2];
    reset(f);
    {
        sv::Out o(port(f), sv::kGame);
        sv::put_Game(o, g);
        o.end();
    }
    auto word = [&](u32 off) {
        u32 v;
        std::memcpy(&v, f.data + 2 + off, 4);
        return v;
    };
    check(word(0x71c) == 0 && word(0x17 + 0x14 + 0x19e) == 0 && word(0x30b + 3 * 0x1a + 1) == 0, "null -> 0");
    const u32 low = static_cast<u32>(reinterpret_cast<std::uintptr_t>(heap_like + 2));
    check(word(0x728) == (sv::kX86Layout ? low : (low | 1u)), "pointer -> raw (x86) / token (x64)", "", word(0x728), low);
    check(word(0x30b + 3 * 0x1a + 5) != 0, "non-null -> non-0");
    f.pos = 0;
    jo::Game h;
    fill(&h, sizeof h);
    sv::In in(port(f), sv::kGame);
    sv::get_Game(in, h);
    check(in.end() == 1, "pointer read");
    check(h.word_set == nullptr && h.scenario.local.name == nullptr && h.end_conditions[3].text == nullptr,
          "0 -> null");
    check(h.word_stack != nullptr && h.end_conditions[3].text2 != nullptr, "non-0 -> non-null");
    out("pointers: null iff 0, raw on x86, tokens on x64\n");
}

// The golden stream: every record kind's chunks of test_records' objects, in kRecs order.
u32 build_golden(MemFile& g) {
    reset(g);
    for (int k = 0; k < kN; ++k) {
        for (const Rt& t : kRecs) {
            fill(g_a, t.obj_size);
            sv::Out o(port(g), *t.rec);
            t.put(o, g_a);
            o.end();
        }
    }
    return g.len;
}

void check_golden(const char* path) {
    MemFile& g = g_files[0];
    MemFile& r = g_files[1];
    const long n = load_file(path, g.data, sizeof g.data);
    check(n > 0, "golden file readable");
    if (n <= 0) return;
    g.len = static_cast<u32>(n);
    g.pos = 0;
    reset(r);
    u32 chunks = 0, pointers = 0;
    while (g.pos < g.len) {
        for (const Rt& t : kRecs) {
            const u32 at = g.pos + 2;
            fill(g_a, t.obj_size);
            sv::In in(port(g), *t.rec);
            t.get(in, g_a);
            check(in.end() == 1, "golden read", t.name);
            sv::Out o(port(r), *t.rec);
            t.put(o, g_a);
            u32 bad = 0;
            check(same_chunk(*t.rec, o.data(), g.data + at, &bad), "golden re-encoded == golden", t.name, bad);
            if (std::memcmp(o.data(), g.data + at, t.rec->size) != 0) ++pointers;
            o.end();
            ++chunks;
        }
    }
    check(r.len == g.len, "golden stream length", "", r.len, g.len);
    out("golden: ");
    dec(chunks);
    out(" chunks decoded and re-encoded (");
    dec(g.len);
    out(" bytes), ");
    dec(pointers);
    out(" differ only in pointer tokens\n");
    if (sv::kX86Layout) check(pointers == 0, "x86 re-encode is exact", "", pointers);
}

}  // namespace

int test_main(int argc, char** argv) {
    install();
    out(sv::kX86Layout ? "save test S4, x86 layout (4-byte pointers)\n" : "save test S4, x64 layout (8-byte pointers)\n");
    test_tables();
    test_records();
    test_pointers();
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
