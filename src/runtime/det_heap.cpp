#include "det_heap.hpp"

#include "../platform/win32_types.hpp"

#include <cstring>
#include <mutex>
#include <initializer_list>
#include <map>
#include <vector>

#include "../registry.hpp"
#include "../platform/memory.hpp"

namespace joan::det_heap {
namespace {

// Address space of the arena. 512 MB on 32-bit Windows (a 2 GB process; the layout of every recording made there);
// 1.5 GB elsewhere: the Linux x86 build ran out of 512 MB while mission 1 loaded (Phase 1, 2026-10-29) - the bump
// pointer only reuses freed blocks of a close size, so a session's peak is above the game's live data. The arena
// size changes no offset: a recording and its replay lay blocks out the same way whatever the limit, as long as
// neither runs out.
#if defined(_WIN32) && !defined(_WIN64)
constexpr std::size_t kReserve = 512u << 20;
#else
constexpr std::size_t kReserve = 1536u << 20;
#endif
constexpr std::size_t kCommitStep = 1u << 20;
constexpr std::size_t kHeader = 16;            // keeps user blocks 16-byte aligned
constexpr std::uint32_t kMagic = 0x44484550;   // "DHEP" live block
constexpr std::uint32_t kFreeMagic = 0x46524545;  // "FREE"
constexpr std::size_t kSmallClasses = 256;      // capacities 16 .. 4096 in steps of 16

// 32-bit size fields on every target (also x64): alloc_locked refuses requests above 2 GB and the arena is at most 1.5 GB, so
// no size or capacity needs more; the header stays 16 bytes and user blocks 16-byte aligned on x86 and x64.
struct Header {
    std::uint32_t size;  // requested size (what HeapSize reports)
    std::uint32_t cap;   // usable capacity
    std::uint32_t magic;
    std::uint32_t pad;
};

std::uintptr_t g_base = 0, g_bump = 0, g_committed = 0, g_end = 0;
bool g_active = false;
void (*g_trace)(std::size_t, std::uintptr_t) = nullptr;
std::mutex g_lock;
std::vector<void*> g_small[kSmallClasses + 1];     // LIFO per capacity / 16
std::multimap<std::size_t, void*> g_large;         // capacity -> block (smallest fitting first, then oldest)
std::size_t g_small_free = 0, g_large_free = 0;    // bytes of capacity on the free lists
std::size_t g_allocs = 0, g_releases = 0, g_carved = 0;
bool g_exhausted = false;

struct Lock {
    Lock() { g_lock.lock(); }
    ~Lock() { g_lock.unlock(); }
};

Header* header_of(const void* p) {
    return reinterpret_cast<Header*>(reinterpret_cast<std::uintptr_t>(p) - kHeader);
}

bool owns(const void* p) {
    const auto v = reinterpret_cast<std::uintptr_t>(p);
    return g_active && v >= g_base + kHeader && v < g_bump && (v & 15) == 0 && header_of(p)->magic == kMagic;
}

std::size_t capacity_for(std::size_t n) { return n == 0 ? 16 : (n + 15) & ~static_cast<std::size_t>(15); }

void report_locked(const char* when);

void* carve(std::size_t cap) {
    const std::size_t need = kHeader + cap;
    if (g_bump + need > g_end) {
        if (!g_exhausted) {
            g_exhausted = true;
            hook::log_line("det_heap: OUT OF ARENA: a request of %u bytes does not fit", static_cast<unsigned>(cap));
            report_locked("at exhaustion");
        }
        return nullptr;
    }
    while (g_bump + need > g_committed) {
        if (!platform::memory::commit(reinterpret_cast<void*>(g_committed), kCommitStep, platform::memory::kReadWrite)) return nullptr;
        g_committed += kCommitStep;
    }
    ++g_carved;
    auto* h = reinterpret_cast<Header*>(g_bump);
    g_bump += need;
    h->cap = static_cast<std::uint32_t>(cap);
    return reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(h) + kHeader);
}

void* take(std::size_t cap) {
    if (cap / 16 <= kSmallClasses) {
        auto& list = g_small[cap / 16];
        if (!list.empty()) {
            void* p = list.back();
            list.pop_back();
            g_small_free -= cap;
            return p;
        }
        return carve(cap);
    }
    const auto it = g_large.lower_bound(cap);
    if (it != g_large.end() && it->first <= cap + cap / 4) {  // reuse a block at most 25 % larger
        void* p = it->second;
        g_large_free -= it->first;
        g_large.erase(it);
        return p;
    }
    return carve(cap);
}

void put_back(void* p) {
    Header* h = header_of(p);
    h->magic = kFreeMagic;
    ++g_releases;
    if (h->cap / 16 <= kSmallClasses) {
        g_small[h->cap / 16].push_back(p);
        g_small_free += h->cap;
    } else {
        g_large.emplace(h->cap, p);
        g_large_free += h->cap;
    }
}

void report_locked(const char* when) {
    if (!g_active) return;
    hook::log_line("det_heap %s: %u MB of %u MB used (bump), %u MB free in %u large blocks, %u MB in small lists; %u "
                   "allocations, %u releases, %u carved",
                   when, static_cast<unsigned>((g_bump - g_base) >> 20), static_cast<unsigned>(kReserve >> 20),
                   static_cast<unsigned>(g_large_free >> 20), static_cast<unsigned>(g_large.size()),
                   static_cast<unsigned>(g_small_free >> 20), static_cast<unsigned>(g_allocs),
                   static_cast<unsigned>(g_releases), static_cast<unsigned>(g_carved));
}

void* alloc_locked(std::size_t n, bool zero) {
    if (n > 0x7fffffffu) return nullptr;
    const std::size_t cap = capacity_for(n);
    void* p = take(cap);
    if (!p) return nullptr;
    ++g_allocs;
    Header* h = header_of(p);
    h->size = static_cast<std::uint32_t>(n);
    h->magic = kMagic;
    if (zero) std::memset(p, 0, h->cap);
    if (g_trace) g_trace(n, reinterpret_cast<std::uintptr_t>(p) - g_base);
    return p;
}

}  // namespace

