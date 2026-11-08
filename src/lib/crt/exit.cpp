// Original library sources (re/attribution.csv original_file, under lib/): crt/abort.c, crt/crt0.c, crt/crt0dat.c,
//   crt/crt0msg.c, crt/crtmbox.c, crt/onexit.c, crt/purevirt.c, crt/unhandld.cpp, crt/winsig.c, crt/winxfltr.c.
//
// Batch 114 / list 02: MSVC 6 CRT family "exit" (CRT_SYSTEM.md families D + J): the onexit / atexit table, the
// exit paths, the initialiser-table walker, the runtime-error reporting, signals and the exception filters.
// Module crt/exit (one switch for the whole family; disable it only as a unit: [Hooks] Disable=crt/exit).
//
// Surface replaced (address, VC6 name, signature; all cdecl unless noted) and every caller found
// (joan.asm direct calls + data references + src/modules address constants):
//   0x007b359c _onexit_t _onexit(_onexit_t)        <- atexit (only)
//   0x007b361a int atexit(void (*)(void))          <- 262 game static-init sites, ~64 hook C++ sites (cdecl_call)
//   0x007b362c void __onexitinit(void)             <- .CRT$XI slot 0x009f4d90 (run by _cinit)
//   0x007b47cf void _cinit(void)                   <- WinMainCRTStartup 0x007b728b (crt/startup family)
//   0x007b47fc void exit(int)                      <- game 0x582e03 0x586a9e 0x586b81 0x586d5e 0x5a0410, libjpeg
//                                                     error_exit 0x765e47, startup 0x7b72ca, hook C++ (b108_09 ...)
//   0x007b480d void _exit(int)                     <- _threadstart handler 0x7b44e8, startup handler 0x7b72e9,
//                                                     raise 0x7c1c9e, abort 0x7c219e, pointer _aexit_rtn [0x00a30cbc]
//   0x007b481e void _cexit(void)                   <- none (kept for completeness of the family)
//   0x007b482d void _c_exit(void)                  <- none
//   0x007b483c void doexit(int code, int quick, int retcaller)   <- the four wrappers above
//   0x007b48f3 void _initterm(_PVFV* begin, _PVFV* end)          <- _cinit, doexit
//   0x007b70f4 void _purecall(void)                <- 12 vtable slots (0x9da6f8.., 0x9da950.., 0x9da98c.., 0x9dcaa4..)
//   0x007b72ee void _amsg_exit(int)                <- 13 CRT sites (heap / mt / stdio / lowio / env / onexit init,
//                                                     _fptrap 0x7c27c2, _purecall)
//   0x007b7313 void fast_error_exit(int)           <- WinMainCRTStartup 0x7b724b / 0x7b725c
//   0x007c164f void _FF_MSGBANNER(void)            <- _amsg_exit, fast_error_exit
//   0x007c1688 void _NMSG_WRITE(int)               <- _amsg_exit, fast_error_exit, _FF_MSGBANNER, abort 0x7c218e
//   0x007c4b11 int __crtMessageBoxA(LPCSTR, LPCSTR, UINT)        <- _NMSG_WRITE
//   0x007c218c void abort(void)                    <- terminate 0x7b8d90 (jmp; C++ EH runtime, family L).
//              Not named in the task list but it is the exit-family tail of terminate() and calls only this
//              family (_NMSG_WRITE / raise / _exit); no other batch-114 list claims it.
//   0x007c19f2 _PHNDLR signal(int, _PHNDLR)        <- none in the image (no call, no data reference)
//   0x007c1bd6 int raise(int)                      <- abort 0x7c2195
//   0x007c1d58 struct _XCPT_ACTION* siglookup(int signum, struct _XCPT_ACTION* table)   <- signal, raise
//   0x007bd2f9 int _XcptFilter(unsigned long code, struct _EXCEPTION_POINTERS*)
//                                                  <- startup filter 0x7b72db, _threadstart filter 0x7b44da
//   0x007bd437 struct _XCPT_ACTION* xcptlookup(unsigned long code, struct _XCPT_ACTION* table) <- _XcptFilter
//   0x007c115b LONG __stdcall __CxxUnhandledExceptionFilter(EXCEPTION_POINTERS*) (ret 4)
//                                                  <- only as the pointer installed by 0x007c11a1
//   0x007c11a1 void __CxxSetUnhandledExceptionFilter(void)       <- .CRT$XI slot 0x009f4ea4
//   0x007c11b2 void __CxxRestoreUnhandledExceptionFilter(void)   <- .CRT$XT slot 0x009f54bc
// Left original (dead once this module is on): _lockexit 0x007b48e1 / _unlockexit 0x007b48ea (only callers are
// _onexit / doexit: we call _lock(13) / _unlock(13) at their addresses directly), ctrlevent_capture 0x007c1b74
// (only reference: the SetConsoleCtrlHandler argument in signal; our signal passes that original address, as the
// original does - it only uses _lock(1) / _unlock(1) and the action slots at their original addresses),
// _GET_RTERRMSG 0x007c17db (no caller). The startup's own SEH filter / handler 0x7b72cf / 0x7b72e3 belong to
// WinMainCRTStartup (crt/startup).
//
// State: kept at the ORIGINAL addresses with the original layout, so that the family can be switched on and off as
// a unit (and single functions disabled by name) and so that the remaining original readers still see it:
//   __onexitbegin 0x00a6cd40 / __onexitend 0x00a6cd3c (heap block; readers: only this family),
//   _exitflag (byte) 0x00a6b218 (read by __endstdio 0x7bcdcc, crt/stdio), _C_Termination_Done 0x00a6b21c,
//   _C_Exit_Done 0x00a6b220, _FPinit 0x00a30948 (-> _fpmath, crt/startup K), _aexit_rtn 0x00a30cbc (-> _exit),
//   __app_type 0x00a30cc0 (= 2, also read by lowio 0x7c3307 / 0x7c338a), __error_mode 0x00a6b248, _adbgmsg
//   0x00a6b544 (never written: 0), __crtMessageBoxA's user32 cache 0x00a6b570 / 74 / 78 (unused since P2), signal actions
//   ctrlc 0x00a6b548 / ctrlbreak 0x00a6b54c / abort 0x00a6b550 / term 0x00a6b554, ConsoleCtrlHandler_Installed
//   0x00a6b558, _XcptActTab 0x00a31720 (10 x {XcptNum, SigNum, XcptAction}) with _First_FPE_Indx 0x00a31798,
//   _Num_FPE 0x00a3179c, _XcptActTabSize 0x00a317a0, _XcptActTabCount 0x00a317a4 (the ptd's +0x50 points at it or
//   at a malloc'd copy: crt/mt's _initptd writes it, _freeptd frees the copy), previous unhandled-exception filter
//   0x00a6b438, the init tables XI 0x009f4c8c..0x009f4fa8, XC 0x009f4000..0x009f4b88, XP 0x009f50ac..0x009f52b4,
//   XT 0x009f53b8..0x009f55c0 (entries are read live and called; the XI / XP / XT entries of other families
//   (__initmbctable, __inittime, __initstdio, __endstdio) are called at their addresses and chain to whatever
//   implementation is installed there). The per-thread data (ptd, VC6 0x74 layout; struct Ptd below) is reached
//   only through _getptd 0x007b8c21: +0x50 _pxcptacttab, +0x54 _tpxcptinfoptrs, +0x58 _tfpecode.
//
// Calls to other families go through the original CRT addresses (CRT_SYSTEM.md 5.1 rule 4): _lock 0x7b82aa /
// _unlock 0x7b830b (crt/mt), _getptd 0x7b8c21, _errno 0x7bbfb5, __doserrno 0x7bbfbe, malloc 0x7b6c54, realloc
// 0x7bc089, _msize 0x7bc1c1 (crt/heap), strlen 0x7b1ef0, strcpy 0x7b2020, strcat 0x7b2030, strncpy 0x7b1db0, memcpy
// 0x7b2a20, terminate 0x7b8d34 and _ValidateExecute 0x7c2174 (EH runtime). Calls inside this family also go
// through the original addresses (so a function disabled by name falls back to the original consistently). Win32
// calls go through joan.exe's IAT slots, as the original (openjoan.exe's loader wraps ExitProcess 0x00a6d9b0 to run
// host::shutdown; the other slots are the plain kernel32 functions).
//
// Exactness notes:
//  * _onexit: same call order (_msize(begin), _msize(begin), realloc(begin, size + 16)), same growth rule (grow
//    when _msize(begin) < end - begin + 4, unsigned), new end = new begin + ((int)(end - begin) >> 2) * 4, func
//    stored at the old/new end, end += 4, result = func or NULL when realloc fails; all under _lock(13).
//  * doexit: _lock(13); re-entry (_C_Exit_Done == 1) -> TerminateProcess(GetCurrentProcess(), code);
//    _C_Termination_Done = 1; _exitflag = (char)retcaller; unless quick: onexit table backwards from end - 4 (end
//    read once, begin re-read before every step, unsigned compare, NULL entries skipped), then XP; always XT; then
//    either _unlock(13) + return (retcaller) or _C_Exit_Done = 1 + ExitProcess(code). The lock stays held on the
//    exit path, as in the original.
//  * _NMSG_WRITE: the message lookup reads the 8-byte entries 0x00a32018..0x00a320a8 and, for an unknown code, the
//    dword right after the table (0x00a320a8, value 0x400) exactly as the original; the two stack buffers keep the
//    original sizes and order (program name 0x104 bytes, then the 0xa0-byte message; longest message 139 bytes).
//    The original passes its own argument slot as WriteFile's lpNumberOfBytesWritten (the stderr path) and
//    _XcptFilter overwrites its first argument slot with the handler: both slots are dead for every caller (they
//    push a copy / an immediate and pop it), so ours write a local instead.
//  * _XcptFilter / raise / signal: the FPE loops re-read _First_FPE_Indx / _Num_FPE and ptd->_pxcptacttab on every
//    iteration (signed compare), table bounds are computed with 32-bit wrap like the lea sequences; raise reads
//    the handler before taking lock 1, signal's SetConsoleCtrlHandler result must be exactly TRUE (1).
//  * raise on a ptd signal whose siglookup returns NULL reads address 8 (VC6 bug; cannot happen with the
//    constant table) - reproduced as a plain read of address 8.
//  * __CxxUnhandledExceptionFilter: a VC6 C++ exception (0xE06D7363, 3 parameters, magic 0x19930520) ->
//    terminate() (the original tail-jumps with one extra dword on the stack; terminate never returns); otherwise
//    the previous filter if non-NULL and _ValidateExecute(prev) (IsBadCodePtr) says it is code, else 0.
//    The setter installs the filter by its original address 0x007c115b (entry-patched to ours), so the pointer the
//    OS holds and the [0x00a6b438] chain are identical to the original's.
//  * No x87 / register contract: none of these functions touches the FPU; _cinit relies on _fpmath (via the live
//    _FPinit pointer) to set CW 0x027F before XI / XC, exactly as the original.
#include "../../callconv.hpp"
#include "../../platform/win32_types.hpp"
#include <cstddef>
#include <cstdint>

