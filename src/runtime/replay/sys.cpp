// SPDX-License-Identifier: GPL-3.0-or-later
// Record / replay: the recorder's own system services (sys.hpp). Windows keeps the Win32 calls replay.cpp made
// before Phase 1 (same behaviour, moved here); everywhere else the platform layer (memory maps, clocks, thread ids,
// the game folder's case-insensitive names) and the C++ standard library answer.
#include "sys.hpp"

#if defined(__linux__)
#include <cerrno>
#include <sys/uio.h>
#include <unistd.h>
#endif

#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <system_error>

#include "../../compiler.hpp"
#include "../crash.hpp"
#include "../loader/import_overrides.hpp"
#include "../../platform/files.hpp"
#include "../../platform/memory.hpp"
#include "../../platform/threads.hpp"
#include "../../platform/window.hpp"
#include "../../registry.hpp"
#include "stream.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <dbghelp.h>
#else
#include "../../platform/win32_types.hpp"
#endif

namespace fs = std::filesystem;

namespace joan::replay::sys {

// ---------------------------------------------------------------- threads, errors

unsigned long thread_id() { return platform::thread_id(); }
unsigned long last_error() { return platform::last_error(); }
void set_last_error(unsigned long e) { platform::set_last_error(e); }

// ---------------------------------------------------------------- the process

std::string command_line() {
    using Fn = LPSTR(WINAPI*)();
    if (const Fn fn = loader::override_function<Fn>("KERNEL32.dll", "GetCommandLineA")) {
        const char* s = fn();
        return s ? s : "";
    }
#if defined(_WIN32)
    const char* s = ::GetCommandLineA();
    return s ? s : "";
#else
    return "";
#endif
}

std::uint32_t exe_stamp() {
#if defined(_WIN32)
    const auto base = reinterpret_cast<const std::uint8_t*>(::GetModuleHandleA(nullptr));
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    return nt->FileHeader.TimeDateStamp;
#else
    return 0;
#endif
}

void exe_range(std::uintptr_t& low, std::uintptr_t& high) { platform::memory::self_range(low, high); }

void end_process(unsigned code) { platform::memory::end_process(code); }

// ---------------------------------------------------------------- memory

void committed_regions(std::vector<MemRegion>& out) {
    out.clear();
#if defined(_WIN32)
    // Heaps / VirtualAlloc blocks / stacks (private) and modules (image); file views too (the game's archives are
    // mapped: MapViewOfFile pointers).
    std::uintptr_t a = 0x10000;
    MEMORY_BASIC_INFORMATION mbi;
#if defined(_WIN64)
    const std::uintptr_t top = 0x7fffffff0000ull;
#else
    const std::uintptr_t top = 0x7fff0000u;
#endif
    while (a < top && ::VirtualQuery(reinterpret_cast<const void*>(a), &mbi, sizeof(mbi)) == sizeof(mbi)) {
        const auto begin = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const std::uintptr_t end = begin + mbi.RegionSize;
        if (end <= a) break;
        if (mbi.State == MEM_COMMIT) {
            MemRegion r{};
            r.begin = begin;
            r.end = end;
            r.allocation_base = reinterpret_cast<std::uintptr_t>(mbi.AllocationBase);
            r.readable = (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) == 0 && mbi.Protect != 0;
            r.image = mbi.Type == MEM_IMAGE;
            out.push_back(r);
        }
        a = end;
    }
#elif defined(__linux__)
    // One read of /proc/self/maps per call (a frame): "start-end perms offset dev inode path" per line. Anonymous
    // mappings (inode 0: heaps, stacks, allocations) are private memory; file-backed ones are images (the
    // executable, shared libraries) or data files; a mapping is its own allocation.
    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line)) {
        unsigned long long begin = 0, end = 0, offset = 0, inode = 0;
        char perms[8] = {};
        char dev[32] = {};
        if (std::sscanf(line.c_str(), "%llx-%llx %7s %llx %31s %llu", &begin, &end, perms, &offset, dev, &inode) < 6)
            continue;
        if (end <= begin) continue;
        MemRegion r{};
        r.begin = static_cast<std::uintptr_t>(begin);
        r.end = static_cast<std::uintptr_t>(end);
        r.allocation_base = r.begin;
        r.readable = perms[0] == 'r';
        r.image = inode != 0;  // file-backed: the executable's and the libraries' mappings (code and data segments)
        out.push_back(r);
    }
#else
    // Other POSIX systems: platform::memory::query from the bottom of the address space upwards.
    std::uintptr_t a = 0x10000;
    platform::memory::Region reg;
    while (platform::memory::query(reinterpret_cast<const void*>(a), reg)) {
        if (reg.size == 0) break;
        const std::uintptr_t end = reg.base + reg.size;
        if (end <= a) break;
        if (reg.committed) {
            MemRegion r{};
            r.begin = reg.base;
            r.end = end;
            r.allocation_base = reg.base;
            r.readable = (reg.access & platform::memory::kRead) != 0 && !reg.guard;
            r.image = !reg.private_memory;
            out.push_back(r);
        }
        a = end;
    }
