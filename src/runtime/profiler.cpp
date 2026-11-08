// [Debug] Profile - see profiler.hpp.
#include "profiler.hpp"

#ifdef _WIN32
#include <windows.h>

#include <dbghelp.h>  // types only: dbghelp.dll is loaded at run time (no import library)
#else
#include <dlfcn.h>  // dladdr: symbol_at elsewhere
#endif

#include <algorithm>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../platform/window.hpp"
#include "../registry.hpp"
#include "../platform/memory.hpp"

namespace joan::prof {
namespace {

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))

constexpr int kMaxFrames = 40;

// The sampled thread's stack top, copied while it was suspended (only the sampler thread uses these).
std::uint8_t g_stack_copy[256 * 1024];
std::uintptr_t g_copy_base = 0;
std::size_t g_copy_size = 0;

// StackWalk64's memory reads: the stack from the copy, anything else (code, for frame-pointer-omitted
// functions' unwind data) from the process, which does not change under us.
BOOL CALLBACK read_memory(HANDLE, DWORD64 address, PVOID buffer, DWORD size, LPDWORD read) {
    const auto a = static_cast<std::uintptr_t>(address);
    if (a >= g_copy_base && a - g_copy_base + size <= g_copy_size) {
        std::memcpy(buffer, g_stack_copy + (a - g_copy_base), size);
        if (read) *read = size;
        return TRUE;
    }
    if (a >= g_copy_base && a < g_copy_base + 0x1000000) return FALSE;  // the live stack beyond the copy: not read
    SIZE_T got = 0;
    const BOOL ok = ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(a), buffer, size, &got);
    if (read) *read = static_cast<DWORD>(got);
    return ok;
}

struct Api {
    decltype(&::SymSetOptions) set_options = nullptr;
    decltype(&::SymInitialize) initialize = nullptr;
    decltype(&::SymFromAddr) from_addr = nullptr;
    decltype(&::StackWalk64) stack_walk = nullptr;
    decltype(&::SymFunctionTableAccess64) table_access = nullptr;
    decltype(&::SymGetModuleBase64) module_base = nullptr;
    HANDLE process = nullptr;  // our own handle: a dbghelp session apart from the crash handler's
};

bool load(Api& a) {
    HMODULE m = LoadLibraryW(L"dbghelp.dll");
    if (!m) return false;
    auto get = [m](const char* name) { return reinterpret_cast<void*>(GetProcAddress(m, name)); };
    a.set_options = reinterpret_cast<decltype(a.set_options)>(get("SymSetOptions"));
    a.initialize = reinterpret_cast<decltype(a.initialize)>(get("SymInitialize"));
    a.from_addr = reinterpret_cast<decltype(a.from_addr)>(get("SymFromAddr"));
    a.stack_walk = reinterpret_cast<decltype(a.stack_walk)>(get("StackWalk64"));
    a.table_access = reinterpret_cast<decltype(a.table_access)>(get("SymFunctionTableAccess64"));
    a.module_base = reinterpret_cast<decltype(a.module_base)>(get("SymGetModuleBase64"));
    if (!a.set_options || !a.initialize || !a.from_addr || !a.stack_walk || !a.table_access || !a.module_base) return false;
    if (!DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(), &a.process, 0, FALSE,
                         DUPLICATE_SAME_ACCESS))
        return false;
    char dir[MAX_PATH] = {};
    const DWORD n = GetModuleFileNameA(nullptr, dir, MAX_PATH);
    if (n > 0 && n < MAX_PATH)
        for (char* p = dir + n; p > dir; --p)
            if (p[-1] == '\\' || p[-1] == '/') {
                p[-1] = 0;
                break;
            }
    a.set_options(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_NO_PROMPTS);
    return a.initialize(a.process, dir[0] ? dir : nullptr, TRUE) != FALSE;
}

struct Sampler {
    Api api;
    HANDLE thread = nullptr;
    int period_s = 0;
    std::vector<std::uint64_t> stacks;  // per sample: frame count, then the frames (innermost first)
    unsigned samples = 0, failed = 0;
    // Symbolisation cache: pc -> function id; id -> name.
    std::unordered_map<std::uint64_t, int> pc_function;
    std::unordered_map<std::string, int> name_id;
    std::vector<std::string> names;