#include "crt.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/direct.hpp"
#include "../../original/bind.hpp"
#if defined(_M_X64) || defined(__x86_64__)
#include "../../compiler.hpp"  // JOAN_RETURN_ADDRESS

#include "../../runtime/crash.hpp"  // describe_address
#endif

namespace {
using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

JOAN_COUNTER(c_onexit, "crt::_onexit");
JOAN_COUNTER(c_doexit, "crt::doexit");
JOAN_COUNTER(c_initterm, "crt::_initterm");
JOAN_COUNTER(c_nmsg, "crt::_NMSG_WRITE");
JOAN_COUNTER(c_xcpt, "crt::_XcptFilter");
JOAN_COUNTER(c_raise, "crt::raise");
JOAN_COUNTER(c_signal, "crt::signal");
JOAN_COUNTER(c_cxxfilter, "crt::__CxxUnhandledExceptionFilter");

using joan::call::cdecl_call;
// Win32 call through one of joan.exe's import address table slots (the slot is read at the call).
template <typename R, typename... A>
R api(img::Addr slot, A... args) {
    return (*reinterpret_cast<R(JOAN_STDCALL* volatile*)(A...)>(img::addr(slot)))(args...);
}
using uptr = std::uintptr_t;
inline volatile u32& g32(uptr addr) { return *reinterpret_cast<volatile u32*>(addr); }
inline volatile u8& g8(img::Addr a) { return img::global<volatile u8>(a); }
inline volatile u32& g32(img::Addr a) { return img::global<volatile u32>(a); }

// ---- original addresses -------------------------------------------------------------------------------------
// this family
constexpr fn::Code kOnexit = fn::crt_onexit;
constexpr fn::Code kDoexit = fn::crt_doexit;
constexpr fn::Code kExit_ = fn::crt_exit_7b480d;  // _exit
constexpr fn::Code kInitterm = fn::crt_initterm;
constexpr fn::Code kAmsgExit = fn::crt_amsg_exit;
constexpr fn::Code kMsgBanner = fn::crt_FF_MSGBANNER;
constexpr fn::Code kNmsgWrite = fn::crt_NMSG_WRITE;
constexpr fn::Code kMessageBox = fn::crt_crtMessageBoxA;
constexpr fn::Code kRaise = fn::crt_raise;
constexpr fn::Code kSiglookup = fn::crt_siglookup;
constexpr fn::Code kXcptlookup = fn::crt_xcptlookup;
constexpr fn::Code kCxxFilter = fn::crt_CxxUnhandledExceptionFilter;
constexpr fn::Code kCtrlEventCapture = fn::crt_ctrlevent_capture;  // original HandlerRoutine (left original)
// other families
constexpr fn::Code kLock = fn::crt_lock;
constexpr fn::Code kUnlock = fn::crt_unlock;
constexpr fn::Code kGetptd = fn::crt_getptd;
constexpr fn::Code kErrno = fn::crt_errno;
constexpr fn::Code kDoserrno = fn::crt_doserrno;
constexpr fn::Code kMalloc = fn::malloc;
constexpr fn::Code kRealloc = fn::realloc;
constexpr fn::Code kMsize = fn::msize;
constexpr fn::Code kStrlen = fn::crt_strlen;
constexpr fn::Code kStrcpy = fn::crt_strcpy;
constexpr fn::Code kStrcat = fn::crt_strcat;
constexpr fn::Code kStrncpy = fn::crt_strncpy;
constexpr fn::Code kMemcpy = fn::crt_memcpy_7b2a20;
constexpr fn::Code kTerminate = fn::crt_terminate;
constexpr fn::Code kValidateExecute = fn::ValidateExecute;
// IAT slots (KERNEL32)
constexpr img::Addr kWriteFile = d::iat_KERNEL32_WriteFile;
constexpr img::Addr kGetModuleFileNameA = d::iat_KERNEL32_GetModuleFileNameA;
constexpr img::Addr kTerminateProcess = d::iat_KERNEL32_TerminateProcess;
constexpr img::Addr kExitProcess = d::iat_KERNEL32_ExitProcess;
constexpr img::Addr kGetCurrentProcess = d::iat_KERNEL32_GetCurrentProcess;
constexpr img::Addr kGetLastError = d::iat_KERNEL32_GetLastError;
constexpr img::Addr kSetConsoleCtrlHandler = d::iat_KERNEL32_SetConsoleCtrlHandler;
constexpr img::Addr kSetUnhandledExceptionFilter = d::iat_KERNEL32_SetUnhandledExceptionFilter;
constexpr img::Addr kGetStdHandle = d::iat_KERNEL32_GetStdHandle;
constexpr img::Addr kUnhandledExceptionFilter = d::iat_KERNEL32_UnhandledExceptionFilter;
constexpr img::Addr kMessageBoxA = d::iat_USER32_MessageBoxA;  // joan.exe's own import (the game's message boxes)
// state
constexpr img::Addr kOnexitBegin = d::g_onexitBegin;
constexpr img::Addr kOnexitEnd = d::g_onexitEnd;
constexpr img::Addr kExitflag = d::g_exitflag;          // char
constexpr img::Addr kCTerminationDone = d::g_cTerminationDone;
constexpr img::Addr kCExitDone = d::g_cExitDone;
constexpr img::Addr kFPinit = d::g_fPinit;
constexpr img::Addr kAexitRtn = d::g_aexitRtn;
constexpr img::Addr kAppType = d::g_appType;
constexpr img::Addr kErrorMode = d::g_errorMode;
constexpr img::Addr kAdbgmsg = d::g_adbgmsg;
constexpr img::Addr kCtrlcAction = d::g_ctrlcAction;
constexpr img::Addr kCtrlbreakAction = d::g_ctrlbreakAction;
constexpr img::Addr kAbortAction = d::g_abortAction;
constexpr img::Addr kTermAction = d::g_termAction;
constexpr img::Addr kCtrlHandlerInstalled = d::g_ctrlHandlerInstalled;
constexpr img::Addr kXcptActTab = d::g_xcptActTab;
constexpr img::Addr kFirstFpeIndx = d::g_firstFpeIndx;
constexpr img::Addr kNumFpe = d::g_numFpe;
constexpr img::Addr kXcptActTabSize = d::g_xcptActTabSize;
constexpr img::Addr kXcptActTabCount = d::g_xcptActTabCount;
constexpr img::Addr kPrevFilter = d::g_prevFilter;
constexpr img::Addr kRtErrTab = d::g_rtErrTab;  // {int code; const char* msg} x 18, end 0x00a320a8
constexpr img::Addr kRtErrTabEnd = d::g_rtErrTabEnd;
// init tables
constexpr img::Addr kXiA = d::g_xiA, kXiZ = d::g_xiZ;
constexpr img::Addr kXcA = d::g_xcA, kXcZ = d::g_xcZ;
constexpr img::Addr kXpA = d::g_xpA, kXpZ = d::g_xpZ;
constexpr img::Addr kXtA = d::g_xtA, kXtZ = d::g_xtZ;
// the runtime-error box (.rdata 0x009e7ccc, 0x009e7cb0, 0x009e7c88)
const char* const kStrUnknownProgram = "<program name unknown>";
const char* const kStrRuntimeError = "Runtime Error!\n\nProgram: ";
const char* const kStrTitle = "Microsoft Visual C++ Runtime Library";
constexpr img::Addr kStrDots = d::s_dot_dot_dot;            // "..."
constexpr img::Addr kStrNewlines = d::kStrNewlines;        // "\n\n"

constexpr u32 kSigInt = 2, kSigIll = 4, kSigFpe = 8, kSigSegv = 11, kSigTerm = 15, kSigBreak = 21, kSigAbrt = 22;
constexpr u32 kSigDfl = 0, kSigIgn = 1, kSigSge = 3, kSigAck = 4, kSigDie = 5;
constexpr u32 kEinval = 0x16;

inline void lock(u32 n) { cdecl_call<void>(kLock, n); }
inline void unlock(u32 n) { cdecl_call<void>(kUnlock, n); }
// The leading part of VC6 _tiddata as crt/mt allocates it (lib/crt/mt.cpp Tiddata, the same members: natural
// alignment, pointers widen on x64; keep the two in step). x86 offsets: +0x50 _pxcptacttab, +0x54 _tpxcptinfoptrs,
// +0x58 _tfpecode.
struct Ptd {
    u32 tid;
    uptr thandle;
    int terrno;
    u32 tdoserrno;  // VC6 unsigned long = 32 bits; a fixed width keeps the layout on LP64 Linux (long is 8 bytes there)
    u32 fpds, holdrand;
    char* token;
    WCHAR* wtoken;
    u8* mtoken;
    char* errmsg;
    char* namebuf0;
    WCHAR* wnamebuf0;
    char* namebuf1;
    WCHAR* wnamebuf1;
    char* asctimebuf;
    WCHAR* wasctimebuf;
    void* gmtimebuf;
    char* cvtbuf;
    void* initaddr;
    void* initarg;
    uptr pxcptacttab;      // _XCPT_ACTION table (x86 layout, 12-byte entries: the image's or a malloc'd copy)
    void* tpxcptinfoptrs;  // EXCEPTION_POINTERS*
    u32 tfpecode;
};
static_assert(sizeof(void*) != 4 || (offsetof(Ptd, pxcptacttab) == 0x50 && offsetof(Ptd, tpxcptinfoptrs) == 0x54 &&
                                     offsetof(Ptd, tfpecode) == 0x58),
              "VC6 _tiddata");
static_assert(sizeof(void*) != 8 || (offsetof(Ptd, gmtimebuf) == 0x70 && offsetof(Ptd, pxcptacttab) == 0x90 &&
                                     offsetof(Ptd, tfpecode) == 0xa0),
              "_tiddata x64 (lib/crt/mt.cpp Tiddata, lib/crt/time.cpp gmtimebuf)");
inline volatile Ptd* getptd() { return cdecl_call<volatile Ptd*>(kGetptd); }
inline void call_void(uptr fn) { reinterpret_cast<void(JOAN_CDECL*)()>(fn)(); }

// ------------------------------------------------------------------ 0x007b359c _onexit
// The table is a heap array of code addresses (4 bytes each in the original, native pointer width here: kPtrSize);
// __onexitbegin / __onexitend are pointer slots of the image. Readers: only this family.
constexpr u32 kPtrSize = static_cast<u32>(sizeof(uptr));
constexpr int kPtrShift = sizeof(uptr) == 4 ? 2 : 3;
inline volatile uptr& gp(img::Addr a) { return img::global<volatile uptr>(a); }

uptr JOAN_CDECL crt_onexit(uptr func) {
    c_onexit.hit();
    lock(0xd);  // _lockexit 0x7b48e1
    const u32 size = cdecl_call<u32>(kMsize, gp(kOnexitBegin));
    const uptr begin = gp(kOnexitBegin);
    uptr slot = gp(kOnexitEnd);
    uptr result;
    if (size < slot - begin + kPtrSize) {
        const u32 want = cdecl_call<u32>(kMsize, begin) + 4 * kPtrSize;  // 4 more entries (0x10 bytes on x86)
        const uptr p = cdecl_call<uptr>(kRealloc, gp(kOnexitBegin), want);
        if (p == 0) {
            result = 0;
            unlock(0xd);  // _unlockexit 0x7b48ea
            return result;
        }
        const uptr used = gp(kOnexitEnd) - gp(kOnexitBegin);
        gp(kOnexitBegin) = p;
        slot = p + (static_cast<uptr>(static_cast<std::intptr_t>(used) >> kPtrShift) << kPtrShift);  // sar 2, * 4
        gp(kOnexitEnd) = slot;
    }
    *reinterpret_cast<uptr*>(slot) = func;
    gp(kOnexitEnd) = gp(kOnexitEnd) + kPtrSize;
    result = func;
    unlock(0xd);
    return result;
}

// ------------------------------------------------------------------ 0x007b361a atexit
int JOAN_CDECL crt_atexit(uptr func) { return cdecl_call<uptr>(kOnexit, func) == 0 ? -1 : 0; }

// ------------------------------------------------------------------ 0x007b362c __onexitinit
void JOAN_CDECL crt_onexitinit() {
    uptr p = cdecl_call<uptr>(kMalloc, 32 * kPtrSize);  // 32 entries (0x80 bytes on x86)
    gp(kOnexitBegin) = p;
    if (p == 0) {
        cdecl_call<void>(kAmsgExit, 0x18);  // _RT_ONEXIT (does not return)
        p = gp(kOnexitBegin);
    }
    *reinterpret_cast<uptr*>(p) = 0;
    gp(kOnexitEnd) = gp(kOnexitBegin);
}

// ------------------------------------------------------------------ 0x007b48f3 _initterm
// The tables are image data walked in dwords. x64: a non-null entry is a widened pointer (8 bytes), a null entry stays
// one dword - every function is still called once (the x64 walk reads each slot at its translated width).
void JOAN_CDECL crt_initterm(std::uintptr_t begin, std::uintptr_t end) {
    c_initterm.hit();
#if JOAN_IMAGE_X64
    // The table is walked by ORIGINAL address (4-byte steps) through the image's translation: a slot the layout
    // widened is a whole code pointer (an address above 4 GB under PIE), any other slot (null) one dword.
    if (img::in_image(begin) && img::in_image(end)) {
        const std::uint32_t va_end = img::original_of(static_cast<std::uint32_t>(end - img::base()));
        for (std::uint32_t va = img::original_of(static_cast<std::uint32_t>(begin - img::base())); va < va_end;
             va += 4) {
            const std::uint32_t off = img::offset_of(va);
            const uptr slot = img::base() + off;
            const uptr fn = img::offset_of(va + 4) - off == sizeof(uptr) ? *reinterpret_cast<volatile uptr*>(slot)
                                                                         : *reinterpret_cast<volatile u32*>(slot);
            if (fn != 0) call_void(fn);
        }
        return;
    }
#endif
    for (std::uintptr_t p = begin; p < end; p += 4) {
        const u32 fn = *reinterpret_cast<volatile u32*>(p);
        if (fn != 0) call_void(fn);
    }
}

// ------------------------------------------------------------------ 0x007b47cf _cinit
void JOAN_CDECL crt_cinit() {
    const uptr fpinit = gp(kFPinit);  // _fpmath 0x7b1d43: _cfltcvt_init, FDIV test, CW 0x027F, fnclex
    if (fpinit != 0) call_void(fpinit);
    cdecl_call<void>(kInitterm, img::addr(kXiA), img::addr(kXiZ));
    cdecl_call<void>(kInitterm, img::addr(kXcA), img::addr(kXcZ));
}

// ------------------------------------------------------------------ 0x007b483c doexit
void JOAN_CDECL crt_doexit(u32 code, u32 quick, u32 retcaller) {
    c_doexit.hit();
    lock(0xd);  // _lockexit
    if (g32(kCExitDone) == 1) {
        void* const process = api<void*>(kGetCurrentProcess);  // the pseudo handle (HANDLE)-1: pointer-wide on x64
        api<u32>(kTerminateProcess, process, code);
    }
    g32(kCTerminationDone) = 1;
    g8(kExitflag) = static_cast<u8>(retcaller);
    if (quick == 0) {
        if (gp(kOnexitBegin) != 0) {
            for (uptr p = gp(kOnexitEnd) - kPtrSize; p >= gp(kOnexitBegin); p -= kPtrSize) {
                const uptr fn = *reinterpret_cast<volatile uptr*>(p);
                if (fn != 0) call_void(fn);
            }
        }
        cdecl_call<void>(kInitterm, img::addr(kXpA), img::addr(kXpZ));  // __endstdio 0x7bcdcc
    }
    cdecl_call<void>(kInitterm, img::addr(kXtA), img::addr(kXtZ));  // restore the unhandled-exception filter 0x7c11b2
    if (retcaller != 0) {
        unlock(0xd);  // _unlockexit
        return;
    }
    g32(kCExitDone) = 1;
    api<void>(kExitProcess, code);
}

// ------------------------------------------------------------------ 0x007b47fc exit / _exit / _cexit / _c_exit
#if defined(_M_X64) || defined(__x86_64__)
// x64 diagnosis: which game code ends the process (openjoan.log, before the end-of-run reports).
void JOAN_CDECL crt_exit(u32 code) {
    joan::hook::log_line("exit(%d) called from %s", static_cast<int>(code),
                         joan::hook::describe_address(reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS())).c_str());
    cdecl_call<void>(kDoexit, code, 0u, 0u);
}
void JOAN_CDECL crt__exit(u32 code) {
    joan::hook::log_line("_exit(%d) called from %s", static_cast<int>(code),
                         joan::hook::describe_address(reinterpret_cast<std::uintptr_t>(JOAN_RETURN_ADDRESS())).c_str());
    cdecl_call<void>(kDoexit, code, 1u, 0u);
}
#else
void JOAN_CDECL crt_exit(u32 code) { cdecl_call<void>(kDoexit, code, 0u, 0u); }
void JOAN_CDECL crt__exit(u32 code) { cdecl_call<void>(kDoexit, code, 1u, 0u); }
#endif
void JOAN_CDECL crt_cexit() { cdecl_call<void>(kDoexit, 0u, 0u, 1u); }
void JOAN_CDECL crt_c_exit() { cdecl_call<void>(kDoexit, 0u, 1u, 1u); }