#endif
}

#if defined(_WIN32)
// The SEH-guarded read itself (kept free of objects with destructors: __try cannot share a function with unwinding).
static bool read_u32_raw(std::uintptr_t a, std::uint32_t* out) {
#if defined(_MSC_VER) && !defined(__clang__)
    __try {
        *out = *reinterpret_cast<const volatile std::uint32_t*>(a);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
#else
    *out = *reinterpret_cast<const volatile std::uint32_t*>(a);
    return true;
#endif
}
#endif

bool read_u32(std::uintptr_t a, std::uint32_t* out) {
#if defined(_WIN32)
    return read_u32_raw(a, out);
#else
#if defined(__linux__)
    // A read that cannot fault: the kernel copies the 4 bytes or returns EFAULT. Checking the mapping first and then
    // reading raced with the other threads (FFmpeg, OpenAL, the streamers): a buffer freed between the check and the
    // read ended a Linux x86 recording with SIGSEGV in the image classifier (2026-11-05, address 0xc3cfdd28).
    std::uint32_t v = 0;
    struct iovec local {&v, sizeof v};
    struct iovec remote {reinterpret_cast<void*>(a), sizeof v};
    const ssize_t got = ::process_vm_readv(::getpid(), &local, 1, &remote, 1, 0);
    if (got == static_cast<ssize_t>(sizeof v)) {
        *out = v;
        return true;
    }
    if (got >= 0 || errno == EFAULT) return false;
    // ENOSYS / EPERM (a sandbox without the call): the mapping check below, racy as described.
#endif
    if (!platform::memory::accessible(reinterpret_cast<const void*>(a), platform::memory::kRead)) return false;
    *out = *reinterpret_cast<const volatile std::uint32_t*>(a);
    return true;
#endif
}

// ---------------------------------------------------------------- clocks

unsigned long long tick_ms() {
    return static_cast<unsigned long long>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count());
}

void sleep_ms(unsigned ms) { platform::sleep_ms(ms); }

