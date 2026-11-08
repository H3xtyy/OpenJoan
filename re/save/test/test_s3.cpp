// Offline proof of batch S3 (re/a6/SAVES_PLAN.md 5.1): the generated world records of section s3
// (src/save/gen/records_s3.hpp + src/save/records_s3.hpp) on the game's real typed structs (World, Scene, Cell,
// SceneNode (SceneS), Player, CamPose, LightSys from src/original/gen), through a memory file with File's vary-size framing.
//
// Built twice by build_s3.sh: freestanding i686 Linux (the x86 layout, like joan_re.exe) and native x86_64 Linux.
//   i686:   every chunk == the old raw bytes of the object range (Player: except +0x1261, which goes out as the
//           original address - D1), every load == the raw read, SaveCompare silent; --write-golden FILE.
//   both:   round trip: get_<R>(chunk) then put_<R> gives the chunk again; --check-golden FILE decodes the i686
//           stream and re-encodes it byte for byte (the x64 path writes x86 bytes without a game).
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "original/gen/cam_pose.hpp"
#include "original/gen/cell.hpp"
#include "original/gen/light_sys.hpp"
#include "original/gen/player.hpp"
#include "original/gen/scene.hpp"
#include "original/gen/scene_s.hpp"
#include "original/gen/world.hpp"
#include "save/io.hpp"
#include "save/records_s3.hpp"

namespace jo = joan::orig;
namespace sv = joan::save;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using uptr = std::uintptr_t;

