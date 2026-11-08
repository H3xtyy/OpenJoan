// Original library sources (re/attribution.csv original_file, under lib/): crt/_file.c, crt/dosmap.c, crt/fcvt.c,
//   crt/mlock.c, crt/rand.c, crt/strtok.c, crt/thread.c, crt/tidtable.c.
//
// Batch 114 / list 04: CRT family C "MT core" (VC6 LIBCMT mlock.c / tidtable.c / thread.c / dosmap.c / errno
// accessors / rand.c / strtok.c / _file.c lock wrappers / _fcvt's per-thread buffer), installed at birth.
//
// Module "crt/mt" (one switch for the whole family: it owns TLS / ptd / locks, so it must be enabled or
// disabled as a unit - never disable single functions of it).
//
// Surface replaced (every function of the family that original code or our C++ can reach; all cdecl `ret`
// unless noted; callers found by scanning joan.asm for call/jmp/push/mov of each address + src/modules):
//   0x007b8b9c _mtinit()                int     caller: WinMainCRTStartup 0x7b71e6 (or its C++ transcription)
//   0x007b8bf0 _mtterm()                void    no caller (registered for completeness)
//   0x007b8215 _mtinitlocks()           void    caller: _mtinit
//   0x007b823e _mtdeletelocks()         void    caller: _mtterm
//   0x007b82aa _lock(int)               void    ~105 CRT call sites (heap 9, exit 13, setlocale 19, mbcp 25, ...)
//   0x007b830b _unlock(int)             void    ~105 CRT call sites
//   0x007bcde0 _lock_file(FILE*)        void    stdio (fclose, fprintf, fread, ftell, fseek, fgetc, ...)
//   0x007bce0f _lock_file2(int, FILE*)  void    flsall 0x7b7e0f, _getstream 0x7bd1b6
//   0x007bce32 _unlock_file(FILE*)      void    stdio
//   0x007bce61 _unlock_file2(int, FILE*) void   flsall, _getstream
//   0x007b8c0e _initptd(ptd)            ptd     _mtinit, _getptd, _beginthread
//   0x007b8c21 _getptd()                ptd     EH runtime (0x7b1a95, 0x7b83d3, 0x7b8586, 0x7b87bc, 0x7b8889,
//                                               terminate 0x7b8d34, 0x7b8d90), _XcptFilter 0x7bd2f9, gmtime
//                                               0x7bda49, signal/raise/siglookup 0x7c180c/0x7c1bd6/0x7c1d58,
//                                               and the functions below
//   0x007b8c88 _freeptd(ptd)            BOOL    _endthread
//   0x007bbfb5 _errno()                 int*    52 CRT call sites (math errors, find*, lowio, collation, ...)
//   0x007bbfbe __doserrno()             ulong*  20 CRT call sites
//   0x007bbf42 _dosmaperr(ulong)        int*    11 CRT call sites (table 0xa31328..0xa31490 copied below)
//   0x007b43e3 _beginthread(fn, stack, arg)  uintptr  game 0x578e1b / 0x578f37 (C++ game_b97_05)
//   0x007b445a _threadstart(ptd)        stdcall ret 4 (CreateThread start routine; ours is passed directly)
//   0x007b44ed _endthread()             noreturn  game 0x578fd9 / 0x579001 (C++ game_b105_15), _threadstart
//   0x007b3e93 srand(unsigned)          void    game 0x4c5a8c / 0x4c67fc (C++ game_b108_01 / _02)
//   0x007b3ea0 rand()                   int     no caller (registered for completeness)
//   0x007b3148 strtok(char*, const char*) char*  10 game sites (C++ b83_06, b98_01, b103_08, b106_04)
//   0x007c61de _fpcvt(STRFLT*, int, int*, int*)  char*  _fcvt / _ecvt (ptd->_cvtbuf +0x44)
//   0x007c616e _fcvt(double, int, int*, int*)    char*  game 0x58fe45 / 0x5a29ce (C++ game_stream2, misc)
//
// Not replaced (and why):
//   0x007c61a1 _ecvt: no caller and no address constant anywhere in the image (unreachable).
//   0x007b8d28 __threadid / 0x007b8d2e __threadhandle (jmp [IAT]): no caller (unreachable).
//   EH runtime (__CxxFrameHandler & co., terminate/unexpected), _XcptFilter, signal/raise, gmtime: other
//   families (L / crt/exit / crt/time); they reach the ptd only through _getptd's address -> ours.
//
// State handling (CRT_SYSTEM.md 5.1 rule 2) - everything stays at the original address, original layout:
//   __tlsindex     0x00a30fa0 (initial -1). Readers in the image: only the family (_mtinit, _mtterm, _getptd,
//                  _freeptd, _threadstart) - kept there anyway, as the task requires.
//   _locktable     0x00a30edc + 4*n, n = 0..47 (CRITICAL_SECTION*). Readers: only _lock/_unlock/_mtinitlocks/
//                  _mtdeletelocks. Entries 1/9/13/17 are pre-initialised .data pointers to the static CS objects
//                  0xa6b250 / 0xa6b280 / 0xa6b298 / 0xa6b268 (_SIGNAL / _HEAP / _EXIT1 / _LOCKTAB), the
//                  others are created lazily as malloc(0x18) under lock 17, exactly as VC6.
//   ptd            VC6 _tiddata, 0x74 bytes from calloc (0x7b7b87, heap family): +0 tid, +4 thandle (-1 for
//                  threads not made by _beginthread), +8 errno, +0xc doserrno, +0x14 holdrand (1),
//                  +0x18 strtok token, +0x24/+0x28/+0x30/+0x38/+0x40/+0x44 freed by _freeptd, +0x48/+0x4c
//                  _beginthread fn/arg, +0x50 _pxcptacttab = 0xa31720 (_XcptActTab, read by _XcptFilter/signal),
//                  +0x60..+0x70 read by the original C++ EH runtime. Foreign threads (DirectSound / OpenAL
//                  callbacks) get a lazily allocated ptd from _getptd as in VC6.
//   _FPmtinit / _FPmtterm pointers 0xa3094c / 0xa30950 both hold 0x7b1d5a (a bare `ret`) and are never
//                  written anywhere in the image: their calls are no-ops and are omitted.
// Other families are reached only through their original addresses (rule 4): calloc 0x7b7b87, malloc
// 0x7b6c54, free 0x7b4e1d (heap), _amsg_exit 0x7b72ee, _exit 0x7b480d, _XcptFilter 0x7bd2f9 (exit/filters),
// _fptostr 0x7c2637 (printf; _fltout2 0x7c26ae is called directly, see _fcvt). Inside the family the C++ functions
// call each other directly. Win32 calls go through k32:: (see below).
//
// (Since the portable platform layer the KERNEL32 calls go to the same functions joan.exe's import slots hold - our
// critical sections, TLS, threads, last error (platform/system.cpp, process.cpp, files.cpp) - through k32:: below,
// as the original family reached them through its imports; ExitThread / SetLastError, which joan.exe does not
// import, are platform/threads.hpp's.)
// Notes on exactness:
//  - _getptd saves / restores GetLastError around TlsGetValue (VC6); on allocation failure _amsg_exit(0x10).
//  - _lock_file range test is the _iob array 0xa314a0..0xa31700 (unsigned), lock 0x1c + index; otherwise the
//    _FILEX critical section at FILE+0x20. These are lock-layer functions shared with crt/stdio, which must keep
//    _iob at 0xa314a0 while it calls them.
//  - _beginthread: calloc failure still calls free(NULL) (as the original); CREATE_SUSPENDED + lpThreadId = ptd
//    (the tid lands in ptd+0); returns the handle or -1 after _dosmaperr(GetLastError()) when GetLastError != 0.
//  - _threadstart: the original's __try/__except (filter _XcptFilter(code, pointers), handler _exit(code)) is not
//    kept. It is reached only by a structured exception (fault) in a thread function (the two ilib/audio.cpp
//    threads), i.e. a crash: _XcptFilter maps such codes to signals whose actions are all SIG_DFL (nothing calls
//    signal()), and for SIG_DFL or an unlisted code it returns UnhandledExceptionFilter(pointers) - the same
//    top-level filter chain (__CxxUnhandledExceptionFilter -> crash.cpp) the system's thread-start frame calls
//    without our frame. Only the final exit differs (_exit(code) vs. the system's NtTerminateProcess(code)), after
//    the crash report; nothing faults there in a normal run.
//  - _fcvt: the double copied with the original fld / fstp's effect (SNaN quieted), _fltout2 called directly
//    (lib/crt/printf.cpp, joan::crt::fltout2).
#include "../../callconv.hpp"
#include "../../original/helpers.hpp"
#include "../../platform/win32_types.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "../../original/direct.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"
#include "crt.hpp"
#include "../../runtime/loader/import_overrides.hpp"
#include "../../platform/threads.hpp"