std::string local_time_stamp() {
    const std::time_t t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char stamp[32];
    std::snprintf(stamp, sizeof(stamp), "%04d%02d%02d-%02d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                  tm.tm_hour, tm.tm_min, tm.tm_sec);
    return stamp;
}

// ---------------------------------------------------------------- files

std::string native(const std::string& path) {
#if defined(_WIN32)
    return path;
#else
    return platform::files::native_path(path.c_str());
#endif
}

bool is_plain_file(const std::string& path) {
    std::error_code ec;
    return fs::is_regular_file(native(path), ec);
}

bool exists(const std::string& path) {
    std::error_code ec;
    return fs::exists(native(path), ec);
}

std::uint64_t hash_file(const std::string& path, std::uint64_t* size) {
    *size = 0;
    std::FILE* f = std::fopen(native(path).c_str(), "rb");
    if (!f) return 0;
    std::vector<std::uint8_t> buf(1 << 20);
    std::uint64_t hash = 0xcbf29ce484222325ull;
    for (std::size_t got; (got = std::fread(buf.data(), 1, buf.size(), f)) > 0;) {
        hash = hash_bytes(buf.data(), got, hash);
        *size += got;
    }
    std::fclose(f);
    return hash;
}

std::uint64_t quick_hash_file(const std::string& path, std::uint64_t* size) {
    *size = 0;
    const std::string p = native(path);
    std::error_code ec;
    const auto n = fs::file_size(p, ec);
    if (ec) return 0;
    std::FILE* f = std::fopen(p.c_str(), "rb");
    if (!f) return 0;
    *size = static_cast<std::uint64_t>(n);
    std::uint64_t hash = hash_bytes(size, sizeof(*size));
    static std::vector<std::uint8_t> buf(65536);
    std::size_t got = std::fread(buf.data(), 1, buf.size(), f);
    if (got) hash = hash_bytes(buf.data(), got, hash);
    if (*size > buf.size() * 2) {
        if (std::fseek(f, static_cast<long>(*size - buf.size()), SEEK_SET) == 0 &&
            (got = std::fread(buf.data(), 1, buf.size(), f)) > 0)
            hash = hash_bytes(buf.data(), got, hash);
    }
    std::fclose(f);
    return hash;
}

bool read_whole(const std::string& path, std::string& out) {
    out.clear();
    std::FILE* f = std::fopen(native(path).c_str(), "rb");
    if (!f) return false;
    char buf[65536];
    for (std::size_t got; (got = std::fread(buf, 1, sizeof(buf), f)) > 0;) out.append(buf, got);
    const bool ok = std::ferror(f) == 0;
    std::fclose(f);
    return ok;
}

bool write_whole(const std::string& path, const std::string& bytes) {
    std::string p = native(path);
    std::error_code ec;
    const fs::path parent = fs::path(p).parent_path();
    if (!parent.empty()) fs::create_directories(parent, ec);  // fails harmlessly for existing parts
    std::FILE* f = std::fopen(p.c_str(), "wb");
    if (!f) return false;
    const bool ok = std::fwrite(bytes.data(), 1, bytes.size(), f) == bytes.size();
    return (std::fclose(f) == 0) && ok;
}

void list_files(const std::string& root, const std::string& rel, int depth, std::vector<std::string>& out) {
    std::error_code ec;
    const fs::path dir = native(root + rel);
    for (fs::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
        const std::string name = it->path().filename().string();
        if (name == "." || name == "..") continue;
        const std::string r = rel + "\\" + name;
        if (it->is_directory(ec)) {
            if (depth < 4) list_files(root, r, depth + 1, out);
        } else if (it->is_regular_file(ec)) {
            out.push_back(r);
        }
    }
}

bool make_dir(const std::string& path) {
    std::error_code ec;
    return fs::create_directory(native(path), ec) || fs::is_directory(native(path), ec);
}

bool move_path(const std::string& from, const std::string& to, std::string* error) {
    std::error_code ec;
    fs::rename(native(from), to, ec);
    if (ec && error) *error = ec.message();
    return !ec;
}

// ---------------------------------------------------------------- diagnostics

std::string describe_frame(std::uintptr_t a) {
#if defined(_WIN32)
    // dbghelp names when openjoan.pdb is next to the exe (else module + offset).
    using SymInitializeFn = BOOL(WINAPI*)(HANDLE, PCSTR, BOOL);
    using SymFromAddrFn = BOOL(WINAPI*)(HANDLE, DWORD64, PDWORD64, void*);
    using SymSetOptionsFn = DWORD(WINAPI*)(DWORD);
    static HMODULE dbghelp = ::LoadLibraryA("dbghelp.dll");
    static auto sym_init = dbghelp ? reinterpret_cast<SymInitializeFn>(::GetProcAddress(dbghelp, "SymInitialize")) : nullptr;
    static auto sym_from = dbghelp ? reinterpret_cast<SymFromAddrFn>(::GetProcAddress(dbghelp, "SymFromAddr")) : nullptr;
    static auto sym_opts = dbghelp ? reinterpret_cast<SymSetOptionsFn>(::GetProcAddress(dbghelp, "SymSetOptions")) : nullptr;
    static bool ready = false;
    if (!ready && sym_init) {
        if (sym_opts) sym_opts(0x00000002 /* SYMOPT_UNDNAME */ | 0x00000004 /* SYMOPT_DEFERRED_LOADS */);
        ready = sym_init(::GetCurrentProcess(), nullptr, TRUE) != 0;
    }
    char line[400];
    alignas(8) unsigned char buf[sizeof(SYMBOL_INFO) + 256] = {};
    auto* sym = reinterpret_cast<SYMBOL_INFO*>(buf);
    sym->SizeOfStruct = sizeof(SYMBOL_INFO);
    sym->MaxNameLen = 255;
    DWORD64 disp = 0;
    HMODULE mod = nullptr;
    ::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                         reinterpret_cast<LPCSTR>(a), &mod);
    char mod_name[MAX_PATH] = "?";
    if (mod) ::GetModuleFileNameA(mod, mod_name, MAX_PATH);
    const char* base = std::strrchr(mod_name, '\\');
    base = base ? base + 1 : mod_name;
    const auto off = static_cast<unsigned>(a - reinterpret_cast<std::uintptr_t>(mod));
    if (ready && sym_from && sym_from(::GetCurrentProcess(), a, &disp, sym))
        std::snprintf(line, sizeof(line), "%s+0x%x  %s+0x%x", base, off, sym->Name, static_cast<unsigned>(disp));
    else
        std::snprintf(line, sizeof(line), "%s+0x%x", base, off);
    return line;
#else
    return hook::describe_address(a);
#endif
}

int capture_stack(unsigned skip, void** frames, int n) {
    if (n <= 0) return 0;
    return static_cast<int>(JOAN_STACK_TRACE(skip + 1, static_cast<unsigned>(n), frames));
}

void replay_stopped_notice(const std::string& text, void* window) {
#if defined(_WIN32)
    if (window) ::ShowWindow(static_cast<HWND>(window), SW_MINIMIZE);
    ::MessageBoxA(nullptr, text.c_str(), "OpenJoan replay", MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
#else
    (void)window;
    hook::log_line("replay: %s", text.c_str());
    std::fprintf(stderr, "OpenJoan replay: %s\n", text.c_str());
#endif
}

bool watch_supported() {
#if defined(_WIN32)
    return true;
#else
    return false;
#endif
}

bool heap_hash_supported() {
#if defined(_WIN32)
    return true;
#else
    return false;
#endif
}

}  // namespace joan::replay::sys
