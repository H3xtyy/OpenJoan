// Original library sources (re/attribution.csv original_file, under lib/): crt/a_env.c, crt/crt0.c, crt/fp8.c,
//   crt/fpinit.c, crt/getenv.c, crt/intel/testfdiv.c, crt/stdargv.c, crt/stdenvp.c, crt/wincmdln.c.
//
// Batch 114 / list 01: the MSVC 6 CRT startup (crt0.c, _WINMAIN_ + _MT) and the two small families it
// owns: K (floating-point init) and I (environment / command line). See notes CRT_SYSTEM.md 1, 4.6, 4.9,
// 5.2 (families I, K), 5.3 step 1 and the 5.4 checklist.
//
// Rule followed everywhere: every other CRT piece is called through its ORIGINAL address (cdecl_call /
// a code slot), so whichever implementation is installed there (original or another
// batch-114 family) runs; every Win32 call goes through joan.exe's IAT slot (the openjoan.exe loader
// wraps ExitProcess there; GetModuleHandleA(NULL) is openjoan.exe since S7d). All state stays at its original
// address with the original layout (other original CRT code reads it, see the per-family notes).
//
// ---------------------------------------------------------------------------------------------------
// Module "crt/startup"
//   0x007b71e6 WinMainCRTStartup(void)  (process entry; openjoan.exe calls it as void(*)())
//     Plain C++ (port round P2): startup_pre (the original's part before its __try), then startup_body
//     (the part inside it), call for call:
//       GetVersion [0xa6d988] -> _winminor 0xa6b1f0 = (v>>8)&0xff, _winmajor 0xa6b1ec = v&0xff,
//         _winver 0xa6b1e8 = major<<8 + minor, _osver 0xa6b1e4 = v>>16 (same store order);
//       _heap_init(1) 0x007bdb53, 0 -> fast_error_exit(0x1c) 0x007b7313;
//       _mtinit() 0x007b8b9c, 0 -> fast_error_exit(0x10);
//       __try: _ioinit 0x007c0e73; _acmdln 0xa6ca70 = GetCommandLineA [0xa6d98c];
//       _aenvptr 0xa6b240 = __crtGetEnvironmentStringsA 0x007c151d; _setargv 0x007c12d0;
//       _setenvp 0x007c1217; _cinit 0x007b47cf; GetStartupInfoA [0xa6d99c] (dwFlags pre-cleared);
//       lpszCmd = _wincmdln 0x007c11bf; nShow = (dwFlags & STARTF_USESHOWWINDOW) ? wShowWindow : 10;
//       WinMain(GetModuleHandleA(NULL) [0xa6d950], NULL, lpszCmd, nShow) via thunk 0x0040a169;
//       exit(ret) 0x007b47fc.
//     The original's VC6 __try / __except frame (scope table 0x009e7078: filter 0x007b72cf = return
//     _XcptFilter(code, pointers), handler 0x007b72e3 = _exit(code)) is gone: no SEH frame on x64 /
//     Linux. What that changes, only for an exception nothing in the game handles: before, _XcptFilter
//     found no signal action (signal() has no caller, every action is SIG_DFL) and returned
//     UnhandledExceptionFilter(pointers), i.e. the top-level filter chain (__CxxUnhandledExceptionFilter
//     -> crash.cpp's handler, which writes the crash log) - now the OS runs that same chain itself when the
//     exception leaves the thread. Only when the chain answers EXCEPTION_EXECUTE_HANDLER the process used
//     to end through _exit(code) (XT terminators, the loader's ExitProcess reports); now the OS ends it
//     with the exception code. exit() does not return (the original would fall into its filter funclet;
//     its int3 is std::abort here).
//
// ---------------------------------------------------------------------------------------------------
// Module "crt/fpinit" (family K)
//   0x007b1d43 _fpmath(void)                 _cfltcvt_init; _adjust_fdiv 0xa6b1c4 = _ms_p5_mp_test_fdiv();
//                                            _setdefaultprecision; fnclex (std::feclearexcept; reached
//                                            through the _FPinit pointer 0xa30948 from _cinit, no direct
//                                            caller). The three calls go through their code slots.
//   0x007b1d5b _cfltcvt_init(void)           _cfltcvt_tab 0xa30fa8..0xa30fbc = _cfltcvt 0x7b922c,
//                                            _cropzeros 0x7b8ed6, _fassign 0x7b8f3c, _forcdecpt 0x7b8e7c,
//                                            _positive 0x7b8f24, _cfltcvt (same store order). The table
//                                            stays at its address: the original _output / _input call
//                                            through it (0x7bb9ed, 0x7bba0e, 0x7bba25, 0x7bc6a3).
//   0x007b8e03 _setdefaultprecision(void)    _controlfp(_PC_53 0x10000, _MCW_PC 0x30000) 0x007b78af
//                                            (eax = _controlfp's result, as the original leaves it)
//   0x007b8e53 _ms_p5_mp_test_fdiv(void)     0. The original asks IsProcessorFeaturePresent(
//                                            PF_FLOATING_POINT_PRECISION_ERRATA) via GetModuleHandleA
//                                            ("KERNEL32") / GetProcAddress (FALSE on every CPU without the
//                                            1994 Pentium FDIV erratum, i.e. every CPU this runs on), else
//                                            _ms_p5_test_fdiv 0x7b8e15 (deleted in cleanup C1). Port round P2
//                                            dropped the two KERNEL32 calls (not recorded by the replay).
//   _adjust_fdiv 0xa6b1c4 is therefore always 0 (as before: bss, written with 0); nothing reads it any more
//   (the FDIV workaround library 0x7ba7a0..0x7bb52b, entered only when it is 1, is deleted).
//   NOT taken: _fpreset 0x007b78c5 - it is unreachable (no call, no jump, no pointer anywhere in the
//   image) and also on the crt/math list (l114_13); registering it twice would break the hook. Its
//   behaviour, for whoever owns it: esi = *__pxcptinfoptrs() 0x7c1d9e (ptd+0x54), fninit,
//   _setdefaultprecision(), and if esi && (esi->ContextRecord->ContextFlags & 0x10008):
//   FloatSave.StatusWord (+0x20) = 0, TagWord (+0x24) = 0xffff.
//
// ---------------------------------------------------------------------------------------------------
// Module "crt/env" (family I) - state kept at the original addresses: _acmdln 0xa6ca70 (read by
// _setargv / _wincmdln), _aenvptr 0xa6b240, _environ 0xa6b200 and __env_initialized 0xa6cd28 (read by
// _getenv_lk, the original __wtomb_environ 0x7c20ce / __crtsetenv 0x7c4b9a), _wenviron 0xa6b208,
// __argc 0xa6b1f4 / __argv 0xa6b1f8 / _pgmptr 0xa6b210 / _pgmname 0xa6b43c[260] (no reader in the
// image), the environment-API choice 0xa6b540 (only __crtGetEnvironmentStringsA). Strings / arrays are
// allocated with the CRT malloc 0x007b6c54 and freed with free 0x007b4e1d (heap family B decides).
//   0x007c151d char* __crtGetEnvironmentStringsA(void)   W API + WideCharToMultiByte(CP_ACP) (cache 1);
//              malloc'd copy of the double-NUL block or NULL. The original falls back to the A API
//              (cache 2) when GetEnvironmentStringsW returns NULL; ours (platform/process.cpp) never does,
//              so that branch and its two imports (GetEnvironmentStrings / FreeEnvironmentStringsA) went
//              (port round P2): a NULL block now gives NULL, the original's result when both APIs fail.
//   0x007c12d0 int _setargv(void)    __initmbctable 0x7b524d when 0xa6cd38 == 0; GetModuleFileNameA
//              (NULL, _pgmname, 260) [0xa6d960]; _pgmptr = _pgmname; parse_cmdline twice (count, fill)
//              on _acmdln (or _pgmname when the command line is empty); malloc(numargs*4 + numchars),
//              NULL -> _amsg_exit(8) 0x007b72ee; __argv, __argc = numargs - 1 (also returned in eax).
//   0x007c1369 parse_cmdline(cmdstart, argv, args, numargs*, numchars*)  VC6 stdargv.c, _MBCS:
//              lead bytes tested on _mbctype+1 (0xa6cc21) & _M1 (4) read live; keeps the VC6 quirk of
//              not advancing past a lead byte inside the quoted program name when args == NULL (count
//              pass). Returns numargs (eax as the original leaves it).
//   0x007c1217 void _setenvp(void)   __initmbctable if needed; count the non-'=' strings of _aenvptr
//              (strlen 0x7b1ef0), _environ = malloc((n+1)*4) (stored before the NULL check),
//              strdup each one (malloc + strcpy 0x7b2020), NULL -> _amsg_exit(9); free(_aenvptr),
//              _aenvptr = 0, terminating NULL, __env_initialized = 1.
//   0x007c11bf char* _wincmdln(void) skip the program name (quoted: _ismbblead 0x7c4a97 per byte;
//              unquoted: bytes > ' ' unsigned), then bytes 1..0x20.
//   0x007b7eb3 char* getenv(const char*)        _lock(_ENV_LOCK 0xc) 0x7b82aa, _getenv_lk, _unlock
//              0x7b830b. Callers: libjpeg jinit_memory_mgr 0x7aabd6 ("JPEGMEM").
//   0x007b7ed4 char* _getenv_lk(const char*)    NULL unless __env_initialized; _environ == NULL ->
//              (_wenviron NULL -> NULL; __wtomb_environ() != 0 -> NULL; re-read _environ); NULL name
//              -> NULL; first entry with strlen > len, '=' at [len] and _mbsnbicoll 0x7c208f(entry,
//              name, len) == 0 -> entry + len + 1. Callers: getenv, _tzset 0x7bd4e6 ("TZ").
//   Left original (unreachable): __wtomb_environ 0x7c20ce and __crtsetenv 0x7c4b9a. The only writer
//   of _wenviron 0xa6b208 is __crtsetenv (0x7c4c33), reached only from __wtomb_environ, which
//   _getenv_lk calls only when _wenviron != NULL: _wenviron is always 0, both are dead. They read
//   and write _environ at the original address with the original layout, so even if reached they
//   interoperate. _getenv_lk still calls 0x7c20ce through its address.
//
// Tests (batch 114, scratchpad w114_01/h; before the P2 changes above): differential harness on 32-bit Linux with joan.exe mapped at
// 0x400000, originals run natively next to this file's functions, every callee / IAT slot stubbed and
// logged: parse_cmdline 200k, _wincmdln 100k, _setenvp 50k (incl. malloc failures -> _amsg_exit),
// __crtGetEnvironmentStringsA 50k (all cache modes, API / WideCharToMultiByte / malloc failures),
// _setargv 50k, getenv / _getenv_lk 100k, the FP init functions under 6 control words x 16 API
// variants (cw / sw / globals), the entry itself 16 ways (heap / mt init failures, STARTUPINFO flags;
// registration record, trylevel, call log) and the filter funclet: 86603 checks, 0 differences, at -O0
// and -O2; mutations of the C++ are detected.
//
// Prologues: every address here is in the library range; all are installed by the 5-byte library
// entry patch (re/check_lib_entries.py reports them safe). No thunks.
// Counters: crt::startup, __crtGetEnvironmentStringsA, _setargv, parse_cmdline, _setenvp, _wincmdln,
// _getenv_lk (the others are a handful of instructions).

