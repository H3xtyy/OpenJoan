#include "../callconv.hpp"
#include "crash.hpp"

#ifdef _WIN32
#include <windows.h>
#if defined(_M_X64) || defined(__x86_64__)
#include <dbghelp.h>  // types only: dbghelp.dll is loaded at crash time (no import library)
#include <intrin.h>   // _ReturnAddress
#include <stdlib.h>   // _set_invalid_parameter_handler, _set_purecall_handler

#include <csignal>
#include <atomic>
#include <cstdlib>
#include <exception>
#endif
#else
// Elsewhere: the fault handlers of platform/memory.hpp (signals), dladdr for module + offset, backtrace for the stack.
#include <dlfcn.h>
#include <execinfo.h>

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>

#include "../compiler.hpp"
#endif

#include <cstdint>
#include <cstdio>
#include <string>

#include "../compiler.hpp"  // JOAN_RETURN_ADDRESS
#include "code_table.hpp"
#include "../registry.hpp"
#include "../platform/memory.hpp"

namespace joan::hook {

#ifdef _WIN32
namespace {

LPTOP_LEVEL_EXCEPTION_FILTER g_previous = nullptr;

}  // namespace

std::string describe_address(std::uintptr_t address) {
    HMODULE module = nullptr;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCSTR>(address), &module) ||
        !module)
        return {};
    char path[MAX_PATH] = {};
    GetModuleFileNameA(module, path, MAX_PATH);
    const char* name = path;
    for (const char* p = path; *p; ++p)
        if (*p == '\\' || *p == '/') name = p + 1;
    char buf[MAX_PATH + 32];
    std::snprintf(buf, sizeof buf, "%s+0x%x", name,
                  static_cast<unsigned>(address - reinterpret_cast<std::uintptr_t>(module)));
    return buf;
}
#else
std::string describe_address(std::uintptr_t address) {
    Dl_info info{};
    if (!dladdr(reinterpret_cast<const void*>(address), &info) || !info.dli_fname || !info.dli_fbase) return {};
    const char* name = std::strrchr(info.dli_fname, '/');
    name = name ? name + 1 : info.dli_fname;
    char buf[512];
    std::snprintf(buf, sizeof buf, "%s+0x%llx", name,
                  static_cast<unsigned long long>(address - reinterpret_cast<std::uintptr_t>(info.dli_fbase)));
    return buf;
}
#endif

