// Original library sources (re/attribution.csv original_file, under lib/): crt/calloc.c, crt/delete.cpp, crt/free.c,
//   crt/handler.cpp, crt/heapinit.c, crt/malloc.c, crt/msize.c, crt/new.cpp, crt/realloc.c.
//
// Batch 114 / list 03: family B of the statically linked MSVC 6 C runtime - the heap (CRT_SYSTEM.md 4.1, 5.3 step 2).
//
// Family surface replaced as one unit (module "crt/heap", all cdecl, plain `ret`, entry-patched: library code has
// no incremental-link thunks; re/check_lib_entries.py: all entries safe):
//   0x007b1d2a void   operator delete(void*)            = free(p)               game 1278 call sites incl. libs
//   0x007b1d35 void*  operator new(size_t)              = _nh_malloc(n, 1)      1014 call sites
//   0x007b4c45 _PNH   _set_new_handler(_PNH)            (_pnhHeap 0x00a6b224)    game 1 (Heap ctor 0x586950, C++ b100_10)
//   0x007b4c6f int    _callnewh(size_t)                 internal (_nh_malloc, calloc, realloc)
//   0x007b4e1d void   free(void*)                       136 call sites (game, D3DX, libpng/libjpeg/zlib, ALUT, CRT)
//   0x007b6c54 void*  malloc(size_t)                    = _nh_malloc(n, _newmode 0x00a6b434)   59 call sites
//   0x007b6c66 void*  _nh_malloc(size_t, int)           internal (operator new, malloc)
//   0x007b6c92 void*  _heap_alloc(size_t)               internal (_nh_malloc)
//   0x007b7b87 void*  calloc(size_t, size_t)            8 call sites (zlib zcalloc 0x7a42a7, _beginthread, _mtinit,
//                                                       _getptd, __initstdio x2, __init_time, __init_monetary)
//   0x007bc089 void*  realloc(void*, size_t)            3 call sites (_onexit 0x7b359c, __crtsetenv 0x7c4b9a x2)
//   0x007bc1c1 size_t _msize(void*)                     2 call sites (both in _onexit 0x7b359c)
//   0x007bdb53 int    _heap_init(int mtflag)            1 call site (WinMainCRTStartup 0x7b71e6, mtflag 1)
// Callers were found as direct `call`/`jmp` targets in joan.asm (no address constants of any heap function exist in
// the image data, and src/modules only call 0x7b1d35 / 0x7b1d2a / 0x7b4c45 through their addresses).
//
// Left original because unreachable (no call, jump or address constant anywhere in joan.exe or the hook):
//   0x007b4c69 _query_new_handler, 0x007bdb8f _heap_term, 0x007bdc04 _get_sbh_threshold,
//   0x007bdc0a _set_sbh_threshold, and the whole V6 small-block heap: __sbh_heap_init 0x7bdc21,
//   __sbh_find_block 0x7bdc5f, __sbh_free_block 0x7bdc8a, __sbh_alloc_block 0x7bdfb5, __sbh_alloc_new_region
//   0x7be2be, __sbh_alloc_new_group 0x7be36f, __sbh_resize_block 0x7be46a (functions.json merges it with the
//   following __sbh_heapmin / __sbh_heap_check, which have no callers either). Every caller of these is in the
//   surface above, so with the family installed they are dead code. There is no _expand, _recalloc, _heapchk,
//   _heapmin, _heapwalk or _set_new_mode in this image (0x00a6b434 _newmode is never written: always 0).
//
// State:
//   * Blocks live in a private Win32 heap, HeapCreate(0, 0x1000, 0) like the original _heap_init(1) (serialized,
//     growable). It is created by _heap_init, or lazily by the first allocation if anything allocates earlier
//     (thread-safe). Its handle is also written to _crtheap 0x00a6b704 so the global keeps a valid value; nothing
//     outside the replaced surface reads it (data-reference scan of joan.asm: only the heap functions above).
//   * [Debug] HeapGuard=1 (a debugging aid, off by default) makes the heap joan::guard_heap instead (each block on
//     its own pages, its end touching a no-access page: an overrun faults at the writer; src/guard_heap.hpp). Not
//     while recording / replaying: those keep their heap (det_heap below, or the Win32 heap) and log a warning.
//   * SBH globals 0x00a6b6e0..0x00a6b700 are no longer written (readers: only the dead SBH functions).
//     __sbh_threshold 0x00a31860 = 0x3f8 is a constant here (its only writer, _set_sbh_threshold, is unreachable).
//   * _pnhHeap 0x00a6b224 stays at its address (written by _set_new_handler, read by _callnewh); _newmode
//     0x00a6b434 is read at its address.
//   * The original's lock 9 (_HEAP_LOCK, taken only by the heap functions) is not used: the Win32 heap is
//     serialized itself, and _set_new_handler swaps the pointer atomically. (This also keeps the family free of
//     the MT-core family: the first allocations happen before _mtinit creates the locks.)
//
// Block sizes (the VC6 usable sizes, so _msize and the tolerated overrun slack are as in the original):
//   n <= 0x3f8 (SBH class):   ((n + 0x17) & ~0xf) - 8   (malloc(0) -> 8 usable bytes, non-NULL)
//   n >  0x3f8 (Win32 class): (n + 0xf) & ~0xf
// Win32-class blocks are allocated with exactly that size (as the original's HeapAlloc), so _msize = HeapSize.
// SBH-class blocks are allocated with 4 bytes more (((n + 0x17) & ~0xf) - 4, i.e. the original SBH entry minus its
// 4-byte header) and _msize = HeapSize - 4: in the original the 4 bytes after the usable area were the entry's
// trailing size tag, which a small overrun (e.g. an off-by-one NUL) clobbered without immediate damage; with an
// exact-size HeapAlloc (8-byte granularity, no slack) the same overrun would hit the next Win32 heap entry header.
// The class of an existing block is recoverable from its HeapSize (SBH-class sizes are 12 mod 16, Win32-class
// sizes 0 mod 16), which realloc and _msize use:
//   SBH-class block,   n <= 0x3f8 -> SBH-class size (original: __sbh_resize_block in place or a new SBH block)
//   SBH-class block,   n >  0x3f8 -> (n + 0xf) & ~0xf (original: HeapAlloc + copy + SBH free)
//   Win32-class block, any n      -> (n + 0xf) & ~0xf (original: HeapReAlloc, even when n <= 0x3f8)
// Other observable rules kept: requests > 0xffffffe0 fail without allocating; calloc multiplies with 32-bit wrap
// (imul); realloc(NULL, n) = malloc(n); realloc(p, 0) = free(p) + NULL; a failed realloc leaves the block intact;
// operator new runs the new-handler loop (_callnewh(n) while the handler returns nonzero), malloc / calloc / realloc
// only when _newmode != 0 (never in this game) with the same handler arguments as the original (calloc: the rounded
// size, or the raw product when it exceeds 0xffffffe0; realloc: the raw size if > 0xffffffe0, else the size rounded
// to 16, which the original then also retries with).
//
// Deliberate differences (unobservable for defined behaviour):
//   * Block addresses and the contents of fresh blocks differ (the SBH recycles freed blocks and VirtualAlloc'd
//     regions start zeroed; HeapAlloc returns whatever its free lists hold). Original functions that read
//     never-written heap memory are UB that the fix/uninit census covers; none relies on SBH recycling patterns
//     that we know of.
//   * calloc zeroes the whole usable block (the original's SBH path zeroed only the requested n*size bytes; the
//     tail up to the usable size was stale data).
//   * realloc (HeapReAlloc) copies min(old, new) allocated bytes (usable + the 4 slack bytes for SBH-class
//     blocks); the original's SBH path copied min(old block size, new
//     size), i.e. up to 8 bytes past the old usable end (the trailing size tag and the next header) - garbage
//     either way.
//   * Out of memory inside the SBH (no new region) made the original fall back to HeapAlloc with the Win32-class
//     size; here an allocation fails only when HeapAlloc fails.
//   * eax after free / operator delete (void functions; no caller reads it).
//   * free / realloc / _msize of a pointer that is not a live block: the original looked it up in the SBH region
//     list first and otherwise handed it to HeapFree / HeapReAlloc / HeapSize on _crtheap, so a foreign or stale
//     pointer outside the SBH regions got exactly the Win32 heap's reaction (HeapFree: FALSE, ignored by free) -
//     the same as here. A stale SBH pointer (double free) was silently mis-handled by the SBH bitmaps; here it is
//     the Win32 heap's double-free handling. Both are UB in the game; none is known.
//
// No game code reads heap block headers: the only [p-4] tag reads of CRT blocks are in realloc / _msize (replaced);
// the [p-4] reads in game code are the compiler's new[] element-count cookies inside the user block, unaffected.
// _msize is used only by _onexit (growth test `_msize(begin) < used + 4`, any value >= the request works).
//
// AT BIRTH: this family must be installed before the CRT entry 0x007b71e6 runs (host::start does that), and it must be enabled or disabled as a whole ([Hooks] Disable=crt/heap): a block
// allocated by the original SBH and freed here (or the reverse) corrupts both heaps. Never disable a single function
// of this module.
#include "../../callconv.hpp"
#include "../../platform/win32_types.hpp"

