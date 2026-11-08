// Host test of the typed image hash (src/replay/typed_hash.hpp / .cpp with this build's hash_types.inc): the table's
// invariants, its form in a recording, and the hash of a synthetic data image made by two "runs" with different address
// layouts (image base, heap base): pointer fields at any alignment normalise to the same values, typed data that looks
// like a pointer of one run is compared as it is, and a real change of typed data or of a pointer's target is seen.
// Build and run: re/replay/build.sh
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>

#include "runtime/replay/typed_hash.hpp"

using namespace joan::replay;

namespace {

constexpr std::uint32_t kDataVa = 0x009f4000u, kDataEnd = 0x00a6d000u, kSize = kDataEnd - kDataVa;
constexpr std::uint32_t kImageVa = 0x009cf000u, kImageSize = 0xa1000u;  // original addresses of joan_image
constexpr std::uint32_t kTagPointer = 0xA110C000u, kTagHeap = 0x80000000u;

int g_failures = 0;
#define CHECK(c)                                                                  \
    do {                                                                          \
        if (!(c)) {                                                               \
            std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c);              \
            ++g_failures;                                                         \
        }                                                                         \
    } while (0)

std::uint64_t fnv(const std::uint8_t* p, std::size_t n) {
    std::uint64_t h = 0xcbf29ce484222325ull;
    for (std::size_t i = 0; i < n; ++i) {
        h ^= p[i];
        h *= 0x100000001b3ull;
    }
    return h;
}

// A run's layout, and forced_normalise's rules for it (replay.cpp: heap -> tag | offset, image -> original address,
// anything else non-zero -> one tag).
struct Layout {
    std::uint32_t image, heap, heap_used;
    std::uint32_t pointer_to_image(std::uint32_t original) const { return image + (original - kImageVa); }
    std::uint32_t pointer_to_heap(std::uint32_t offset) const { return heap + offset; }
    std::uint32_t normalise(std::uint32_t v) const {
        if (v == 0) return 0;
        if (v - heap < heap_used) return kTagHeap | (v - heap);
        if (v - image < kImageSize) return kImageVa + (v - image);
        return kTagPointer;
    }
};

std::uint32_t get32(const std::vector<std::uint8_t>& b, std::uint32_t o) {
    std::uint32_t v = 0;
    std::memcpy(&v, b.data() + o, 4);
    return v;
}
void put32(std::vector<std::uint8_t>& b, std::uint32_t o, std::uint32_t v) { std::memcpy(b.data() + o, &v, 4); }

// The typed scheme's hashed bytes of `image` (untyped bytes: as they are here; the heuristics are replay.cpp's).
std::uint64_t typed_hash(const typed::Table& t, const std::vector<std::uint8_t>& image, const Layout& l) {
    std::vector<std::uint8_t> dst = image;
    typed::normalise_pointers(t, image.data(), dst.data(), static_cast<std::uint32_t>(image.size()),
                              [&l](std::uint32_t v) { return l.normalise(v); });
    return fnv(dst.data(), dst.size());
}