namespace joan::crt {
// 0x007c26ae _fltout2 (lib/crt/printf.cpp; flt is that file's STRFLT, the same VC6 layout as Strflt below).
void* JOAN_CDECL fltout2(std::uint32_t lo, std::uint32_t hi, void* flt, char* resultstr);
}  // namespace joan::crt

namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

JOAN_COUNTER(c_mtinit, "crt::_mtinit");
JOAN_COUNTER(c_beginthread, "crt::_beginthread");
JOAN_COUNTER(c_threadstart, "crt::_threadstart");
JOAN_COUNTER(c_endthread, "crt::_endthread");
JOAN_COUNTER(c_strtok, "crt::strtok");
JOAN_COUNTER(c_dosmaperr, "crt::_dosmaperr");
JOAN_COUNTER(c_fpcvt, "crt::_fpcvt");

using joan::call::cdecl_call;

// KERNEL32 as joan.exe's import slots answer it (the header): k32::X calls our function for the import X.
namespace k32 {
#define JOAN_K32(name)                                                                                                  \
    template <class... A>                                                                                               \
    auto name(A... a) {                                                                                                 \
        static const auto f = joan::loader::override_function<decltype(&::name)>("KERNEL32.dll", #name);               \
        return f(a...);                                                                                                 \
    }
JOAN_K32(InitializeCriticalSection)
JOAN_K32(DeleteCriticalSection)
JOAN_K32(EnterCriticalSection)
JOAN_K32(LeaveCriticalSection)
JOAN_K32(TlsAlloc)
JOAN_K32(TlsFree)
JOAN_K32(TlsGetValue)
JOAN_K32(TlsSetValue)
JOAN_K32(GetLastError)
JOAN_K32(GetCurrentThreadId)
JOAN_K32(CreateThread)
JOAN_K32(ResumeThread)
JOAN_K32(CloseHandle)
#undef JOAN_K32
void SetLastError(DWORD code) { joan::platform::set_last_error(code); }
[[noreturn]] void ExitThread(DWORD code) { joan::platform::exit_thread(code); }
}  // namespace k32

// VC6 _tiddata (mtdll.h): the per-thread data, 0x74 bytes on x86 (natural alignment; pointers widen on x64).
// lib/crt/exit.cpp and lib/crt/time.cpp still reach +0x40 / +0x50..+0x58 through x86 offsets.
struct Tiddata {
    u32 tid;                // 0x00
    std::uintptr_t thandle; // 0x04 (-1 for threads not made by _beginthread)
    int terrno;             // 0x08
    u32 tdoserrno;          // 0x0c (VC6 unsigned long = 32 bits; fixed width: long is 8 bytes on LP64 Linux x64)
    u32 fpds;               // 0x10
    u32 holdrand;           // 0x14
    char* token;            // 0x18 strtok
    WCHAR* wtoken;        // 0x1c
    u8* mtoken;             // 0x20
    char* errmsg;           // 0x24
    char* namebuf0;         // 0x28
    WCHAR* wnamebuf0;     // 0x2c
    char* namebuf1;         // 0x30
    WCHAR* wnamebuf1;     // 0x34
    char* asctimebuf;       // 0x38
    WCHAR* wasctimebuf;   // 0x3c
    void* gmtimebuf;        // 0x40
    char* cvtbuf;           // 0x44
    void* initaddr;         // 0x48 _beginthread fn
    void* initarg;          // 0x4c _beginthread arg
    void* pxcptacttab;      // 0x50
    void* tpxcptinfoptrs;   // 0x54
    int tfpecode;           // 0x58
    u32 nlg_code;           // 0x5c
    void* terminate;        // 0x60 (C++ EH runtime)
    void* unexpected;       // 0x64
    void* translator;       // 0x68
    void* curexception;     // 0x6c
    void* curcontext;       // 0x70
};
#if UINTPTR_MAX == 0xffffffffu
static_assert(sizeof(Tiddata) == 0x74 && offsetof(Tiddata, holdrand) == 0x14 && offsetof(Tiddata, cvtbuf) == 0x44 &&
              offsetof(Tiddata, pxcptacttab) == 0x50 && offsetof(Tiddata, curcontext) == 0x70, "VC6 _tiddata");
#endif
static_assert(sizeof(void*) != 8 || (offsetof(Tiddata, gmtimebuf) == 0x70 && offsetof(Tiddata, pxcptacttab) == 0x90 &&
                                     offsetof(Tiddata, tfpecode) == 0xa0),
              "_tiddata x64 (lib/crt/exit.cpp Ptd, lib/crt/time.cpp gmtimebuf)");
Tiddata* tid(void* ptd) { return static_cast<Tiddata*>(ptd); }

// ------------------------------------------------------------------ state at the original addresses
DWORD& tls_index() { return img::global<DWORD>(d::g_tlsindex); }                  // __tlsindex
constexpr int kLockCount = 48;              // 0xa30edc .. 0xa30f9c
#if !JOAN_IMAGE_X64
CRITICAL_SECTION** lock_table() { return &img::global<CRITICAL_SECTION*>(d::g_a30edc); }  // _locktable[48]
#else
// x64: the image keeps _locktable's x86 bytes (4-byte entries, of which the layout widened only 1/9/13/17 to 8 bytes
// at their x86 offsets, so table[n] does not land on entry n) and the static CRITICAL_SECTIONs 0xa6b250.. are 0x18
// bytes apart (an x64 CRITICAL_SECTION is 0x28). Nothing else refers to either (no data relocation points into them;
// only this family reads the table), so the table and the four static locks are native here, same initial state.
CRITICAL_SECTION** lock_table() {
    static CRITICAL_SECTION cs_static[4];  // _SIGNAL (1) / _HEAP (9) / _EXIT1 (13) / _LOCKTAB (17)
    static CRITICAL_SECTION* t[kLockCount] = {nullptr, &cs_static[0], nullptr, nullptr, nullptr, nullptr, nullptr,
                                              nullptr, nullptr, &cs_static[1], nullptr, nullptr, nullptr, &cs_static[2],
                                              nullptr, nullptr, nullptr, &cs_static[3]};
    return t;
}
#endif
constexpr img::Addr kXcptActTab = d::g_xcptActTab;    // _XcptActTab (signals family data)
#if !JOAN_IMAGE_X64
constexpr img::Addr kIobBegin = d::g_a314a0;      // _iob[0]
constexpr img::Addr kIobLast = d::g_iobLast;       // &_iob[19]
#endif
constexpr int kLockTabLock = 0x11;          // _LOCKTAB_LOCK (17)
constexpr int kStreamLock0 = 0x1c;          // _STREAM_LOCKS (28)

// Other families, by original address.
void* crt_calloc(u32 n, u32 size) { return joan::orig::arg::as<void*>(joan::orig::calloc(joan::orig::arg::u32(n), joan::orig::arg::u32(size))); }
void* crt_malloc(u32 size) { return joan::orig::arg::as<void*>(joan::orig::malloc_(joan::orig::arg::u32(size))); }
using joan::orig::helpers::crt_free;
void amsg_exit(int code) { joan::orig::libc::_amsg_exit(joan::orig::arg::u32(code)); }

// ------------------------------------------------------------------ 0x007b8215 _mtinitlocks
void JOAN_CDECL mt_initlocks() {
    CRITICAL_SECTION** t = lock_table();
    k32::InitializeCriticalSection(t[17]);  // [0xa30f20] -> 0xa6b268
    k32::InitializeCriticalSection(t[13]);  // [0xa30f10] -> 0xa6b298
    k32::InitializeCriticalSection(t[9]);   // [0xa30f00] -> 0xa6b280
    k32::InitializeCriticalSection(t[1]);   // [0xa30ee0] -> 0xa6b250
}

// ------------------------------------------------------------------ 0x007b823e _mtdeletelocks
void JOAN_CDECL mt_deletelocks() {
    CRITICAL_SECTION** t = lock_table();
    for (int i = 0; i < kLockCount; ++i) {
        CRITICAL_SECTION* cs = t[i];
        if (cs == nullptr || i == 17 || i == 13 || i == 9 || i == 1) continue;
        k32::DeleteCriticalSection(cs);
        crt_free(t[i]);  // the table entry is left as is (VC6)
    }
    k32::DeleteCriticalSection(t[9]);
    k32::DeleteCriticalSection(t[13]);
    k32::DeleteCriticalSection(t[17]);
    k32::DeleteCriticalSection(t[1]);
}

// ------------------------------------------------------------------ 0x007b82aa _lock / 0x007b830b _unlock
void JOAN_CDECL mt_unlock(int n) { k32::LeaveCriticalSection(lock_table()[n]); }

void JOAN_CDECL mt_lock(int n) {
    CRITICAL_SECTION** slot = lock_table() + n;
    if (*slot == nullptr) {
        auto* cs = static_cast<CRITICAL_SECTION*>(crt_malloc(static_cast<u32>(sizeof(CRITICAL_SECTION))));  // 0x18 on x86
        if (cs == nullptr) amsg_exit(0x11);  // _RT_LOCK (does not return)
        mt_lock(kLockTabLock);
        if (*slot == nullptr) {
            k32::InitializeCriticalSection(cs);
            *slot = cs;
        } else {
            crt_free(cs);
        }
        mt_unlock(kLockTabLock);
    }
    k32::EnterCriticalSection(*slot);
}

// ------------------------------------------------------------------ 0x007bcde0.. stream lock wrappers
// The CRITICAL_SECTION of a malloc'd _FILEX (stream >= 20): after its FILE (+0x20; x64: lib/crt/stdio.cpp).
inline CRITICAL_SECTION* stream_cs(void* pf) {
#if !JOAN_IMAGE_X64
    return reinterpret_cast<CRITICAL_SECTION*>(static_cast<u8*>(pf) + 0x20);
#else
    return static_cast<CRITICAL_SECTION*>(joan::crt::filex_cs(pf));
#endif
}
void JOAN_CDECL mt_lock_file(void* pf) {
#if !JOAN_IMAGE_X64
    const u32 a = reinterpret_cast<u32>(pf);
    if (a >= img::addr(kIobBegin) && a <= img::addr(kIobLast))
        mt_lock((static_cast<i32>(a - img::addr(kIobBegin)) >> 5) + kStreamLock0);  // sar 5, + 0x1c
    else
        k32::EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(a + 0x20));
#else  // _iob is a native array of 48-byte FILEs (lib/crt/stdio.cpp)
    const int i = joan::crt::iob_index(pf);
    if (i >= 0)
        mt_lock(i + kStreamLock0);
    else
        k32::EnterCriticalSection(stream_cs(pf));
#endif
}
void JOAN_CDECL mt_lock_file2(int i, void* pf) {
    if (i < 0x14)
        mt_lock(i + kStreamLock0);
    else
        k32::EnterCriticalSection(stream_cs(pf));
}
void JOAN_CDECL mt_unlock_file(void* pf) {
#if !JOAN_IMAGE_X64
    const u32 a = reinterpret_cast<u32>(pf);
    if (a >= img::addr(kIobBegin) && a <= img::addr(kIobLast))
        mt_unlock((static_cast<i32>(a - img::addr(kIobBegin)) >> 5) + kStreamLock0);
    else
        k32::LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(a + 0x20));