namespace {

// Module + offset, and what the code table knows about the address (the nearest replacement of ours, a trap stub, an
// original code address): joan.exe's code no longer exists, every frame of the game is our C++.
std::string describe(std::uintptr_t address) {
    std::string s = describe_address(address);
    char what[200];
    code_table::describe(address, what, sizeof what);
    if (what[0]) {
        s += s.empty() ? "(" : " (";
        s += what;
        s += ')';
    }
    return s;
}

bool readable(const void* p, std::size_t n) {
    platform::memory::Region r;
    if (!platform::memory::query(p, r) || !r.committed) return false;
    if (r.access == platform::memory::kNone || r.guard) return false;
    return reinterpret_cast<std::uintptr_t>(p) + n <= r.base + r.size;
}

bool executable(std::uintptr_t address) {
    platform::memory::Region r;
    return platform::memory::query(reinterpret_cast<const void*>(address), r) && r.committed &&
           (r.access & platform::memory::kExec) != 0;
}

#ifdef _WIN32
#if defined(_M_X64) || defined(__x86_64__)
// dbghelp, loaded on the first crash (LoadLibraryW / GetProcAddress: no import library). Search path: the exe folder,
// where openjoan.pdb lies. MSVC only: a MinGW build has DWARF, which Windows' dbghelp does not read (and Wine's
// crashed on it), so its frames are described as before (module+offset, code table).
#if defined(_MSC_VER)
#define JOAN_CRASH_DBGHELP 1
#else
#define JOAN_CRASH_DBGHELP 0
#endif
bool g_in_dbghelp = false;  // a fault inside dbghelp re-enters the handler: that report goes without it
struct Dbghelp {
    decltype(&::SymFromAddr) from_addr = nullptr;
    decltype(&::SymGetLineFromAddr64) line_from_addr = nullptr;
    HANDLE process = nullptr;
    bool ok = false;
};
const Dbghelp& dbghelp() {
    static const Dbghelp d = [] {
        Dbghelp r;
        HMODULE m = LoadLibraryW(L"dbghelp.dll");
        if (!m) return r;
        const auto set_options = reinterpret_cast<decltype(&::SymSetOptions)>(
            reinterpret_cast<void*>(GetProcAddress(m, "SymSetOptions")));
        const auto initialize = reinterpret_cast<decltype(&::SymInitialize)>(
            reinterpret_cast<void*>(GetProcAddress(m, "SymInitialize")));
        r.from_addr = reinterpret_cast<decltype(&::SymFromAddr)>(reinterpret_cast<void*>(GetProcAddress(m, "SymFromAddr")));
        r.line_from_addr = reinterpret_cast<decltype(&::SymGetLineFromAddr64)>(
            reinterpret_cast<void*>(GetProcAddress(m, "SymGetLineFromAddr64")));
        if (!set_options || !initialize || !r.from_addr) return r;
        char dir[MAX_PATH] = {};
        const DWORD n = GetModuleFileNameA(nullptr, dir, MAX_PATH);
        if (n == 0 || n >= MAX_PATH) dir[0] = 0;
        for (char* p = dir + (n < MAX_PATH ? n : 0); p > dir; --p)
            if (p[-1] == '\\' || p[-1] == '/') {
                p[-1] = 0;
                break;
            }
        set_options(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES | SYMOPT_FAIL_CRITICAL_ERRORS |
                    SYMOPT_NO_PROMPTS);
        r.process = GetCurrentProcess();
        r.ok = initialize(r.process, dir[0] ? dir : nullptr, TRUE) != FALSE;
        return r;
    }();
    return d;
}

// "module+0x.. name+0x.. (file:line)" from dbghelp, or describe() when it has no symbol for the address. pc is the
// faulting instruction (frame 0) or a return address (the call's line is looked up at pc - 1).
std::string describe_frame(std::uintptr_t pc, bool return_address) {
    if (!JOAN_CRASH_DBGHELP || g_in_dbghelp) return describe(pc);
    g_in_dbghelp = true;
    const Dbghelp& d = dbghelp();
    g_in_dbghelp = false;
    if (!d.ok) return describe(pc);
    const DWORD64 at = return_address ? pc - 1 : pc;
    alignas(SYMBOL_INFO) char buf[sizeof(SYMBOL_INFO) + 256] = {};
    auto* sym = reinterpret_cast<SYMBOL_INFO*>(buf);
    sym->SizeOfStruct = sizeof(SYMBOL_INFO);
    sym->MaxNameLen = 256 - 1;
    DWORD64 disp = 0;
    g_in_dbghelp = true;
    const BOOL found = d.from_addr(d.process, at, &disp, sym);
    g_in_dbghelp = false;
    if (!found) return describe(pc);
    std::string s = describe_address(pc);
    char tmp[MAX_PATH + 320];
    std::snprintf(tmp, sizeof tmp, "%s%s+0x%llx", s.empty() ? "" : " ", sym->Name,
                  static_cast<unsigned long long>(disp + (return_address ? 1 : 0)));
    s += tmp;
    IMAGEHLP_LINE64 line = {};
    line.SizeOfStruct = sizeof line;
    DWORD col = 0;
    g_in_dbghelp = true;
    const BOOL has_line = d.line_from_addr && d.line_from_addr(d.process, at, &col, &line);
    g_in_dbghelp = false;
    if (has_line && line.FileName) {
        std::snprintf(tmp, sizeof tmp, " (%s:%lu)", line.FileName, static_cast<unsigned long>(line.LineNumber));
        s += tmp;
    }
    return s;
}

// The call stack from the exception context: RtlLookupFunctionEntry / RtlVirtualUnwind per frame (a function
// without unwind data is a leaf: its return address is at [rsp]).
void unwind_stack(const CONTEXT& start) {
    CONTEXT ctx = start;
    for (int depth = 0; depth < 48; ++depth) {
        const DWORD64 pc = ctx.Rip;
        if (pc == 0 || !executable(static_cast<std::uintptr_t>(pc))) break;
        log_line("     frame %2d: 0x%016llx %s", depth, static_cast<unsigned long long>(pc),
                 describe_frame(static_cast<std::uintptr_t>(pc), depth != 0).c_str());
        const DWORD64 sp = ctx.Rsp;
        DWORD64 image_base = 0;
        PRUNTIME_FUNCTION fe = RtlLookupFunctionEntry(depth == 0 ? pc : pc - 1, &image_base, nullptr);
        if (fe) {
            PVOID handler_data = nullptr;
            DWORD64 establisher = 0;
            RtlVirtualUnwind(UNW_FLAG_NHANDLER, image_base, pc, fe, &ctx, &handler_data, &establisher, nullptr);
        } else {
            if (!readable(reinterpret_cast<const void*>(sp), sizeof(DWORD64))) break;
            ctx.Rip = *reinterpret_cast<const DWORD64*>(sp);
            ctx.Rsp = sp + sizeof(DWORD64);
        }
        if (ctx.Rsp <= sp) break;  // no progress (every unwound frame pops at least its return address)
    }
}
#endif

LONG WINAPI on_unhandled(EXCEPTION_POINTERS* info) {
    const EXCEPTION_RECORD* rec = info->ExceptionRecord;
    const CONTEXT* c = info->ContextRecord;
    const auto at = reinterpret_cast<std::uintptr_t>(rec->ExceptionAddress);
    log_line("**** CRASH: exception 0x%08lx at 0x%08llx %s", rec->ExceptionCode, static_cast<unsigned long long>(at),
             describe(at).c_str());
    if (rec->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && rec->NumberParameters >= 2)
        log_line("     %s address 0x%08llx", rec->ExceptionInformation[0] == 0 ? "reading" : rec->ExceptionInformation[0] == 1 ? "writing" : "executing",
                 static_cast<unsigned long long>(rec->ExceptionInformation[1]));
#if defined(_M_IX86)
    log_line("     eax %08lx ebx %08lx ecx %08lx edx %08lx esi %08lx edi %08lx", c->Eax, c->Ebx, c->Ecx, c->Edx, c->Esi, c->Edi);
    log_line("     eip %08lx esp %08lx ebp %08lx", c->Eip, c->Esp, c->Ebp);
    // EBP chain (the ported game code keeps frame pointers).
    std::uintptr_t ebp = c->Ebp;
    for (int depth = 0; depth < 32 && ebp && readable(reinterpret_cast<const void*>(ebp), 8); ++depth) {
        const auto* frame = reinterpret_cast<const std::uintptr_t*>(ebp);
        const std::uintptr_t ret = frame[1];
        log_line("     frame %2d: return 0x%08x %s", depth, static_cast<unsigned>(ret), describe(ret).c_str());
        if (frame[0] <= ebp) break;
        ebp = frame[0];
    }
    // Code addresses on the stack (catches frames the chain misses, e.g. in system DLLs).
    const auto* sp = reinterpret_cast<const std::uintptr_t*>(c->Esp);
    int shown = 0;
    for (int i = 0; i < 1024 && shown < 24 && readable(sp + i, 4); ++i) {
        if (executable(sp[i])) {
            log_line("     stack[esp+0x%03x] 0x%08x %s", i * 4, static_cast<unsigned>(sp[i]), describe(sp[i]).c_str());
            ++shown;
        }
    }
#elif defined(_M_X64) || defined(__x86_64__)
    log_line("     rax %016llx rbx %016llx rcx %016llx rdx %016llx", static_cast<unsigned long long>(c->Rax),
             static_cast<unsigned long long>(c->Rbx), static_cast<unsigned long long>(c->Rcx),
             static_cast<unsigned long long>(c->Rdx));
    log_line("     rsi %016llx rdi %016llx r8  %016llx r9  %016llx", static_cast<unsigned long long>(c->Rsi),
             static_cast<unsigned long long>(c->Rdi), static_cast<unsigned long long>(c->R8),
             static_cast<unsigned long long>(c->R9));
    log_line("     rip %016llx rsp %016llx rbp %016llx", static_cast<unsigned long long>(c->Rip),
             static_cast<unsigned long long>(c->Rsp), static_cast<unsigned long long>(c->Rbp));
    // x64 frames have no frame-pointer chain: unwind with the .pdata tables, then code addresses on the stack.
    unwind_stack(*c);
    const auto* sp = reinterpret_cast<const std::uintptr_t*>(c->Rsp);
    int shown = 0;
    for (int i = 0; i < 1024 && shown < 24 && readable(sp + i, sizeof *sp); ++i) {
        if (executable(sp[i])) {
            log_line("     stack[rsp+0x%03x] 0x%016llx %s", i * 8, static_cast<unsigned long long>(sp[i]),
                     describe(sp[i]).c_str());
            ++shown;
        }
    }
#endif
    write_counter_report();
    return g_previous ? g_previous(info) : EXCEPTION_CONTINUE_SEARCH;
}

}  // namespace