void table_invariants(const typed::Table& t) {
    CHECK(t.pointers.size() > 3000);
    CHECK(!t.untyped.empty());
    for (std::size_t i = 1; i < t.pointers.size(); ++i) CHECK(t.pointers[i - 1] + 4 <= t.pointers[i]);
    for (std::size_t i = 1; i < t.untyped.size(); ++i) CHECK(t.untyped[i - 1].end <= t.untyped[i].begin);
    std::vector<std::uint8_t> cls;
    CHECK(typed::classes(t, kSize, cls));
    std::size_t counts[3] = {};
    for (const std::uint8_t c : cls) ++counts[c];
    CHECK(counts[typed::kPointer] == 4 * t.pointers.size());
    std::printf("table: %zu pointer fields (%zu unaligned), %zu untyped ranges (%zu bytes), %zu bytes typed data\n",
                t.pointers.size(),
                static_cast<std::size_t>(std::count_if(t.pointers.begin(), t.pointers.end(),
                                                       [](std::uint32_t o) { return (o & 3) != 0; })),
                t.untyped.size(), counts[typed::kUntyped], counts[typed::kTyped]);
    auto cls_at = [&](std::uint32_t va) { return cls[va - kDataVa]; };
    // the words of the P2 test54 report: Game (0x00a58f90) .scenario timers (packed u32 at odd offsets) and
    // Game::u32_740, typed data now; the DirectInput mouse buffer 0x00a65378 (reviewed as a byte buffer). (Scenario
    // +0x1d6, 0x00a5917d, is still an untyped gap: the old symbol get_a59179 u32[2] says it is read as a u32.)
    for (std::uint32_t va = 0x00a5916du; va < 0x00a5917du; ++va) CHECK(cls_at(va) == typed::kTyped);
    for (std::uint32_t va = 0x00a5917du; va < 0x00a59181u; ++va) CHECK(cls_at(va) == typed::kUntyped);
    for (std::uint32_t va = 0x00a59181u; va < 0x00a591b5u; ++va) CHECK(cls_at(va) == typed::kTyped);
    for (std::uint32_t va = 0x00a596d0u; va < 0x00a596d4u; ++va) CHECK(cls_at(va) == typed::kTyped);
    for (std::uint32_t va = 0x00a65378u; va < 0x00a65378u + 0xfb0u; ++va) CHECK(cls_at(va) == typed::kTyped);
    // SysEn (0x00a52420) caps / usage counters 0x00a5253c / 0x00a52637: typed
    for (std::uint32_t va = 0x00a5253cu; va < 0x00a52540u; ++va) CHECK(cls_at(va) == typed::kTyped);
    for (std::uint32_t va = 0x00a52637u; va < 0x00a5263bu; ++va) CHECK(cls_at(va) == typed::kTyped);
    // pointers: Game.scenario.entries (unaligned, 0x00a58f90 + 0x17 + 0x10), the Font2d texture 0x00a596d8 + 0x26,
    // the CRT initialiser table 0x009f4104.., vtable slots (the menu 0x00a3cf38 and its button 0 at the odd +0x39),
    // Camera::scene of the Sprite2D 0x00a46128 (hidden by Sprite2D::scene: hash_types.inc names it through its base)
    for (const std::uint32_t va : {0x00a58fb7u, 0x00a596feu, 0x009f4104u, 0x00a3cf38u, 0x00a3cf71u, 0x00a4612cu}) {
        CHECK(std::binary_search(t.pointers.begin(), t.pointers.end(), va - kDataVa));
        for (std::uint32_t k = 0; k < 4; ++k) CHECK(cls_at(va + k) == typed::kPointer);
    }
    // the heuristics only see windows wholly in untyped bytes
    const typed::Span u = t.untyped.front();
    CHECK(typed::untyped4(cls, u.begin));
    CHECK(!typed::untyped4(cls, u.end - 3));
    if (u.begin > 0) CHECK(!typed::untyped4(cls, u.begin - 1));
    CHECK(!typed::untyped4(cls, 0x00a5916du - kDataVa));
}

void serialisation(const typed::Table& t) {
    const std::string s = typed::serialise(t);
    typed::Table back;
    CHECK(typed::deserialise(reinterpret_cast<const std::uint8_t*>(s.data()), s.size(), back));
    CHECK(back.pointers == t.pointers);
    CHECK(back.untyped.size() == t.untyped.size());
    CHECK(typed::fingerprint(back) == typed::fingerprint(t));
    typed::Table bad;
    CHECK(!typed::deserialise(reinterpret_cast<const std::uint8_t*>(s.data()), s.size() - 1, bad));  // cut short
    std::string v2 = s;
    v2[0] = 9;  // another version
    CHECK(!typed::deserialise(reinterpret_cast<const std::uint8_t*>(v2.data()), v2.size(), bad));
    typed::Table other = t;
    other.pointers.pop_back();
    CHECK(typed::fingerprint(other) != typed::fingerprint(t));
    // a damaged table (overlapping entries) is refused by classes()
    typed::Table overlap = t;
    overlap.untyped.push_back({t.pointers[0], t.pointers[0] + 1});
    std::vector<std::uint8_t> cls;
    CHECK(!typed::classes(overlap, kSize, cls));
    std::printf("serialised table: %zu bytes\n", s.size());
}