#include "../../callconv.hpp"
#include "../../original/helpers.hpp"
#include <cfenv>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "../../original/direct.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"

namespace {
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

using joan::img::global;

using joan::call::cdecl_call;

// Win32 call through one of joan.exe's IAT slots (stdcall).
template <class R, class... A>
R winapi(img::Addr slot, A... args) {
    return reinterpret_cast<R(JOAN_STDCALL*)(A...)>(img::global<std::uintptr_t>(slot))(args...);
}

// ---- joan.exe IAT slots (KERNEL32)
constexpr img::Addr kGetVersion = d::iat_KERNEL32_GetVersion;
constexpr img::Addr kGetCommandLineA = d::iat_KERNEL32_GetCommandLineA;
constexpr img::Addr kGetStartupInfoA = d::iat_KERNEL32_GetStartupInfoA;
constexpr img::Addr kGetModuleHandleA = d::iat_KERNEL32_GetModuleHandleA;
constexpr img::Addr kGetModuleFileNameA = d::iat_KERNEL32_GetModuleFileNameA;
constexpr img::Addr kGetEnvironmentStringsW = d::iat_KERNEL32_GetEnvironmentStringsW;
constexpr img::Addr kFreeEnvironmentStringsW = d::iat_KERNEL32_FreeEnvironmentStringsW;
constexpr img::Addr kWideCharToMultiByte = d::iat_KERNEL32_WideCharToMultiByte;

// ---- original CRT entry points (whatever implementation is installed there runs)
void* crt_malloc(u32 n) { return joan::orig::arg::as<void*>(joan::orig::malloc_(joan::orig::arg::u32(n))); }
using joan::orig::helpers::crt_free;
u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
void crt_amsg_exit(i32 code) { joan::orig::libc::_amsg_exit(joan::orig::arg::u32(code)); }
void crt_initmbctable() { joan::orig::libc::__initmbctable(); }
bool mbctype_initialized() { return global<i32>(d::g_mbctype_initialized) != 0; }  // __mbctype_initialized
// _ismbblead macro of the VC6 headers: (_mbctype + 1)[c] & _M1
bool lead_byte(char c) { return ((&img::global<const u8>(d::g_a6cc21))[static_cast<u8>(c)] & 4) != 0; }

// Bytes of one argv / environ entry (a native char*; 4 in the original).
constexpr u32 kPtrSize = static_cast<u32>(sizeof(char*));

// ---- state (original addresses)
constexpr img::Addr kAcmdln = d::g_acmdln;
constexpr img::Addr kAenvptr = d::g_aenvptr;
constexpr img::Addr kEnviron = d::g_environ;
constexpr img::Addr kWenviron = d::g_wenviron;
constexpr img::Addr kEnvInitialized = d::g_envInitialized;
constexpr img::Addr kEnvApiChoice = d::g_envApiChoice;
constexpr img::Addr kAdjustFdiv{0x00a6b1c4u};  // _adjust_fdiv (bss b_a6b1c4, no d:: name)
constexpr img::Addr kArgc = d::g_argc;
constexpr img::Addr kArgv = d::g_argv;
constexpr img::Addr kPgmptr = d::g_pgmptr;
constexpr img::Addr kPgmname = d::g_pgmname;  // char[260]

JOAN_COUNTER(c_startup, "crt::WinMainCRTStartup");
JOAN_COUNTER(c_getenvstrings, "crt::__crtGetEnvironmentStringsA");
JOAN_COUNTER(c_setargv, "crt::_setargv");
JOAN_COUNTER(c_parse_cmdline, "crt::parse_cmdline");
JOAN_COUNTER(c_setenvp, "crt::_setenvp");
JOAN_COUNTER(c_wincmdln, "crt::_wincmdln");
JOAN_COUNTER(c_getenv_lk, "crt::_getenv_lk");

// =====================================================================================================
// crt/startup
// =====================================================================================================

// STARTUPINFOA (0x44 bytes); only dwFlags (+0x2c) and wShowWindow (+0x30) are read.
struct StartupInfoA {
    u32 cb;
    char* lpReserved;
    char* lpDesktop;
    char* lpTitle;
    u32 dwX, dwY, dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute;
    u32 dwFlags;
    u16 wShowWindow;
    u16 cbReserved2;
    u8* lpReserved2;
    void* hStdInput;
    void* hStdOutput;
    void* hStdError;
};
#if UINTPTR_MAX == 0xffffffffu  // x86 (on x64 the pointer members make it Windows' x64 STARTUPINFOA, 0x68 bytes)
static_assert(sizeof(StartupInfoA) == 0x44, "STARTUPINFOA layout");
static_assert(offsetof(StartupInfoA, dwFlags) == 0x2c, "STARTUPINFOA.dwFlags");
static_assert(offsetof(StartupInfoA, wShowWindow) == 0x30, "STARTUPINFOA.wShowWindow");
#endif

// Before the __try (trylevel -1): version globals, heap, multithread core.
void JOAN_CDECL startup_pre() {
    c_startup.hit();
    const u32 v = winapi<u32>(kGetVersion);
    const u32 minor = (v >> 8) & 0xff;
    global<u32>(d::g_winminor) = minor;              // _winminor
    const u32 major = v & 0xff;
    global<u32>(d::g_winmajor) = major;              // _winmajor
    global<u32>(d::g_winver) = (major << 8) + minor;  // _winver
    global<u32>(d::g_osver) = v >> 16;            // _osver
    if (joan::orig::arg::as<i32>(joan::orig::_heap_init(1)) == 0)      // _heap_init(1)
        joan::orig::libc::fast_error_exit(0x1cu);             // fast_error_exit(_RT_HEAPINIT)
    if (joan::orig::arg::as<i32>(joan::orig::libc::_mtinit()) == 0)         // _mtinit()
        joan::orig::libc::fast_error_exit(0x10u);             // fast_error_exit(_RT_THREAD)
}

// The original's __try part (trylevel 0). Ends with exit(), which does not return.
void JOAN_CDECL startup_body() {
    joan::orig::libc::_ioinit();                                          // _ioinit
    global<char*>(kAcmdln) = winapi<char*>(kGetCommandLineA);        // _acmdln
    global<char*>(kAenvptr) = joan::orig::arg::as<char*>(joan::orig::__crt_get_environment_strings_a());         // __crtGetEnvironmentStringsA
    joan::orig::_setargv();                                          // _setargv
    joan::orig::_setenvp();                                          // _setenvp
    joan::orig::libc::_cinit();                                          // _cinit
    StartupInfoA si{};
    si.dwFlags = 0;
    winapi<void>(kGetStartupInfoA, &si);
    char* const cmd = joan::orig::arg::as<char*>(joan::orig::_wincmdln());                 // _wincmdln
    const i32 show = (si.dwFlags & 1) ? static_cast<i32>(si.wShowWindow) : 10;  // STARTF_USESHOWWINDOW / SW_SHOWDEFAULT
    void* const instance = winapi<void*>(kGetModuleHandleA, static_cast<const char*>(nullptr));
    const i32 ret = reinterpret_cast<i32(JOAN_STDCALL*)(void*, void*, char*, i32)>(fn::code_value(fn::WinMain))(  // WinMain 0x00420ea0
        instance, nullptr, cmd, show);
    joan::orig::libc::exit(joan::orig::arg::u32(ret));                                     // exit
}

// 0x007b71e6 WinMainCRTStartup (no SEH frame any more: see the top of this file)
void JOAN_CDECL win_main_crt_startup() {
    startup_pre();
    startup_body();
    std::abort();  // exit() does not return (the original's int3)
}

// =====================================================================================================
// crt/fpinit (family K)
// =====================================================================================================

// 0x007b1d5b _cfltcvt_init
void JOAN_CDECL cfltcvt_init() {
    global<std::uintptr_t>(d::g_tabCropzeros) = fn::code_value(fn::crt_cropzeros);  // _cropzeros
    global<std::uintptr_t>(d::g_tabCfltcvt) = fn::code_value(fn::crt_cfltcvt);  // _cfltcvt
    global<std::uintptr_t>(d::fns_crt_a30fb0) = fn::code_value(fn::crt_fassign);  // _fassign
    global<std::uintptr_t>(d::g_tabForcdecpt) = fn::code_value(fn::crt_forcdecpt);  // _forcdecpt
    global<std::uintptr_t>(d::fns_crt_a30fb8) = fn::code_value(fn::crt_positive);  // _positive
    global<std::uintptr_t>(d::fns_crt_a30fbc) = fn::code_value(fn::crt_cfltcvt);  // _cfltcvt (the _cldcvt slot)
}

// 0x007b8e03 _setdefaultprecision
u32 JOAN_CDECL setdefaultprecision() { return joan::orig::arg::as<u32>(joan::orig::libc::_controlfp(0x10000u, 0x30000u)); }  // _controlfp(_PC_53, _MCW_PC)

// 0x007b8e53 _ms_p5_mp_test_fdiv: no FDIV erratum (see the top of this file).
i32 JOAN_CDECL ms_p5_mp_test_fdiv() { return 0; }

// 0x007b1d43 _fpmath
void JOAN_CDECL fpmath() {
    cdecl_call<void>(fn::cfltcvt_init);                                      // _cfltcvt_init
    global<i32>(kAdjustFdiv) = cdecl_call<i32>(fn::ms_p5_mp_test_fdiv);      // _adjust_fdiv
    cdecl_call<u32>(fn::setdefaultprecision);                                 // _setdefaultprecision
    std::feclearexcept(FE_ALL_EXCEPT);                                        // fnclex
}

// =====================================================================================================
// crt/env (family I)
// =====================================================================================================

// Wide branch of __crtGetEnvironmentStringsA: convert the block (CP_ACP) into a malloc'd copy.
char* env_from_wide(const u16* wenv) {
    const u16* p = wenv;
    if (*p != 0) {
        do {
            ++p;
            while (*p != 0) ++p;
            ++p;
        } while (*p != 0);
    }
    const i32 count = static_cast<i32>(reinterpret_cast<std::uintptr_t>(p) - reinterpret_cast<std::uintptr_t>(wenv)) / 2 + 1;
    char* result = nullptr;
    const i32 size = winapi<i32>(kWideCharToMultiByte, 0u, 0u, wenv, count, static_cast<char*>(nullptr), 0,
                                 static_cast<const char*>(nullptr), static_cast<i32*>(nullptr));
    if (size != 0) {
        char* buf = static_cast<char*>(crt_malloc(static_cast<u32>(size)));
        if (buf) {
            if (winapi<i32>(kWideCharToMultiByte, 0u, 0u, wenv, count, buf, size, static_cast<const char*>(nullptr),
                            static_cast<i32*>(nullptr)) == 0) {
                crt_free(buf);
                buf = nullptr;
            }
            result = buf;
        }
    }
    winapi<i32>(kFreeEnvironmentStringsW, wenv);
    return result;
}

// 0x007c151d __crtGetEnvironmentStringsA (mode 2, the narrow API, is chosen only after GetEnvironmentStringsW failed:
// see the top of this file)
char* JOAN_CDECL crt_get_environment_strings_a() {
    c_getenvstrings.hit();
    const u32 mode = global<u32>(kEnvApiChoice);
    if (mode != 0 && mode != 1) return nullptr;
    u16* const wenv = winapi<u16*>(kGetEnvironmentStringsW);
    if (!wenv) return nullptr;
    if (mode == 0) global<u32>(kEnvApiChoice) = 1;
    return env_from_wide(wenv);
}

// 0x007c1369 parse_cmdline (VC6 stdargv.c, _MBCS)
i32* JOAN_CDECL parse_cmdline(char* cmdstart, char** argv, char* args, i32* numargs, i32* numchars) {
    c_parse_cmdline.hit();
    *numchars = 0;
    *numargs = 1;
    char* p = cmdstart;
    if (argv) *argv++ = args;

    // the program name
    if (*p == '"') {
        while (*++p != '"' && *p != 0) {
            if (lead_byte(*p)) {
                ++*numchars;
                if (args) *args++ = *p++;  // (VC6: p is advanced only when copying)
            }
            ++*numchars;
            if (args) *args++ = *p;
        }
        ++*numchars;
        if (args) *args++ = 0;
        if (*p == '"') ++p;
    } else {
        char c;
        do {
            ++*numchars;
            if (args) *args++ = *p;
            c = *p++;
            if (lead_byte(c)) {
                ++*numchars;
                if (args) *args++ = *p;
                ++p;
            }
        } while (c != ' ' && c != 0 && c != '\t');
        if (c == 0)
            --p;
        else if (args)
            args[-1] = 0;
    }

    // the arguments
    i32 inquote = 0;
    for (;;) {
        if (*p) {
            while (*p == ' ' || *p == '\t') ++p;
        }
        if (*p == 0) break;
        if (argv) *argv++ = args;
        ++*numargs;
        for (;;) {
            i32 copychar = 1;
            u32 numslash = 0;
            while (*p == '\\') {
                ++p;
                ++numslash;
            }
            if (*p == '"') {
                if ((numslash & 1) == 0) {
                    if (inquote) {
                        if (p[1] == '"')
                            ++p;
                        else
                            copychar = 0;
                    } else {
                        copychar = 0;
                    }
                    inquote = inquote == 0;
                }
                numslash >>= 1;
            }
            while (numslash--) {
                if (args) *args++ = '\\';
                ++*numchars;
            }
            if (*p == 0 || (!inquote && (*p == ' ' || *p == '\t'))) break;
            if (copychar) {
                if (args) {
                    if (lead_byte(*p)) {
                        *args++ = *p++;
                        ++*numchars;
                    }
                    *args++ = *p;
                } else if (lead_byte(*p)) {
                    ++p;
                    ++*numchars;
                }
                ++*numchars;
            }
            ++p;
        }
        if (args) *args++ = 0;
        ++*numchars;
    }
    if (argv) *argv = nullptr;
    ++*numargs;
    return numargs;
}

// 0x007c12d0 _setargv
i32 JOAN_CDECL setargv() {
    c_setargv.hit();
    if (!mbctype_initialized()) crt_initmbctable();
    char* const pgmname = &img::global<char>(kPgmname);
    winapi<u32>(kGetModuleFileNameA, static_cast<void*>(nullptr), pgmname, 0x104u);
    char* const acmdln = global<char*>(kAcmdln);
    global<char*>(kPgmptr) = pgmname;
    char* const cmdstart = *acmdln == 0 ? pgmname : acmdln;
    i32 numargs, numchars;
    joan::orig::parse_cmdline(joan::orig::arg::str(cmdstart), joan::orig::arg::mptr(static_cast<char**>(nullptr)), joan::orig::arg::str(static_cast<char*>(nullptr)), &numargs, &numchars);  // parse_cmdline (count)
    // numargs pointers (4 bytes each in the original; native char* here), then the strings
    char** const block = static_cast<char**>(crt_malloc(static_cast<u32>(numchars) + static_cast<u32>(numargs) * kPtrSize));
    if (!block) crt_amsg_exit(8);  // _RT_SPACEARG
    char* const strings = reinterpret_cast<char*>(reinterpret_cast<std::uintptr_t>(block) + static_cast<u32>(numargs) * kPtrSize);
    joan::orig::parse_cmdline(joan::orig::arg::str(cmdstart), joan::orig::arg::mptr(block), joan::orig::arg::str(strings), &numargs, &numchars);  // parse_cmdline (fill)
    global<char**>(kArgv) = block;
    const i32 argc = numargs - 1;
    global<i32>(kArgc) = argc;
    return argc;
}

// 0x007c1217 _setenvp
void JOAN_CDECL setenvp() {
    c_setenvp.hit();
    if (!mbctype_initialized()) crt_initmbctable();
    const char* s = global<char*>(kAenvptr);
    u32 count = 0;
    while (*s != 0) {
        if (*s != '=') ++count;
        s += crt_strlen(s) + 1;
    }
    char** env = static_cast<char**>(crt_malloc((count + 1) * kPtrSize));  // count entries + NULL (4 bytes each in the original)
    global<char**>(kEnviron) = env;
    if (!env) crt_amsg_exit(9);  // _RT_SPACEENV
    const char* e = global<char*>(kAenvptr);
    while (*e != 0) {
        const u32 len = crt_strlen(e) + 1;
        if (*e != '=') {
            *env = static_cast<char*>(crt_malloc(len));
            if (!*env) crt_amsg_exit(9);
            crt_strcpy(*env, e);
            ++env;
        }
        e += len;
    }
    crt_free(global<char*>(kAenvptr));
    global<char*>(kAenvptr) = nullptr;
    *env = nullptr;
    global<i32>(kEnvInitialized) = 1;
}

// 0x007c11bf _wincmdln
char* JOAN_CDECL wincmdln() {
    c_wincmdln.hit();
    if (!mbctype_initialized()) crt_initmbctable();
    const u8* p = global<const u8*>(kAcmdln);
    if (*p == '"') {
        for (;;) {
            const u8 c = *++p;
            if (c == '"' || c == 0) break;
            if (joan::orig::arg::as<i32>(joan::orig::libc::_ismbblead(joan::orig::arg::u32(static_cast<i32>(c)))) != 0) ++p;  // _ismbblead
        }
        if (*p == '"') ++p;
    } else {
        while (*p > ' ') ++p;
    }
    while (*p != 0 && *p <= ' ') ++p;
    return const_cast<char*>(reinterpret_cast<const char*>(p));
}

// 0x007b7ed4 _getenv_lk
char* JOAN_CDECL getenv_lk(const char* option) {
    c_getenv_lk.hit();
    if (global<i32>(kEnvInitialized) == 0) return nullptr;
    char** search = global<char**>(kEnviron);
    if (!search) {
        if (global<void*>(kWenviron) == nullptr) return nullptr;
        if (joan::orig::arg::as<i32>(joan::orig::libc::__wtomb_environ()) != 0) return nullptr;  // __wtomb_environ
        search = global<char**>(kEnviron);
        if (!search) return nullptr;
    }
    if (!option) return nullptr;
    const u32 length = crt_strlen(option);
    for (; *search; ++search) {
        if (crt_strlen(*search) > length && (*search)[length] == '=' &&
            joan::orig::arg::as<i32>(joan::orig::libc::_mbsnbicoll(joan::orig::arg::mptr(*search), joan::orig::arg::mptr(option), joan::orig::arg::u32(length))) == 0)  // _mbsnbicoll
            return *search + length + 1;
    }
    return nullptr;
}

// 0x007b7eb3 getenv
char* JOAN_CDECL crt_getenv(const char* option) {
    joan::orig::libc::_lock(0xc);                                  // _lock(_ENV_LOCK)
    char* const result = joan::orig::arg::as<char*>(joan::orig::_getenv_lk(joan::orig::arg::str(option)));   // _getenv_lk
    joan::orig::libc::_unlock(0xc);                                  // _unlock(_ENV_LOCK)
    return result;
}

}  // namespace