#if defined(_M_X64) || defined(__x86_64__)
// x64 (first MSVC x64 runs: the game window closed after the first frame with neither a crash report nor the call
// statistics): the ways a process ends without reaching the unhandled-exception filter above get a log line too.
//  - Windows' heap ends the process on a corrupted heap block (STATUS_HEAP_CORRUPTION 0xc0000374, raised as a
//    non-continuable exception, then terminated: no unhandled-exception filter), /GS cookie failures and __fastfail
//    (0xc0000409), stack overflow (0xc00000fd): a vectored handler sees the exception first and writes the report.
//    (A __fastfail that bypasses exception dispatch entirely leaves only Windows' event log / WER; see
//    openjoan.log's last lines and Event Viewer > Windows Logs > Application, "Application Error".)
//  - the C runtime's invalid-parameter handler, abort() (SIGABRT), std::terminate and pure virtual calls: logged with
//    the caller, then the runtime's default behaviour (the process ends as before).
namespace {

const char* fatal_name(DWORD code) {
    switch (code) {
        case 0xc0000374u: return "heap corruption (STATUS_HEAP_CORRUPTION)";
        case 0xc0000409u: return "stack buffer overrun / fast fail (STATUS_STACK_BUFFER_OVERRUN)";
        case 0xc0000417u: return "invalid C runtime parameter (STATUS_INVALID_CRUNTIME_PARAMETER)";
        case 0xc00000fdu: return "stack overflow (STATUS_STACK_OVERFLOW)";
        default: return nullptr;
    }
}

void report_once(const char* what, std::uintptr_t caller) {
    static std::atomic<bool> done{false};
    log_line("**** %s (caller 0x%016llx %s)", what, static_cast<unsigned long long>(caller), describe(caller).c_str());
    if (!done.exchange(true)) write_counter_report();
}

LONG CALLBACK on_vectored(EXCEPTION_POINTERS* info) {
    const DWORD code = info->ExceptionRecord->ExceptionCode;
    const char* const name = fatal_name(code);
    if (!name) return EXCEPTION_CONTINUE_SEARCH;  // everything else: the unhandled filter (or a handler) decides
    static std::atomic<bool> reported{false};
    if (!reported.exchange(true)) {
        log_line("**** fatal exception 0x%08lx: %s - Windows ends the process without the crash filter", code, name);
        // registers, call stack, call statistics; without the chained filter (Windows' own filter must not run
        // from here)
        const LPTOP_LEVEL_EXCEPTION_FILTER previous = g_previous;
        g_previous = nullptr;
        on_unhandled(info);
        g_previous = previous;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

void JOAN_CDECL on_invalid_parameter(const wchar_t*, const wchar_t*, const wchar_t*, unsigned, std::uintptr_t) {
    report_once("invalid parameter passed to a C runtime function", reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS()));
    RaiseFailFastException(nullptr, nullptr, 0);  // the runtime's default (release): end the process
}

void JOAN_CDECL on_abort_signal(int) {
    report_once("abort() called (SIGABRT)", reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS()));
    std::signal(SIGABRT, SIG_DFL);  // abort() goes on and ends the process
}

[[noreturn]] void on_terminate() {
    report_once("std::terminate called (C++ exception not caught, or thrown from a noexcept function)",
                reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS()));
    std::signal(SIGABRT, SIG_DFL);
    std::abort();
}