    int function_of(std::uint64_t pc) {
        if (auto it = pc_function.find(pc); it != pc_function.end()) return it->second;
        std::string name;
        alignas(SYMBOL_INFO) char buffer[sizeof(SYMBOL_INFO) + 512] = {};
        auto* sym = reinterpret_cast<SYMBOL_INFO*>(buffer);
        sym->SizeOfStruct = sizeof(SYMBOL_INFO);
        sym->MaxNameLen = 511;
        DWORD64 displacement = 0;
        if (api.from_addr(api.process, pc, &displacement, sym)) {
            name.assign(sym->Name, sym->NameLen);
        } else {
            HMODULE module = nullptr;
            char path[MAX_PATH] = "?";
            if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                   reinterpret_cast<LPCSTR>(static_cast<std::uintptr_t>(pc)), &module))
                GetModuleFileNameA(module, path, MAX_PATH);
            const char* base = std::max(std::strrchr(path, '\\'), std::strrchr(path, '/'));
            name = std::string("[") + (base ? base + 1 : path) + "]";
        }
        auto [it, added] = name_id.try_emplace(name, static_cast<int>(names.size()));
        if (added) names.push_back(name);
        pc_function.emplace(pc, it->second);
        return it->second;
    }

    // While the thread is suspended only its registers and the top of its stack are copied (no
    // allocation, no dbghelp: the thread may hold the heap lock); the walk reads the copy afterwards.
    void sample() {
        if (SuspendThread(thread) == static_cast<DWORD>(-1)) return;
        CONTEXT ctx = {};
        ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
        bool have = GetThreadContext(thread, &ctx) != FALSE;
        if (have) {
#if defined(_M_IX86)
            const std::uintptr_t sp = ctx.Esp;
#else
            const std::uintptr_t sp = ctx.Rsp;
#endif
            joan::platform::memory::Region region;
            have = joan::platform::memory::query(reinterpret_cast<const void*>(sp), region) && region.committed;
            if (have) {
                const std::uintptr_t end = region.base + region.size;
                g_copy_size = static_cast<std::size_t>(std::min<std::uintptr_t>(end - sp, sizeof g_stack_copy));
                g_copy_base = sp;
                std::memcpy(g_stack_copy, reinterpret_cast<const void*>(sp), g_copy_size);
            }
        }
        ResumeThread(thread);
        std::uint64_t frames[kMaxFrames];
        int n = 0;
        if (have) {
            STACKFRAME64 f = {};
#if defined(_M_IX86)
            const DWORD machine = IMAGE_FILE_MACHINE_I386;
            f.AddrPC.Offset = ctx.Eip, f.AddrStack.Offset = ctx.Esp, f.AddrFrame.Offset = ctx.Ebp;
#else
            const DWORD machine = IMAGE_FILE_MACHINE_AMD64;
            f.AddrPC.Offset = ctx.Rip, f.AddrStack.Offset = ctx.Rsp, f.AddrFrame.Offset = ctx.Rbp;
#endif
            f.AddrPC.Mode = f.AddrStack.Mode = f.AddrFrame.Mode = AddrModeFlat;
            while (n < kMaxFrames &&
                   api.stack_walk(machine, api.process, thread, &f, &ctx, &read_memory, api.table_access, api.module_base, nullptr)) {
                if (f.AddrPC.Offset == 0) break;
                frames[n++] = f.AddrPC.Offset;
            }
        }
        if (n == 0) {
            ++failed;
            return;
        }
        ++samples;
        stacks.push_back(static_cast<std::uint64_t>(n));
        stacks.insert(stacks.end(), frames, frames + n);
    }

    void report(double seconds) {
        std::vector<unsigned> self(names.size()), total(names.size());
        std::unordered_set<int> seen;
        for (std::size_t i = 0; i < stacks.size();) {
            const auto n = static_cast<std::size_t>(stacks[i++]);
            seen.clear();
            for (std::size_t k = 0; k < n; ++k) {
                const int id = function_of(stacks[i + k]);
                if (static_cast<std::size_t>(id) >= self.size()) self.resize(names.size()), total.resize(names.size());
                if (k == 0) ++self[static_cast<std::size_t>(id)];
                if (seen.insert(id).second) ++total[static_cast<std::size_t>(id)];
            }
            i += n;
        }
        auto top = [&](const std::vector<unsigned>& counts, const char* what, std::size_t limit) {
            std::vector<int> order;
            for (std::size_t i = 0; i < counts.size(); ++i)
                if (counts[i]) order.push_back(static_cast<int>(i));
            std::sort(order.begin(), order.end(), [&](int a, int b) { return counts[static_cast<std::size_t>(a)] > counts[static_cast<std::size_t>(b)]; });
            if (order.size() > limit) order.resize(limit);
            hook::log_line("[Debug] Profile: top %zu by %s time (%% of %u samples in %.1f s):", order.size(), what, samples, seconds);
            for (int id : order)
                hook::log_line("[Debug] Profile:   %5.1f%%  %s", 100.0 * counts[static_cast<std::size_t>(id)] / std::max(1u, samples),
                               names[static_cast<std::size_t>(id)].c_str());
        };
        top(self, "self", 40);
        top(total, "total (on the stack)", 70);
        if (failed) hook::log_line("[Debug] Profile: %u samples without a stack", failed);
        stacks.clear();
        samples = failed = 0;
    }

    [[noreturn]] void run() {
        // ~1 ms between samples (a high-resolution waitable timer; plain Sleep(1) where there is none).
        HANDLE timer = CreateWaitableTimerExW(nullptr, nullptr, 0x00000002 /* CREATE_WAITABLE_TIMER_HIGH_RESOLUTION */,
                                              TIMER_ALL_ACCESS);
        const long long freq = joan::platform::perf_frequency();
        long long start = joan::platform::perf_counter();
        for (;;) {
            LARGE_INTEGER due;
            due.QuadPart = -10000;  // 1 ms, relative
            if (timer && SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE)) WaitForSingleObject(timer, INFINITE);
            else Sleep(1);
            sample();
            const long long now = joan::platform::perf_counter();
            const double seconds = static_cast<double>(now - start) / static_cast<double>(freq);
            if (seconds >= period_s) {
                report(seconds);
                start = joan::platform::perf_counter();
            }
        }
    }
};