// ------------------------------------------------------------------ 0x007c4b11 __crtMessageBoxA
// The original loads user32.dll (LoadLibraryA / GetProcAddress, cached in 0x00a6b570 / 74 / 78) and owns the box by
// GetLastActivePopup(GetActiveWindow()). Port round P2: joan.exe's own MessageBoxA import (platform/sdl_window.cpp's
// SDL message box), without an owner window; LoadLibraryA / GetProcAddress are no longer called, the cache stays 0.
int JOAN_CDECL crt_message_box(const char* text, const char* caption, u32 type) {
    return api<int>(kMessageBoxA, static_cast<void*>(nullptr), text, caption, type);
}

// ------------------------------------------------------------------ 0x007c1688 _NMSG_WRITE
void JOAN_CDECL crt_nmsg_write(u32 code) {
    c_nmsg.hit();
    // The table is walked by original address (x64: its message pointers widen the 8-byte entries; the image maps
    // every original address of an entry to its place).
    u32 i = 0;
    for (img::Addr e = kRtErrTab; code != g32(e);) {
        e += 8;
        ++i;
        // The original's signed `jl` compares two addresses of the same table: unsigned here, the same result for
        // any image base (a signed compare would break above 0x80000000).
        if (!(e < kRtErrTabEnd)) break;  // jl
    }
    const img::Addr entry = kRtErrTab + i * 8;  // i == 18 reads the dword after the table, as the original
    if (code != g32(entry)) return;
    const u32 mode = g32(kErrorMode);
    if (mode == 1 || (mode == 0 && g32(kAppType) == 1)) {
        const char* const msg = img::global<const char* volatile>(entry + 4);
        const u32 len = cdecl_call<u32>(kStrlen, msg);
        void* const err = api<void*>(kGetStdHandle, static_cast<u32>(-12));
        u32 written = 0;  // the original uses its (dead) argument slot
        api<u32>(kWriteFile, err, img::global<const char* volatile>(entry + 4), len, &written, static_cast<void*>(nullptr));
        return;
    }
    if (code == 0xfc) return;
    struct {
        char prog[0x104];  // ebp-0x1a4
        char out[0xa0];    // ebp-0xa0
    } buf;
    char* const prog = buf.prog;
    char* const out = buf.out;
    if (api<u32>(kGetModuleFileNameA, static_cast<void*>(nullptr), prog, 0x104u) == 0)
        cdecl_call<char*>(kStrcpy, prog, kStrUnknownProgram);
    char* name = prog;
    if (cdecl_call<u32>(kStrlen, prog) + 1 > 0x3c) {
        name = prog + cdecl_call<u32>(kStrlen, prog) - 0x3b;
        cdecl_call<char*>(kStrncpy, name, img::str(kStrDots), 3u);
    }
    cdecl_call<char*>(kStrcpy, out, kStrRuntimeError);
    cdecl_call<char*>(kStrcat, out, name);
    cdecl_call<char*>(kStrcat, out, img::str(kStrNewlines));
    cdecl_call<char*>(kStrcat, out, img::global<const char* volatile>(entry + 4));
    cdecl_call<int>(kMessageBox, out, kStrTitle, 0x12010u);  // MB_OK|MB_ICONHAND|MB_TASKMODAL|MB_SETFOREGROUND
}