#include <atomic>
#include <cstdint>

#include "crt.hpp"
#include "../../data_names.hpp"
#include "../../runtime/det_heap.hpp"
#include "../../runtime/guard_heap.hpp"
#include "../../registry.hpp"
#include "../../runtime/replay/replay.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"
#include "../../runtime/loader/import_overrides.hpp"

namespace {
using u32 = std::uint32_t;
using NewHandler = int(JOAN_CDECL*)(u32);

constexpr img::Addr kCrtHeap = d::g_crtHeap;     // _crtheap
constexpr img::Addr kNewHandler = d::g_newHandler;  // _pnhHeap
constexpr img::Addr kNewMode = d::g_newMode;     // _newmode
constexpr u32 kSbhThreshold = 0x3f8;                // __sbh_threshold 0x00a31860 (never changed)
constexpr u32 kMaxRequest = 0xffffffe0u;            // _HEAP_MAXREQ

#ifndef _WIN32
// Elsewhere the private heap is the portable one that answers joan.exe's heap imports (platform/process.cpp);
// on Windows it is Windows' own.
namespace k32 {
#define JOAN_K32(name)                                                                                    \
    template <class... A>                                                                                 \
    auto name(A... a) {                                                                                   \
        static const auto f = joan::loader::override_function<decltype(&::name)>("KERNEL32.dll", #name); \
        return f(a...);                                                                                   \
    }
JOAN_K32(HeapCreate)
JOAN_K32(HeapDestroy)
JOAN_K32(HeapAlloc)
JOAN_K32(HeapFree)
JOAN_K32(HeapReAlloc)
JOAN_K32(HeapSize)
#undef JOAN_K32
}  // namespace k32
using k32::HeapAlloc;
using k32::HeapCreate;
using k32::HeapDestroy;
using k32::HeapFree;
using k32::HeapReAlloc;
using k32::HeapSize;
#endif

JOAN_COUNTER(c_new, "crt::operator new");
JOAN_COUNTER(c_delete, "crt::operator delete");
JOAN_COUNTER(c_malloc, "crt::malloc");
JOAN_COUNTER(c_free, "crt::free");
JOAN_COUNTER(c_calloc, "crt::calloc");
JOAN_COUNTER(c_realloc, "crt::realloc");
JOAN_COUNTER(c_msize, "crt::_msize");
JOAN_COUNTER(c_newh, "crt::_callnewh");
JOAN_COUNTER(c_heap_init, "crt::_heap_init");

std::atomic<HANDLE> g_heap{nullptr};

// While recording / replaying the heap is joan::det_heap (deterministic block placement, src/det_heap.hpp); its
// "handle" is this sentinel, and the h_* functions below route every heap call there.
HANDLE const kDetHeap = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(0x44480000u));
// [Debug] HeapGuard=1 (not while recording / replaying): joan::guard_heap (src/guard_heap.hpp), every block ending at
// a no-access page so that an overrun faults at the writer.
HANDLE const kGuardHeap = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(0x47480000u));

