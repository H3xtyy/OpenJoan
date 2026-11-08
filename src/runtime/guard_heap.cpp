#include "guard_heap.hpp"

#include "../compiler.hpp"
#include "../platform/win32_types.hpp"

#include <cstdio>
#include <mutex>
#include <cstring>
#include <iterator>
#include <map>
#include <string>

#include "crash.hpp"
#include "../registry.hpp"
#include "../platform/ini.hpp"
#include "../platform/memory.hpp"

namespace joan::guard_heap {
namespace {

constexpr std::uintptr_t kPage = 0x1000;
constexpr std::uintptr_t kAlign = 2 * sizeof(void*);  // HeapAlloc's alignment: 8 on x86, 16 on x64
constexpr std::uintptr_t kChunk = 256u << 20;         // address space reserved at a time
constexpr unsigned char kSlackFill = 0xfd;
constexpr int kFrames = 6;

struct Slot {
    std::uint32_t pages = 0;  // data pages + the guard page (the last one, never committed)
    bool live = false;
    bool used = false;        // held a block once (then freed: a hit is a use after free)
    std::uintptr_t p = 0;     // the block (live or last one)
    std::size_t size = 0;
    void* alloc_at[kFrames] = {};
    void* free_at[kFrames] = {};
};

bool g_active = false;
std::uintptr_t g_bump = 0, g_end = 0;           // fresh space of the current chunk
std::map<std::uintptr_t, Slot> g_slots;         // slot base -> slot (every slot ever carved)
std::multimap<std::uint32_t, std::uintptr_t> g_free;  // pages -> base of a freed slot
std::size_t g_live = 0, g_peak = 0, g_total = 0, g_reserved = 0;
std::mutex g_lock;

struct Lock {
    Lock() { g_lock.lock(); }
    ~Lock() { g_lock.unlock(); }
};

std::string frames_text(void* const (&frames)[kFrames]) {
    std::string s;
    for (void* f : frames) {
        if (!f) break;
        const std::string d = hook::describe_address(reinterpret_cast<std::uintptr_t>(f));
        char buf[40];
        std::snprintf(buf, sizeof buf, "%s0x%llx", s.empty() ? "" : " <- ",
                      static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(f)));
        s += buf;
        if (!d.empty()) s += " " + d;
    }
    return s;
}

bool reserve_chunk(std::size_t at_least) {
    const std::size_t size = at_least > kChunk ? (at_least + 0xffff) & ~static_cast<std::size_t>(0xffff) : kChunk;
    void* r = platform::memory::reserve(size);
    if (!r) {
        hook::log_line("heap guard: cannot reserve %u MB more address space (%u MB reserved, %u live blocks)",
                       static_cast<unsigned>(size >> 20), static_cast<unsigned>(g_reserved >> 20),
                       static_cast<unsigned>(g_live));
        return false;
    }
    // The rest of the previous chunk becomes a free slot (if it can hold a page and its guard).
    if (g_end - g_bump >= 2 * kPage) {
        Slot& s = g_slots[g_bump];
        s.pages = static_cast<std::uint32_t>((g_end - g_bump) / kPage);
        g_free.emplace(s.pages, g_bump);
    }
    g_bump = reinterpret_cast<std::uintptr_t>(r);
    g_end = g_bump + size;
    g_reserved += size;
    return true;
}

// A slot of `pages` pages (guard included): a freed one of that size or larger (split: its tail is used), else
// fresh space. 0 when none can be had.
std::uintptr_t take_slot(std::uint32_t pages) {
    if (g_end - g_bump < pages * kPage) {
        auto it = g_free.lower_bound(pages);
        if (it != g_free.end()) {
            const std::uint32_t have = it->first;
            std::uintptr_t base = it->second;
            g_free.erase(it);
            if (have - pages >= 2) {  // split: the front stays a free slot (its own last page is its guard)
                Slot& front = g_slots[base];
                front.pages = have - pages;
                front.used = false;  // (its old block may lie in the part split off)
                g_free.emplace(have - pages, base);
                base += static_cast<std::uintptr_t>(have - pages) * kPage;
                g_slots[base] = Slot{};
                g_slots[base].pages = pages;
            }
            return base;
        }
        if (!reserve_chunk(pages * kPage)) return 0;
    }
    const std::uintptr_t base = g_bump;
    g_bump += pages * kPage;
    Slot& s = g_slots[base];
    s = Slot{};
    s.pages = pages;
    return base;
}

void* alloc_locked(std::size_t n, void* const (&at)[kFrames]) {
    if (n > 0x7fffffffu) return nullptr;
    const std::size_t data_pages = (n + kAlign - 1 + kPage - 1) / kPage;
    const auto pages = static_cast<std::uint32_t>((data_pages ? data_pages : 1) + 1);
    const std::uintptr_t base = take_slot(pages);
    if (!base) return nullptr;
    Slot& s = g_slots[base];
    const std::uintptr_t guard = base + (s.pages - 1) * static_cast<std::uintptr_t>(kPage);
    const std::uintptr_t p = (guard - n) & ~(kAlign - 1);
    const std::uintptr_t first = p & ~(kPage - 1);
    if (guard > first && !platform::memory::commit(reinterpret_cast<void*>(first), guard - first, platform::memory::kReadWrite)) {
        s.live = false;
        g_free.emplace(s.pages, base);
        return nullptr;
    }
    // (committed pages are zero: HEAP_ZERO_MEMORY needs nothing more)
    std::memset(reinterpret_cast<void*>(p + n), kSlackFill, guard - p - n);
    s.live = s.used = true;
    s.p = p;
    s.size = n;
    std::memcpy(s.alloc_at, at, sizeof s.alloc_at);
    std::memset(s.free_at, 0, sizeof s.free_at);
    ++g_total;
    if (++g_live > g_peak) g_peak = g_live;
    return reinterpret_cast<void*>(p);
}

Slot* live_slot(const void* p) {
    const auto v = reinterpret_cast<std::uintptr_t>(p);
    auto it = g_slots.upper_bound(v);
    if (it == g_slots.begin()) return nullptr;
    --it;
    return it->second.live && it->second.p == v ? &it->second : nullptr;
}

void check_slack(const Slot& s) {
    static int reported = 0;
    const auto* tail = reinterpret_cast<const unsigned char*>(s.p + s.size);
    const std::size_t slack = (kAlign - (s.size & (kAlign - 1))) & (kAlign - 1);
    std::size_t last = 0;
    for (std::size_t i = 0; i < slack; ++i)
        if (tail[i] != kSlackFill) last = i + 1;
    if (last && reported < 64) {
        ++reported;
        hook::log_line("heap guard: block 0x%llx (%u bytes) was written up to %u bytes past its end; allocated at %s",
                       static_cast<unsigned long long>(s.p), static_cast<unsigned>(s.size), static_cast<unsigned>(last),
                       frames_text(s.alloc_at).c_str());
    }
}

void free_locked(Slot& s, std::uintptr_t base, void* const (&at)[kFrames]) {
    check_slack(s);
    platform::memory::decommit(reinterpret_cast<void*>(base), (s.pages - 1) * static_cast<std::size_t>(kPage));
    s.live = false;
    std::memcpy(s.free_at, at, sizeof s.free_at);
    g_free.emplace(s.pages, base);
    --g_live;
}

void bad_pointer(const char* what, const void* p, void* const (&at)[kFrames]) {
    static int reported = 0;
    if (reported >= 32) return;
    ++reported;
    hook::log_line("heap guard: %s of 0x%llx, which is not a live block (double free / foreign pointer), at %s", what,
                   static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(p)), frames_text(at).c_str());
}