// ------------------------------------------------------------------ 0x007c164f _FF_MSGBANNER
void JOAN_CDECL crt_ff_msgbanner() {
    const u32 mode = g32(kErrorMode);
    if (mode == 1 || (mode == 0 && g32(kAppType) == 1)) {
        cdecl_call<void>(kNmsgWrite, 0xfcu);  // _RT_CRNL
        const uptr dbg = gp(kAdbgmsg);
        if (dbg != 0) call_void(dbg);
        cdecl_call<void>(kNmsgWrite, 0xffu);  // _RT_BANNER
    }
}

// ------------------------------------------------------------------ 0x007b72ee _amsg_exit
void JOAN_CDECL crt_amsg_exit(u32 code) {
    if (g32(kErrorMode) == 1) cdecl_call<void>(kMsgBanner);
    cdecl_call<void>(kNmsgWrite, code);
    reinterpret_cast<void(JOAN_CDECL*)(u32)>(gp(kAexitRtn))(0xffu);  // _exit 0x7b480d
}

// ------------------------------------------------------------------ 0x007b7313 fast_error_exit
void JOAN_CDECL crt_fast_error_exit(u32 code) {
    if (g32(kErrorMode) == 1) cdecl_call<void>(kMsgBanner);
    cdecl_call<void>(kNmsgWrite, code);
    api<void>(kExitProcess, 0xffu);
}