// ------------------------------------------------------------------------------------------------- platform
#ifdef SAVE_TEST_FREESTANDING
namespace {
long sys(long n, long a, long b, long c) {
    long r;
    __asm__ volatile("int $0x80" : "=a"(r) : "a"(n), "b"(a), "c"(b), "d"(c) : "memory");
    return r;
}
void out(const char* s) { sys(4, 1, reinterpret_cast<long>(s), static_cast<long>(std::strlen(s))); }
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

u32 g_rng = 0x6d2b79f5u;
u32 rnd() {
    g_rng ^= g_rng << 13;
    g_rng ^= g_rng >> 17;
    g_rng ^= g_rng << 5;
    return g_rng;
}
void fill(void* p, std::size_t n) {
    auto* b = static_cast<u8*>(p);
    for (std::size_t i = 0; i < n; ++i) b[i] = static_cast<u8>(rnd());
}
// Signalling / quiet NaNs, infinities, denormals, -0 for float members now and then.
float odd_float() {
    static const u32 kFloats[] = {0x7f800001u, 0xff800001u, 0x7fc00000u, 0x7f800000u, 0x00000001u, 0x80000000u};
    return std::bit_cast<float>(kFloats[rnd() % 6]);
}
template <class P>
void maybe_null(P*& p) {
    if (rnd() % 4 == 0) p = nullptr;
}

// ------------------------------------------------------------------------- memory File (src/ilib/file.cpp)
struct MemFile {
    u8 data[1u << 22];
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

// ----------------------------------------------------------------- image addresses (src/image.hpp)
constexpr u32 kImgVa = 0x00a40000, kImgSize = 0x10000;
alignas(16) u8 g_img[kImgSize];
uptr img_base() { return reinterpret_cast<uptr>(g_img); }
u32 image_out(uptr v) {
    if (v - img_base() < kImgSize) return kImgVa + static_cast<u32>(v - img_base());
    return static_cast<u32>(v);
}
uptr image_in(u32 v) {
    if (v - kImgVa < kImgSize) return img_base() + (v - kImgVa);
    return v;
}

// ---------------------------------------------------------------------------------------------- hooks
int g_compared = 0, g_mismatch = 0, g_misuse = 0;
bool yes() { return true; }
bool no() { return false; }
void on_event(const sv::Event& e) {
    if (e.kind == sv::Event::Compared) ++g_compared;
    if (e.kind == sv::Event::Mismatch) ++g_mismatch;
    if (e.kind == sv::Event::Misuse) ++g_misuse;
}
void words_unused(void*, const void*, u32) {}
void words_in_unused(void*, u32) {}
void install() {
    sv::Hooks& h = sv::hooks();
    h.fieldwise = &yes;
    h.compare = &yes;
    h.audit = &no;
    h.image_out = &image_out;
    h.image_in = &image_in;
    h.legacy_out = &words_unused;  // s3 has no ImageWords records
    h.legacy_in = &words_in_unused;
    h.event = &on_event;
}

// ------------------------------------------------------------------------------------- test objects
void rand_world(jo::World& w) {
    fill(&w, sizeof w);
    for (float& f : w.bounds)
        if (rnd() % 3 == 0) f = odd_float();
    maybe_null(w.scene);
    maybe_null(w.cells);
    maybe_null(w.bitmaps.entries);
}
void rand_scene(jo::Scene& s) {
    fill(&s, sizeof s);
    if (rnd() % 3 == 0) s.light_pick.best = odd_float();
    maybe_null(s.sky);
    maybe_null(s.camera);
}
void rand_cell(jo::Cell& c) {
    fill(&c, sizeof c);
    if (rnd() % 3 == 0) c.box[0] = odd_float();
    maybe_null(c.world);
    maybe_null(c.sectors);
}
void rand_node(jo::SceneS& n) {  // the 0xe4-byte SceneNode record is the sector SceneS (round P8)
    fill(&n, sizeof n);
    if (rnd() % 3 == 0) n.inner_box[0] = odd_float();
    maybe_null(n.node);
    maybe_null(n.nodes_head);
}
int g_image_words = 0;  // Player +0x1261 values inside the (fake) data image: D1 cases
void rand_player(jo::Player& p) {
    fill(&p, sizeof p);
    maybe_null(p.cam_zones);
    if (rnd() % 3 == 0) p.f32_126a = odd_float();
    switch (rnd() % 4) {
    case 0:  // the joan_image address of 0x00a48f50 (x86: fits the dword)
        if (sv::kX86Layout) p.u32_1261 = reinterpret_cast<jo::BmpRes*>(img_base() + 0x8f50);
        break;
    case 1: p.u32_1261 = reinterpret_cast<jo::BmpRes*>(std::uintptr_t{0x00a48f50u}); break;  // the original address (a save of joan.exe)
    case 2: p.u32_1261 = nullptr; break;
    default: break;
    }
}
void rand_cam(jo::CamPose& c) {
    fill(&c, sizeof c);
    if (rnd() % 3 == 0) c.eye[1] = odd_float();
}
void rand_light(jo::LightSys& l) {
    fill(&l, sizeof l);
    if (rnd() % 3 == 0) l.event_a_rate = odd_float();
    if (rnd() % 3 == 0) l.current.params.sun_dir[2] = odd_float();
    if (rnd() % 3 == 0) l.current.lights[3].colour[1] = odd_float();
}

// The bytes the old raw path wrote: the object range (x86); Player with D1 applied (= save::player_legacy).
template <class T>
const u8* range(const T& x, u32 from) {
    return reinterpret_cast<const u8*>(&x) + from;
}

// One record type: N random objects written field-wise (and raw), read back (and raw), re-encoded.
template <class T>
struct Case {
    const char* name;
    const sv::Record& rec;
    u32 from;  // x86 offset of the chunk in T
    void (*put)(sv::Out&, const T&);
    void (*get)(sv::In&, T&);
    void (*rand)(T&);
    void (*keep)(T& y, const u8* chunk);  // the reader's Kept fields (x64): what the game keeps in memory, or nullptr
    bool player;                            // D1 at +0x1261
    int n;
};

alignas(16) u8 g_obj[2][0x8000];  // one T at a time (largest: Player)

template <class T>
T& obj(int k) {
    static_assert(sizeof(T) <= sizeof g_obj[0]);
    return *reinterpret_cast<T*>(g_obj[k]);
}

// Writes the case's chunks to b (golden part); x86: also the raw stream to a and the byte identity checks.
template <class T>
void write_case(const Case<T>& c, MemFile& a, MemFile& b) {
    const u32 size = c.rec.size;
    g_compared = g_mismatch = 0;
    int d1 = 0;
    for (int i = 0; i < c.n; ++i) {
        T& x = obj<T>(0);
        c.rand(x);
        alignas(4) u8 expect[0x2000];
        std::memcpy(expect, range(x, c.from), size);
        const void* legacy = range(x, c.from);
        alignas(4) u8 old[0x126e];
        if (c.player) {
            legacy = sv::player_legacy(old, &x);
            std::memcpy(expect, old, size);
            d1 += std::memcmp(expect, range(x, c.from), size) != 0;
        }
        sv::Out o(port(b), c.rec, sv::kX86Layout ? legacy : nullptr);
        c.put(o, x);
        if (sv::kX86Layout) {
            check(std::memcmp(o.data(), expect, size) == 0, c.name, static_cast<u32>(i), 1);
            mf_write(&a, expect, size);  // old: file_write(file, obj + from, size) (Player: + D1)
        }
        check(o.end() == 1, "end", static_cast<u32>(i));
    }
    if (sv::kX86Layout) {
        check(a.len == b.len && std::memcmp(a.data, b.data, a.len) == 0, c.name, a.len, b.len);
        check(g_compared == c.n && g_mismatch == 0, "SaveCompare silent", static_cast<u32>(g_compared),
              static_cast<u32>(g_mismatch));
    }
    out(c.name);
    out(": ");
    dec(static_cast<u32>(c.n));
    out(" chunks of ");
    dec(size);
    out(sv::kX86Layout ? " bytes == raw" : " bytes");
    if (c.player && sv::kX86Layout) {
        out(", D1 at +0x1261 in ");
        dec(static_cast<u32>(d1));
        g_image_words += d1;
    }
    out("\n");
}

// A re-encoded chunk vs the chunk it was decoded from: bytes equal, except that a pointer field written by another
// build only has to agree in null / non-null (x86 writes the raw value, x64 the token low32 | 1; SAVES_PLAN 5.6).
int g_token_diffs = 0;
bool same_chunk(const sv::Record& rec, const u8* now, const u8* was, bool tokens) {
    if (!tokens) return std::memcmp(now, was, rec.size) == 0;
    for (u32 k = 0; k < rec.count; ++k) {
        const sv::Field& f = rec.fields[k];
        if (f.type == sv::Type::Pointer) {
            for (u32 e = 0; e < f.size; e += 4) {
                u32 x, y;
                std::memcpy(&x, now + f.off + e, 4);
                std::memcpy(&y, was + f.off + e, 4);
                if ((x == 0) != (y == 0)) return false;
                g_token_diffs += x != y;
            }
        } else if (std::memcmp(now + f.off, was + f.off, f.size) != 0) {
            return false;
        }
    }
    return true;
}

// Reads the case's chunks from b (x86: and the raw stream a: load == raw read) and re-encodes them to r.
// tokens: b was written by the other build (pointer fields compared as null / non-null).
template <class T>
void read_case(const Case<T>& c, MemFile& a, MemFile& b, MemFile& r, bool tokens = false) {
    const u32 size = c.rec.size;
    g_compared = g_mismatch = 0;
    for (int i = 0; i < c.n; ++i) {
        T& y = obj<T>(0);
        T& raw = obj<T>(1);
        fill(&y, sizeof y);
        std::memcpy(&raw, &y, sizeof raw);
        const u8* const chunk = b.data + b.pos + 2;
        {
            sv::In in(port(b), c.rec, sv::kX86Layout ? const_cast<u8*>(range(y, c.from)) : nullptr);
            c.get(in, y);
            check(in.end() == 1, "read result", static_cast<u32>(i));
        }
        if (sv::kX86Layout) {
            mf_read(&a, const_cast<u8*>(range(raw, c.from)), size);  // old: file_read(file, obj + from, size)
            check(std::memcmp(&raw, &y, sizeof raw) == 0, "load == raw read", static_cast<u32>(i));
        } else if (c.keep != nullptr) {
            c.keep(y, chunk);
        }
        sv::Out o(port(r), c.rec);
        c.put(o, y);
        check(same_chunk(c.rec, o.data(), chunk, tokens), "round trip", static_cast<u32>(i));
        o.end();
    }
    if (sv::kX86Layout)
        check(g_compared == c.n && g_mismatch == 0, "load SaveCompare silent", static_cast<u32>(g_compared),
              static_cast<u32>(g_mismatch));
}

// Cell::world is Ptr::Kept: on x64 In leaves the reader's own value; the test's "own value" is the saved one.
void keep_cell(jo::Cell& y, const u8* chunk) {
    u32 v;
    std::memcpy(&v, chunk, 4);
    y.world = reinterpret_cast<jo::World*>(static_cast<uptr>(v));
}

const Case<jo::World> kWorldCase{"World", sv::kWorld, 0, &sv::put_World<jo::World>, &sv::get_World<jo::World>,
                                 &rand_world, nullptr, false, 2000};
const Case<jo::Scene> kSceneCase{"Scene", sv::kScene, 0, &sv::put_Scene<jo::Scene>, &sv::get_Scene<jo::Scene>,
                                 &rand_scene, nullptr, false, 2000};
const Case<jo::Cell> kCellCase{"Cell", sv::kCell, 0, &sv::put_Cell<jo::Cell>, &sv::get_Cell<jo::Cell>, &rand_cell,
                               &keep_cell, false, 2000};
const Case<jo::SceneS> kNodeCase{"SceneNode", sv::kSceneNode, 0, &sv::put_SceneNode<jo::SceneS>,
                                 &sv::get_SceneNode<jo::SceneS>, &rand_node, nullptr, false, 2000};
const Case<jo::Player> kPlayerCase{"Player", sv::kPlayer, 0, &sv::put_Player<jo::Player>, &sv::get_Player<jo::Player>,
                                   &rand_player, nullptr, true, 400};
const Case<jo::CamPose> kCamCase{"Camera", sv::kCamera, 0, &sv::put_Camera<jo::CamPose>, &sv::get_Camera<jo::CamPose>,
                                 &rand_cam, nullptr, false, 2000};
#define LIGHT(R, from) \
    Case<jo::LightSys>{"LightSys" #R, sv::kLightSys##R, from, &sv::put_LightSys##R<jo::LightSys>, \
                       &sv::get_LightSys##R<jo::LightSys>, &rand_light, nullptr, false, 300}
const Case<jo::LightSys> kLightCases[] = {
    LIGHT(EventAChance, 0x58c), LIGHT(EventARate, 0x590), LIGHT(EventATime, 0x594), LIGHT(EventAState, 0x598),
    LIGHT(EventBChance, 0x59c), LIGHT(EventBRate, 0x5a0), LIGHT(EventBTime, 0x5a4), LIGHT(EventBState, 0x5a8),
    LIGHT(Current, 0xc),        LIGHT(StateA, 0x198),     LIGHT(StateB, 0x32c),
};
#undef LIGHT

// Runs every case: write (golden into g), then read back + re-encode; with golden_in, the reads decode that file.
template <class F>
void each_case(F f) {
    f(kWorldCase);
    f(kSceneCase);
    f(kCellCase);
    f(kNodeCase);
    f(kPlayerCase);
    f(kCamCase);
    for (const auto& c : kLightCases) f(c);
}

void test_all(MemFile& golden) {
    MemFile& a = g_files[0];
    MemFile& r = g_files[1];
    reset(golden);
    reset(a);
    each_case([&](const auto& c) { write_case(c, a, golden); });
    check(!sv::kX86Layout || g_image_words > 0, "test has D1 cases", static_cast<u32>(g_image_words));
    golden.pos = a.pos = 0;
    reset(r);
    each_case([&](const auto& c) { read_case(c, a, golden, r); });
    check(r.len == golden.len && std::memcmp(r.data, golden.data, r.len) == 0, "re-encoded stream == stream", r.len,
          golden.len);
    check(g_misuse == 0, "no misuse", static_cast<u32>(g_misuse));
    out("all s3 records: stream ");
    dec(golden.len);
    out(" bytes, read back and re-encoded identically\n");
}

// Decodes a stream written by the i686 build (same cases, same order) and re-encodes it: byte for byte the same.
void check_golden(const char* path) {
    MemFile& g = g_files[2];
    MemFile& r = g_files[3];
    MemFile& unused = g_files[0];
    const long n = load_file(path, g.data, sizeof g.data);
    check(n > 0, "golden file readable");
    if (n <= 0) return;
    g.len = static_cast<u32>(n);
    g.pos = 0;
    reset(r);
    reset(unused);
    std::memcpy(unused.data, g.data, g.len);  // x86: the raw reads see the same stream
    unused.len = g.len;
    g_token_diffs = 0;
    each_case([&](const auto& c) { read_case(c, unused, g, r, !sv::kX86Layout); });
    check(g.pos == g.len, "golden consumed", g.pos, g.len);
    check(r.len == g.len, "golden re-encoded length", r.len, g.len);
    if (sv::kX86Layout)
        check(std::memcmp(r.data, g.data, g.len) == 0, "golden re-encoded == golden");
    out("golden: decoded and re-encoded, ");
    dec(g.len);
    out(" bytes identical");
    if (!sv::kX86Layout) {
        out(" except ");
        dec(static_cast<u32>(g_token_diffs));
        out(" non-null pointer dwords (x86 raw value vs x64 token, null-ness equal)");
    }
    out("\n");
}

}  // namespace

int test_main(int argc, char** argv) {
    install();
    out(sv::kX86Layout ? "s3 save test, x86 layout (4-byte pointers)\n" : "s3 save test, x64 layout (8-byte pointers)\n");
    bool golden_only = false;
    for (int i = 1; i + 1 < argc; i += 2)
        if (std::strcmp(argv[i], "--check-golden") == 0) golden_only = true;
    if (!golden_only || sv::kX86Layout) test_all(g_files[2]);
    for (int i = 1; i + 1 < argc; i += 2) {
        if (std::strcmp(argv[i], "--write-golden") == 0) {
            check(save_file(argv[i + 1], g_files[2].data, g_files[2].len), "golden written");
            out("golden written: ");
            dec(g_files[2].len);
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