#else
    const int i = joan::crt::iob_index(pf);
    if (i >= 0)
        mt_unlock(i + kStreamLock0);
    else
        k32::LeaveCriticalSection(stream_cs(pf));
#endif
}
void JOAN_CDECL mt_unlock_file2(int i, void* pf) {
    if (i < 0x14)
        mt_unlock(i + kStreamLock0);
    else
        k32::LeaveCriticalSection(stream_cs(pf));
}

// ------------------------------------------------------------------ 0x007b8c0e _initptd
void* JOAN_CDECL mt_initptd(void* ptd) {
    tid(ptd)->pxcptacttab = img::obj(kXcptActTab);  // _pxcptacttab
    tid(ptd)->holdrand = 1;            // _holdrand
    return ptd;
}

// ------------------------------------------------------------------ 0x007b8b9c _mtinit
int JOAN_CDECL mt_init() {
    c_mtinit.hit();
    mt_initlocks();
    const DWORD idx = k32::TlsAlloc();
    tls_index() = idx;
    if (idx == TLS_OUT_OF_INDEXES) return 0;
    void* ptd = crt_calloc(1, sizeof(Tiddata));
    if (ptd == nullptr) return 0;
    if (!k32::TlsSetValue(tls_index(), ptd)) return 0;
    mt_initptd(ptd);
    const DWORD thread = k32::GetCurrentThreadId();
    tid(ptd)->thandle = static_cast<std::uintptr_t>(-1);
    tid(ptd)->tid = thread;
    return 1;
}