void* h_alloc(HANDLE h, DWORD flags, SIZE_T n) {
    if (h == kDetHeap) return joan::det_heap::alloc(n, (flags & HEAP_ZERO_MEMORY) != 0);
    if (h == kGuardHeap) return joan::guard_heap::alloc(n, (flags & HEAP_ZERO_MEMORY) != 0);
    return HeapAlloc(h, flags, n);
}
void h_free(HANDLE h, void* p) {
    if (h == kDetHeap) joan::det_heap::release(p);
    else if (h == kGuardHeap) joan::guard_heap::release(p);
    else HeapFree(h, 0, p);
}
void* h_realloc(HANDLE h, void* p, SIZE_T n) {
    if (h == kDetHeap) return joan::det_heap::realloc(p, n);
    if (h == kGuardHeap) return joan::guard_heap::realloc(p, n);
    return HeapReAlloc(h, 0, p, n);
}
SIZE_T h_size(HANDLE h, const void* p) {
    if (h == kDetHeap) return joan::det_heap::size(p);
    if (h == kGuardHeap) return joan::guard_heap::size(p);
    return HeapSize(h, 0, p);
}

// The guard heap if [Debug] HeapGuard asks for it and no recording / replay runs (they keep their own heap).
HANDLE guard_heap_if_requested() {
    if (!joan::guard_heap::requested()) return nullptr;
    if (joan::replay::active()) {
        joan::hook::log_line("heap guard: [Debug] HeapGuard=1 is ignored while recording / replaying (the replay keeps "
                             "its heap)");
        return nullptr;
    }
    return joan::guard_heap::init() ? kGuardHeap : nullptr;
}

