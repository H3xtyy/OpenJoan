// joan.exe's process / thread / memory imports on portable code (phase C5 round F3a, src/platform/README.md).
//
// The census of a session (openjoan_imports.txt) shows few of these called at all - most belong to start-up code of the
// C runtime or to paths that never run with the portable layers (the streaming worker thread of ilib/audio.cpp is only
// started when ReadFileEx fails, which platform/files.cpp's never does). They are all here so that the game's import
// table needs as little of KERNEL32 as possible:
//   - threads, events, waits: CreateThread (std::thread; CREATE_SUSPENDED until ResumeThread), CreateEventA / SetEvent /
//     ResetEvent, WaitForSingleObject and (platform/files.cpp) WaitForMultipleObjectsEx on them; handles of ours
//     (0x6C000004 + 4 n), closed by CloseHandle (files.cpp dispatches here); thread-local storage (TlsAlloc ...);
//     GetCurrentThreadId (SDL's id: the OS thread id), the pseudo handles of GetCurrentProcess / GetCurrentThread;
//   - memory: HeapCreate / HeapAlloc / HeapReAlloc / HeapSize / HeapFree / HeapDestroy / HeapValidate /
//     GetProcessHeap on the C heap (a block that is not ours - D3DX frees d3d8.dll's validator text with
//     HeapFree(GetProcessHeap()) - goes to Windows), VirtualAlloc / VirtualFree (zeroed, 64 KB-aligned blocks);
//   - the start-up queries of the C runtime: GetStartupInfoA (no inherited handles, default show state), GetStdHandle /
//     SetStdHandle (none: a windowed program), SetHandleCount, GetVersion / GetVersionExA (Windows 8 = 6.2.9200, what
//     Windows reports to a program without a compatibility manifest), GlobalMemoryStatus (the computer's memory,
//     clamped to the 2 GB a 32-bit program sees), SetConsoleCtrlHandler;
//   - small helpers: MulDiv (Windows' rounding), IsBadReadPtr / IsBadWritePtr / IsBadCodePtr (null checks: the C
//     runtime's argument validation), OutputDebugStringA (openjoan.log), DebugBreak, FatalAppExitA (a message box,
//     then exit), TerminateProcess of this process, GetTempPathA / GetTempFileNameA.
//   - the process image (port round P2): GetModuleHandleA (the game's instance handle: this executable, whose
//     resources platform/sdl_window.cpp reads), GetModuleFileNameA (the C runtime's _pgmname and its runtime-error
//     text), GetCommandLineA (the C runtime's _acmdln: what WinMain gets and __argv is parsed from), the environment
//     block GetEnvironmentStringsW / FreeEnvironmentStringsW (the C runtime's _environ; never NULL: an empty block
//     instead, so the C runtime's narrow fallback is not needed), SetEnvironmentVariableA (_chdir's "=C:" variable,
//     _putenv), GetLastError (the value platform code of ours set; the replay records it on the game thread).
//     On Windows these are Windows' values (the replay's recordings compare them: the module file name, for one, is
//     in the hashed .data); elsewhere they come from the C library / SDL and our own state.
// LoadLibraryA / GetProcAddress / FreeLibrary, GetEnvironmentStrings / FreeEnvironmentStringsA and RaiseException have
// no caller left (dead imports: traps). Still Windows': RtlUnwind (C++ EH runtime), ExitThread.
#include "win32_types.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#ifndef _WIN32
#include <unistd.h>  // environ
#include <pthread.h>  // pthread_exit (exit_thread of a thread not ours)
#endif

#include "../runtime/loader/import_overrides.hpp"
#include "../registry.hpp"
#include "files.hpp"
#include "threads.hpp"
#include "memory.hpp"

namespace fs = std::filesystem;