// ------------------------------------------------------------------ 0x007b8bf0 _mtterm
void JOAN_CDECL mt_term() {
    mt_deletelocks();
    if (tls_index() != TLS_OUT_OF_INDEXES) {
        k32::TlsFree(tls_index());
        tls_index() = TLS_OUT_OF_INDEXES;
    }
}

// ------------------------------------------------------------------ 0x007b8c21 _getptd
void* JOAN_CDECL mt_getptd() {
    const DWORD saved = k32::GetLastError();
    void* ptd = k32::TlsGetValue(tls_index());
    if (ptd == nullptr) {
        ptd = crt_calloc(1, sizeof(Tiddata));
        if (ptd != nullptr && k32::TlsSetValue(tls_index(), ptd)) {
            mt_initptd(ptd);
            const DWORD thread = k32::GetCurrentThreadId();
            tid(ptd)->thandle = static_cast<std::uintptr_t>(-1);
            tid(ptd)->tid = thread;
        } else {
            amsg_exit(0x10);  // _RT_THREAD (does not return)
        }
    }
    k32::SetLastError(saved);
    return ptd;
}

// ------------------------------------------------------------------ 0x007b8c88 _freeptd
BOOL JOAN_CDECL mt_freeptd(void* ptd) {
    if (tls_index() == TLS_OUT_OF_INDEXES) return static_cast<BOOL>(TLS_OUT_OF_INDEXES);  // eax = -1
    if (ptd == nullptr) ptd = k32::TlsGetValue(tls_index());
    if (ptd != nullptr) {
        Tiddata* const t = tid(ptd);
        const auto release = [](void* b) {
            if (b != nullptr) crt_free(b);
        };
        release(t->errmsg);
        release(t->namebuf0);
        release(t->namebuf1);
        release(t->asctimebuf);
        release(t->gmtimebuf);
        release(t->cvtbuf);
        if (t->pxcptacttab != img::obj(kXcptActTab)) crt_free(t->pxcptacttab);
        crt_free(ptd);
    }
    return k32::TlsSetValue(tls_index(), nullptr);
}

