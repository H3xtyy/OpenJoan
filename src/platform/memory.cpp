// Virtual memory and processor faults for our own code (memory.hpp): Windows (VirtualAlloc / VirtualProtect /
// VirtualQuery / FlushInstructionCache / vectored exception handlers) and POSIX (mmap / mprotect / /proc/self/maps /
// sigaction) - the same answers from both.

#include "memory.hpp"

#include <atomic>

#if defined(_WIN32)
#include <windows.h>
#else
#include <fcntl.h>
#include <link.h>
#include <signal.h>
#include <sys/mman.h>
#include <ucontext.h>
#include <unistd.h>

#include <cstring>
#endif

namespace joan::platform::memory {

Writable::Writable(void* p, std::size_t n, unsigned access) : p_(p), n_(n), ok_(false) {
    Region r;
    const unsigned keep = query(p, r) && r.committed ? r.access : kNone;  // e.g. the execute right of patched code
    ok_ = protect(p, n, keep | access, &previous_);
}
Writable::~Writable() {
    if (ok_) restore(p_, n_, previous_);
}

bool accessible(const void* p, unsigned need) {
    Region r;
    return query(p, r) && r.committed && !r.guard && (r.access & need) == need;
}

namespace {

constexpr int kMaxHandlers = 8;
FaultHandler g_handlers[kMaxHandlers] = {};
std::atomic<int> g_handler_count{0};

}  // namespace

#if defined(_WIN32)
// ================================================================ Windows

namespace {

DWORD to_native(unsigned a) {
    switch (a & kAll) {
        case kNone: return PAGE_NOACCESS;
        case kRead: return PAGE_READONLY;
        case kWrite:
        case kReadWrite: return PAGE_READWRITE;
        case kExec: return PAGE_EXECUTE;
        case kReadExec: return PAGE_EXECUTE_READ;
        default: return PAGE_EXECUTE_READWRITE;  // kWrite | kExec, kAll
    }
}
unsigned from_native(DWORD p) {
    switch (p & 0xff) {
        case PAGE_READONLY: return kRead;
        case PAGE_READWRITE:
        case PAGE_WRITECOPY: return kReadWrite;
        case PAGE_EXECUTE: return kExec;
        case PAGE_EXECUTE_READ: return kReadExec;
        case PAGE_EXECUTE_READWRITE:
        case PAGE_EXECUTE_WRITECOPY: return kAll;
        default: return kNone;
    }
}

Fault translate(EXCEPTION_POINTERS* info) {
    const EXCEPTION_RECORD* rec = info->ExceptionRecord;
    Fault f;
    f.code = rec->ExceptionCode;
    switch (rec->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION: f.type = Fault::kAccess; break;
        case EXCEPTION_BREAKPOINT: f.type = Fault::kBreakpoint; break;
        case EXCEPTION_ILLEGAL_INSTRUCTION: f.type = Fault::kIllegalInstruction; break;
        default: f.type = Fault::kOther; break;
    }
    if (f.type == Fault::kAccess && rec->NumberParameters >= 2) {
        f.address = static_cast<std::uintptr_t>(rec->ExceptionInformation[1]);
        f.access = rec->ExceptionInformation[0] == 1 ? kWrite : rec->ExceptionInformation[0] == 8 ? kExec : kRead;
    }
    f.pc = reinterpret_cast<std::uintptr_t>(rec->ExceptionAddress);
#if defined(_M_X64) || defined(__x86_64__)
    f.sp = static_cast<std::uintptr_t>(info->ContextRecord->Rsp);
#elif defined(_M_ARM64) || defined(__aarch64__)
    f.sp = static_cast<std::uintptr_t>(info->ContextRecord->Sp);
#else
    f.sp = static_cast<std::uintptr_t>(info->ContextRecord->Esp);
#endif
    f.native = info;
    return f;
}

// One vectored handler per registration (AddVectoredExceptionHandler(1, ...): each new one runs first, as before).
template <int I>
LONG CALLBACK trampoline(EXCEPTION_POINTERS* info) {
    const FaultHandler h = g_handlers[I];
    if (h == nullptr) return EXCEPTION_CONTINUE_SEARCH;
    return h(translate(info)) == FaultAction::kResume ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH;
}
using Trampoline = LONG(CALLBACK*)(EXCEPTION_POINTERS*);
constexpr Trampoline kTrampolines[kMaxHandlers] = {&trampoline<0>, &trampoline<1>, &trampoline<2>, &trampoline<3>,
                                                   &trampoline<4>, &trampoline<5>, &trampoline<6>, &trampoline<7>};

}  // namespace

std::size_t page_size() {
    static const std::size_t n = [] {
        SYSTEM_INFO si{};
        GetSystemInfo(&si);
        return static_cast<std::size_t>(si.dwPageSize);
    }();
    return n;
}

void* reserve(std::size_t n, void* at) { return VirtualAlloc(at, n, MEM_RESERVE, PAGE_NOACCESS); }
bool commit(void* p, std::size_t n, unsigned access) { return VirtualAlloc(p, n, MEM_COMMIT, to_native(access)) != nullptr; }
void* allocate(std::size_t n, unsigned access, void* at) {
    return VirtualAlloc(at, n, MEM_COMMIT | MEM_RESERVE, to_native(access));
}
bool decommit(void* p, std::size_t n) { return VirtualFree(p, n, MEM_DECOMMIT) != 0; }
bool release(void* p, std::size_t) { return VirtualFree(p, 0, MEM_RELEASE) != 0; }

bool protect(void* p, std::size_t n, unsigned access, Protection* previous) {
    DWORD old = 0;
    if (!VirtualProtect(p, n, to_native(access), &old)) return false;
    if (previous) previous->native = old;
    return true;
}
bool restore(void* p, std::size_t n, Protection previous) {
    DWORD old = 0;
    return VirtualProtect(p, n, previous.native, &old) != 0;
}

void flush_code(const void* p, std::size_t n) { FlushInstructionCache(GetCurrentProcess(), p, p ? n : 0); }

bool query(const void* p, Region& out) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(p, &mbi, sizeof mbi) == 0) return false;
    out.base = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
    out.size = static_cast<std::uintptr_t>(mbi.RegionSize);
    out.committed = mbi.State == MEM_COMMIT;
    out.access = out.committed ? from_native(mbi.Protect) : kNone;
    out.guard = (mbi.Protect & PAGE_GUARD) != 0;
    out.private_memory = mbi.Type == MEM_PRIVATE;
    return true;
}