// Describes an access violation inside the guarded heap (vectored handler: runs before the crash filter).
platform::memory::FaultAction on_fault(const platform::memory::Fault& f) {
    namespace mem = platform::memory;
    if (f.type != mem::Fault::kAccess) return mem::FaultAction::kPass;
    const std::uintptr_t a = f.address;
    if (!g_lock.try_lock()) return mem::FaultAction::kPass;  // the fault is inside the heap itself
    auto it = g_slots.upper_bound(a);
    if (it != g_slots.begin()) {
        --it;
        const Slot& s = it->second;
        const std::uintptr_t end = it->first + s.pages * static_cast<std::uintptr_t>(kPage);
        if (a < end) {
            static int reported = 0;
            if (reported++ < 16) {
                const char* kind = f.access == mem::kWrite ? "writing" : f.access == mem::kExec ? "executing" : "reading";
                const auto at = static_cast<unsigned long long>(f.pc);
                if (s.live && a >= s.p + s.size)
                    hook::log_line("**** heap guard: %s 0x%llx = %llu bytes past the end of block 0x%llx (%u bytes), "
                                   "at 0x%llx %s; the block was allocated at %s",
                                   kind, static_cast<unsigned long long>(a),
                                   static_cast<unsigned long long>(a - (s.p + s.size)),
                                   static_cast<unsigned long long>(s.p), static_cast<unsigned>(s.size), at,
                                   hook::describe_address(static_cast<std::uintptr_t>(at)).c_str(),
                                   frames_text(s.alloc_at).c_str());
                else if (!s.live && s.used)
                    hook::log_line("**** heap guard: %s 0x%llx in a freed block (0x%llx, %u bytes, +0x%llx), at 0x%llx "
                                   "%s; it was allocated at %s; freed at %s",
                                   kind, static_cast<unsigned long long>(a), static_cast<unsigned long long>(s.p),
                                   static_cast<unsigned>(s.size), static_cast<unsigned long long>(a - s.p), at,
                                   hook::describe_address(static_cast<std::uintptr_t>(at)).c_str(),
                                   frames_text(s.alloc_at).c_str(), frames_text(s.free_at).c_str());
                else
                    hook::log_line("**** heap guard: %s 0x%llx, in front of block 0x%llx (%u bytes) or in an unused "
                                   "slot, at 0x%llx %s",
                                   kind, static_cast<unsigned long long>(a), static_cast<unsigned long long>(s.p),
                                   static_cast<unsigned>(s.size), at,
                                   hook::describe_address(static_cast<std::uintptr_t>(at)).c_str());
            }
        }
    }
    g_lock.unlock();
    return mem::FaultAction::kPass;
}