// ------------------------------------------------------------------ 0x007bbfb5 _errno / 0x007bbfbe __doserrno
int* JOAN_CDECL mt_errno() { return &tid(mt_getptd())->terrno; }
u32* JOAN_CDECL mt_doserrno() { return &tid(mt_getptd())->tdoserrno; }

// ------------------------------------------------------------------ 0x007bbf42 _dosmaperr
struct ErrEntry {
    u32 oscode;
    i32 errnocode;
};
// errtable[] 0xa31328..0xa31490 (45 entries), same order.
const ErrEntry kErrTable[45] = {
    {0x1, 22},  {0x2, 2},   {0x3, 2},   {0x4, 24},  {0x5, 13},  {0x6, 9},    {0x7, 12},  {0x8, 12},  {0x9, 12},
    {0xa, 7},   {0xb, 8},   {0xc, 22},  {0xd, 22},  {0xf, 2},   {0x10, 13},  {0x11, 18}, {0x12, 2},  {0x21, 13},
    {0x35, 2},  {0x41, 13}, {0x43, 2},  {0x50, 17}, {0x52, 13}, {0x53, 13},  {0x57, 22}, {0x59, 11}, {0x6c, 13},
    {0x6d, 32}, {0x70, 28}, {0x72, 9},  {0x6, 22},  {0x80, 10}, {0x81, 10},  {0x82, 9},  {0x83, 22}, {0x84, 13},
    {0x91, 41}, {0x9e, 13}, {0xa1, 2},  {0xa4, 11}, {0xa7, 13}, {0xb7, 17},  {0xce, 2},  {0xd7, 11}, {0x718, 12},
};