// The private heap, created on first use (normally by _heap_init, before anything else allocates).
HANDLE heap() {
    HANDLE h = g_heap.load(std::memory_order_acquire);
    if (h) return h;
    if (HANDLE guard = guard_heap_if_requested()) {
        HANDLE expected = nullptr;
        if (g_heap.compare_exchange_strong(expected, guard, std::memory_order_acq_rel)) {
            img::global<HANDLE volatile>(kCrtHeap) = guard;
            return guard;
        }
        return expected;
    }
    if (joan::replay::wants_deterministic_heap() && joan::det_heap::init()) {
        HANDLE expected = nullptr;
        if (g_heap.compare_exchange_strong(expected, kDetHeap, std::memory_order_acq_rel)) {
            img::global<HANDLE volatile>(kCrtHeap) = kDetHeap;
            return kDetHeap;
        }
        return expected;
    }
    HANDLE created = HeapCreate(0, 0x1000, 0);
    if (!created) return nullptr;
    HANDLE expected = nullptr;
    if (!g_heap.compare_exchange_strong(expected, created, std::memory_order_acq_rel)) {
        HeapDestroy(created);  // another thread won the race
        return expected;
    }
    img::global<HANDLE volatile>(kCrtHeap) = created;
    return created;
}

u32 round16(u32 n) { return (n + 0xf) & ~0xfu; }
// HeapAlloc size of a fresh block for request n (n <= 0xffffffe0), see the header: SBH class = VC6 usable size + 4
// (12 mod 16), Win32 class = VC6 usable size (0 mod 16).
u32 block_size(u32 n) { return n <= kSbhThreshold ? ((n + 0x17) & ~0xfu) - 4 : round16(n); }
constexpr SIZE_T kSbhSlack = 4;
bool sbh_class(SIZE_T heap_size) { return (heap_size & 0xf) == 0xc; }  // HeapSize failure ((SIZE_T)-1) -> false

int newmode() { return img::global<const volatile int>(kNewMode); }

// ---------------------------------------------------------------------------------------------------- 0x007b4c6f
// _callnewh: calls the new handler with the request size; 1 if it returned nonzero (retry), else 0.
int JOAN_CDECL crt_callnewh(u32 n) {
    c_newh.hit();
    const NewHandler pnh = img::global<NewHandler volatile>(kNewHandler);
    if (pnh && pnh(n) != 0) return 1;
    return 0;
}

// ---------------------------------------------------------------------------------------------------- 0x007b6c92
void* JOAN_CDECL crt_heap_alloc(u32 n) {
    HANDLE h = heap();
    return h ? h_alloc(h, 0, block_size(n)) : nullptr;
}