// ------------------------------------------------------------------ 0x007b70f4 _purecall
void JOAN_CDECL crt_purecall() { cdecl_call<void>(kAmsgExit, 0x19u); }  // _RT_PUREVIRT

// ------------------------------------------------------------------ 0x007c218c abort
void JOAN_CDECL crt_abort() {
    cdecl_call<void>(kNmsgWrite, 0xau);  // _RT_ABORT "abnormal program termination"
    cdecl_call<int>(kRaise, kSigAbrt);
    cdecl_call<void>(kExit_, 3u);
}

// ------------------------------------------------------------------ 0x007bd437 xcptlookup / 0x007c1d58 siglookup
// struct _XCPT_ACTION { unsigned long XcptNum; int SigNum; _PHNDLR XcptAction; }  (12 bytes)
// x64: needs the action table (image 0xa31720 u8[120], copied by crt/mt _initptd / signal) and the signal action
// slots 0xa6b548..0xa6b554 (i32) as native code pointers. Nothing installs a handler: crt::signal (0x007c19f2) has
// no caller in the game or in our code (0 calls in every run), so these fields only ever hold SIG_DFL / SIG_IGN.
//
// A handler read from one of these 32-bit fields (SIG_DFL / SIG_IGN / SIG_DIE tested by the caller first) is called
// through handler_fn: the field is the original's fixed x86 layout (image u8[120] / i32, not widened), so on x64 it
// could hold only a code address below 4 GB (zero-extended here): a handler, if one were ever installed on x64, would
// need the table and slots pointer-wide first (TODO(x64): only if crt::signal ever gets a caller).
template <class F>
F handler_fn(u32 field) {
    return reinterpret_cast<F>(static_cast<uptr>(field));
}
using SigHandler1 = void(JOAN_CDECL*)(u32);
using SigHandler2 = void(JOAN_CDECL*)(u32, u32);  // SIGFPE: (sig, _tfpecode)

