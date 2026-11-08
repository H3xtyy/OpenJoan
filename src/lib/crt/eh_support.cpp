// Original library sources (re/attribution.csv original_file, under lib/): crt/ehvecctr.cpp, crt/hooks.cpp,
//   crt/intel/exsup3.asm, crt/mbtowc.c, crt/setenv.c, crt/wctomb.c, crt/winsig.c,
//   crt/wtombenv.c.
//
// Batch 117 / task 07: the last MSVC 6 CRT functions that our own C++ still calls by address (plus the private
// helpers they call). Read with notes CRT_SYSTEM.md (1, 4.6, 4.8, 4.9, 4.10, 5.1) and the batch-114 family files
// lib/crt/startup.cpp (crt/startup, crt/env), lib/crt/exit.cpp (crt/exit), lib/crt/mbstring.cpp (crt/mbs),
// lib/crt/printf.cpp / lib/crt/scanf.cpp (printf / scanf), lib/crt/x87_math.cpp (crt/math), lib/crt/mem_intrinsics.cpp
// (vector iterators).
//
// Rules followed (NOTE113 / NOTE114 / NOTE115): exact transcription; every other CRT function and every Win32 API
// is reached through its ORIGINAL address / joan.exe IAT slot (whatever implementation is installed there runs);
// all state stays at its original address with the original layout; entry-patched (library range, no thunks).
//
// Modules (one per family, the same names the batch-114 families use):
//   "crt/eh"   EH runtime pieces (plain C++ since port round P2; the VC6 SEH runtime is gone, see below)
//     0x007b30d4 void __stdcall __ArrayUnwind(void* ptr, size_t size, int count, void(__thiscall* dtor)(void*))
//                ret 0x10. Callers: the guard of eh vector ctor / dtor iterator (0x7b2f98 / 0x7b3054,
//                lib/crt/mem_intrinsics.cpp, by address).
//     0x007b8d34 void terminate(void)  (never returns). Callers: __CxxUnhandledExceptionFilter (lib/crt/exit.cpp, by
//                address), __ArrayUnwind; data: the pointer at 0x00a30fa4 (VC6 __pInconsistency).
//   "crt/exit" (signals are part of the exit family J, lib/crt/exit.cpp)
//     0x007c1b74 BOOL __stdcall ctrlevent_capture(DWORD CtrlType) ret 4: the HandlerRoutine that signal()
//                (lib/crt/exit.cpp) passes to SetConsoleCtrlHandler by its original address.
//   "crt/env" (family I, lib/crt/startup.cpp)
//     0x007c20ce int __wtomb_environ(void)            <- _getenv_lk (lib/crt/startup.cpp, by address), __crtsetenv
//     0x007c4b9a int __crtsetenv(const char* option, int primary)   <- __wtomb_environ only
//     0x007c4d21 int findenv(const char* name, int len)             <- __crtsetenv only (static in setenv.c)
//     0x007c4d79 char** copy_environ(char** oldenv)                 <- __crtsetenv only (static in setenv.c)
//   "crt/mbs" (multibyte <-> wide character conversion; locale readers like the rest of crt/mbs)
//     0x007c2e16 int wctomb(char* s, wchar_t wc)              <- _output (lib/crt/printf.cpp, by address)
//     0x007c2e6f int _wctomb_lk(char* s, wchar_t wc)          <- wctomb only
//     0x007c2f55 int mbtowc(wchar_t* pwc, const char* s, size_t n)   <- _input (lib/crt/scanf.cpp, by address)
//     0x007c2fb2 int _mbtowc_lk(wchar_t* pwc, const char* s, size_t n) <- mbtowc only
//
// ---------------------------------------------------------------------------------------------------------------
// SEH (port round P2): every frame of this program is our C++, and none uses VC6 SEH any more - the naked frames that
// pushed _except_handler3 (startup, __ArrayUnwind, terminate) and _local_unwind2's own record are gone, the game's
// C++ exceptions are C++ throws (original/cxx_eh.hpp). So _except_handler3 0x7bbe58 (and with it _global_unwind2,
// _local_unwind2, __unwind_handler, _NLG_Notify of lib/crt/seh_unwind.cpp, the RtlUnwind import stub, src/seh.cpp's
// SafeSEH check and src/seh_handlers.asm) is deleted: no frame names it, no data holds its address, nothing compares
// a handler with it (the VC6 setjmp3 check _seh_longjmp_unwind 0x7bbf15 is unreferenced). Its code slot is a trap.
//
// __ArrayUnwind: __try { while (--count >= 0) dtor(ptr -= size); } __except (ArrayUnwindFilter 0x7b3132): a C++
//   exception (0xE06D7363) -> terminate(), anything else EXCEPTION_CONTINUE_SEARCH (the __except handler 0x7b311a is
//   unreachable). Here: try / catch (...) -> terminate(); catch (...) takes exactly the C++ exceptions, a structured
//   exception passes on as before (no frame of this program handles one: crash handler).
// terminate: __try { if (_getptd()->_terminate (+0x60)) { __try { _getptd()->_terminate(); } __except (1) {} } }
//   __finally { abort(); }. ptd+0x60 is written only by set_terminate, which is not in the image (no code, no
//   caller); every ptd comes zeroed from calloc (crt/mt _mtinit / _getptd / _beginthread). So the field is 0 and
//   terminate() = _getptd(), abort() (0x7c218c, crt/exit; it never returns).
//
// Exactness notes (C++ functions):
//  * ctrlevent_capture: _lock(1); CTRL_C_EVENT (0) -> ctrlc action 0xa6b548 / SIGINT 2, else ctrlbreak 0xa6b54c /
//    SIGBREAK 21; SIG_DFL (0): _unlock, FALSE; SIG_IGN (1): _unlock, TRUE; else the slot is reset to SIG_DFL
//    before _unlock, then handler(sig) (cdecl), TRUE.
//  * __wtomb_environ: for each _wenviron 0xa6b208 string: WideCharToMultiByte(CP_OEMCP 1, 0, w, -1, NULL, 0, NULL,
//    NULL) size (0 -> -1), malloc(size) (NULL -> -1), second conversion re-reading the slot (0 -> -1),
//    __crtsetenv(buf, 0); returns 0. The buffer is not freed (it becomes the environment entry).
//  * __crtsetenv: VC6 setenv.c, including its quirks: _environ == __initenv (0xa6b204, always 0 in this process,
//    so only when _environ is 0 too) -> copy_environ; on removal the array is shrunk with realloc(env, n * 4)
//    where n excludes the terminating NULL (VC6 bug, kept), a failed realloc keeps the old array; the new-entry
//    path uses |findenv| and realloc(env, (n + 2) * 4); SetEnvironmentVariableA(name, remove ? NULL : value) on a
//    strlen + 2 malloc'd copy (malloc failure silently skips it). Return 0 / -1 exactly as the original.
//  * findenv: re-reads _environ for the index (byte difference >> 2, signed), negative when not found.
//  * copy_environ: count, malloc((n + 1) * 4) (NULL -> _amsg_exit(9) and then continues writing through the NULL
//    like the original), _strdup 0x7c5afa each entry, NULL terminator.
//  * wctomb / mbtowc: the setlocale read guard (InterlockedIncrement(__unguarded_readlc_active 0xa6ca8c); when
//    __setlc_active 0xa6ca7c != 0: decrement + _lock(_SETLOCALE_LOCK 0x13)), the _lk worker, release.
//    The wchar_t argument is passed on as the full stack dword (only its low 16 bits are read).
//  * _wctomb_lk: s NULL -> 0; C locale (__lc_handle[LC_CTYPE] 0xa6b2b8 == 0): wc > 0xff (unsigned 16 bit) ->
//    EILSEQ, else *s = (char)wc, 1; else WideCharToMultiByte(__lc_codepage 0xa6b2c8, WC_COMPOSITECHECK |
//    WC_SEPCHARS 0x220, &wc, 1, s, __mb_cur_max 0xa30ed0, NULL, &defused): 0 or defused -> *_errno() = EILSEQ (42),
//    -1; else the byte count. (The original's &defused is its own first argument slot, dead for its only caller;
//    ours is a local.)
//  * _mbtowc_lk: s NULL or n 0 -> 0; *s == 0 -> *pwc = 0 (if pwc), 0; C locale -> *pwc = (u8)*s, 1; lead byte
//    (_pctype 0xa30cc4 [c] & _LEADBYTE 0x8000): __mb_cur_max > 1 (signed): (int)n < max (signed) -> EILSEQ;
//    MultiByteToWideChar(cp, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS 9, s, max, pwc, pwc != 0) != 0 -> returns
//    __mb_cur_max re-read after the call; then (also for max <= 1, with the re-read value) n < max (unsigned) or
//    s[1] == 0 -> EILSEQ, else max; single byte: MultiByteToWideChar(cp, 9, s, 1, pwc, pwc != 0) ? 1 : EILSEQ.
//
// Not taken here (dead, cleanup C1): the Pentium FDIV workaround library (adj_fdiv.asm, 0x7ba7a0..0x7bb52b) - only
// entered when _adjust_fdiv 0xa6b1c4 == 1, which never happens (lib/crt/startup.cpp _fpmath: IsProcessorFeaturePresent(
// PF_FLOATING_POINT_PRECISION_ERRATA) is FALSE without the 1994 Pentium erratum); _seh_longjmp_unwind 0x7bbf15
// (no reference in the image); the original funclets 0x7b3110 / 0x7b311a / 0x7b3132 / 0x7b8d7c / 0x7b8d80 /
// 0x7b8d90 (folded into the C++ functions); unexpected 0x7b8d95 / _inconsistency (EH runtime family, dead).
// Deleted in port round P2: _except_handler3 0x7bbe58 (see "SEH" above).
//
// Tests: scratchpad w117_07/h (32-bit Linux, joan.exe mapped at 0x400000, fs set up with set_thread_area). Each
// function runs as the original and as this file's version with every callee / IAT slot stubbed and logged
// (batch 117; the SEH-frame versions of __ArrayUnwind / terminate were compared there too, 100k / 30k cases; the
// P2 C++ versions: re/p2/eh, a host test of the throw / catch / unwind semantics), ctrlevent_capture 20k,
// environment 200k (crtsetenv /
// __wtomb_environ / findenv / copy_environ incl. malloc / realloc / conversion failures and the original's NULL
// dereference paths), wctomb / mbtowc + _lk 500k. Mutations of the transcriptions are detected.