int* JOAN_CDECL mt_dosmaperr(u32 oserr) {  // DWORD (VC6 unsigned long): 32 bits, not LP64 unsigned long
    c_dosmaperr.hit();
    *mt_doserrno() = oserr;
    for (const ErrEntry& e : kErrTable) {
        if (e.oscode == oserr) {
            int* p = mt_errno();
            *p = e.errnocode;
            return p;
        }
    }
    int* p = mt_errno();
    if (oserr >= 0x13 && oserr <= 0x24)
        *p = 13;  // EACCES (ERROR_WRITE_PROTECT .. ERROR_SHARING_BUFFER_EXCEEDED)
    else if (oserr >= 0xbc && oserr <= 0xca)
        *p = 8;   // ENOEXEC (ERROR_INVALID_STARTING_CODESEG .. ERROR_INFLOOP_IN_RELOC_CHAIN)
    else
        *p = 22;  // EINVAL
    return p;
}

// ------------------------------------------------------------------ 0x007b44ed _endthread
[[noreturn]] void JOAN_CDECL mt_endthread() {
    c_endthread.hit();
    // (*_FPmtterm)() - 0xa30950 = 0x7b1d5a, a bare `ret`: omitted.
    void* ptd = mt_getptd();
    if (ptd == nullptr) amsg_exit(0x10);
    const std::uintptr_t h = tid(ptd)->thandle;
    if (h != static_cast<std::uintptr_t>(-1)) k32::CloseHandle(reinterpret_cast<HANDLE>(h));
    mt_freeptd(ptd);
    k32::ExitThread(0);
}

// ------------------------------------------------------------------ 0x007b445a _threadstart
DWORD JOAN_STDCALL mt_threadstart(void* ptd) {
    c_threadstart.hit();
    if (!k32::TlsSetValue(tls_index(), ptd)) amsg_exit(0x10);
    // (*_FPmtinit)() - 0xa3094c = 0x7b1d5a, a bare `ret`: omitted.
    reinterpret_cast<void(JOAN_CDECL*)(void*)>(tid(ptd)->initaddr)(tid(ptd)->initarg);
    mt_endthread();  // ExitThread: does not return
}