namespace {

#ifndef _WIN32
thread_local DWORD t_last_error = 0;  // GetLastError (platform code elsewhere has to set it through here as well)
#endif
void set_error(DWORD e) {
#ifdef _WIN32
    ::SetLastError(e);
#else
    t_last_error = e;
#endif
}

// ---------------------------------------------------------------- sync objects

struct Sync {
    enum class Kind { Event, Thread } kind = Kind::Event;
    std::mutex m;
    std::condition_variable cv;
    bool signaled = false;
    bool manual = true;
    // thread
    std::thread thread;
    bool suspended = false;
    DWORD exit_code = STILL_ACTIVE;
    DWORD tid = 0;
    LPTHREAD_START_ROUTINE start = nullptr;
    void* param = nullptr;
};

constexpr std::uintptr_t kSyncBase = 0x6C000000u;
constexpr std::uintptr_t kSyncSpan = 0x00400000u;

std::mutex g_sync_lock;
std::vector<std::shared_ptr<Sync>> g_sync;
std::vector<std::size_t> g_sync_free;

bool is_sync(HANDLE h) {
    const auto v = reinterpret_cast<std::uintptr_t>(h);
    return v > kSyncBase && v < kSyncBase + kSyncSpan && (v & 3) == 0;
}
std::shared_ptr<Sync> sync_of(HANDLE h) {
    if (!is_sync(h)) return nullptr;
    const std::size_t i = (reinterpret_cast<std::uintptr_t>(h) - kSyncBase) / 4 - 1;
    std::lock_guard<std::mutex> l(g_sync_lock);
    return i < g_sync.size() ? g_sync[i] : nullptr;
}
HANDLE add_sync(std::shared_ptr<Sync> s) {
    std::lock_guard<std::mutex> l(g_sync_lock);
    std::size_t i;
    if (!g_sync_free.empty()) {
        i = g_sync_free.back();
        g_sync_free.pop_back();
        g_sync[i] = std::move(s);
    } else {
        i = g_sync.size();
        g_sync.push_back(std::move(s));
    }
    return reinterpret_cast<HANDLE>(kSyncBase + 4 * (i + 1));
}

// Is `s` signaled? Consumes an auto-reset event's signal when `take`.
bool check(Sync& s, bool take) {
    if (!s.signaled) return false;
    if (take && s.kind == Sync::Kind::Event && !s.manual) s.signaled = false;
    return true;
}

#ifdef _WIN32
DWORD current_tid() { return static_cast<DWORD>(SDL_GetCurrentThreadID()); }
#else
// Asked on every draw (Device::owner_thread): the id once per thread, not a call into SDL each time.
DWORD current_tid() {
    thread_local const DWORD id = static_cast<DWORD>(SDL_GetCurrentThreadID());
    return id;
}
#endif

// ---------------------------------------------------------------- threads

// exit_thread on a thread of ours: unwinds to the thread's start (the lambda of j_CreateThread), which finishes it.
struct ThreadExit {
    DWORD code;
};
thread_local bool t_ours = false;  // this thread was started by j_CreateThread

HANDLE WINAPI j_CreateThread(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE start, LPVOID param, DWORD flags,
                             LPDWORD tid) {
    auto s = std::make_shared<Sync>();
    s->kind = Sync::Kind::Thread;
    s->manual = true;
    s->suspended = (flags & CREATE_SUSPENDED) != 0;
    s->start = start;
    s->param = param;
    bool started = false;
    std::mutex sm;
    std::condition_variable scv;
    s->thread = std::thread([s, &started, &sm, &scv] {
        {
            std::lock_guard<std::mutex> l(sm);
            s->tid = current_tid();
            started = true;
            scv.notify_one();  // under the lock: CreateThread's sm / scv live only until it sees `started`
        }
        {
            std::unique_lock<std::mutex> l(s->m);
            s->cv.wait(l, [&] { return !s->suspended; });
        }
        t_ours = true;
        DWORD code = 0;
        try {
            code = s->start(s->param);
        } catch (const ThreadExit& e) {  // ExitThread / _endthread inside the thread (joan::platform::exit_thread)
            code = e.code;
        }
        {
            std::lock_guard<std::mutex> l(s->m);
            s->exit_code = code;
            s->signaled = true;
        }
        s->cv.notify_all();
    });
    {
        std::unique_lock<std::mutex> l(sm);
        scv.wait(l, [&] { return started; });
    }
    s->thread.detach();  // a thread handle may be closed while the thread runs (as Windows)
    if (tid) *tid = s->tid;
    return add_sync(s);
}

DWORD WINAPI j_ResumeThread(HANDLE h) {
    auto s = sync_of(h);
    if (!s || s->kind != Sync::Kind::Thread) JOAN_NOT_OURS(::ResumeThread(h), static_cast<DWORD>(-1));
    DWORD prev = 0;
    {
        std::lock_guard<std::mutex> l(s->m);
        prev = s->suspended ? 1u : 0u;
        s->suspended = false;
    }
    s->cv.notify_all();
    return prev;
}

DWORD WINAPI j_GetCurrentThreadId() { return current_tid(); }
HANDLE WINAPI j_GetCurrentThread() { return reinterpret_cast<HANDLE>(static_cast<std::intptr_t>(-2)); }
HANDLE WINAPI j_GetCurrentProcess() { return reinterpret_cast<HANDLE>(static_cast<std::intptr_t>(-1)); }

BOOL WINAPI j_TerminateProcess(HANDLE h, UINT code) {
    if (h != j_GetCurrentProcess()) JOAN_NOT_OURS(::TerminateProcess(h, code), FALSE);
    std::fflush(nullptr);
    std::_Exit(static_cast<int>(code));
}

// ---------------------------------------------------------------- events, waits

HANDLE WINAPI j_CreateEventA(LPSECURITY_ATTRIBUTES, BOOL manual, BOOL initial, LPCSTR name) {
    if (name && *name) JOAN_NOT_OURS(::CreateEventA(nullptr, manual, initial, name), nullptr);  // named (shared) events: Windows'
    auto s = std::make_shared<Sync>();
    s->kind = Sync::Kind::Event;
    s->manual = manual != FALSE;
    s->signaled = initial != FALSE;
    return add_sync(s);
}

BOOL WINAPI j_SetEvent(HANDLE h) {
    auto s = sync_of(h);
    if (!s) JOAN_NOT_OURS(::SetEvent(h), FALSE);
    {
        std::lock_guard<std::mutex> l(s->m);
        s->signaled = true;
    }
    s->cv.notify_all();
    return TRUE;
}

BOOL WINAPI j_ResetEvent(HANDLE h) {
    auto s = sync_of(h);
    if (!s) JOAN_NOT_OURS(::ResetEvent(h), FALSE);
    std::lock_guard<std::mutex> l(s->m);
    s->signaled = false;
    return TRUE;
}

// One object: its condition variable. Several: polling every millisecond (only the streaming worker waits on several).
DWORD wait_objects(DWORD n, const HANDLE* hs, bool all, DWORD ms, bool alertable) {
    std::vector<std::shared_ptr<Sync>> objs;
    for (DWORD i = 0; i < n; ++i) objs.push_back(sync_of(hs[i]));
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms == INFINITE ? 0 : ms);
    if (n == 1 && !alertable) {
        Sync& s = *objs[0];
        std::unique_lock<std::mutex> l(s.m);
        auto ready = [&] { return s.signaled; };
        if (ms == INFINITE) s.cv.wait(l, ready);
        else if (!s.cv.wait_until(l, deadline, ready)) return WAIT_TIMEOUT;
        check(s, true);
        return WAIT_OBJECT_0;
    }
    for (;;) {
        if (alertable && joan::platform::files::run_pending_io()) return WAIT_IO_COMPLETION;
        if (all) {
            bool every = true;
            for (auto& o : objs) {
                std::lock_guard<std::mutex> l(o->m);
                every = every && o->signaled;
            }
            if (every) {
                for (auto& o : objs) {
                    std::lock_guard<std::mutex> l(o->m);
                    check(*o, true);
                }
                return WAIT_OBJECT_0;
            }
        } else {
            for (DWORD i = 0; i < n; ++i) {
                std::lock_guard<std::mutex> l(objs[i]->m);
                if (check(*objs[i], true)) return WAIT_OBJECT_0 + i;
            }
        }
        if (ms != INFINITE && std::chrono::steady_clock::now() >= deadline) return WAIT_TIMEOUT;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

DWORD WINAPI j_WaitForSingleObject(HANDLE h, DWORD ms) {
    if (!sync_of(h)) JOAN_NOT_OURS(::WaitForSingleObject(h, ms), WAIT_FAILED);
    return wait_objects(1, &h, false, ms, false);
}

// ---------------------------------------------------------------- thread-local storage

constexpr DWORD kTlsSlots = 128;
std::mutex g_tls_lock;
bool g_tls_used[kTlsSlots] = {};
thread_local void* t_tls[kTlsSlots] = {};

DWORD WINAPI j_TlsAlloc() {
    std::lock_guard<std::mutex> l(g_tls_lock);
    for (DWORD i = 0; i < kTlsSlots; ++i)
        if (!g_tls_used[i]) {
            g_tls_used[i] = true;
            return i;
        }
    set_error(ERROR_NO_MORE_ITEMS);
    return TLS_OUT_OF_INDEXES;
}
BOOL WINAPI j_TlsFree(DWORD i) {
    std::lock_guard<std::mutex> l(g_tls_lock);
    if (i >= kTlsSlots || !g_tls_used[i]) {
        set_error(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    g_tls_used[i] = false;
    return TRUE;
}
LPVOID WINAPI j_TlsGetValue(DWORD i) {
    if (i >= kTlsSlots) {
        set_error(ERROR_INVALID_PARAMETER);
        return nullptr;
    }
    set_error(0);  // Windows clears it: a null value is told from an error that way
    return t_tls[i];
}
BOOL WINAPI j_TlsSetValue(DWORD i, LPVOID v) {
    if (i >= kTlsSlots) {
        set_error(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    t_tls[i] = v;
    return TRUE;
}

// ---------------------------------------------------------------- heaps

#ifdef _WIN32
struct Block {
    std::size_t size;
    HANDLE heap;
};
std::unordered_map<void*, Block> g_blocks;
#endif
std::mutex g_heap_lock;
const HANDLE kProcessHeap = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(0x6B000004u));
std::uintptr_t g_next_heap = 0x6B000008u;

bool is_heap(HANDLE h) {
    const auto v = reinterpret_cast<std::uintptr_t>(h);
    return v >= 0x6B000004u && v < 0x6B400000u;
}

HANDLE WINAPI j_GetProcessHeap() { return kProcessHeap; }
HANDLE WINAPI j_HeapCreate(DWORD, SIZE_T, SIZE_T) {
    std::lock_guard<std::mutex> l(g_heap_lock);
    const HANDLE h = reinterpret_cast<HANDLE>(g_next_heap);
    g_next_heap += 4;
    return h;
}
#ifdef _WIN32
BOOL WINAPI j_HeapDestroy(HANDLE heap) {
    if (!is_heap(heap)) JOAN_NOT_OURS(::HeapDestroy(heap), FALSE);
    std::lock_guard<std::mutex> l(g_heap_lock);
    for (auto it = g_blocks.begin(); it != g_blocks.end();) {
        if (it->second.heap == heap) {
            std::free(it->first);
            it = g_blocks.erase(it);
        } else {
            ++it;
        }
    }
    return TRUE;
}
LPVOID WINAPI j_HeapAlloc(HANDLE heap, DWORD flags, SIZE_T n) {
    if (!is_heap(heap)) JOAN_NOT_OURS(::HeapAlloc(heap, flags, n), nullptr);
    void* p = (flags & HEAP_ZERO_MEMORY) ? std::calloc(n ? n : 1, 1) : std::malloc(n ? n : 1);
    if (!p) {
        set_error(ERROR_NOT_ENOUGH_MEMORY);
        return nullptr;
    }
    std::lock_guard<std::mutex> l(g_heap_lock);
    g_blocks[p] = Block{n, heap};
    return p;
}
LPVOID WINAPI j_HeapReAlloc(HANDLE heap, DWORD flags, LPVOID p, SIZE_T n) {
    if (!is_heap(heap)) JOAN_NOT_OURS(::HeapReAlloc(heap, flags, p, n), nullptr);
    std::size_t old = 0;
    {
        std::lock_guard<std::mutex> l(g_heap_lock);
        auto it = g_blocks.find(p);
        if (it == g_blocks.end()) {
            set_error(ERROR_INVALID_PARAMETER);
            return nullptr;
        }
        old = it->second.size;
    }
    if (flags & HEAP_REALLOC_IN_PLACE_ONLY) {
        if (n > old) {
            set_error(ERROR_NOT_ENOUGH_MEMORY);
            return nullptr;
        }
        std::lock_guard<std::mutex> l(g_heap_lock);
        g_blocks[p].size = n;
        return p;
    }
    void* q = std::realloc(p, n ? n : 1);
    if (!q) {
        set_error(ERROR_NOT_ENOUGH_MEMORY);
        return nullptr;
    }
    if ((flags & HEAP_ZERO_MEMORY) && n > old) std::memset(static_cast<std::uint8_t*>(q) + old, 0, n - old);
    std::lock_guard<std::mutex> l(g_heap_lock);
    g_blocks.erase(p);
    g_blocks[q] = Block{n, heap};
    return q;
}
SIZE_T WINAPI j_HeapSize(HANDLE heap, DWORD flags, LPCVOID p) {
    if (!is_heap(heap)) JOAN_NOT_OURS(::HeapSize(heap, flags, p), static_cast<SIZE_T>(-1));
    std::lock_guard<std::mutex> l(g_heap_lock);
    auto it = g_blocks.find(const_cast<void*>(p));
    return it == g_blocks.end() ? static_cast<SIZE_T>(-1) : it->second.size;
}
BOOL WINAPI j_HeapFree(HANDLE heap, DWORD flags, LPVOID p) {
    if (!p) return TRUE;
    {
        std::lock_guard<std::mutex> l(g_heap_lock);
        auto it = g_blocks.find(p);
        if (it != g_blocks.end()) {
            g_blocks.erase(it);
            std::free(p);
            return TRUE;
        }
    }
    // Not ours: Windows' process heap (d3d8.dll's validator text) or a real heap.
    JOAN_NOT_OURS(::HeapFree(is_heap(heap) ? ::GetProcessHeap() : heap, flags, p), FALSE);
}
BOOL WINAPI j_HeapValidate(HANDLE heap, DWORD flags, LPCVOID p) {
    if (!is_heap(heap)) JOAN_NOT_OURS(::HeapValidate(heap, flags, p), FALSE);
    if (!p) return TRUE;
    std::lock_guard<std::mutex> l(g_heap_lock);
    return g_blocks.count(const_cast<void*>(p)) ? TRUE : FALSE;
}
#else
// Elsewhere every heap block is ours (there is no system heap to pass on to): a header in front of each block holds
// its size and heap, so HeapAlloc / HeapFree take no lock and no table - the game's C runtime heap (operator new /
// malloc, lib/crt/heap.cpp) comes through here a few hundred times a frame. The header keeps malloc's alignment.
struct alignas(16) BlockHeader {
    std::size_t size;
    HANDLE heap;
    std::uint32_t magic;
};
static_assert(sizeof(BlockHeader) % 16 == 0);
constexpr std::uint32_t kBlockMagic = 0x424f414au;  // "JAOB"

BlockHeader* header_of(LPCVOID p) {
    return reinterpret_cast<BlockHeader*>(const_cast<char*>(static_cast<const char*>(p)) - sizeof(BlockHeader));
}
bool our_block(LPCVOID p) { return p != nullptr && header_of(p)->magic == kBlockMagic; }

BOOL WINAPI j_HeapDestroy(HANDLE heap) {
    if (!is_heap(heap)) JOAN_NOT_OURS(::HeapDestroy(heap), FALSE);
    // Its blocks are not listed here: they stay until the process ends (the C runtime destroys its heap at exit).
    return TRUE;
}
LPVOID WINAPI j_HeapAlloc(HANDLE heap, DWORD flags, SIZE_T n) {
    if (!is_heap(heap)) JOAN_NOT_OURS(::HeapAlloc(heap, flags, n), nullptr);
    const std::size_t bytes = sizeof(BlockHeader) + n;
    void* const raw = (flags & HEAP_ZERO_MEMORY) ? std::calloc(1, bytes) : std::malloc(bytes);
    if (!raw) {
        set_error(ERROR_NOT_ENOUGH_MEMORY);
        return nullptr;
    }
    auto* const h = static_cast<BlockHeader*>(raw);
    h->size = n;
    h->heap = heap;
    h->magic = kBlockMagic;
    return h + 1;
}
LPVOID WINAPI j_HeapReAlloc(HANDLE heap, DWORD flags, LPVOID p, SIZE_T n) {
    if (!is_heap(heap)) JOAN_NOT_OURS(::HeapReAlloc(heap, flags, p, n), nullptr);
    if (!our_block(p)) {
        set_error(ERROR_INVALID_PARAMETER);
        return nullptr;
    }
    BlockHeader* const h = header_of(p);
    const std::size_t old = h->size;
    if (flags & HEAP_REALLOC_IN_PLACE_ONLY) {
        if (n > old) {
            set_error(ERROR_NOT_ENOUGH_MEMORY);
            return nullptr;
        }
        h->size = n;
        return p;
    }
    auto* const q = static_cast<BlockHeader*>(std::realloc(h, sizeof(BlockHeader) + n));
    if (!q) {
        set_error(ERROR_NOT_ENOUGH_MEMORY);
        return nullptr;
    }
    q->size = n;
    q->heap = heap;
    if ((flags & HEAP_ZERO_MEMORY) && n > old) std::memset(reinterpret_cast<std::uint8_t*>(q + 1) + old, 0, n - old);
    return q + 1;
}
SIZE_T WINAPI j_HeapSize(HANDLE heap, [[maybe_unused]] DWORD flags, LPCVOID p) {
    if (!is_heap(heap)) JOAN_NOT_OURS(::HeapSize(heap, flags, p), static_cast<SIZE_T>(-1));
    return our_block(p) ? header_of(p)->size : static_cast<SIZE_T>(-1);
}
BOOL WINAPI j_HeapFree([[maybe_unused]] HANDLE heap, [[maybe_unused]] DWORD flags, LPVOID p) {
    if (!p) return TRUE;
    if (!our_block(p)) JOAN_NOT_OURS(::HeapFree(heap, flags, p), FALSE);
    BlockHeader* const h = header_of(p);
    h->magic = 0;  // a second free of it is caught
    std::free(h);
    return TRUE;
}
BOOL WINAPI j_HeapValidate(HANDLE heap, [[maybe_unused]] DWORD flags, LPCVOID p) {
    if (!is_heap(heap)) JOAN_NOT_OURS(::HeapValidate(heap, flags, p), FALSE);
    return !p || our_block(p) ? TRUE : FALSE;
}
#endif

// ---------------------------------------------------------------- virtual memory

constexpr std::size_t kGranularity = 0x10000;
std::mutex g_vm_lock;
std::unordered_map<void*, std::size_t> g_regions;  // base -> size

void* aligned_zeroed(std::size_t n) {
#ifdef _WIN32
    void* p = _aligned_malloc(n, kGranularity);
#else
    void* p = std::aligned_alloc(kGranularity, n);
#endif
    if (p) std::memset(p, 0, n);
    return p;
}
void aligned_free(void* p) {
#ifdef _WIN32
    _aligned_free(p);
#else
    std::free(p);
#endif
}
bool region_of(const void* p, void** base) {
    const auto v = reinterpret_cast<std::uintptr_t>(p);
    for (const auto& [b, n] : g_regions) {
        const auto bv = reinterpret_cast<std::uintptr_t>(b);
        if (v >= bv && v < bv + n) {
            *base = b;
            return true;
        }
    }
    return false;
}

// A fixed-address request outside our blocks (the C runtime commits inside blocks of ours, above): the system places
// it (platform/memory.hpp).
std::unordered_map<void*, std::size_t> g_fixed;  // reservations made there: base -> size (POSIX release needs it)
unsigned access_of(DWORD protect) {
    namespace mem = joan::platform::memory;
    switch (protect & 0xff) {
        case PAGE_READONLY: return mem::kRead;
        case PAGE_READWRITE:
        case PAGE_WRITECOPY: return mem::kReadWrite;
        case PAGE_EXECUTE: return mem::kExec;
        case PAGE_EXECUTE_READ: return mem::kReadExec;
        case PAGE_EXECUTE_READWRITE:
        case PAGE_EXECUTE_WRITECOPY: return mem::kAll;
        default: return mem::kNone;
    }
}
LPVOID fixed_alloc(LPVOID addr, SIZE_T n, DWORD type, DWORD protect) {
    namespace mem = joan::platform::memory;
    void* p = addr;
    if (type & MEM_RESERVE) {
        p = mem::reserve(n, addr);
        if (!p) {
            set_error(ERROR_INVALID_ADDRESS);
            return nullptr;
        }
        g_fixed[p] = n;
    }
    if ((type & MEM_COMMIT) && !mem::commit(p, n, access_of(protect))) {
        set_error(ERROR_INVALID_ADDRESS);
        return nullptr;
    }
    return p;
}
BOOL fixed_free(LPVOID addr, SIZE_T n, DWORD type) {
    namespace mem = joan::platform::memory;
    if (type & MEM_RELEASE) {
        const auto it = g_fixed.find(addr);
        if (it == g_fixed.end() || !mem::release(addr, it->second)) {
            set_error(ERROR_INVALID_ADDRESS);
            return FALSE;
        }
        g_fixed.erase(it);
        return TRUE;
    }
    return mem::decommit(addr, n) ? TRUE : FALSE;
}

LPVOID WINAPI j_VirtualAlloc(LPVOID addr, SIZE_T n, DWORD type, DWORD protect) {
    std::lock_guard<std::mutex> l(g_vm_lock);
    if (addr) {
        void* base = nullptr;
        if (region_of(addr, &base)) return addr;  // committing part of a block of ours: it is all usable already
        return fixed_alloc(addr, n, type, protect);
    }
    const std::size_t size = (n + kGranularity - 1) / kGranularity * kGranularity;
    void* p = aligned_zeroed(size ? size : kGranularity);
    if (!p) {
        set_error(ERROR_NOT_ENOUGH_MEMORY);
        return nullptr;
    }
    g_regions[p] = size;
    return p;
}
BOOL WINAPI j_VirtualFree(LPVOID addr, SIZE_T n, DWORD type) {
    std::lock_guard<std::mutex> l(g_vm_lock);
    void* base = nullptr;
    if (!region_of(addr, &base)) return fixed_free(addr, n, type);
    if (type & MEM_RELEASE) {
        if (base != addr) {
            set_error(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        g_regions.erase(base);
        aligned_free(base);
        return TRUE;
    }
    // MEM_DECOMMIT: the pages read as zeros when committed again.
    const std::size_t total = g_regions[base];
    const auto off = static_cast<std::size_t>(static_cast<std::uint8_t*>(addr) - static_cast<std::uint8_t*>(base));
    const std::size_t len = n ? std::min(static_cast<std::size_t>(n), total - off) : total - off;
    std::memset(addr, 0, len);
    return TRUE;
}

// ---------------------------------------------------------------- start-up queries

void WINAPI j_GetStartupInfoA(LPSTARTUPINFOA si) {
    std::memset(si, 0, sizeof(*si));
    si->cb = sizeof(*si);  // no STARTF_ flags: default window state, no inherited handles (lpReserved2 null)
}

HANDLE g_std[3] = {nullptr, nullptr, nullptr};  // a windowed program has no console
int std_index(DWORD which) {
    switch (which) {
        case STD_INPUT_HANDLE: return 0;
        case STD_OUTPUT_HANDLE: return 1;
        case STD_ERROR_HANDLE: return 2;
        default: return -1;
    }
}
HANDLE WINAPI j_GetStdHandle(DWORD which) {
    const int i = std_index(which);
    if (i < 0) {
        set_error(ERROR_INVALID_HANDLE);
        return INVALID_HANDLE_VALUE;
    }
    return g_std[i];
}
BOOL WINAPI j_SetStdHandle(DWORD which, HANDLE h) {
    const int i = std_index(which);
    if (i < 0) {
        set_error(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    g_std[i] = h;
    return TRUE;
}
UINT WINAPI j_SetHandleCount(UINT n) { return n; }
BOOL WINAPI j_SetConsoleCtrlHandler(PHANDLER_ROUTINE, BOOL) { return TRUE; }

DWORD WINAPI j_GetVersion() { return (9200u << 16) | (2u << 8) | 6u; }  // 6.2.9200, NT
BOOL WINAPI j_GetVersionExA(LPOSVERSIONINFOA v) {
    if (!v || (v->dwOSVersionInfoSize != sizeof(OSVERSIONINFOA) && v->dwOSVersionInfoSize != sizeof(OSVERSIONINFOEXA))) {
        set_error(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }
    const DWORD size = v->dwOSVersionInfoSize;
    std::memset(v, 0, size);
    v->dwOSVersionInfoSize = size;
    v->dwMajorVersion = 6;
    v->dwMinorVersion = 2;
    v->dwBuildNumber = 9200;
    v->dwPlatformId = VER_PLATFORM_WIN32_NT;
    if (size == sizeof(OSVERSIONINFOEXA)) reinterpret_cast<OSVERSIONINFOEXA*>(v)->wProductType = VER_NT_WORKSTATION;
    return TRUE;
}

void WINAPI j_GlobalMemoryStatus(LPMEMORYSTATUS m) {
    const std::uint64_t ram = static_cast<std::uint64_t>(SDL_GetSystemRAM()) * 1024 * 1024;
    const SIZE_T phys = static_cast<SIZE_T>(std::min<std::uint64_t>(ram ? ram : 0x40000000ull, 0x7fffffffull));
    std::memset(m, 0, sizeof(*m));
    m->dwLength = sizeof(*m);
    m->dwMemoryLoad = 50;
    m->dwTotalPhys = phys;
    m->dwAvailPhys = phys;
    m->dwTotalPageFile = phys;
    m->dwAvailPageFile = phys;
    m->dwTotalVirtual = 0x7ffe0000u;
    m->dwAvailVirtual = 0x70000000u;
}

// ---------------------------------------------------------------- helpers

int WINAPI j_MulDiv(int a, int b, int c) {
    if (c == 0) return -1;
    const std::int64_t p = static_cast<std::int64_t>(a) * b;
    // Windows rounds half away from zero.
    std::int64_t q = p / c;
    const std::int64_t r = p % c;
    const std::int64_t ar = r < 0 ? -r : r;
    const std::int64_t ac = c < 0 ? -static_cast<std::int64_t>(c) : c;
    if (2 * ar >= ac) q += ((p < 0) != (c < 0)) ? -1 : 1;
    if (q > INT32_MAX || q < INT32_MIN) return -1;
    return static_cast<int>(q);
}

BOOL WINAPI j_IsBadReadPtr(const void* p, UINT_PTR n) { return n != 0 && p == nullptr ? TRUE : FALSE; }
BOOL WINAPI j_IsBadWritePtr(LPVOID p, UINT_PTR n) { return n != 0 && p == nullptr ? TRUE : FALSE; }
BOOL WINAPI j_IsBadCodePtr(FARPROC p) { return p == nullptr ? TRUE : FALSE; }

void WINAPI j_OutputDebugStringA(LPCSTR s) { joan::hook::log_line("debug output: %s", s ? s : ""); }

void WINAPI j_DebugBreak() {
    joan::hook::log_line("DebugBreak");
#ifdef _MSC_VER
    __debugbreak();
#else
    __builtin_trap();
#endif
}

void WINAPI j_FatalAppExitA(UINT, LPCSTR msg) {
    joan::hook::log_line("FatalAppExit: %s", msg ? msg : "");
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Wars and Warriors: Joan of Arc", msg ? msg : "", nullptr);
    std::fflush(nullptr);
    std::_Exit(3);
}

// Copies `s` with its terminator into buf / size as Windows' path functions do.
DWORD copy_out(const std::string& s, DWORD size, LPSTR buf) {
    const auto n = static_cast<DWORD>(s.size());
    if (!buf || size <= n) return n + 1;
    std::memcpy(buf, s.c_str(), s.size() + 1);
    return n;
}

DWORD WINAPI j_GetTempPathA(DWORD size, LPSTR buf) {
    std::error_code ec;
    std::string s = fs::temp_directory_path(ec).string();
    if (ec) {
        set_error(ERROR_PATH_NOT_FOUND);
        return 0;
    }
    if (s.empty() || (s.back() != '\\' && s.back() != '/')) s += static_cast<char>(fs::path::preferred_separator);
    return copy_out(s, size, buf);
}

UINT WINAPI j_GetTempFileNameA(LPCSTR path, LPCSTR prefix, UINT unique, LPSTR out) {
    std::string pre = prefix ? std::string(prefix).substr(0, 3) : std::string();
    const fs::path dir(path ? path : ".");
    UINT n = unique ? unique : static_cast<UINT>(SDL_GetTicks() & 0xffff);
    for (int tries = 0; tries < 0x10000; ++tries, n = (n + 1) & 0xffff) {
        if (n == 0) continue;
        char name[32];
        std::snprintf(name, sizeof name, "%s%X.tmp", pre.c_str(), n);
        const fs::path p = dir / name;
        std::error_code ec;
        if (!unique && fs::exists(p, ec)) continue;
        const std::string s = p.string();
        std::strncpy(out, s.c_str(), MAX_PATH - 1);
        out[MAX_PATH - 1] = '\0';
        if (!unique)
            if (std::FILE* f = std::fopen(s.c_str(), "wb")) std::fclose(f);  // created, as Windows does
        return n;
    }
    set_error(ERROR_FILE_EXISTS);
    return 0;
}

// ---------------------------------------------------------------- the process image

#ifndef _WIN32
// A non-null instance handle: the game only passes it back (Window::create, LoadIconA).
const HMODULE kSelfModule = reinterpret_cast<HMODULE>(static_cast<std::uintptr_t>(0x00400000u));
#endif

HMODULE WINAPI j_GetModuleHandleA(LPCSTR name) {
#ifdef _WIN32
    return ::GetModuleHandleA(name);  // NULL: this executable (the only call: WinMainCRTStartup's instance)
#else
    if (name) {
        set_error(ERROR_MOD_NOT_FOUND);
        return nullptr;
    }
    return kSelfModule;
#endif
}

// Windows' semantics: the length without the terminator; a buffer too small gets size - 1 characters, a terminator
// and the result `size` (ERROR_INSUFFICIENT_BUFFER).
DWORD WINAPI j_GetModuleFileNameA(HMODULE module, LPSTR buf, DWORD size) {
#ifdef _WIN32
    return ::GetModuleFileNameA(module, buf, size);
#else
    if (module && module != kSelfModule) {
        set_error(ERROR_MOD_NOT_FOUND);
        return 0;
    }
    std::string path;
    if (const char* base = SDL_GetBasePath()) path = base;
    std::error_code ec;
    const fs::path exe = fs::read_symlink("/proc/self/exe", ec);
    path += ec ? std::string("openjoan") : exe.filename().string();
    if (size == 0) {
        set_error(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    const auto n = static_cast<DWORD>(std::min<std::size_t>(path.size(), size - 1));
    std::memcpy(buf, path.data(), n);
    buf[n] = '\0';
    if (n < path.size()) {
        set_error(ERROR_INSUFFICIENT_BUFFER);
        return size;
    }
    return n;
#endif
}

LPSTR WINAPI j_GetCommandLineA() {
#ifdef _WIN32
    return ::GetCommandLineA();
#else
    // The arguments (/proc/self/cmdline), quoted the way the C runtime's parse_cmdline splits them again.
    static std::string line = [] {
        std::string out;
        if (std::FILE* f = std::fopen("/proc/self/cmdline", "rb")) {
            std::string all;
            char chunk[512];
            for (std::size_t n; (n = std::fread(chunk, 1, sizeof chunk, f)) > 0;) all.append(chunk, n);
            std::fclose(f);
            std::size_t start = 0;
            while (start < all.size()) {
                const std::size_t end = all.find('\0', start);
                const std::string arg = all.substr(start, end == std::string::npos ? std::string::npos : end - start);
                if (!out.empty()) out += ' ';
                if (arg.empty() || arg.find_first_of(" \t\"") != std::string::npos) {
                    out += '"';
                    for (char c : arg) {
                        if (c == '"') out += '\\';
                        out += c;
                    }
                    out += '"';
                } else {
                    out += arg;
                }
                if (end == std::string::npos) break;
                start = end + 1;
            }
        }
        return out.empty() ? std::string("openjoan") : out;
    }();
    return line.data();
#endif
}

// The block the game's C runtime copies into _environ: "NAME=value\0" ... "\0" in UTF-16. Never NULL.
WCHAR g_empty_environment[2] = {0, 0};

LPWCH WINAPI j_GetEnvironmentStringsW() {
#ifdef _WIN32
    if (LPWCH env = ::GetEnvironmentStringsW()) return env;
    return g_empty_environment;
#else
    // A snapshot of the C library's environment (UTF-8 -> UTF-16; Windows' block lists the same "NAME=value" strings).
    std::vector<WCHAR> block;
    for (char** e = environ; e && *e; ++e) {
        const auto* p = reinterpret_cast<const unsigned char*>(*e);
        while (*p) {
            std::uint32_t c = *p++;
            int more = c >= 0xf0 ? 3 : c >= 0xe0 ? 2 : c >= 0xc0 ? 1 : 0;
            if (more) c &= 0x3fu >> more;
            for (; more > 0 && (*p & 0xc0) == 0x80; --more) c = (c << 6) | (*p++ & 0x3fu);
            if (more > 0 || c > 0x10ffff) c = 0xfffd;
            if (c >= 0x10000) {
                block.push_back(static_cast<WCHAR>(0xd800 + ((c - 0x10000) >> 10)));
                block.push_back(static_cast<WCHAR>(0xdc00 + ((c - 0x10000) & 0x3ff)));
            } else {
                block.push_back(static_cast<WCHAR>(c));
            }
        }
        block.push_back(0);
    }
    block.push_back(0);
    if (block.size() == 1) block.push_back(0);
    auto* env = new WCHAR[block.size()];
    std::memcpy(env, block.data(), block.size() * sizeof(WCHAR));
    return env;
#endif
}

BOOL WINAPI j_FreeEnvironmentStringsW(LPWCH env) {
    if (env == g_empty_environment) return TRUE;
#ifdef _WIN32
    return ::FreeEnvironmentStringsW(env);
#else
    delete[] env;
    return TRUE;
#endif
}

BOOL WINAPI j_SetEnvironmentVariableA(LPCSTR name, LPCSTR value) {
#ifdef _WIN32
    return ::SetEnvironmentVariableA(name, value);
#else
    if (!name || !*name) {
        set_error(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (name[0] == '=') return TRUE;  // _chdir's per-drive directory ("=C:"): no drives here
    const int r = value ? setenv(name, value, 1) : unsetenv(name);
    if (r != 0) set_error(ERROR_INVALID_PARAMETER);
    return r == 0 ? TRUE : FALSE;
#endif
}

DWORD WINAPI j_GetLastError() {
#ifdef _WIN32
    return ::GetLastError();  // our platform functions set Windows' value (set_error), and so do Windows' own
#else
    return t_last_error;
#endif
}

}  // namespace

namespace joan::platform {

unsigned long thread_id() { return current_tid(); }

void exit_thread(unsigned code) {
    if (t_ours) throw ThreadExit{static_cast<DWORD>(code)};
#ifdef _WIN32
    ::ExitThread(code);
#else
    pthread_exit(nullptr);
#endif
}

void set_last_error(unsigned long code) { set_error(static_cast<DWORD>(code)); }
unsigned long last_error() { return j_GetLastError(); }

bool stack_range(std::uintptr_t& low, std::uintptr_t& high) {
#ifdef _WIN32
    const auto* tib = reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
    low = reinterpret_cast<std::uintptr_t>(tib->StackLimit);
    high = reinterpret_cast<std::uintptr_t>(tib->StackBase);
    return true;
#else
    pthread_attr_t attr;
    if (pthread_getattr_np(pthread_self(), &attr) != 0) return false;
    void* base = nullptr;
    std::size_t size = 0;
    const bool ok = pthread_attr_getstack(&attr, &base, &size) == 0;
    pthread_attr_destroy(&attr);
    if (!ok) return false;
    low = reinterpret_cast<std::uintptr_t>(base);
    high = low + size;
    return true;
#endif
}

}  // namespace joan::platform

namespace joan::platform::files {

bool sync_close(HANDLE h) {
    if (!is_sync(h)) return false;
    const std::size_t i = (reinterpret_cast<std::uintptr_t>(h) - kSyncBase) / 4 - 1;
    std::lock_guard<std::mutex> l(g_sync_lock);
    if (i >= g_sync.size() || !g_sync[i]) return false;
    g_sync[i].reset();  // a running thread keeps its own reference
    g_sync_free.push_back(i);
    return true;
}

bool sync_wait(DWORD n, const HANDLE* hs, bool all, DWORD ms, bool alertable, DWORD& result) {
    if (n == 0) return false;
    for (DWORD i = 0; i < n; ++i)
        if (!sync_of(hs[i])) return false;
    result = wait_objects(n, hs, all, ms, alertable);
    return true;
}

}  // namespace joan::platform::files

// The events / threads must be understood by CloseHandle and the waits: platform/files.cpp's.
#define K(name) JOAN_IMPORT_OVERRIDE("KERNEL32.dll", #name, &j_##name, "portable " #name)
K(CreateThread);
K(ResumeThread);
K(CreateEventA);
K(SetEvent);
K(ResetEvent);
K(WaitForSingleObject);
K(GetCurrentThreadId);
K(GetCurrentThread);
K(GetCurrentProcess);
K(TerminateProcess);
K(TlsAlloc);
K(TlsFree);
K(TlsGetValue);
K(TlsSetValue);
K(GetProcessHeap);
K(HeapCreate);
K(HeapDestroy);
K(HeapAlloc);
K(HeapReAlloc);
K(HeapSize);
K(HeapFree);
K(HeapValidate);
K(VirtualAlloc);
K(VirtualFree);
K(GetStartupInfoA);
K(GetStdHandle);
K(SetStdHandle);
K(SetHandleCount);
K(SetConsoleCtrlHandler);
K(GetVersion);
K(GetVersionExA);
K(GlobalMemoryStatus);
K(MulDiv);
K(IsBadReadPtr);
K(IsBadWritePtr);
K(IsBadCodePtr);
K(OutputDebugStringA);
K(DebugBreak);
K(FatalAppExitA);
K(GetTempPathA);
K(GetTempFileNameA);
K(GetModuleHandleA);
K(GetModuleFileNameA);
K(GetCommandLineA);
K(GetEnvironmentStringsW);
K(FreeEnvironmentStringsW);
K(SetEnvironmentVariableA);
K(GetLastError);
#undef K