#include "../../callconv.hpp"
#include <cstdint>

#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"

namespace {
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::call::cdecl_call;
using joan::call::thiscall;
// Win32 call through one of joan.exe's IAT slots (stdcall; the slot is read at the call).
template <class R, class... A>
R api(img::Addr slot, A... args) {
    return (*reinterpret_cast<R(JOAN_STDCALL* volatile*)(A...)>(img::addr(slot)))(args...);
}
template <class T>
volatile T& g(img::Addr address) {
    return img::global<volatile T>(address);
}

// ---- other families, called at their original addresses
constexpr fn::Code kLock = fn::crt_lock;        // _lock (crt/mt)
constexpr fn::Code kUnlock = fn::crt_unlock;      // _unlock (crt/mt)
constexpr fn::Code kErrno = fn::crt_errno;       // _errno (crt/mt)
constexpr fn::Code kMalloc = fn::malloc;      // malloc (crt/heap)
constexpr fn::Code kFree = fn::free;        // free (crt/heap)
constexpr fn::Code kRealloc = fn::realloc;     // realloc (crt/heap)
constexpr fn::Code kStrlen = fn::crt_strlen;      // strlen
constexpr fn::Code kStrcpy = fn::crt_strcpy;      // strcpy
constexpr fn::Code kStrdup = fn::strdup;      // _strdup
constexpr fn::Code kMbschr = fn::crt_mbschr;      // _mbschr (crt/mbs)
constexpr fn::Code kMbsnbicoll = fn::crt_mbsnbicoll;  // _mbsnbicoll (crt/mbs)
constexpr fn::Code kAmsgExit = fn::crt_amsg_exit;    // _amsg_exit (crt/exit)
// ---- this file's functions (calls inside a family go through the original addresses too)
constexpr fn::Code kWtombEnviron = fn::crt_wtomb_environ;
constexpr fn::Code kCrtsetenv = fn::crt_crtsetenv;
constexpr fn::Code kFindenv = fn::crt_findenv;
constexpr fn::Code kCopyEnviron = fn::crt_copy_environ;
// Bytes of one environment array entry (char* / wchar_t*): 4 in the original; the arrays hold native pointers.
constexpr u32 kPtrSize = static_cast<u32>(sizeof(char*));
constexpr fn::Code kWctombLk = fn::crt_wctomb_lk;
constexpr fn::Code kMbtowcLk = fn::crt_mbtowc_lk;
// ---- IAT slots (KERNEL32)
constexpr img::Addr kWideCharToMultiByte = d::iat_KERNEL32_WideCharToMultiByte;
constexpr img::Addr kMultiByteToWideChar = d::iat_KERNEL32_MultiByteToWideChar;
constexpr img::Addr kInterlockedIncrement = d::iat_KERNEL32_InterlockedIncrement;
constexpr img::Addr kInterlockedDecrement = d::iat_KERNEL32_InterlockedDecrement;
constexpr img::Addr kSetEnvironmentVariableA = d::iat_KERNEL32_SetEnvironmentVariableA;
// ---- state (original addresses)
constexpr img::Addr kEnviron = d::g_environ;     // char** _environ
constexpr img::Addr kInitenv = d::g_initenv;     // char** __initenv
constexpr img::Addr kWenviron = d::g_wenviron;    // wchar_t** _wenviron
constexpr img::Addr kCtrlcAction = d::g_ctrlcAction;
constexpr img::Addr kCtrlbreakAction = d::g_ctrlbreakAction;
constexpr img::Addr kLcCtype = d::g_lcCtype;     // __lc_handle[LC_CTYPE]
constexpr img::Addr kLcCodepage = d::g_lc_codepage;  // __lc_codepage
constexpr img::Addr kMbCurMax = d::g_mb_cur_max;    // __mb_cur_max
constexpr img::Addr kPctype = d::g_pctype;      // _pctype
constexpr img::Addr kSetlcActive = d::g_setlc_active;      // __setlc_active
constexpr img::Addr kUnguardedReadlc = d::g_unguarded_readlc_active;  // __unguarded_readlc_active
constexpr i32 kSetlocaleLock = 0x13;
constexpr i32 kEilseq = 0x2a;

JOAN_COUNTER(c_wtomb_environ, "crt::__wtomb_environ");
JOAN_COUNTER(c_crtsetenv, "crt::__crtsetenv");
JOAN_COUNTER(c_findenv, "crt::findenv");
JOAN_COUNTER(c_copy_environ, "crt::copy_environ");
JOAN_COUNTER(c_ctrlevent, "crt::ctrlevent_capture");
JOAN_COUNTER(c_mbtowc_lk, "crt::_mbtowc_lk");

// =====================================================================================================
// crt/eh
// =====================================================================================================

constexpr fn::Code kTerminate = fn::crt_terminate;  // terminate (called through its slot, like the original's call)
constexpr fn::Code kGetptd = fn::crt_getptd;        // _getptd (crt/mt)
constexpr fn::Code kAbort = fn::crt_abort;          // abort (crt/exit)

// 0x007b30d4 __ArrayUnwind(ptr, size, count, dtor), stdcall
void JOAN_STDCALL array_unwind(void* ptr, u32 size, i32 count, void* dtor) {
    try {
        u8* p = static_cast<u8*>(ptr);
        for (;;) {
            count = static_cast<i32>(static_cast<u32>(count) - 1u);  // dec / js
            if (count < 0) break;
            p -= size;
            thiscall(reinterpret_cast<std::uintptr_t>(dtor), p);
        }
    } catch (...) {  // ArrayUnwindFilter 0x7b3132: a C++ exception -> terminate()
        cdecl_call(kTerminate);
    }
}

// 0x007b8d34 terminate()
void JOAN_CDECL crt_terminate() {
    cdecl_call<void*>(kGetptd);  // its _terminate (+0x60) is always 0 (see the header)
    cdecl_call(kAbort);          // the __finally: abort() never returns
}

// =====================================================================================================
// crt/exit: console control handler of signal()
// =====================================================================================================

// 0x007c1b74 ctrlevent_capture(CtrlType), HandlerRoutine (stdcall, ret 4)
i32 JOAN_STDCALL ctrlevent_capture(u32 ctrl_type) {
    c_ctrlevent.hit();
    cdecl_call(kLock, 1);
    img::Addr slot{0};
    i32 sig;
    if (ctrl_type == 0) {  // CTRL_C_EVENT
        slot = kCtrlcAction;
        sig = 2;           // SIGINT
    } else {
        slot = kCtrlbreakAction;
        sig = 0x15;        // SIGBREAK
    }
    const u32 action = g<u32>(slot);
    if (action == 0) {  // SIG_DFL
        cdecl_call(kUnlock, 1);
        return 0;
    }
    if (action != 1) {  // a handler: reset to SIG_DFL, release, call it
        g<u32>(slot) = 0;
        cdecl_call(kUnlock, 1);
        // The slot is a 32-bit field of the image (i32, the original's fixed layout, as in lib/crt/exit.cpp handler_fn):
        // on x64 it could hold only a code address below 4 GB; nothing installs one (crt::signal has no caller).
        reinterpret_cast<void(JOAN_CDECL*)(i32)>(static_cast<std::uintptr_t>(action))(sig);
        return 1;
    }
    cdecl_call(kUnlock, 1);  // SIG_IGN
    return 1;
}

// =====================================================================================================
// crt/env: __wtomb_environ, __crtsetenv and setenv.c's statics
// =====================================================================================================

// 0x007c20ce __wtomb_environ
i32 JOAN_CDECL wtomb_environ() {
    c_wtomb_environ.hit();
    const u16* volatile* wenvp = g<const u16* volatile*>(kWenviron);
    while (const u16* const w = *wenvp) {
        const i32 size = api<i32>(kWideCharToMultiByte, 1u, 0u, w, -1, static_cast<char*>(nullptr), 0,
                                  static_cast<const char*>(nullptr), static_cast<i32*>(nullptr));
        if (size == 0) return -1;
        char* const envp = cdecl_call<char*>(kMalloc, size);
        if (envp == nullptr) return -1;
        if (api<i32>(kWideCharToMultiByte, 1u, 0u, *wenvp, -1, envp, size, static_cast<const char*>(nullptr),
                     static_cast<i32*>(nullptr)) == 0)
            return -1;
        cdecl_call<i32>(kCrtsetenv, envp, 0);
        ++wenvp;
    }
    return 0;
}

// 0x007c4d21 findenv(name, len): index of "name=" / "name" in _environ, or -(number of entries).
i32 JOAN_CDECL findenv(const char* name, i32 len) {
    c_findenv.hit();
    char* volatile* env = g<char* volatile*>(kEnviron);
    for (; *env != nullptr; ++env) {
        if (cdecl_call<i32>(kMbsnbicoll, name, *env, len) == 0) {
            const char c = (*env)[len];
            if (c == '=' || c == 0)
                return static_cast<i32>(env - g<char* volatile*>(kEnviron));  // (byte difference >> 2, signed)
        }
    }
    return -static_cast<i32>(env - g<char* volatile*>(kEnviron));
}

// 0x007c4d79 copy_environ(oldenv)
char** JOAN_CDECL copy_environ(char** oldenv) {
    c_copy_environ.hit();
    if (oldenv == nullptr) return nullptr;
    u32 count = 0;
    if (*oldenv != nullptr) {
        char* volatile* p = oldenv + 1;
        char* e;
        do {
            e = *p;
            ++count;
            ++p;
        } while (e != nullptr);
    }
    char** const newenv = cdecl_call<char**>(kMalloc, (count + 1) * kPtrSize);
    if (newenv == nullptr) cdecl_call(kAmsgExit, 9);  // _RT_SPACEENV; the original continues afterwards
    char* volatile* out = newenv;
    for (char* volatile* in = oldenv; *in != nullptr; ++in) *out++ = cdecl_call<char*>(kStrdup, *in);
    *out = nullptr;
    return newenv;
}

// 0x007c4b9a __crtsetenv(option "name=value" / "name=", primary)
i32 JOAN_CDECL crtsetenv(const char* option, i32 primary) {
    c_crtsetenv.hit();
    if (option == nullptr) return -1;
    const char* const equal = cdecl_call<const char*>(kMbschr, option, 0x3du);
    if (equal == nullptr || equal == option) return -1;
    const bool remove = equal[1] == 0;
    char** env = g<char**>(kEnviron);
    if (env == g<char**>(kInitenv)) {
        env = cdecl_call<char**>(kCopyEnviron, env);
        g<char**>(kEnviron) = env;
    }
    if (env == nullptr) {
        if (primary != 0 && g<u16**>(kWenviron) != nullptr) {
            if (cdecl_call<i32>(kWtombEnviron) != 0) return -1;
        } else {
            if (remove) return 0;
            char** const e = cdecl_call<char**>(kMalloc, kPtrSize);  // { NULL }
            g<char**>(kEnviron) = e;
            if (e == nullptr) return -1;
            *e = nullptr;
            if (g<u16**>(kWenviron) == nullptr) {
                u16** const w = cdecl_call<u16**>(kMalloc, kPtrSize);
                g<u16**>(kWenviron) = w;
                if (w == nullptr) return -1;
                *w = nullptr;
            }
        }
    }
    const i32 len = static_cast<i32>(equal - option);
    char* volatile* const envp = g<char* volatile*>(kEnviron);
    i32 ix = cdecl_call<i32>(kFindenv, option, len);
    if (ix >= 0 && *envp != nullptr) {
        if (remove) {
            char* volatile* p = envp + ix;
            cdecl_call(kFree, *p);
            while (*p != nullptr) {
                ++ix;
                *p = p[1];
                ++p;
            }
            void* const r = cdecl_call<void*>(kRealloc, static_cast<void*>(const_cast<char**>(envp)),
                                              static_cast<u32>(ix) * kPtrSize);
            if (r != nullptr) g<void*>(kEnviron) = r;
        } else {
            envp[ix] = const_cast<char*>(option);
        }
    } else {
        if (remove) return 0;
        if (ix < 0) ix = -ix;
        char** const r = cdecl_call<char**>(kRealloc, static_cast<void*>(const_cast<char**>(envp)),
                                            (static_cast<u32>(ix) + 2) * kPtrSize);
        if (r == nullptr) return -1;
        r[ix] = const_cast<char*>(option);
        r[ix + 1] = nullptr;
        g<char**>(kEnviron) = r;
    }
    if (primary != 0) {
        char* const buf = cdecl_call<char*>(kMalloc, cdecl_call<u32>(kStrlen, option) + 2);
        if (buf != nullptr) {
            cdecl_call<char*>(kStrcpy, buf, option);
            char* const value = buf + (equal - option);
            *value = 0;
            api<i32>(kSetEnvironmentVariableA, static_cast<const char*>(buf),
                     static_cast<const char*>(remove ? nullptr : value + 1));
            cdecl_call(kFree, buf);
        }
    }
    return 0;
}

// =====================================================================================================
// crt/mbs: wctomb / mbtowc
// =====================================================================================================

// The setlocale read guard of the C-coded locale readers.
bool readlc_enter() {
    volatile i32* const unguarded = &img::global<volatile i32>(kUnguardedReadlc);  // LONG: 32 bits
    api<i32>(kInterlockedIncrement, unguarded);
    if (g<i32>(kSetlcActive) != 0) {
        api<i32>(kInterlockedDecrement, unguarded);
        cdecl_call(kLock, kSetlocaleLock);
        return true;
    }
    return false;
}
void readlc_leave(bool locked) {
    if (locked)
        cdecl_call(kUnlock, kSetlocaleLock);
    else
        api<i32>(kInterlockedDecrement, &img::global<volatile i32>(kUnguardedReadlc));
}

// 0x007c2e6f _wctomb_lk(s, wc)  (wc: the full argument dword, low 16 bits used)
i32 JOAN_CDECL wctomb_lk(char* s, u32 wc_arg) {
    if (s == nullptr) return 0;
    const u16 wc = static_cast<u16>(wc_arg);
    if (g<u32>(kLcCtype) == 0) {
        if (wc > 0xff) {
            *cdecl_call<i32*>(kErrno) = kEilseq;
            return -1;
        }
        *s = static_cast<char>(wc);
        return 1;
    }
    i32 defused = 0;
    const i32 r = api<i32>(kWideCharToMultiByte, g<u32>(kLcCodepage), 0x220u, &wc, 1, s, g<i32>(kMbCurMax),
                           static_cast<const char*>(nullptr), &defused);
    if (r == 0 || defused != 0) {
        *cdecl_call<i32*>(kErrno) = kEilseq;
        return -1;
    }
    return r;
}

// 0x007c2e16 wctomb(s, wc)
i32 JOAN_CDECL wctomb(char* s, u32 wc_arg) {
    const bool locked = readlc_enter();
    const i32 r = cdecl_call<i32>(kWctombLk, s, wc_arg);
    readlc_leave(locked);
    return r;
}

// 0x007c2fb2 _mbtowc_lk(pwc, s, n)
i32 JOAN_CDECL mbtowc_lk(u16* pwc, const char* s, u32 n) {
    c_mbtowc_lk.hit();
    if (s == nullptr || n == 0) return 0;
    const u8 c = static_cast<u8>(*s);
    if (c == 0) {
        if (pwc != nullptr) *pwc = 0;
        return 0;
    }
    if (g<u32>(kLcCtype) == 0) {
        if (pwc != nullptr) *pwc = c;
        return 1;
    }
    const u8* const pctype = g<const u8*>(kPctype);
    if ((pctype[c * 2 + 1] & 0x80) != 0) {  // _LEADBYTE
        i32 mbmax = g<i32>(kMbCurMax);
        if (mbmax > 1) {
            if (static_cast<i32>(n) < mbmax) goto eilseq;
            const i32 r = api<i32>(kMultiByteToWideChar, g<u32>(kLcCodepage), 9u, s, mbmax, pwc,
                                   pwc != nullptr ? 1 : 0);
            mbmax = g<i32>(kMbCurMax);
            if (r != 0) return mbmax;
        }
        if (n < static_cast<u32>(mbmax) || s[1] == 0) goto eilseq;
        return mbmax;
    }
    if (api<i32>(kMultiByteToWideChar, g<u32>(kLcCodepage), 9u, s, 1, pwc, pwc != nullptr ? 1 : 0) != 0) return 1;
eilseq:
    *cdecl_call<i32*>(kErrno) = kEilseq;
    return -1;
}

// 0x007c2f55 mbtowc(pwc, s, n)
i32 JOAN_CDECL mbtowc(u16* pwc, const char* s, u32 n) {
    const bool locked = readlc_enter();
    const i32 r = cdecl_call<i32>(kMbtowcLk, pwc, s, n);
    readlc_leave(locked);
    return r;
}

#define EH(name, addr, fn) JOAN_REPLACE("crt/eh", name, addr, fn)
EH("crt::__ArrayUnwind", 0x007b30d4, &array_unwind);
EH("crt::terminate", 0x007b8d34, &crt_terminate);
#undef EH
JOAN_REPLACE("crt/exit", "crt::ctrlevent_capture", 0x007c1b74, &ctrlevent_capture);
#define ENV(name, addr, fn) JOAN_REPLACE("crt/env", name, addr, fn)
ENV("crt::__wtomb_environ", 0x007c20ce, &wtomb_environ);
ENV("crt::__crtsetenv", 0x007c4b9a, &crtsetenv);
ENV("crt::findenv", 0x007c4d21, &findenv);
ENV("crt::copy_environ", 0x007c4d79, &copy_environ);
#undef ENV
#define MBS(name, addr, fn) JOAN_REPLACE("crt/mbs", name, addr, fn)
MBS("crt::wctomb", 0x007c2e16, &wctomb);
MBS("crt::_wctomb_lk", 0x007c2e6f, &wctomb_lk);
MBS("crt::mbtowc", 0x007c2f55, &mbtowc);
MBS("crt::_mbtowc_lk", 0x007c2fb2, &mbtowc_lk);
#undef MBS

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::__wtomb_environ() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::wtomb_environ>()); }  // 0x7c20ce
auto joan::orig::libc::mbtowc(void* a0, const char* a1, std::uint32_t a2) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::mbtowc>(a0, a1, a2)); }  // 0x7c2f55