uptr JOAN_CDECL crt_xcptlookup(u32 xcptnum, uptr table) {
    const uptr end = table + g32(kXcptActTabCount) * 12;
    uptr p = table;
    if (g32(p) != xcptnum) {
        do {
            p += 12;
            if (p >= end) break;
        } while (g32(p) != xcptnum);
    }
    if (p >= end || g32(p) != xcptnum) return 0;
    return p;
}

uptr JOAN_CDECL crt_siglookup(u32 signum, uptr table) {
    const uptr end = table + g32(kXcptActTabCount) * 12;
    uptr p = table;
    if (g32(p + 4) != signum) {
        do {
            p += 12;
            if (p >= end) break;
        } while (g32(p + 4) != signum);
    }
    if (p >= end || g32(p + 4) != signum) return 0;
    return p;
}

// Clears the action of every floating-point exception entry of the thread's table (SIG_DFL).
void reset_fpe_actions(volatile Ptd* ptd) {
    for (i32 i = static_cast<i32>(g32(kFirstFpeIndx)); i < static_cast<i32>(g32(kNumFpe) + g32(kFirstFpeIndx));
         ++i)
        g32(ptd->pxcptacttab + static_cast<u32>(i) * 12 + 8) = kSigDfl;
}

// ------------------------------------------------------------------ 0x007bd2f9 _XcptFilter
int JOAN_CDECL crt_xcpt_filter(u32 xcptnum, void* pxcptinfoptrs) {
    c_xcpt.hit();
    volatile Ptd* const ptd = getptd();
    const uptr p = cdecl_call<uptr>(kXcptlookup, xcptnum, ptd->pxcptacttab);
    u32 handler;
    if (p == 0 || (handler = g32(p + 8)) == kSigDfl)
        return api<int>(kUnhandledExceptionFilter, pxcptinfoptrs);
    if (handler == kSigDie) {
        g32(p + 8) = kSigDfl;
        return 1;  // EXCEPTION_EXECUTE_HANDLER
    }
    if (handler == kSigIgn) return -1;  // EXCEPTION_CONTINUE_EXECUTION
    void* const oldptrs = ptd->tpxcptinfoptrs;
    ptd->tpxcptinfoptrs = pxcptinfoptrs;
    if (g32(p + 4) == kSigFpe) {
        reset_fpe_actions(ptd);
        const u32 oldfpecode = ptd->tfpecode;
        switch (g32(p)) {
            case 0xc000008e: ptd->tfpecode = 0x83; break;  // FLT_DIVIDE_BY_ZERO -> _FPE_ZERODIVIDE
            case 0xc0000090: ptd->tfpecode = 0x81; break;  // FLT_INVALID_OPERATION -> _FPE_INVALID
            case 0xc0000091: ptd->tfpecode = 0x84; break;  // FLT_OVERFLOW
            case 0xc0000093: ptd->tfpecode = 0x85; break;  // FLT_UNDERFLOW
            case 0xc000008d: ptd->tfpecode = 0x82; break;  // FLT_DENORMAL_OPERAND
            case 0xc000008f: ptd->tfpecode = 0x86; break;  // FLT_INEXACT_RESULT
            case 0xc0000092: ptd->tfpecode = 0x8a; break;  // FLT_STACK_CHECK
            default: break;
        }
        handler_fn<SigHandler2>(handler)(kSigFpe, ptd->tfpecode);
        ptd->tfpecode = oldfpecode;
    } else {
        g32(p + 8) = kSigDfl;
        handler_fn<SigHandler1>(handler)(g32(p + 4));
    }
    ptd->tpxcptinfoptrs = oldptrs;
    return -1;
}