void JOAN_CDECL on_purecall() {
    report_once("pure virtual function called", reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS()));
    std::signal(SIGABRT, SIG_DFL);
    std::abort();
}

}  // namespace
#endif

void install_crash_handler() {
    g_previous = SetUnhandledExceptionFilter(&on_unhandled);
#if defined(_M_X64) || defined(__x86_64__)
    AddVectoredExceptionHandler(1, &on_vectored);
    _set_invalid_parameter_handler(&on_invalid_parameter);
    _set_purecall_handler(&on_purecall);
    std::signal(SIGABRT, &on_abort_signal);
    std::set_terminate(&on_terminate);
#endif
}

#else
// ---------------------------------------------------------------- elsewhere: signals
// A fault no other handler resolved (this one is installed first, so it runs last): the report, then the system's
// default action ends the process. Registers and the stack as far as the platform layer and backtrace() give them.
platform::memory::FaultAction on_fault(const platform::memory::Fault& f) {
    static std::atomic<bool> reported{false};
    if (reported.exchange(true)) return platform::memory::FaultAction::kPass;
    log_line("**** CRASH: signal %u at 0x%llx %s", static_cast<unsigned>(f.code), static_cast<unsigned long long>(f.pc),
             describe(f.pc).c_str());
    if (f.type == platform::memory::Fault::kAccess)
        log_line("     %s address 0x%llx",
                 f.access == platform::memory::kExec ? "executing" : f.access == platform::memory::kWrite ? "writing" : "reading",
                 static_cast<unsigned long long>(f.address));
    log_line("     pc %llx sp %llx", static_cast<unsigned long long>(f.pc), static_cast<unsigned long long>(f.sp));
    // The call stack through the signal frame, then code addresses on the stack. x86-64: the frame-pointer walk of
    // compiler.hpp (glibc's backtrace() unwinds through DWARF and faults on the code table's generated stubs, which
    // turned the first x64 shutdown crash into a crash inside the crash handler); elsewhere the unwind tables.
    void* frames[48];
#if defined(__x86_64__)
    const int n = static_cast<int>(joan::cpu::stack_trace_from(0, 48, frames));
#else
    const int n = backtrace(frames, 48);
#endif
    for (int i = 0; i < n; ++i) {
        const auto pc = reinterpret_cast<std::uintptr_t>(frames[i]);
        log_line("     frame %2d: 0x%llx %s", i, static_cast<unsigned long long>(pc), describe(pc).c_str());
    }
    const auto* sp = reinterpret_cast<const std::uintptr_t*>(f.sp);
    int shown = 0;
    for (int i = 0; i < 1024 && shown < 24 && f.sp && readable(sp + i, sizeof *sp); ++i) {
        if (executable(sp[i])) {
            log_line("     stack[sp+0x%03x] 0x%llx %s", i * static_cast<int>(sizeof *sp),
                     static_cast<unsigned long long>(sp[i]), describe(sp[i]).c_str());
            ++shown;
        }
    }
    write_counter_report();
    return platform::memory::FaultAction::kPass;
}

void report_once(const char* what, std::uintptr_t caller) {
    static std::atomic<bool> done{false};
    log_line("**** %s (caller 0x%llx %s)", what, static_cast<unsigned long long>(caller), describe(caller).c_str());
    if (!done.exchange(true)) write_counter_report();
}

void on_abort_signal(int) {
    report_once("abort() called (SIGABRT)", reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS()));
    std::signal(SIGABRT, SIG_DFL);  // abort() goes on and ends the process
}

[[noreturn]] void on_terminate() {
    report_once("std::terminate called (C++ exception not caught, or thrown from a noexcept function)",
                reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS()));
    std::signal(SIGABRT, SIG_DFL);
    std::abort();
}

}  // namespace

void install_crash_handler() {
    platform::memory::add_fault_handler(&on_fault);
    std::signal(SIGABRT, &on_abort_signal);
    std::set_terminate(&on_terminate);
}
#endif

}  // namespace joan::hook