#define CALLERS(name)            \
    void* name[kFrames] = {};    \
    JOAN_STACK_TRACE(1, kFrames, name)

}  // namespace

bool requested() {
    static const bool on =
        !kReleaseBuild && joan::platform::ini::get_int("Debug", "HeapGuard", 0, hook::ini_path().c_str()) != 0;
    return on;
}

bool init() {
    Lock lock;
    if (g_active) return true;
    if (!reserve_chunk(kChunk)) return false;
    platform::memory::add_fault_handler(&on_fault);
    g_active = true;
    hook::log_line("heap guard: [Debug] HeapGuard=1 - every CRT heap block has its own pages and ends at a "
                   "no-access page (alignment %u; slow, for diagnosis only)",
                   static_cast<unsigned>(kAlign));
    return true;
}

bool active() { return g_active; }

void* alloc(std::size_t n, bool /*zero: committed pages are zero*/) {
    CALLERS(at);
    Lock lock;
    return alloc_locked(n, at);
}

bool release(void* p) {
    CALLERS(at);
    Lock lock;
    Slot* s = live_slot(p);
    if (!s) {
        bad_pointer("free", p, at);
        return false;
    }
    const std::uintptr_t base = std::prev(g_slots.upper_bound(reinterpret_cast<std::uintptr_t>(p)))->first;
    free_locked(*s, base, at);
    return true;
}

void* realloc(void* p, std::size_t n) {
    CALLERS(at);
    std::size_t old = 0;
    {
        Lock lock;
        const Slot* s = live_slot(p);
        if (!s) {
            bad_pointer("realloc", p, at);
            return nullptr;
        }
        old = s->size;
    }
    void* q = nullptr;
    {
        Lock lock;
        q = alloc_locked(n, at);
    }
    if (!q) return nullptr;  // the old block stays intact
    std::memcpy(q, p, old < n ? old : n);
    Lock lock;
    if (Slot* s = live_slot(p)) {
        const std::uintptr_t base = std::prev(g_slots.upper_bound(reinterpret_cast<std::uintptr_t>(p)))->first;
        free_locked(*s, base, at);
    }
    return q;
}

std::size_t size(const void* p) {
    Lock lock;
    const Slot* s = live_slot(p);
    return s ? s->size : static_cast<std::size_t>(-1);
}

}  // namespace joan::guard_heap