// ------------------------------------------------------------------ 0x007b43e3 _beginthread
std::uintptr_t JOAN_CDECL mt_beginthread(void* fn, unsigned stack, void* arg) {
    c_beginthread.hit();
    DWORD err = 0;
    void* ptd = crt_calloc(1, sizeof(Tiddata));
    if (ptd != nullptr) {
        mt_initptd(ptd);
        tid(ptd)->initaddr = fn;
        tid(ptd)->initarg = arg;
        HANDLE h = k32::CreateThread(nullptr, stack, &mt_threadstart, ptd, CREATE_SUSPENDED, static_cast<LPDWORD>(ptd));
        tid(ptd)->thandle = reinterpret_cast<std::uintptr_t>(h);
        if (h != nullptr && k32::ResumeThread(h) != 0xffffffffu) return reinterpret_cast<std::uintptr_t>(h);
        err = k32::GetLastError();
    }
    crt_free(ptd);  // free(NULL) too when calloc failed, as the original
    if (err != 0) mt_dosmaperr(err);
    return static_cast<std::uintptr_t>(-1);
}

// ------------------------------------------------------------------ 0x007b3e93 srand / 0x007b3ea0 rand
void JOAN_CDECL mt_srand(unsigned seed) { tid(mt_getptd())->holdrand = seed; }

int JOAN_CDECL mt_rand() {
    void* ptd = mt_getptd();
    const u32 next = tid(ptd)->holdrand * 0x343fdu + 0x269ec3u;
    tid(ptd)->holdrand = next;
    return static_cast<int>((next >> 16) & 0x7fffu);
}

// ------------------------------------------------------------------ 0x007b3148 strtok
char* JOAN_CDECL mt_strtok(char* string, const char* control) {
    c_strtok.hit();
    void* ptd = mt_getptd();
    u8 map[32] = {};
    const u8* ctl = reinterpret_cast<const u8*>(control);
    u8 c;
    do {  // the terminating NUL's bit is set too
        c = *ctl++;
        map[c >> 3] |= static_cast<u8>(1u << (c & 7));
    } while (c != 0);
    u8* str = reinterpret_cast<u8*>(string != nullptr ? string : tid(ptd)->token);
    while ((map[*str >> 3] & (1u << (*str & 7))) != 0 && *str != 0) ++str;
    u8* const start = str;
    for (; *str != 0; ++str) {
        if ((map[*str >> 3] & (1u << (*str & 7))) != 0) {
            *str++ = 0;
            break;
        }
    }
    tid(ptd)->token = reinterpret_cast<char*>(str);
    return start == str ? nullptr : reinterpret_cast<char*>(start);
}

// ------------------------------------------------------------------ 0x007c61de _fpcvt / 0x007c616e _fcvt
struct Strflt {  // VC6 STRFLT (cvt.h)
    i32 sign;    // '-' (0x2d) or ' '
    i32 decpt;
    i32 flag;
    char* mantissa;
};

char* JOAN_CDECL mt_fpcvt(Strflt* pflt, int digits, int* decpt, int* sign) {
    c_fpcvt.hit();
    void* ptd = mt_getptd();
    if (tid(ptd)->cvtbuf == nullptr) {
        char* b = static_cast<char*>(crt_malloc(0x15d));
        tid(ptd)->cvtbuf = b;
        if (b == nullptr) return nullptr;
    }
    char* buf = tid(ptd)->cvtbuf;
    const int n = digits > 0x15b ? 0x15b : digits;
    joan::orig::libc::_fptostr(joan::orig::arg::str(buf), joan::orig::arg::i32(n), joan::orig::arg::mptr(pflt));  // _fptostr (printf family)
    *sign = pflt->sign == 0x2d ? 1 : 0;
    *decpt = pflt->decpt;
    return buf;
}

// The double is copied as the original's fld / fstp does (an SNaN comes out quieted), split into its two dwords for
// _fltout2 (24-byte digit buffer and STRFLT in the frame, as the original), then _fpcvt(pflt, decpt + ndigit).
char* JOAN_CDECL mt_fcvt(double value, int ndigit, int* decpt, int* sign) {
    std::uint64_t b;
    std::memcpy(&b, &value, 8);
    if ((b & 0x7ff0000000000000ull) == 0x7ff0000000000000ull && (b & 0x000fffffffffffffull) != 0)
        b |= 0x0008000000000000ull;
    Strflt flt;
    char digits[0x18];
    Strflt* const pflt =
        static_cast<Strflt*>(joan::crt::fltout2(static_cast<u32>(b), static_cast<u32>(b >> 32), &flt, digits));
    return mt_fpcvt(pflt, static_cast<int>(static_cast<u32>(pflt->decpt) + static_cast<u32>(ndigit)), decpt, sign);
}