JOAN_REPLACE("crt/startup", "WinMainCRTStartup", 0x007b71e6, &win_main_crt_startup);

JOAN_REPLACE("crt/fpinit", "_fpmath", 0x007b1d43, &fpmath);
JOAN_REPLACE("crt/fpinit", "_cfltcvt_init", 0x007b1d5b, &cfltcvt_init);
JOAN_REPLACE("crt/fpinit", "_setdefaultprecision", 0x007b8e03, &setdefaultprecision);
JOAN_REPLACE("crt/fpinit", "_ms_p5_mp_test_fdiv", 0x007b8e53, &ms_p5_mp_test_fdiv);

JOAN_REPLACE("crt/env", "getenv", 0x007b7eb3, &crt_getenv);
JOAN_REPLACE("crt/env", "_getenv_lk", 0x007b7ed4, &getenv_lk);
JOAN_REPLACE("crt/env", "_wincmdln", 0x007c11bf, &wincmdln);
JOAN_REPLACE("crt/env", "_setenvp", 0x007c1217, &setenvp);
JOAN_REPLACE("crt/env", "_setargv", 0x007c12d0, &setargv);
JOAN_REPLACE("crt/env", "parse_cmdline", 0x007c1369, &parse_cmdline);
JOAN_REPLACE("crt/env", "__crtGetEnvironmentStringsA", 0x007c151d, &crt_get_environment_strings_a);

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::_getenv_lk(const char* a0) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::getenv_lk>(a0)); }  // 0x7b7ed4
auto joan::orig::_wincmdln() -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::wincmdln>()); }  // 0x7c11bf
auto joan::orig::_setenvp() -> void { joan::bind::free<&::setenvp>(); }  // 0x7c1217
auto joan::orig::_setargv() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::setargv>()); }  // 0x7c12d0
auto joan::orig::parse_cmdline(const char* a0, void* a1, const char* a2, void* a3, void* a4) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::parse_cmdline>(a0, a1, a2, a3, a4)); }  // 0x7c1369
auto joan::orig::__crt_get_environment_strings_a() -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_get_environment_strings_a>()); }  // 0x7c151d