void self_range(std::uintptr_t& low, std::uintptr_t& high) {
    const auto self = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(self);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(self + static_cast<std::uintptr_t>(dos->e_lfanew));
    low = self;
    high = self + nt->OptionalHeader.SizeOfImage;
}

bool add_fault_handler(FaultHandler h) {
    const int i = g_handler_count.fetch_add(1);
    if (i >= kMaxHandlers) return false;
    g_handlers[i] = h;
    return AddVectoredExceptionHandler(1, kTrampolines[i]) != nullptr;
}

void end_process(unsigned code) {
    TerminateProcess(GetCurrentProcess(), code);
    for (;;) Sleep(INFINITE);
}

#else
// ================================================================ POSIX

namespace {

int to_native(unsigned a) {
    return ((a & kRead) ? PROT_READ : 0) | ((a & kWrite) ? PROT_WRITE : 0) | ((a & kExec) ? PROT_EXEC : 0);
}

// /proc/self/maps, read with plain system calls (no allocation: also usable inside a fault handler).
// Each line: "start-end perms offset dev inode path".
bool maps_lookup(std::uintptr_t a, Region& out) {
#if defined(__linux__)
    const int fd = ::open("/proc/self/maps", O_RDONLY | O_CLOEXEC);
    if (fd < 0) return false;
    char buf[4096];
    char line[512];
    std::size_t len = 0;
    bool found = false, ok = true;
    std::uintptr_t prev_end = 0;
    auto hex = [](const char*& s) {
        std::uintptr_t v = 0;
        for (;; ++s) {
            const char c = *s;
            if (c >= '0' && c <= '9') v = v * 16 + static_cast<std::uintptr_t>(c - '0');
            else if (c >= 'a' && c <= 'f') v = v * 16 + static_cast<std::uintptr_t>(c - 'a' + 10);
            else break;
        }
        return v;
    };
    auto line_done = [&]() {  // the line in line[0..len)
        line[len] = '\0';
        const char* s = line;
        const std::uintptr_t start = hex(s);
        if (*s++ != '-') return;
        const std::uintptr_t end = hex(s);
        if (*s++ != ' ') return;
        if (a < start) {  // in the gap before this mapping: not mapped
            out.base = prev_end;
            out.size = start - prev_end;
            out.committed = false;
            out.access = kNone;
            out.private_memory = false;
            found = true;
            return;
        }
        prev_end = end;
        if (a >= end) return;
        out.base = start;
        out.size = end - start;
        out.committed = true;
        out.access = (s[0] == 'r' ? kRead : 0u) | (s[1] == 'w' ? kWrite : 0u) | (s[2] == 'x' ? kExec : 0u);
        // "perms offset dev inode": inode 0 = anonymous memory (heaps, stacks, allocations).
        for (int field = 0; field < 3 && *s; ++field) {
            while (*s && *s != ' ') ++s;
            while (*s == ' ') ++s;
        }
        out.private_memory = *s == '0' && (s[1] == ' ' || s[1] == '\0');
        found = true;
    };
    while (!found) {
        const ssize_t got = ::read(fd, buf, sizeof buf);
        if (got < 0) {
            ok = false;
            break;
        }
        if (got == 0) break;
        for (ssize_t i = 0; i < got && !found; ++i) {
            if (buf[i] == '\n') {
                line_done();
                len = 0;
            } else if (len + 1 < sizeof line) {
                line[len++] = buf[i];
            }
        }
    }
    ::close(fd);
    if (!ok) return false;
    if (!found) {  // past the last mapping
        out.base = prev_end;
        out.size = 0;
        out.committed = false;
        out.access = kNone;
    }
    out.guard = false;
    return true;
#else
    (void)a;
    (void)out;
    return false;
#endif
}

struct sigaction g_previous[4];
constexpr int kSignals[4] = {SIGSEGV, SIGBUS, SIGILL, SIGTRAP};

void on_signal(int sig, siginfo_t* si, void* context) {
    Fault f;
    f.code = static_cast<std::uint32_t>(sig);
    f.type = sig == SIGSEGV || sig == SIGBUS ? Fault::kAccess
             : sig == SIGILL                 ? Fault::kIllegalInstruction
             : sig == SIGTRAP                ? Fault::kBreakpoint
                                             : Fault::kOther;
    f.address = reinterpret_cast<std::uintptr_t>(si->si_addr);
    f.native = context;
#if defined(__linux__)
    auto* uc = static_cast<ucontext_t*>(context);
#if defined(__x86_64__)
    f.pc = static_cast<std::uintptr_t>(uc->uc_mcontext.gregs[REG_RIP]);
    f.sp = static_cast<std::uintptr_t>(uc->uc_mcontext.gregs[REG_RSP]);
    const auto err = static_cast<std::uintptr_t>(uc->uc_mcontext.gregs[REG_ERR]);  // page fault error code
    f.access = (err & 0x10) ? kExec : (err & 0x2) ? kWrite : kRead;
#elif defined(__i386__)
    f.pc = static_cast<std::uintptr_t>(uc->uc_mcontext.gregs[REG_EIP]);
    f.sp = static_cast<std::uintptr_t>(uc->uc_mcontext.gregs[REG_ESP]);
    const auto err = static_cast<std::uintptr_t>(uc->uc_mcontext.gregs[REG_ERR]);
    f.access = (err & 0x10) ? kExec : (err & 0x2) ? kWrite : kRead;
#elif defined(__aarch64__)
    f.pc = static_cast<std::uintptr_t>(uc->uc_mcontext.pc);
    f.sp = static_cast<std::uintptr_t>(uc->uc_mcontext.sp);
    f.access = f.address == f.pc ? kExec : kRead;
#endif
#endif
    for (int i = g_handler_count.load() - 1; i >= 0; --i) {  // the newest first
        if (i < kMaxHandlers && g_handlers[i] && g_handlers[i](f) == FaultAction::kResume) return;
    }
    // Nobody's: whatever was installed before us, else the default action (the instruction faults again, untrapped).
    for (int k = 0; k < 4; ++k) {
        if (kSignals[k] != sig) continue;
        const struct sigaction& prev = g_previous[k];
        if ((prev.sa_flags & SA_SIGINFO) && prev.sa_sigaction) {
            prev.sa_sigaction(sig, si, context);
            return;
        }
        if (prev.sa_handler != SIG_DFL && prev.sa_handler != SIG_IGN && prev.sa_handler != nullptr) {
            prev.sa_handler(sig);
            return;
        }
    }
    struct sigaction dfl {};
    dfl.sa_handler = SIG_DFL;
    sigemptyset(&dfl.sa_mask);
    sigaction(sig, &dfl, nullptr);
}

int self_range_cb(dl_phdr_info* info, std::size_t, void* data) {
    auto* range = static_cast<std::uintptr_t*>(data);  // [0] = an address inside us, [1] / [2] = the result
    std::uintptr_t low = ~std::uintptr_t{0}, high = 0;
    for (int i = 0; i < info->dlpi_phnum; ++i) {
        const auto& ph = info->dlpi_phdr[i];
        if (ph.p_type != PT_LOAD) continue;
        const std::uintptr_t b = info->dlpi_addr + ph.p_vaddr, e = b + ph.p_memsz;
        if (b < low) low = b;
        if (e > high) high = e;
    }
    if (range[0] < low || range[0] >= high) return 0;
    range[1] = low;
    range[2] = high;
    return 1;
}

}  // namespace