// Two runs of the same game state: the image base and heap base differ (ASLR, another build), so every pointer value
// differs; typed data is the same numbers. Some typed numbers are chosen to look like pointers of run A.
void two_runs(const typed::Table& t) {
    const Layout a{0x01230000u, 0x0a000000u, 0x00400000u};
    const Layout b{0x00e40000u, 0x05b10000u, 0x00400000u};
    std::vector<std::uint8_t> cls;
    typed::classes(t, kSize, cls);
    std::mt19937 rng(54);
    std::vector<std::uint8_t> ia(kSize), ib(kSize);
    for (std::uint32_t o = 0; o < kSize; ++o) ia[o] = ib[o] = static_cast<std::uint8_t>(rng());
    // typed u32 that look like pointers of run A (timers / counters 0x1000 apart, like 0x00a59171.. in test54)
    for (std::uint32_t k = 0; k < 4; ++k) {
        put32(ia, 0x00a59171u - kDataVa + 4 * k, a.heap + 0x1000u * (k + 1));
        put32(ib, 0x00a59171u - kDataVa + 4 * k, a.heap + 0x1000u * (k + 1));
    }
    put32(ia, 0x00a596d0u - kDataVa, a.image + 0x1234u);  // Game::u32_740: a number inside run A's image range
    put32(ib, 0x00a596d0u - kDataVa, a.image + 0x1234u);
    // pointer fields: the same targets in both runs (image objects, heap blocks, null, an outside pointer)
    for (std::size_t i = 0; i < t.pointers.size(); ++i) {
        const std::uint32_t o = t.pointers[i];
        std::uint32_t va = 0, vb = 0;
        switch (i % 4) {
            case 0: break;  // null
            case 1: {
                const std::uint32_t target = kDataVa + static_cast<std::uint32_t>(rng() % kSize);
                va = a.pointer_to_image(target);
                vb = b.pointer_to_image(target);
                break;
            }
            case 2: {
                const std::uint32_t off = static_cast<std::uint32_t>(rng() % a.heap_used) & ~7u;
                va = a.pointer_to_heap(off);
                vb = b.pointer_to_heap(off);
                break;
            }
            default:  // a DLL / COM object: some address outside both
                va = 0x6f000000u + static_cast<std::uint32_t>(i);
                vb = 0x70a00000u + static_cast<std::uint32_t>(i);
        }
        put32(ia, o, va);
        put32(ib, o, vb);
    }
    const std::uint64_t ha = typed_hash(t, ia, a), hb = typed_hash(t, ib, b);
    CHECK(ha == hb);
    CHECK(get32(ia, 0x00a58fb7u - kDataVa) != get32(ib, 0x00a58fb7u - kDataVa) ||
          get32(ia, 0x00a58fb7u - kDataVa) == 0);  // (raw pointer values differ unless null)
    // a typed timer that differs is a difference, even where it looks like a pointer
    std::vector<std::uint8_t> ib2 = ib;
    put32(ib2, 0x00a59171u - kDataVa, a.heap + 0x2000u);
    CHECK(typed_hash(t, ib2, b) != ha);
    // a typed byte anywhere
    ib2 = ib;
    std::uint32_t typed_byte = 0;
    while (cls[typed_byte] != typed::kTyped) ++typed_byte;
    ib2[typed_byte] ^= 1;
    CHECK(typed_hash(t, ib2, b) != ha);
    // a pointer to another heap block / image object
    ib2 = ib;
    const std::uint32_t p_heap = t.pointers[2], p_img = t.pointers[1];
    put32(ib2, p_heap, get32(ib2, p_heap) + 8);
    CHECK(typed_hash(t, ib2, b) != ha);
    ib2 = ib;
    put32(ib2, p_img, get32(ib2, p_img) + 4);
    CHECK(typed_hash(t, ib2, b) != ha);
    // the old way (every 4-aligned word normalised by address range) is not layout-independent for these numbers
    auto word_hash = [](const std::vector<std::uint8_t>& img, const Layout& l) {
        std::vector<std::uint8_t> d = img;
        for (std::uint32_t o = 0; o + 4 <= d.size(); o += 4) put32(d, o, l.normalise(get32(img, o)));
        return fnv(d.data(), d.size());
    };
    CHECK(word_hash(ia, a) != word_hash(ib, b));
    std::printf("two runs: typed hashes %016llx / %016llx\n", static_cast<unsigned long long>(ha),
                static_cast<unsigned long long>(hb));
}

}  // namespace

int main() {
    const typed::Table t = typed::builtin(kDataVa, kSize);
    table_invariants(t);
    serialisation(t);
    two_runs(t);
    // a table cut to a sub-range keeps only the entries inside it, as offsets from its start
    const typed::Table part = typed::builtin(0x00a58f90u, 0x800u);
    CHECK(!part.pointers.empty());
    CHECK(std::binary_search(part.pointers.begin(), part.pointers.end(), 0x00a58fb7u - 0x00a58f90u));
    std::printf(g_failures ? "%d FAILURES\n" : "all checks passed\n", g_failures);
    return g_failures ? 1 : 0;
}