bool init() {
    Lock lock;
    if (g_active) return true;
    // A fixed base first: the game's state holds pointers into this heap, also in unaligned / split copies that the
    // replay hash cannot normalise, and some game code orders objects by address. With a base chosen by Windows the
    // recording and its replay could get different bases (OpenAL Soft's soft_oal.dll and the replay's mapped
    // recording file change where the first free 512 MB are: test39 recorded at 0x035e.... and replayed at
    // 0x07b6...., and diverged at frame 4048). These bases lie above the executable and its heaps and below the
    // system DLLs of a 32-bit process; the first free one is taken, Windows' choice only if all are in use.
    // x64: the same bases 0x7e00'0000'0000 higher, far above 4 GB like every other address of a 64-bit process (the
    // game code must not depend on its heap fitting 32 bits), with the same low 32 bits as on x86, which is all that
    // the replay's 32-bit records keep of a heap address (replay.cpp normalise_in: kTagHeap | offset).
    constexpr std::uintptr_t kHigh = sizeof(void*) == 8 ? static_cast<std::uintptr_t>(0x7e00ull << 32) : 0;
    void* r = nullptr;
    for (const std::uintptr_t want : {0x20000000u, 0x30000000u, 0x40000000u, 0x50000000u}) {
        r = platform::memory::reserve(kReserve, reinterpret_cast<void*>(kHigh + want));
        if (r) break;
    }
    if (!r) {
        r = platform::memory::reserve(kReserve);
        if (r) hook::log_line("det_heap: none of the fixed bases is free - replays of this run may differ in layout");
    }
    if (!r) {
        hook::log_line("det_heap: cannot reserve %u MB - the game heap stays the Win32 heap", static_cast<unsigned>(kReserve >> 20));
        return false;
    }
    g_base = reinterpret_cast<std::uintptr_t>(r);
    g_bump = g_base;
    g_committed = g_base;
    g_end = g_base + kReserve;
    g_active = true;
    hook::log_line("det_heap: the game's CRT heap is the deterministic heap at 0x%08llx (%u MB reserved)",
                   static_cast<unsigned long long>(g_base), static_cast<unsigned>(kReserve >> 20));
    return true;
}

bool active() { return g_active; }
void report(const char* when) {
    Lock lock;
    report_locked(when);
}
void set_trace(void (*fn)(std::size_t, std::uintptr_t)) { g_trace = fn; }
std::uintptr_t base() { return g_base; }
std::uintptr_t used_end() { return g_bump; }

void* alloc(std::size_t n, bool zero) {
    Lock lock;
    return alloc_locked(n, zero);
}

bool release(void* p) {
    Lock lock;
    if (!owns(p)) return false;
    if (g_trace) g_trace(static_cast<std::size_t>(-1), reinterpret_cast<std::uintptr_t>(p) - g_base);
    put_back(p);
    return true;
}

void* realloc(void* p, std::size_t n) {
    Lock lock;
    if (!owns(p)) return nullptr;
    Header* h = header_of(p);
    if (n <= h->cap) {  // fits: in place (like HeapReAlloc growing within its block)
        h->size = static_cast<std::uint32_t>(n);
        return p;
    }
    void* q = alloc_locked(n, false);
    if (!q) return nullptr;  // the old block stays intact
    std::memcpy(q, p, h->size < n ? h->size : n);
    put_back(p);
    return q;
}

std::size_t size(const void* p) {
    Lock lock;
    return owns(p) ? header_of(p)->size : static_cast<std::size_t>(-1);
}

}  // namespace joan::det_heap