std::size_t page_size() {
    static const std::size_t n = static_cast<std::size_t>(::sysconf(_SC_PAGESIZE));
    return n;
}

void* reserve(std::size_t n, void* at) {
    int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;
#if defined(MAP_FIXED_NOREPLACE)
    if (at) flags |= MAP_FIXED_NOREPLACE;
#endif
    void* p = ::mmap(at, n, PROT_NONE, flags, -1, 0);
    if (p == MAP_FAILED) return nullptr;
    if (at && p != at) {  // the place is taken
        ::munmap(p, n);
        return nullptr;
    }
    return p;
}
// [p, p + n) rounded out to whole pages: mmap / mprotect / madvise take them from a page-aligned start (VirtualAlloc /
// VirtualProtect / VirtualFree round a range out to its pages themselves).
static void page_range(void* p, std::size_t n, void*& start, std::size_t& bytes) {
    const std::uintptr_t page = page_size();
    const std::uintptr_t s = reinterpret_cast<std::uintptr_t>(p) & ~(page - 1);
    const std::uintptr_t e = (reinterpret_cast<std::uintptr_t>(p) + (n ? n : 1) + page - 1) & ~(page - 1);
    start = reinterpret_cast<void*>(s);
    bytes = e - s;
}
static int protect_pages(void* p, std::size_t n, unsigned access) {
    void* start = nullptr;
    std::size_t bytes = 0;
    page_range(p, n, start, bytes);
    return ::mprotect(start, bytes, to_native(access));
}