DWORD WINAPI thread_main(void* p) {
    static_cast<Sampler*>(p)->run();
}

std::atomic<bool> g_started{false};

#endif

}  // namespace

void start_for_current_thread() {
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    const int seconds = hook::settings().profile;
    if (seconds <= 0 || g_started.exchange(true)) return;
    auto* s = new Sampler;  // lives as long as the process
    s->period_s = seconds;
    if (!load(s->api) ||
        !DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &s->thread,
                         THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, 0)) {
        hook::log_line("[Debug] Profile: dbghelp.dll or the thread handle is not available - no profile");
        return;
    }
    HANDLE t = CreateThread(nullptr, 0, &thread_main, s, 0, nullptr);
    if (!t) return;
    SetThreadPriority(t, THREAD_PRIORITY_TIME_CRITICAL);
    CloseHandle(t);
    hook::log_line("[Debug] Profile: sampling the main thread, a report every %d s", seconds);
#else
    if (hook::settings().profile > 0)
        hook::log_line("[Debug] Profile: the sampling profiler needs Windows (dbghelp) and MSVC - no profile");
#endif
}

std::string symbol_at(std::uintptr_t pc) {
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    static Api api;  // a session of its own (the sampler thread uses its own)
    static const bool loaded = load(api);
    char text[600];
    if (loaded) {
        alignas(SYMBOL_INFO) char buffer[sizeof(SYMBOL_INFO) + 512] = {};
        auto* sym = reinterpret_cast<SYMBOL_INFO*>(buffer);
        sym->SizeOfStruct = sizeof(SYMBOL_INFO);
        sym->MaxNameLen = 511;
        DWORD64 displacement = 0;
        if (api.from_addr(api.process, pc, &displacement, sym)) {
            std::snprintf(text, sizeof text, "%.*s+0x%llx", static_cast<int>(sym->NameLen), sym->Name,
                          static_cast<unsigned long long>(displacement));
            return text;
        }
    }
    HMODULE module = nullptr;
    char path[MAX_PATH] = "?";
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCSTR>(pc), &module))
        return "?";
    GetModuleFileNameA(module, path, MAX_PATH);
    const char* base = std::max(std::strrchr(path, '\\'), std::strrchr(path, '/'));
    std::snprintf(text, sizeof text, "[%s]+0x%llx", base ? base + 1 : path,
                  static_cast<unsigned long long>(pc - reinterpret_cast<std::uintptr_t>(module)));
    return text;
#elif !defined(_WIN32)
    // The dynamic symbol table (exported names: a build linked with -rdynamic names our functions too).
    Dl_info info{};
    if (!dladdr(reinterpret_cast<const void*>(pc), &info) || !info.dli_fname) return "?";
    char text[600];
    if (info.dli_sname && info.dli_saddr) {
        std::snprintf(text, sizeof text, "%s+0x%llx", info.dli_sname,
                      static_cast<unsigned long long>(pc - reinterpret_cast<std::uintptr_t>(info.dli_saddr)));
        return text;
    }
    const char* base = std::strrchr(info.dli_fname, '/');
    std::snprintf(text, sizeof text, "[%s]+0x%llx", base ? base + 1 : info.dli_fname,
                  static_cast<unsigned long long>(pc - reinterpret_cast<std::uintptr_t>(info.dli_fbase)));
    return text;
#else
    (void)pc;
    return "?";
#endif
}

}  // namespace joan::prof