// ------------------------------------------------------------------ 0x007c1bd6 raise
int JOAN_CDECL crt_raise(u32 sig) {
    c_raise.hit();
    bool locked = false;
    volatile Ptd* ptd = nullptr;
    uptr slot;
    u32 handler;
    switch (sig) {
        case kSigInt: slot = img::addr(kCtrlcAction); break;
        case kSigIll:
        case kSigFpe:
        case kSigSegv:
            ptd = getptd();
            slot = cdecl_call<uptr>(kSiglookup, sig, ptd->pxcptacttab) + 8;  // NULL -> reads address 8, as VC6
            break;
        case kSigTerm: slot = img::addr(kTermAction); break;
        case kSigBreak: slot = img::addr(kCtrlbreakAction); break;
        case kSigAbrt: slot = img::addr(kAbortAction); break;
        default: return -1;
    }
    handler = g32(slot);
    if (sig != kSigIll && sig != kSigFpe && sig != kSigSegv) {
        locked = true;
        lock(1);  // _SIGNAL_LOCK
    }
    if (handler == kSigIgn) {
        if (locked) unlock(handler);
        return 0;
    }
    if (handler == kSigDfl) {
        if (locked) unlock(1);
        cdecl_call<void>(kExit_, 3u);
    }
    void* oldptrs = nullptr;
    u32 oldfpecode = 0;
    if (sig == kSigFpe || sig == kSigSegv || sig == kSigIll) {
        oldptrs = ptd->tpxcptinfoptrs;
        ptd->tpxcptinfoptrs = nullptr;
        if (sig == kSigFpe) {
            oldfpecode = ptd->tfpecode;
            ptd->tfpecode = 0x8c;  // _FPE_EXPLICITGEN
        }
    }
    if (sig == kSigFpe)
        reset_fpe_actions(ptd);
    else
        g32(slot) = kSigDfl;
    if (locked) unlock(1);
    if (sig == kSigFpe) {
        handler_fn<SigHandler2>(handler)(kSigFpe, ptd->tfpecode);
    } else {
        handler_fn<SigHandler1>(handler)(sig);
        if (sig != kSigSegv && sig != kSigIll) return 0;
    }
    ptd->tpxcptinfoptrs = oldptrs;
    if (sig == kSigFpe) ptd->tfpecode = oldfpecode;
    return 0;
}

// ------------------------------------------------------------------ 0x007c19f2 signal
u32 JOAN_CDECL crt_signal(u32 sig, u32 action) {
    c_signal.hit();
    if (action != kSigAck && action != kSigSge) {
        if (sig == kSigInt || sig == kSigBreak || sig == kSigAbrt || sig == kSigTerm) {
            lock(1);
            bool ok = true;
            if ((sig == kSigInt || sig == kSigBreak) && g32(kCtrlHandlerInstalled) == 0) {
                if (api<u32>(kSetConsoleCtrlHandler, fn::code_value(kCtrlEventCapture), 1u) == 1) {
                    g32(kCtrlHandlerInstalled) = 1;
                } else {
                    const u32 err = api<u32>(kGetLastError);
                    *cdecl_call<u32*>(kDoserrno) = err;
                    unlock(1);
                    ok = false;
                }
            }
            if (ok) {
                u32 old;
                switch (sig) {
                    case kSigInt: old = g32(kCtrlcAction); g32(kCtrlcAction) = action; break;
                    case kSigTerm: old = g32(kTermAction); g32(kTermAction) = action; break;
                    case kSigBreak: old = g32(kCtrlbreakAction); g32(kCtrlbreakAction) = action; break;
                    default: old = g32(kAbortAction); g32(kAbortAction) = action; break;  // SIGABRT
                }
                unlock(1);
                return old;
            }
        } else if (sig == kSigFpe || sig == kSigIll || sig == kSigSegv) {
            volatile Ptd* const ptd = getptd();
            bool ok = true;
            if (ptd->pxcptacttab == img::addr(kXcptActTab)) {
                const uptr copy = cdecl_call<uptr>(kMalloc, g32(kXcptActTabSize));
                ptd->pxcptacttab = copy;
                if (copy == 0)
                    ok = false;
                else
                    cdecl_call<uptr>(kMemcpy, copy, img::addr(kXcptActTab), g32(kXcptActTabSize));
            }
            if (ok) {
                uptr p = cdecl_call<uptr>(kSiglookup, sig, ptd->pxcptacttab);
                if (p != 0) {
                    const u32 old = g32(p + 8);
                    while (g32(p + 4) == sig) {
                        g32(p + 8) = action;
                        p += 12;
                        if (p >= ptd->pxcptacttab + g32(kXcptActTabCount) * 12) break;
                    }
                    return old;
                }
            }
        }
    }
    *cdecl_call<u32*>(kErrno) = kEinval;
    return static_cast<u32>(-1);  // SIG_ERR
}