bool commit(void* p, std::size_t n, unsigned access) { return protect_pages(p, n, access) == 0; }
void* allocate(std::size_t n, unsigned access, void* at) {
    void* p = reserve(n, at);
    if (p && !commit(p, n, access)) {
        ::munmap(p, n);
        return nullptr;
    }
    return p;
}
bool decommit(void* p, std::size_t n) {
    void* start = nullptr;
    std::size_t bytes = 0;
    page_range(p, n, start, bytes);
    ::madvise(start, bytes, MADV_DONTNEED);
    return ::mprotect(start, bytes, PROT_NONE) == 0;
}
bool release(void* p, std::size_t n) { return ::munmap(p, n) == 0; }

bool protect(void* p, std::size_t n, unsigned access, Protection* previous) {
    if (previous) {
        Region r;
        previous->native = query(p, r) && r.committed ? r.access : kNone;
    }
    return protect_pages(p, n, access) == 0;
}
bool restore(void* p, std::size_t n, Protection previous) { return protect_pages(p, n, previous.native) == 0; }

void flush_code(const void* p, std::size_t n) {
    if (p == nullptr) return;  // x86 keeps its caches coherent; elsewhere every writer passes its range
    __builtin___clear_cache(static_cast<char*>(const_cast<void*>(p)), static_cast<char*>(const_cast<void*>(p)) + n);
}

bool query(const void* p, Region& out) { return maps_lookup(reinterpret_cast<std::uintptr_t>(p), out); }

void self_range(std::uintptr_t& low, std::uintptr_t& high) {
    std::uintptr_t range[3] = {reinterpret_cast<std::uintptr_t>(&self_range_cb), 0, 0};
    dl_iterate_phdr(&self_range_cb, range);
    low = range[1];
    high = range[2];
}

bool add_fault_handler(FaultHandler h) {
    static const bool installed = [] {
        for (int k = 0; k < 4; ++k) {
            struct sigaction sa {};
            sa.sa_sigaction = &on_signal;
            sa.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_NODEFER;
            sigemptyset(&sa.sa_mask);
            if (sigaction(kSignals[k], &sa, &g_previous[k]) != 0) return false;
        }
        return true;
    }();
    if (!installed) return false;
    const int i = g_handler_count.load();
    if (i >= kMaxHandlers) return false;
    g_handlers[i] = h;
    g_handler_count.store(i + 1);
    return true;
}

void end_process(unsigned code) { ::_exit(static_cast<int>(code)); }

#endif

}  // namespace joan::platform::memory