// ---------------------------------------------------------------------------------------------------- 0x007b6c66
void* JOAN_CDECL crt_nh_malloc(u32 n, int nh_flag) {
    if (n > kMaxRequest) return nullptr;
    for (;;) {
        void* p = crt_heap_alloc(n);
        if (p) return p;
        if (nh_flag == 0) return nullptr;
        if (!crt_callnewh(n)) return nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------- 0x007b6c54
void* JOAN_CDECL crt_malloc(u32 n) {
    c_malloc.hit();
    return crt_nh_malloc(n, newmode());
}

// ---------------------------------------------------------------------------------------------------- 0x007b1d35
void* JOAN_CDECL crt_operator_new(u32 n) {
    c_new.hit();
    return crt_nh_malloc(n, 1);
}

// ---------------------------------------------------------------------------------------------------- 0x007b4e1d
void JOAN_CDECL crt_free(void* p) {
    c_free.hit();
    if (!p) return;
    HANDLE h = g_heap.load(std::memory_order_acquire);
    if (h) h_free(h, p);
}

// ---------------------------------------------------------------------------------------------------- 0x007b1d2a
void JOAN_CDECL crt_operator_delete(void* p) {
    c_delete.hit();
    crt_free(p);
}

// ---------------------------------------------------------------------------------------------------- 0x007b7b87
void* JOAN_CDECL crt_calloc(u32 num, u32 size) {
    c_calloc.hit();
    const u32 total = num * size;  // imul: 32-bit wrap as in the original
    u32 rounded = total;
    if (rounded <= kMaxRequest) rounded = round16(rounded == 0 ? 1 : rounded);
    for (;;) {
        if (rounded <= kMaxRequest) {
            HANDLE h = heap();
            void* p = h ? h_alloc(h, HEAP_ZERO_MEMORY, total <= kSbhThreshold ? block_size(total) : rounded)
                        : nullptr;
            if (p) return p;
        }
        if (newmode() == 0) return nullptr;
        if (!crt_callnewh(rounded)) return nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------- 0x007bc089
void* JOAN_CDECL crt_realloc(void* p, u32 n) {
    c_realloc.hit();
    if (!p) return crt_malloc(n);  // the original calls malloc 0x7b6c54
    if (n == 0) {
        crt_free(p);
        return nullptr;
    }
    for (;;) {
        if (n <= kMaxRequest) {
            HANDLE h = heap();
            if (h) {
                const bool sbh_block = sbh_class(h_size(h, p));
                void* q = h_realloc(h, p, sbh_block && n <= kSbhThreshold ? block_size(n) : round16(n));
                if (q) return q;
            }
            n = round16(n);  // the original rounds its size variable in place before the failed HeapAlloc
        }
        if (newmode() == 0) return nullptr;
        if (!crt_callnewh(n)) return nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------- 0x007bc1c1
u32 JOAN_CDECL crt_msize(void* p) {
    c_msize.hit();
    const SIZE_T cur = h_size(heap(), p);
    return static_cast<u32>(sbh_class(cur) ? cur - kSbhSlack : cur);
}

// ---------------------------------------------------------------------------------------------------- 0x007b4c45
NewHandler JOAN_CDECL crt_set_new_handler(NewHandler pnh) {
    auto* slot = &img::global<std::atomic<NewHandler>>(kNewHandler);
    static_assert(sizeof(std::atomic<NewHandler>) == sizeof(NewHandler), "lock-free pointer atomic");
    return slot->exchange(pnh);
}

// ---------------------------------------------------------------------------------------------------- 0x007bdb53
// _heap_init(mtflag): creates our heap (if no early allocation did already) and returns 1; 0 if HeapCreate failed
// (the startup then stops with R6028 "unable to initialize heap" through fast_error_exit 0x7b7313, as in the original). The original's HEAP_NO_SERIALIZE
// for mtflag 0 is not reproduced: the only call passes 1.
int JOAN_CDECL crt_heap_init(int /*mtflag*/) {
    c_heap_init.hit();
    return heap() ? 1 : 0;
}

#define B(name, addr, fn) JOAN_REPLACE("crt/heap", name, addr, fn)
B("operator delete", 0x007b1d2a, &crt_operator_delete);
B("operator new", 0x007b1d35, &crt_operator_new);
B("_set_new_handler", 0x007b4c45, &crt_set_new_handler);
B("_callnewh", 0x007b4c6f, &crt_callnewh);
B("free", 0x007b4e1d, &crt_free);
B("malloc", 0x007b6c54, &crt_malloc);
B("_nh_malloc", 0x007b6c66, &crt_nh_malloc);
B("_heap_alloc", 0x007b6c92, &crt_heap_alloc);
B("calloc", 0x007b7b87, &crt_calloc);
B("realloc", 0x007bc089, &crt_realloc);
B("_msize", 0x007bc1c1, &crt_msize);
B("_heap_init", 0x007bdb53, &crt_heap_init);
#undef B

}  // namespace

// Direct entry points (crt.hpp).
namespace joan::crt {
void* JOAN_CDECL operator_new(std::uint32_t n) { return crt_operator_new(n); }
void JOAN_CDECL operator_delete(void* p) { crt_operator_delete(p); }
}  // namespace joan::crt

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::operator_(void* a0) -> void { joan::bind::free<&::crt_operator_delete>(a0); }  // 0x7b1d2a
auto joan::orig::free_(void* a0) -> void { joan::bind::free<&::crt_free>(a0); }  // 0x7b4e1d
auto joan::orig::malloc_(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_malloc>(a0)); }  // 0x7b6c54
auto joan::orig::calloc(std::uint32_t a0, std::uint32_t a1) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_calloc>(a0, a1)); }  // 0x7b7b87
auto joan::orig::_heap_init(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_heap_init>(a0)); }  // 0x7bdb53