// ------------------------------------------------------------------ 0x007c115b __CxxUnhandledExceptionFilter
// EXCEPTION_RECORD / EXCEPTION_POINTERS with native pointer widths (x86: NumberParameters +0x10, the magic +0x14).
struct XcptRecord {
    u32 code;
    u32 flags;
    XcptRecord* next;
    void* address;
    u32 nparams;
    std::uintptr_t info[15];
};
struct XcptPointers {
    XcptRecord* record;
    void* context;
};
static_assert(sizeof(void*) != 4 || (offsetof(XcptRecord, nparams) == 0x10 && offsetof(XcptRecord, info) == 0x14),
              "EXCEPTION_RECORD layout");

inline volatile std::uintptr_t& prev_filter() { return img::global<volatile std::uintptr_t>(kPrevFilter); }

std::int32_t JOAN_STDCALL crt_cxx_unhandled_filter(XcptPointers* pointers) {  // LONG: 32 bits (long is 64 on LP64)
    c_cxxfilter.hit();
    const XcptRecord* const rec = pointers->record;
    if (rec->code == 0xe06d7363 && rec->nparams == 3 && static_cast<u32>(rec->info[0]) == 0x19930520) {
        cdecl_call<void>(kTerminate);  // never returns (abort)
    }
    if (prev_filter() != 0 && cdecl_call<int>(kValidateExecute, prev_filter()) != 0)
        return reinterpret_cast<std::int32_t(JOAN_STDCALL*)(XcptPointers*)>(prev_filter())(pointers);
    return 0;
}

// ------------------------------------------------------------------ 0x007c11a1 / 0x007c11b2 filter setter / restorer
void JOAN_CDECL crt_cxx_set_filter() {
    prev_filter() = api<std::uintptr_t>(kSetUnhandledExceptionFilter, fn::code_value(kCxxFilter));
}
void JOAN_CDECL crt_cxx_restore_filter() { api<std::uintptr_t>(kSetUnhandledExceptionFilter, prev_filter()); }

#define X(name, addr, fn) JOAN_REPLACE("crt/exit", name, addr, fn)
X("crt::_onexit", 0x007b359c, &crt_onexit);
X("crt::atexit", 0x007b361a, &crt_atexit);
X("crt::__onexitinit", 0x007b362c, &crt_onexitinit);
X("crt::_cinit", 0x007b47cf, &crt_cinit);
X("crt::exit", 0x007b47fc, &crt_exit);
X("crt::_exit", 0x007b480d, &crt__exit);
X("crt::_cexit", 0x007b481e, &crt_cexit);
X("crt::_c_exit", 0x007b482d, &crt_c_exit);
X("crt::doexit", 0x007b483c, &crt_doexit);
X("crt::_initterm", 0x007b48f3, &crt_initterm);
X("crt::_purecall", 0x007b70f4, &crt_purecall);
X("crt::_amsg_exit", 0x007b72ee, &crt_amsg_exit);
X("crt::fast_error_exit", 0x007b7313, &crt_fast_error_exit);
X("crt::_FF_MSGBANNER", 0x007c164f, &crt_ff_msgbanner);
X("crt::_NMSG_WRITE", 0x007c1688, &crt_nmsg_write);
X("crt::__crtMessageBoxA", 0x007c4b11, &crt_message_box);
X("crt::abort", 0x007c218c, &crt_abort);
X("crt::signal", 0x007c19f2, &crt_signal);
X("crt::raise", 0x007c1bd6, &crt_raise);
X("crt::siglookup", 0x007c1d58, &crt_siglookup);
X("crt::_XcptFilter", 0x007bd2f9, &crt_xcpt_filter);
X("crt::xcptlookup", 0x007bd437, &crt_xcptlookup);
X("crt::__CxxUnhandledExceptionFilter", 0x007c115b, &crt_cxx_unhandled_filter);
X("crt::__CxxSetUnhandledExceptionFilter", 0x007c11a1, &crt_cxx_set_filter);
X("crt::__CxxRestoreUnhandledExceptionFilter", 0x007c11b2, &crt_cxx_restore_filter);
#undef X

}  // namespace

// Direct entry points (crt.hpp).
namespace joan::crt {
int JOAN_CDECL atexit(std::uintptr_t func) { return crt_atexit(func); }
}  // namespace joan::crt

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::_cinit() -> void { joan::bind::free<&::crt_cinit>(); }  // 0x7b47cf
auto joan::orig::libc::exit(std::uint32_t a0) -> void { joan::bind::free<&::crt_exit>(a0); }  // 0x7b47fc
auto joan::orig::libc::_exit(std::uint32_t a0) -> void { joan::bind::free<&::crt__exit>(a0); }  // 0x7b480d
auto joan::orig::libc::_amsg_exit(std::uint32_t a0) -> void { joan::bind::free<&::crt_amsg_exit>(a0); }  // 0x7b72ee
auto joan::orig::libc::fast_error_exit(std::uint32_t a0) -> void { joan::bind::free<&::crt_fast_error_exit>(a0); }  // 0x7b7313
auto joan::orig::libc::_xcpt_filter(std::uint32_t a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_xcpt_filter>(a0, a1)); }  // 0x7bd2f9
