// Host test of the x86-layout view of joan_image (src/replay/x86_view.hpp), which the replay hashes on x64 so that an x64
// run compares its frame hashes with an x86 recording (test57):
//   1. the run table of src/data/image_xlate.inc: sorted, complete, offset_of / original_of inverse on every address;
//   2. every entry of src/replay/hash_types.inc (x86 offsets): a pointer field is a widened 8-byte slot at the x64
//      location of its address (its low 4 bytes are the x86 view's 4 bytes), an untyped range is contiguous;
//   3. two runs of the same game state, an x86 one and an x64 one (other image / heap bases, x64 image built field
//      by field through the runs): the view of the x64 image is byte for byte the x86 image of the same values, and
//      the typed hash with each run's pointer normaliser is equal; a changed typed byte / pointer target is seen;
//   4. an identity run table (the x86 layout) makes the view a plain copy: on i686 replay.cpp hashes joan_image itself
//      with the same code as before (no view), so its hash values do not change.
// Build and run: re/replay/check_x64_view.sh
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>

#include "runtime/replay/typed_hash.hpp"
#include "runtime/replay/x86_view.hpp"

using namespace joan::replay;
namespace xv = joan::replay::x86_view;

namespace {

constexpr std::uint32_t kDataVa = 0x009f4000u, kDataEnd = 0x00a6d000u, kSize = kDataEnd - kDataVa;
constexpr std::uint32_t kImageVa = xv::kImageVa, kImageEnd = 0x00a70000u;
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
std::uint32_t get32(const std::uint8_t* b, std::uint32_t o) {
    std::uint32_t v = 0;
    std::memcpy(&v, b + o, 4);
    return v;
}
void put32(std::uint8_t* b, std::uint32_t o, std::uint32_t v) { std::memcpy(b + o, &v, 4); }

constexpr std::uint32_t x64(std::uint32_t va) { return xv::x64_offset_of(va); }

struct PtrEntry {
    std::uint32_t va;
};
struct UntypedEntry {
    std::uint32_t va, size;
};
const PtrEntry kPtrs[] = {
#define JOAN_HASH_PTR(va, path) {va},
#include "runtime/replay/hash_types.inc"
#undef JOAN_HASH_PTR
};
const UntypedEntry kUntyped[] = {
#define JOAN_HASH_UNTYPED(va, size, path) {va, size},
#include "runtime/replay/hash_types.inc"
#undef JOAN_HASH_UNTYPED
};

// 1. the run table
void run_table() {
    const auto* r = xv::kX64Runs;
    const std::size_t n = xv::kX64RunCount;
    CHECK(r[0].va == kImageVa && r[0].off == 0);
    CHECK(r[n - 1].va == kImageEnd && r[n - 1].off == xv::kX64ImageSize);
    std::size_t widened = 0, other = 0;
    for (std::size_t i = 0; i + 1 < n; ++i) {
        CHECK(r[i].va < r[i + 1].va && r[i].off < r[i + 1].off);
        const std::uint32_t len86 = r[i + 1].va - r[i].va, len64 = r[i + 1].off - r[i].off;
        if (len64 == len86 + 4 && len86 >= 4) ++widened;
        else if (len64 != len86) ++other;
    }
    CHECK(other == 0);  // pack(1): the only difference of the two layouts is the pointer width
    CHECK(xv::kX64ImageSize == (kImageEnd - kImageVa) + 4 * widened);
    std::size_t bad = 0;
    for (std::uint32_t va = kImageVa; va < kImageEnd; ++va)
        if (xv::x64_original_of(x64(va)) != va) ++bad;
    CHECK(bad == 0);
    std::printf("runs: %zu, %zu widened pointer slots, x64 image 0x%x bytes\n", n, widened,
                static_cast<unsigned>(xv::kX64ImageSize));
}

// 2. every typed entry
void typed_entries() {
    std::size_t bad_ptr = 0, bad_untyped = 0;
    std::vector<std::uint8_t> is_entry(kSize, 0);
    for (const PtrEntry& p : kPtrs) {
        bool ok = true;
        for (std::uint32_t k = 0; k < 4; ++k) ok = ok && x64(p.va + k) == x64(p.va) + k;
        ok = ok && x64(p.va + 4) == x64(p.va) + 8;  // the slot is 8 bytes: the next field starts 8 further
        if (!ok && bad_ptr++ < 10) std::printf("  pointer 0x%08x: not a widened slot\n", static_cast<unsigned>(p.va));
        is_entry[p.va - kDataVa] = 1;
    }
    for (const UntypedEntry& u : kUntyped) {
        const bool ok = x64(u.va + u.size - 1) == x64(u.va) + u.size - 1;
        if (!ok && bad_untyped++ < 10)
            std::printf("  untyped 0x%08x+0x%x: not contiguous\n", static_cast<unsigned>(u.va), static_cast<unsigned>(u.size));
    }
    CHECK(bad_ptr == 0);
    CHECK(bad_untyped == 0);
    // widened slots of the hashed range that are not pointer entries (typed data on x86 that is pointer-wide on x64)
    std::size_t widened = 0, not_entry = 0;
    for (std::size_t i = 0; i + 1 < xv::kX64RunCount; ++i) {
        const xv::Run a = xv::kX64Runs[i], b = xv::kX64Runs[i + 1];
        if (b.off - a.off != b.va - a.va + 4 || b.va - 4 < kDataVa || b.va > kDataEnd) continue;
        ++widened;
        if (!is_entry[b.va - 4 - kDataVa] && not_entry++ < 10)
            std::printf("  widened slot 0x%08x: no pointer entry\n", static_cast<unsigned>(b.va - 4));
    }
    CHECK(not_entry == 0);
    CHECK(widened == std::size(kPtrs));
    std::printf("typed entries: %zu pointers, %zu untyped ranges checked; %zu widened slots in .data/.bss\n",
                std::size(kPtrs), std::size(kUntyped), widened);
}

// A run's address layout. x64: an image pointer to original address t is image + x64 offset of t.
struct Run {
    bool x64;
    std::uint32_t image, heap, heap_used;
    std::uint32_t pointer_to_image(std::uint32_t t) const { return image + (x64 ? xv::x64_offset_of(t) : t - kImageVa); }
    std::uint32_t normalise(std::uint32_t v) const {  // replay.cpp normalise_in / forced_normalise, reduced
        if (v == 0) return 0;
        if (v - heap < heap_used) return kTagHeap | (v - heap);
        if (v - image < (x64 ? xv::kX64ImageSize : xv::kX86ImageSize))
            return x64 ? xv::x64_original_of(v - image) : kImageVa + (v - image);
        return kTagPointer;
    }
};

std::uint64_t typed_hash(const typed::Table& t, const std::uint8_t* view, const Run& l) {
    std::vector<std::uint8_t> dst(view, view + kSize);
    typed::normalise_pointers(t, view, dst.data(), kSize, [&l](std::uint32_t v) { return l.normalise(v); });
    return fnv(dst.data(), dst.size());
}

// The x64 image of an x86-layout image: every run's bytes at its x64 offset, the upper halves of widened pointers 0.
std::vector<std::uint8_t> widen(const std::vector<std::uint8_t>& x86) {
    std::vector<std::uint8_t> out(xv::kX64ImageSize, 0);
    for (std::size_t i = 0; i + 1 < xv::kX64RunCount; ++i) {
        const xv::Run a = xv::kX64Runs[i], b = xv::kX64Runs[i + 1];
        std::memcpy(out.data() + a.off, x86.data() + (a.va - kImageVa), b.va - a.va);
    }
    return out;
}

// 3. an x86 run and an x64 run of the same state
void two_runs(const typed::Table& t) {
    const Run a{false, 0x01230000u, 0x20000000u, 0x00400000u};  // the x86 recording
    const Run b{true, 0x10400000u, 0x30000000u, 0x00400000u};   // the x64 replay
    std::mt19937 rng(57);
    std::vector<std::uint8_t> ia(kImageEnd - kImageVa), ib86;
    for (auto& c : ia) c = static_cast<std::uint8_t>(rng());
    ib86 = ia;  // the x64 run's state, in the x86 layout (the reference for its view)
    std::uint8_t* da = ia.data() + (kDataVa - kImageVa);
    std::uint8_t* db = ib86.data() + (kDataVa - kImageVa);
    for (std::size_t i = 0; i < t.pointers.size(); ++i) {
        const std::uint32_t o = t.pointers[i];
        std::uint32_t va = 0, vb = 0;
        switch (i % 4) {
            case 0: break;
            case 1: {  // an image object (anywhere in the image, any byte)
                const std::uint32_t target = kImageVa + static_cast<std::uint32_t>(rng() % (kImageEnd - kImageVa));
                va = a.pointer_to_image(target);
                vb = b.pointer_to_image(target);
                break;
            }
            case 2: {
                const std::uint32_t off = static_cast<std::uint32_t>(rng() % a.heap_used) & ~15u;
                va = a.heap + off;
                vb = b.heap + off;
                break;
            }
            default:
                va = 0x6f000000u + static_cast<std::uint32_t>(i);
                vb = 0x7a000000u + static_cast<std::uint32_t>(i);
        }
        put32(da, o, va);
        put32(db, o, vb);
    }
    const std::vector<std::uint8_t> ib = widen(ib86);  // the x64 run's joan_image
    std::vector<std::uint8_t> view(kSize);
    xv::copy(xv::kX64Runs, xv::kX64RunCount, ib.data(), kDataVa, kSize, view.data());
    CHECK(std::memcmp(view.data(), db, kSize) == 0);
    const std::uint64_t ha = typed_hash(t, da, a), hb = typed_hash(t, view.data(), b);
    CHECK(ha == hb);
    // sub-ranges (page by page, unaligned ends) give the same bytes
    std::vector<std::uint8_t> part(0x1003);
    for (std::uint32_t va = kDataVa + 1; va + part.size() <= kDataEnd; va += 0x7777) {
        xv::copy(xv::kX64Runs, xv::kX64RunCount, ib.data(), va, static_cast<std::uint32_t>(part.size()), part.data());
        CHECK(std::memcmp(part.data(), db + (va - kDataVa), part.size()) == 0);
    }
    // a typed byte / a pointer target of the x64 run that differs is seen
    std::vector<std::uint8_t> cls;
    typed::classes(t, kSize, cls);
    std::uint32_t typed_byte = 0x1000;
    while (cls[typed_byte] != typed::kTyped) ++typed_byte;
    std::vector<std::uint8_t> ib2 = ib;
    ib2[x64(kDataVa + typed_byte)] ^= 1;
    xv::copy(xv::kX64Runs, xv::kX64RunCount, ib2.data(), kDataVa, kSize, view.data());
    CHECK(typed_hash(t, view.data(), b) != ha);
    ib2 = ib;
    const std::uint32_t p_img = kDataVa + t.pointers[1];
    put32(ib2.data(), x64(p_img), get32(ib2.data(), x64(p_img)) + 4);
    xv::copy(xv::kX64Runs, xv::kX64RunCount, ib2.data(), kDataVa, kSize, view.data());
    CHECK(typed_hash(t, view.data(), b) != ha);
    // the widened slots come with their full value: every x64 pointer slot once, the upper half included
    std::size_t wide = 0;
    xv::for_each_wide_pointer(xv::kX64Runs, xv::kX64RunCount, ib.data(), kDataVa, kSize,
                              [&](std::uint32_t, std::uint64_t) { ++wide; });
    CHECK(wide > 0);
    ib2 = ib;
    const std::uint32_t p4 = kDataVa + t.pointers[4];
    put32(ib2.data(), x64(p4) + 4, 1);  // a pointer above 4 GB
    std::uint64_t seen = 0;
    xv::for_each_wide_pointer(xv::kX64Runs, xv::kX64RunCount, ib2.data(), kDataVa, kSize,
                              [&](std::uint32_t va, std::uint64_t v) {
                                  if (va == p4) seen = v;
                              });
    CHECK(seen == (std::uint64_t{1} << 32 | get32(ib2.data(), x64(p4))));
    std::printf("x86 run / x64 run: typed hashes %016llx / %016llx\n", static_cast<unsigned long long>(ha),
                static_cast<unsigned long long>(hb));
}

// 4. the x86 layout as a run table: the view is the image itself
void identity() {
    const xv::Run runs[] = {{kImageVa, 0}, {kImageEnd, kImageEnd - kImageVa}};
    std::vector<std::uint8_t> image(kImageEnd - kImageVa), view(kSize);
    std::mt19937 rng(86);
    for (auto& c : image) c = static_cast<std::uint8_t>(rng());
    xv::copy(runs, 2, image.data(), kDataVa, kSize, view.data());
    CHECK(std::memcmp(view.data(), image.data() + (kDataVa - kImageVa), kSize) == 0);
    for (std::uint32_t va = kImageVa; va < kImageEnd; va += 0x101)
        CHECK(xv::offset_of(runs, 2, va) == va - kImageVa && xv::original_of(runs, 2, va - kImageVa) == va);
}

}  // namespace

int main() {
    run_table();
    typed_entries();
    two_runs(typed::builtin(kDataVa, kSize));
    identity();
    std::printf(g_failures ? "%d FAILURES\n" : "all checks passed\n", g_failures);
    return g_failures ? 1 : 0;
}