// ------------------------------------------------------------------ registration (module crt/mt)
// One line per registration (re/check_lib_entries.py parses them).
JOAN_REPLACE("crt/mt", "crt::_mtinit", 0x007b8b9c, &mt_init);
JOAN_REPLACE("crt/mt", "crt::_mtterm", 0x007b8bf0, &mt_term);
JOAN_REPLACE("crt/mt", "crt::_mtinitlocks", 0x007b8215, &mt_initlocks);
JOAN_REPLACE("crt/mt", "crt::_mtdeletelocks", 0x007b823e, &mt_deletelocks);
JOAN_REPLACE("crt/mt", "crt::_lock", 0x007b82aa, &mt_lock);
JOAN_REPLACE("crt/mt", "crt::_unlock", 0x007b830b, &mt_unlock);
JOAN_REPLACE("crt/mt", "crt::_lock_file", 0x007bcde0, &mt_lock_file);
JOAN_REPLACE("crt/mt", "crt::_lock_file2", 0x007bce0f, &mt_lock_file2);
JOAN_REPLACE("crt/mt", "crt::_unlock_file", 0x007bce32, &mt_unlock_file);
JOAN_REPLACE("crt/mt", "crt::_unlock_file2", 0x007bce61, &mt_unlock_file2);
JOAN_REPLACE("crt/mt", "crt::_initptd", 0x007b8c0e, &mt_initptd);
JOAN_REPLACE("crt/mt", "crt::_getptd", 0x007b8c21, &mt_getptd);
JOAN_REPLACE("crt/mt", "crt::_freeptd", 0x007b8c88, &mt_freeptd);
JOAN_REPLACE("crt/mt", "crt::_errno", 0x007bbfb5, &mt_errno);
JOAN_REPLACE("crt/mt", "crt::__doserrno", 0x007bbfbe, &mt_doserrno);
JOAN_REPLACE("crt/mt", "crt::_dosmaperr", 0x007bbf42, &mt_dosmaperr);
JOAN_REPLACE("crt/mt", "crt::_beginthread", 0x007b43e3, &mt_beginthread);
JOAN_REPLACE("crt/mt", "crt::_threadstart", 0x007b445a, &mt_threadstart);
JOAN_REPLACE("crt/mt", "crt::_endthread", 0x007b44ed, &mt_endthread);
JOAN_REPLACE("crt/mt", "crt::srand", 0x007b3e93, &mt_srand);
JOAN_REPLACE("crt/mt", "crt::rand", 0x007b3ea0, &mt_rand);
JOAN_REPLACE("crt/mt", "crt::strtok", 0x007b3148, &mt_strtok);
JOAN_REPLACE("crt/mt", "crt::_fpcvt", 0x007c61de, &mt_fpcvt);
JOAN_REPLACE("crt/mt", "crt::_fcvt", 0x007c616e, &mt_fcvt);

}  // namespace

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::strtok(const char* a0, const char* a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::mt_strtok>(a0, a1)); }  // 0x7b3148
auto joan::orig::libc::srand(std::uint32_t a0) -> void { joan::bind::free<&::mt_srand>(a0); }  // 0x7b3e93
auto joan::orig::libc::_beginthread(void* a0, std::uint32_t a1, void* a2) -> std::uint32_t { return joan::bind::conv<std::uint32_t>(joan::bind::free<&::mt_beginthread>(a0, a1, a2)); }  // 0x7b43e3
auto joan::orig::libc::_endthread() -> void { joan::bind::free<&::mt_endthread>(); }  // 0x7b44ed
auto joan::orig::libc::_lock(std::int32_t a0) -> void { joan::bind::free<&::mt_lock>(a0); }  // 0x7b82aa
auto joan::orig::libc::_unlock(std::int32_t a0) -> void { joan::bind::free<&::mt_unlock>(a0); }  // 0x7b830b
auto joan::orig::libc::_mtinit() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::mt_init>()); }  // 0x7b8b9c
auto joan::orig::libc::_getptd() -> void* { return joan::bind::conv<void*>(joan::bind::free<&::mt_getptd>()); }  // 0x7b8c21
auto joan::orig::libc::_dosmaperr(std::uint32_t a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::mt_dosmaperr>(a0)); }  // 0x7bbf42
auto joan::orig::libc::_errno() -> void* { return joan::bind::conv<void*>(joan::bind::free<&::mt_errno>()); }  // 0x7bbfb5
auto joan::orig::libc::__doserrno() -> void* { return joan::bind::conv<void*>(joan::bind::free<&::mt_doserrno>()); }  // 0x7bbfbe
auto joan::orig::libc::_fcvt(double a0, std::int32_t a1, void* a2, void* a3) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::mt_fcvt>(a0, a1, a2, a3)); }  // 0x7c616e

// crt.hpp: the per-thread gmtime buffer slot for time.cpp (Phase 2, request lib-1).
void*& joan::crt::ptd_gmtimebuf(void* ptd) noexcept { return static_cast<Tiddata*>(ptd)->gmtimebuf; }
