// Original library sources (re/attribution.csv original_file, under lib/): crt/chdir.c, crt/clock.c, crt/dtoxtime.c,
//   crt/findfile.c, crt/getcwd.c, crt/gmtime.c, crt/localtim.c, crt/mkdir.c, crt/rename.c, crt/time.c, crt/tzset.c,
//   crt/unlink.c.
//
// Batch 114 / list 12: CRT family H (time + time zone + _find*) and the errno-only file-system
// wrappers of the statically linked MSVC 6 LIBCMT in joan.exe. VC6 sources: time.c, localtim.c,
// gmtime.c, tzset.c, dtoxtime.c, clock.c, findfile.c, remove.c, unlink.c, mkdir.c, chdir.c,
// getcwd.c, rename.c.
//
// Module crt/time (all cdecl, plain `ret`; entry-patched, no thunks):
//   0x007b338a int remove(const char*)                      DeleteFileA, _dosmaperr on failure
//   0x007b33b4 int _unlink(const char*)                     remove() through 0x007b338a
//   0x007b33bf time_t time(time_t*)                         GetLocalTime / GetSystemTime, dst cache
//   0x007b349b int _mkdir(const char*)                      CreateDirectoryA(path, NULL)
//   0x007b3719 long _findfirst(const char*, _finddata_t*)   FindFirstFileA
//   0x007b37e6 int _findnext(long, _finddata_t*)            FindNextFileA
//   0x007b38ae int _findclose(long)                         FindClose
//   0x007b38ce static time_t __timet_from_ft(FILETIME*)     FileTimeToLocalFileTime + __loctotime_t
//   0x007b3ff3 int _chdir(const char*)                      SetCurrentDirectoryA + "=X:" env var
//   0x007b4079 char* _getcwd(char*, int)                    _lock(0xc) + _getdcwd_lk(0, ..)
//   0x007b40c9 static char* _getdcwd_lk(int, char*, int)    GetFullPathNameA / GetCurrentDirectoryA
//   0x007b419e static int _validdrive(unsigned)             GetDriveTypeA("X:\\")
//   0x007b41d5 int rename(const char*, const char*)         MoveFileA
//   0x007b466f struct tm* localtime(const time_t*)
//   0x007b6ac5 clock_t clock(void)                          (FILETIME now - start_tics) / 10000 (_aulldiv)
//   0x007b6b08 __inittime (XI initialiser)                  start_tics = GetSystemTimeAsFileTime
//   0x007bd471 void __tzset(void)                           once, under _lock(0xb)
//   0x007bd4b5 static void _tzset_lk(void)                  TZ variable / GetTimeZoneInformation
//   0x007bd73c int _isindst(struct tm*)                     _lock(0xb) + _isindst_lk
//   0x007bd75d static int _isindst_lk(struct tm*)           incl. the VC6 US default rules (first
//                                                           Sunday of April / last Sunday of October,
//                                                           02:00) when the zone came from TZ
//   0x007bd909 static void cvtdate(11 ints)                 dststart / dstend computation
//   0x007bda49 struct tm* gmtime(const time_t*)             per-thread buffer ptd+0x40 (malloc 0x24),
//                                                           static 0x00a6b3a0 when malloc fails
// Module fix/uninit (only with [Fixes] UninitReads=1):
//   0x007bbfc7 time_t __loctotime_t(yr, mo, dy, hr, mn, sc, dstflag)  - see "uninitialised" below.
//
// Surface (rule 5.1/1): callers of every function touched, after this module:
//   game / our C++ -> remove (0x4731e0, game_b84_05), _unlink (none: 0x447150 no longer touches files), time (6 game
//   sites, date.cpp / misc.cpp / game_b97_02 / b99_02 / b95_02), _mkdir (0x476fa0, 0x603b20,
//   game_b108_09 / b108_14), _findfirst / _findnext / _findclose (6 / 6 / 4 game sites + 7 modules),
//   _chdir / _getcwd (0x52f930), rename (none since w129: was 0x447150), localtime (0x580430, 0x5804a0,
//   date.cpp), clock (0x58ef30, game_b103_10), __inittime (XI table entry, called by _initterm).
//   Remaining ORIGINAL callers of family internals: __loctotime_t 0x7bbfc7 (when fix/uninit is off)
//   calls __tzset and _isindst through their addresses (= ours); _expandtime 0x7c3c90 calls __tzset
//   but is dead (strftime 0x7c3b7b has no caller / no pointer anywhere in the image); the public
//   _tzset 0x7bd49f (lock 0xb + _tzset_lk) has no caller either and is left as it is (it would call
//   our _tzset_lk). Every other internal (__timet_from_ft, _getdcwd_lk, _validdrive, _tzset_lk,
//   _isindst_lk, cvtdate, gmtime) is only called by functions of this module; they are replaced too
//   and called through their original addresses, so each piece still chains if one is disabled.
//
// State (rule 5.1/2): everything stays at its ORIGINAL address with the original layout, because
// the original __loctotime_t (default) and _expandtime (dead) still read it:
//   _timezone 0xa317a8, _daylight 0xa317ac, _dstbias 0xa317b0, _tzname[2] 0xa31834 / 0xa31838
//   (-> 64-byte buffers 0xa317b4 / 0xa317f4), dststart {yr, yd, ms} 0xa31840, dstend 0xa31850,
//   tzapiused 0xa6b2e0, tzinfo (TIME_ZONE_INFORMATION) 0xa6b2e8, last TZ string 0xa6b394 (heap),
//   __tzset done flag 0xa6b398, time() cache: dst flag 0xa6b1c8 + last UTC SYSTEMTIME (first 16
//   bytes) 0xa6b1d0, clock start_tics (int64) 0xa6b238, gmtime fallback struct tm 0xa6b3a0,
//   __lc_codepage 0xa6b2c8 (read only, locale family). Read-only day tables 0xa320dc..0xa32144
//   (_lpdays / _days, -1 based cumulative days) are embedded as a byte-identical local copy.
//   Since the layout is unchanged, switching the module on at any time (even after the original
//   __inittime / __tzset already ran) is safe.
//
// Other families, all called through their ORIGINAL addresses (rule 5.1/4): _lock 0x7b82aa /
//   _unlock 0x7b830b (MT), _getptd 0x7b8c21, _errno 0x7bbfb5, __doserrno 0x7bbfbe, _dosmaperr
//   0x7bbf42 (MT / errno), malloc 0x7b6c54, free 0x7b4e1d (heap), _getenv_lk 0x7b7ed4 (env, called
//   with lock 0xc held as the original does), atol 0x7b1540, strcmp 0x7b16f0, strlen 0x7b1ef0,
//   strcpy 0x7b2020, strncpy 0x7b1db0 (string), _mbctoupper 0x7bd27e (mbcs). Win32 functions go
//   through the game's IAT slots, like the original `call [slot]`.
//
// Uninitialised read in the original (why __loctotime_t is fix/uninit, not crt/time): VC6
//   __loctotime_t fills only tm_year / tm_yday / tm_mon / tm_hour of its local struct tm before
//   _isindst(&tb) (dstflag == -1, i.e. every _findfirst / _findnext file time, and time() when
//   GetTimeZoneInformation fails); _isindst_lk reads tm_min / tm_sec (stack garbage left by
//   kernel32's FileTimeToSystemTime etc.) when tm_yday is a DST transition day. With fix/uninit
//   off the original 0x7bbfc7 keeps running (it uses our __tzset / _isindst and the in-place
//   state), so crt/time alone is exact. The fix passes tm_min = mn, tm_sec = sc (what later CRTs
//   do), the other unread fields 0.
//
// Notes:
//  - _chdir: the original builds the "=X:" variable name in its own argument slot; a local buffer
//    here (the caller's slot is dead after the call in every caller).
//  - _tzset_lk: the WideCharToMultiByte "used default char" flag is initialised to 0 (Windows
//    always writes it on success, and it is read only on success).
//  - Two original functions at 0x7b40a0 (_getdcwd) and 0x7bd49f (_tzset) are not in functions.json
//    and have no callers; left untouched.
#include "../../callconv.hpp"
#include "../../original/helpers.hpp"
#include "crt.hpp"
#include "../../platform/win32_types.hpp"
#include <cstdint>

#include "../../original/direct.hpp"
#include "../../data_names.hpp"
#include "../../fn_names.hpp"
#include "../../registry.hpp"
#include "../../original/bind.hpp"

namespace {
using joan::hook::global;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using joan::call::cdecl_call;
using joan::call::stdcall_import;

// ---- Win32 structures (same layout as <windows.h>) ----
struct SystemTime {
    u16 wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds;
};
static_assert(sizeof(SystemTime) == 0x10);
struct FileTime {
    u32 lo, hi;
};
struct TimeZoneInfo {
    i32 Bias;
    WCHAR StandardName[32];
    SystemTime StandardDate;
    i32 StandardBias;
    WCHAR DaylightName[32];
    SystemTime DaylightDate;
    i32 DaylightBias;
};
static_assert(sizeof(TimeZoneInfo) == 0xac);
struct Win32FindData {  // WIN32_FIND_DATAA
    u32 dwFileAttributes;
    FileTime ftCreationTime, ftLastAccessTime, ftLastWriteTime;
    u32 nFileSizeHigh, nFileSizeLow, dwReserved0, dwReserved1;
    char cFileName[260];
    char cAlternateFileName[14];
};
static_assert(sizeof(Win32FindData) == 0x140);

// ---- CRT structures ----
struct Tm {  // struct tm
    i32 tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year, tm_wday, tm_yday, tm_isdst;
};
static_assert(sizeof(Tm) == 0x24);
struct FindData {  // struct _finddata_t (VC6)
    u32 attrib;
    i32 time_create, time_access, time_write;
    u32 size;
    char name[260];
};
static_assert(sizeof(FindData) == 0x118);
struct TransitionDate {
    i32 yr, yd, ms;
};

// ---- IAT slots ----
constexpr img::Addr kDeleteFileA = d::iat_KERNEL32_DeleteFileA;
constexpr img::Addr kGetLastError = d::iat_KERNEL32_GetLastError;
constexpr img::Addr kGetLocalTime = d::iat_KERNEL32_GetLocalTime;
constexpr img::Addr kGetSystemTime = d::iat_KERNEL32_GetSystemTime;
constexpr img::Addr kGetTimeZoneInformation = d::iat_KERNEL32_GetTimeZoneInformation;
constexpr img::Addr kCreateDirectoryA = d::iat_KERNEL32_CreateDirectoryA;
constexpr img::Addr kFindFirstFileA = d::iat_KERNEL32_FindFirstFileA;
constexpr img::Addr kFindNextFileA = d::iat_KERNEL32_FindNextFileA;
constexpr img::Addr kFindClose = d::iat_KERNEL32_FindClose;
constexpr img::Addr kFileTimeToLocalFileTime = d::iat_KERNEL32_FileTimeToLocalFileTime;
constexpr img::Addr kFileTimeToSystemTime = d::iat_KERNEL32_FileTimeToSystemTime;
constexpr img::Addr kSetCurrentDirectoryA = d::iat_KERNEL32_SetCurrentDirectoryA;
constexpr img::Addr kGetCurrentDirectoryA = d::iat_KERNEL32_GetCurrentDirectoryA;
constexpr img::Addr kSetEnvironmentVariableA = d::iat_KERNEL32_SetEnvironmentVariableA;
constexpr img::Addr kGetFullPathNameA = d::iat_KERNEL32_GetFullPathNameA;
constexpr img::Addr kGetDriveTypeA = d::iat_KERNEL32_GetDriveTypeA;
constexpr img::Addr kMoveFileA = d::iat_KERNEL32_MoveFileA;
constexpr img::Addr kGetSystemTimeAsFileTime = d::iat_KERNEL32_GetSystemTimeAsFileTime;
constexpr img::Addr kWideCharToMultiByte = d::iat_KERNEL32_WideCharToMultiByte;

u32 get_last_error() { return stdcall_import<u32>(kGetLastError); }

// ---- other CRT families, through their original addresses ----
void lock(i32 n) { joan::orig::libc::_lock(joan::orig::arg::i32(n)); }    // _lock
void unlock(i32 n) { joan::orig::libc::_unlock(joan::orig::arg::i32(n)); }  // _unlock
i32* errno_ptr() { return joan::orig::arg::as<i32*>(joan::orig::libc::_errno()); }       // _errno
u32* doserrno_ptr() { return joan::orig::arg::as<u32*>(joan::orig::libc::__doserrno()); }    // __doserrno
void dosmaperr(u32 oserr) { joan::orig::libc::_dosmaperr(joan::orig::arg::u32(oserr)); }  // _dosmaperr
u8* getptd() { return joan::orig::arg::as<u8*>(joan::orig::libc::_getptd()); }            // _getptd
void* crt_malloc(u32 n) { return joan::orig::arg::as<void*>(joan::orig::malloc_(joan::orig::arg::u32(n))); }
using joan::orig::helpers::crt_free;
char* crt_strcpy(char* d, const char* s) { return std::strcpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s)); }
char* crt_strncpy(char* d, const char* s, u32 n) { return std::strncpy(static_cast<char*>(joan::orig::arg::mptr(d)), joan::orig::arg::str(s), joan::orig::arg::u32(n)); }
u32 crt_strlen(const char* s) { return joan::orig::arg::as<u32>(std::strlen(joan::orig::arg::str(s))); }
using joan::orig::helpers::crt_strcmp;
i32 crt_atol(const char* s) { return joan::orig::arg::as<i32>(joan::orig::libc::atol(joan::orig::arg::str(s))); }
char* getenv_lk(const char* name) { return joan::orig::arg::as<char*>(joan::orig::_getenv_lk(joan::orig::arg::str(name))); }  // _getenv_lk
i32 mbctoupper(u32 c) { return joan::orig::arg::as<i32>(joan::orig::libc::_mbctoupper(joan::orig::arg::u32(c))); }                 // _mbctoupper

// ---- this family, through the original addresses (each one is replaced below) ----
void call_tzset() { joan::orig::libc::__tzset(); }                       // __tzset
void call_tzset_lk() { joan::orig::libc::_tzset_lk(); }                    // _tzset_lk
i32 call_isindst(Tm* tb) { return joan::orig::arg::as<i32>(joan::orig::libc::_isindst(joan::orig::arg::mptr(tb))); }      // _isindst
i32 call_isindst_lk(Tm* tb) { return joan::orig::arg::as<i32>(joan::orig::libc::_isindst_lk(joan::orig::arg::mptr(tb))); }   // _isindst_lk
Tm* call_gmtime(const i32* t) { return joan::orig::arg::as<Tm*>(joan::orig::libc::gmtime(joan::orig::arg::mptr(t))); }  // gmtime
i32 call_loctotime(i32 yr, i32 mo, i32 dy, i32 hr, i32 mn, i32 sc, i32 dst) {
    return joan::orig::arg::as<i32>(joan::orig::libc::__loctotime_t(joan::orig::arg::i32(yr), joan::orig::arg::i32(mo), joan::orig::arg::i32(dy), joan::orig::arg::i32(hr), joan::orig::arg::i32(mn), joan::orig::arg::i32(sc), joan::orig::arg::i32(dst)));  // __loctotime_t
}
i32 call_timet_from_ft(const FileTime* ft) { return joan::orig::arg::as<i32>(joan::orig::libc::__timet_from_ft(joan::orig::arg::mptr(ft))); }  // __timet_from_ft
void call_cvtdate(i32 trantype, i32 datetype, i32 year, i32 month, i32 week, i32 dayofweek, i32 date, i32 hour,
                  i32 min, i32 sec, i32 msec) {
    joan::orig::libc::cvtdate(joan::orig::arg::i32(trantype), joan::orig::arg::i32(datetype), joan::orig::arg::i32(year), joan::orig::arg::i32(month), joan::orig::arg::i32(week), joan::orig::arg::i32(dayofweek), joan::orig::arg::i32(date), joan::orig::arg::i32(hour), joan::orig::arg::i32(min), joan::orig::arg::i32(sec), joan::orig::arg::i32(msec));
}
i32 call_remove(const char* p) { return joan::orig::arg::as<i32>(joan::orig::libc::remove(joan::orig::arg::str(p))); }
char* call_getdcwd_lk(i32 drive, char* buf, i32 maxlen) { return joan::orig::arg::as<char*>(joan::orig::libc::_getdcwd_lk(joan::orig::arg::i32(drive), joan::orig::arg::str(buf), joan::orig::arg::i32(maxlen))); }
i32 call_validdrive(u32 drive) { return joan::orig::arg::as<i32>(joan::orig::libc::_validdrive(joan::orig::arg::u32(drive))); }

// ---- state at the original addresses ----
i32& g_timezone() { return global<i32>(d::g_timezone); }
i32& g_daylight() { return global<i32>(d::g_daylight); }
i32& g_dstbias() { return global<i32>(d::g_dstbias); }
char*& g_tzname(int i) { return global<char*>(d::strs_a31834 + 4 * static_cast<std::uintptr_t>(i)); }
TransitionDate& g_dststart() { return global<TransitionDate>(d::g_dststart); }
TransitionDate& g_dstend() { return global<TransitionDate>(d::g_dstend); }
i32& g_tzapiused() { return global<i32>(d::g_tzapiused); }
TimeZoneInfo& g_tzinfo() { return global<TimeZoneInfo>(d::g_tzinfo); }
char*& g_lastTZ() { return global<char*>(d::g_lastTZ); }
i32& g_tz_done() { return global<i32>(d::g_tz_done); }
i32& g_time_dstflag() { return global<i32>(d::g_time_dstflag); }
SystemTime& g_time_gmt() { return global<SystemTime>(d::g_time_gmt); }
u32& g_start_lo() { return global<u32>(d::g_start_lo); }
u32& g_start_hi() { return global<u32>(d::g_start_hi); }
constexpr img::Addr kStaticTm(d::g_staticTm);
u32 lc_codepage() { return global<u32>(d::g_lc_codepage); }

// Byte-identical copy of the CRT day tables 0x00a320dc..0x00a32144 (27 dwords):
//   [0] = 0x00a320dc (dword before _lpdays), [1..13] = _lpdays 0x00a320e0, [14..26] = _days 0x00a32114.
constexpr i32 kDayTab[27] = {0,  -1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365,
                             -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364};
// Reads the dword at address `base + 4 * index` inside 0x00a320dc..0x00a32144.
i32 daytab(img::Addr base, i32 index) {
    return kDayTab[(base - d::g_a320dc) / 4 + static_cast<std::uintptr_t>(static_cast<u32>(index))];
}
constexpr img::Addr kLpDays = d::g_lpDays;  // _lpdays
constexpr img::Addr kDays = d::g_days;    // _days

i32 wrap(u32 v) { return static_cast<i32>(v); }
u32 w(i32 v) { return static_cast<u32>(v); }

JOAN_COUNTER(c_time, "crt::time");
JOAN_COUNTER(c_localtime, "crt::localtime");
JOAN_COUNTER(c_gmtime, "crt::gmtime");
JOAN_COUNTER(c_tzset_lk, "crt::_tzset_lk");
JOAN_COUNTER(c_isindst, "crt::_isindst_lk");
JOAN_COUNTER(c_loctotime, "crt::__loctotime_t (fix/uninit)");
JOAN_COUNTER(c_findfirst, "crt::_findfirst");
JOAN_COUNTER(c_findnext, "crt::_findnext");
JOAN_COUNTER(c_ft, "crt::__timet_from_ft");
JOAN_COUNTER(c_chdir, "crt::_chdir");
JOAN_COUNTER(c_getdcwd, "crt::_getdcwd_lk");

// ------------------------------------------------------------------------ 0x007b338a remove
i32 JOAN_CDECL crt_remove(const char* path) {
    u32 err = stdcall_import<i32>(kDeleteFileA, path) == 0 ? get_last_error() : 0;
    if (err != 0) {
        dosmaperr(err);
        return -1;
    }
    return 0;
}

// ------------------------------------------------------------------------ 0x007b33b4 _unlink
i32 JOAN_CDECL crt_unlink(const char* path) { return call_remove(path); }

// ------------------------------------------------------------------------ 0x007b349b _mkdir
i32 JOAN_CDECL crt_mkdir(const char* path) {
    u32 err = stdcall_import<i32>(kCreateDirectoryA, path, static_cast<void*>(nullptr)) == 0 ? get_last_error() : 0;
    if (err != 0) {
        dosmaperr(err);
        return -1;
    }
    return 0;
}

// ------------------------------------------------------------------------ 0x007b41d5 rename
i32 JOAN_CDECL crt_rename(const char* oldname, const char* newname) {
    u32 err = stdcall_import<i32>(kMoveFileA, oldname, newname) == 0 ? get_last_error() : 0;
    if (err != 0) {
        dosmaperr(err);
        return -1;
    }
    return 0;
}

// ------------------------------------------------------------------------ 0x007b33bf time
i32 JOAN_CDECL crt_time(i32* timer) {
    c_time.hit();
    SystemTime loct{}, gmt{};
    stdcall_import<void>(kGetLocalTime, &loct);
    stdcall_import<void>(kGetSystemTime, &gmt);
    SystemTime& cache = g_time_gmt();
    i32 dstflag;
    if (gmt.wMinute == cache.wMinute && gmt.wHour == cache.wHour && gmt.wDay == cache.wDay &&
        gmt.wMonth == cache.wMonth && gmt.wYear == cache.wYear) {
        dstflag = g_time_dstflag();
    } else {
        TimeZoneInfo tzi;
        const u32 r = stdcall_import<u32>(kGetTimeZoneInformation, &tzi);
        if (r == 0xffffffffu)
            dstflag = -1;
        else if (r == 2 && tzi.DaylightDate.wMonth != 0 && tzi.DaylightBias != 0)
            dstflag = 1;
        else
            dstflag = 0;
        cache = gmt;  // movsd x4
        g_time_dstflag() = dstflag;
    }
    const i32 t = call_loctotime(loct.wYear, loct.wMonth, loct.wDay, loct.wHour, loct.wMinute, loct.wSecond, dstflag);
    if (timer != nullptr) *timer = t;
    return t;
}

// ------------------------------------------------------------------------ 0x007b38ce __timet_from_ft
i32 JOAN_CDECL timet_from_ft(const FileTime* pft) {
    c_ft.hit();
    if (pft->lo != 0 || pft->hi != 0) {
        FileTime lft;
        if (stdcall_import<i32>(kFileTimeToLocalFileTime, pft, &lft) != 0) {
            SystemTime st;
            if (stdcall_import<i32>(kFileTimeToSystemTime, &lft, &st) != 0)
                return call_loctotime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, -1);
        }
    }
    return -1;
}

// Common error mapping of _findfirst / _findnext.
void find_error() {
    const u32 e = get_last_error();
    i32 code;
    if (e < 2)
        code = 0x16;  // EINVAL
    else if (e <= 3 || e == 0x12)
        code = 2;  // ENOENT
    else if (e == 8)
        code = 0xc;  // ENOMEM
    else
        code = 0x16;
    *errno_ptr() = code;
}

void fill_finddata(FindData* fd, Win32FindData& wfd) {
    fd->attrib = wfd.dwFileAttributes == 0x80 ? 0 : wfd.dwFileAttributes;  // FILE_ATTRIBUTE_NORMAL -> 0
    fd->time_create = call_timet_from_ft(&wfd.ftCreationTime);
    fd->time_access = call_timet_from_ft(&wfd.ftLastAccessTime);
    fd->time_write = call_timet_from_ft(&wfd.ftLastWriteTime);
    fd->size = wfd.nFileSizeLow;
    crt_strcpy(fd->name, wfd.cFileName);
}

// ------------------------------------------------------------------------ 0x007b3719 _findfirst
i32 JOAN_CDECL crt_findfirst(const char* spec, FindData* fd) {
    c_findfirst.hit();
    Win32FindData wfd;
    // VC6 keeps the search handle in a long; the platform's handles have 32 significant bits, INVALID_HANDLE_VALUE is -1
    const std::intptr_t h = stdcall_import<std::intptr_t>(kFindFirstFileA, spec, &wfd);
    if (h == -1) {
        find_error();
        return -1;
    }
    fill_finddata(fd, wfd);
    return static_cast<i32>(h);
}

// ------------------------------------------------------------------------ 0x007b37e6 _findnext
i32 JOAN_CDECL crt_findnext(i32 handle, FindData* fd) {
    c_findnext.hit();
    Win32FindData wfd;
    if (stdcall_import<i32>(kFindNextFileA, reinterpret_cast<void*>(static_cast<std::intptr_t>(handle)), &wfd) == 0) {
        find_error();
        return -1;
    }
    fill_finddata(fd, wfd);
    return 0;
}

// ------------------------------------------------------------------------ 0x007b38ae _findclose
i32 JOAN_CDECL crt_findclose(i32 handle) {
    if (stdcall_import<i32>(kFindClose, reinterpret_cast<void*>(static_cast<std::intptr_t>(handle))) == 0) {
        *errno_ptr() = 0x16;
        return -1;
    }
    return 0;
}

// ------------------------------------------------------------------------ 0x007b3ff3 _chdir
i32 JOAN_CDECL crt_chdir(const char* path) {
    c_chdir.hit();
    char cwd[0x108];
    if (stdcall_import<i32>(kSetCurrentDirectoryA, path) != 0 &&
        stdcall_import<u32>(kGetCurrentDirectoryA, 0x105u, static_cast<char*>(cwd)) != 0) {
        const char c = cwd[0];
        if ((c == '\\' || c == '/') && c == cwd[1]) return 0;  // UNC path: no per-drive variable
        char name[4];
        name[0] = '=';
        name[1] = static_cast<char>(mbctoupper(static_cast<u8>(c)));
        name[2] = ':';
        name[3] = '\0';
        if (stdcall_import<i32>(kSetEnvironmentVariableA, static_cast<const char*>(name), static_cast<const char*>(cwd)) != 0)
            return 0;
    }
    dosmaperr(get_last_error());
    return -1;
}

// ------------------------------------------------------------------------ 0x007b419e _validdrive
i32 JOAN_CDECL validdrive(u32 drive) {
    if (drive == 0) return 1;
    char root[4];
    root[0] = static_cast<char>(static_cast<u8>(drive) + 0x40);
    root[1] = ':';
    root[2] = '\\';
    root[3] = '\0';
    const u32 type = stdcall_import<u32>(kGetDriveTypeA, static_cast<const char*>(root));
    if (type == 0 || type == 1) return 0;  // DRIVE_UNKNOWN / DRIVE_NO_ROOT_DIR
    return 1;
}

// ------------------------------------------------------------------------ 0x007b40c9 _getdcwd_lk
char* JOAN_CDECL getdcwd_lk(i32 drive, char* buf, i32 maxlen) {
    c_getdcwd.hit();
    char path[0x104];
    u32 len;
    if (drive != 0) {
        if (call_validdrive(static_cast<u32>(drive)) == 0) {
            *doserrno_ptr() = 0xf;  // ERROR_INVALID_DRIVE
            *errno_ptr() = 0xd;     // EACCES
            return nullptr;
        }
        char drvpath[4];
        drvpath[0] = static_cast<char>(static_cast<u8>(drive) + 0x40);
        drvpath[1] = ':';
        drvpath[2] = '.';
        drvpath[3] = '\0';
        char* filepart = nullptr;
        len = stdcall_import<u32>(kGetFullPathNameA, static_cast<const char*>(drvpath), 0x104u, static_cast<char*>(path),
                                  &filepart);
    } else {
        len = stdcall_import<u32>(kGetCurrentDirectoryA, 0x104u, static_cast<char*>(path));
    }
    if (len == 0) return nullptr;
    ++len;
    if (len > 0x104u) return nullptr;
    char* out = buf;
    if (out == nullptr) {
        const i32 n = static_cast<i32>(len) > maxlen ? static_cast<i32>(len) : maxlen;
        out = static_cast<char*>(crt_malloc(static_cast<u32>(n)));
        if (out == nullptr) {
            *errno_ptr() = 0xc;  // ENOMEM
            return nullptr;
        }
    } else if (static_cast<i32>(len) > maxlen) {
        *errno_ptr() = 0x22;  // ERANGE
        return nullptr;
    }
    crt_strcpy(out, path);
    return out;
}

// ------------------------------------------------------------------------ 0x007b4079 _getcwd
char* JOAN_CDECL crt_getcwd(char* buf, i32 maxlen) {
    lock(0xc);
    char* r = call_getdcwd_lk(0, buf, maxlen);
    unlock(0xc);
    return r;
}

// ------------------------------------------------------------------------ 0x007b6ac5 clock
i32 JOAN_CDECL crt_clock() {
    FileTime ft;
    stdcall_import<void>(kGetSystemTimeAsFileTime, &ft);
    const u64 now = (static_cast<u64>(ft.hi) << 32) + ft.lo;
    const u64 start = (static_cast<u64>(g_start_hi()) << 32) | g_start_lo();
    // 0x7b2f30 is _aulldiv (UNSIGNED 64-bit divide): a clock set back below start_tics gives a huge
    // unsigned quotient, not a small negative one.
    return static_cast<i32>(static_cast<u32>((now - start) / 10000u));
}

// ------------------------------------------------------------------------ 0x007b6b08 __inittime
u32 JOAN_CDECL inittime() {
    FileTime ft;
    stdcall_import<void>(kGetSystemTimeAsFileTime, &ft);
    g_start_lo() = ft.lo;
    g_start_hi() = ft.hi;
    return ft.lo;  // eax as the original leaves it (XI initialisers' results are ignored)
}

// ------------------------------------------------------------------------ 0x007bd471 __tzset
void JOAN_CDECL tzset_once() {
    if (g_tz_done() != 0) return;
    lock(0xb);
    if (g_tz_done() == 0) {
        call_tzset_lk();
        ++g_tz_done();
    }
    unlock(0xb);
}

// Copies one zone name with WideCharToMultiByte like _tzset_lk.
void tzname_from_api(char* dst, const WCHAR* src) {
    i32 used_default = 0;
    const i32 r = stdcall_import<i32>(kWideCharToMultiByte, lc_codepage(), 0x220u, src, -1, dst, 0x3f,
                                      static_cast<const char*>(nullptr), &used_default);
    if (r != 0 && used_default == 0)
        dst[0x3f] = '\0';
    else
        dst[0] = '\0';
}

bool is_digit(char c) { return static_cast<signed char>(c) >= '0' && static_cast<signed char>(c) <= '9'; }

// ------------------------------------------------------------------------ 0x007bd4b5 _tzset_lk
void JOAN_CDECL tzset_lk() {
    c_tzset_lk.hit();
    lock(0xc);
    g_dstend().yr = -1;
    g_dststart().yr = -1;
    g_tzapiused() = 0;
    const char* tz = getenv_lk("TZ");  // .rdata 0x009e7354
    if (tz == nullptr) {
        unlock(0xc);
        TimeZoneInfo& tzi = g_tzinfo();
        if (stdcall_import<u32>(kGetTimeZoneInformation, &tzi) == 0xffffffffu) return;
        const i32 stdbias = tzi.StandardBias;
        g_timezone() = wrap(w(tzi.Bias) * 60u);
        g_tzapiused() = 1;
        if (tzi.StandardDate.wMonth != 0) g_timezone() = wrap(w(g_timezone()) + w(stdbias) * 60u);
        if (tzi.DaylightDate.wMonth != 0 && tzi.DaylightBias != 0) {
            g_daylight() = 1;
            g_dstbias() = wrap((w(tzi.DaylightBias) - w(stdbias)) * 60u);
        } else {
            g_daylight() = 0;
            g_dstbias() = 0;
        }
        tzname_from_api(g_tzname(0), tzi.StandardName);
        tzname_from_api(g_tzname(1), tzi.DaylightName);
        return;
    }
    if (*tz == '\0' || (g_lastTZ() != nullptr && crt_strcmp(tz, g_lastTZ()) == 0)) {
        unlock(0xc);
        return;
    }
    crt_free(g_lastTZ());
    g_lastTZ() = static_cast<char*>(crt_malloc(crt_strlen(tz) + 1));
    if (g_lastTZ() == nullptr) {
        unlock(0xc);
        return;
    }
    crt_strcpy(g_lastTZ(), tz);
    unlock(0xc);

    crt_strncpy(g_tzname(0), tz, 3);
    g_tzname(0)[3] = '\0';
    tz += 3;
    bool negdiff = false;
    if (*tz == '-') {
        negdiff = true;
        ++tz;
    }
    u32 zone = w(crt_atol(tz)) * 3600u;
    g_timezone() = wrap(zone);
    while (*tz == '+' || is_digit(*tz)) ++tz;
    if (*tz == ':') {
        ++tz;
        zone = w(g_timezone()) + w(crt_atol(tz)) * 60u;
        g_timezone() = wrap(zone);
        while (is_digit(*tz)) ++tz;
        if (*tz == ':') {
            ++tz;
            zone = w(g_timezone()) + w(crt_atol(tz));
            g_timezone() = wrap(zone);
            while (is_digit(*tz)) ++tz;
        }
    }
    if (negdiff) g_timezone() = wrap(0u - zone);
    g_daylight() = static_cast<signed char>(*tz);
    if (g_daylight() != 0) {
        crt_strncpy(g_tzname(1), tz, 3);
        g_tzname(1)[3] = '\0';
    } else {
        g_tzname(1)[0] = '\0';
    }
}

// ------------------------------------------------------------------------ 0x007bd909 cvtdate
void JOAN_CDECL cvtdate(i32 trantype, i32 datetype, i32 year, i32 month, i32 week, i32 dayofweek, i32 date, i32 hour,
                     i32 min, i32 sec, i32 msec) {
    i32 yearday;
    const bool leap = (year & 3) == 0;
    if (datetype == 1) {
        yearday = wrap(w(daytab(leap ? kLpDays - 4 : kDays - 4, month)) + 1u);
        const i32 monthdow =
            wrap(w(year) * 365u + w(yearday) + w((year - 1) >> 2) - 0x63dbu) % 7;  // idiv: truncating
        if (monthdow < dayofweek)
            yearday = wrap(w(yearday) + (w(week) * 7u - w(monthdow) + w(dayofweek)) - 7u);
        else
            yearday = wrap(w(yearday) + (w(week) * 7u - w(monthdow) + w(dayofweek)));
        if (week == 5 && yearday > daytab(leap ? kLpDays : kDays, month)) yearday -= 7;
    } else {
        yearday = wrap(w(daytab(leap ? kLpDays - 4 : kDays - 4, month)) + w(date));
    }
    if (trantype == 1) {
        TransitionDate& s = g_dststart();
        s.yd = yearday;
        s.yr = year;
        s.ms = wrap(((w(hour) * 60u + w(min)) * 60u + w(sec)) * 1000u + w(msec));
    } else {
        TransitionDate& e = g_dstend();
        e.yd = yearday;
        i32 ms = wrap(((w(hour) * 60u + w(min)) * 60u + w(g_dstbias()) + w(sec)) * 1000u + w(msec));
        e.ms = ms;
        if (ms < 0) {
            e.ms = wrap(w(ms) + 0x5265c00u);
            e.yd = yearday - 1;
        } else if (ms >= 0x5265c00) {
            e.ms = ms - 0x5265c00;
            e.yd = yearday + 1;
        }
        e.yr = year;
    }
}

// ------------------------------------------------------------------------ 0x007bd75d _isindst_lk
i32 JOAN_CDECL isindst_lk(Tm* tb) {
    c_isindst.hit();
    if (g_daylight() == 0) return 0;
    const i32 yr = tb->tm_year;
    if (yr != g_dststart().yr || yr != g_dstend().yr) {
        if (g_tzapiused() != 0) {
            const SystemTime& d = g_tzinfo().DaylightDate;
            if (d.wYear == 0)
                call_cvtdate(1, 1, yr, d.wMonth, d.wDay, d.wDayOfWeek, 0, d.wHour, d.wMinute, d.wSecond, d.wMilliseconds);
            else
                call_cvtdate(1, 0, yr, d.wMonth, 0, 0, d.wDay, d.wHour, d.wMinute, d.wSecond, d.wMilliseconds);
            const SystemTime& s = g_tzinfo().StandardDate;
            if (s.wYear == 0)
                call_cvtdate(0, 1, tb->tm_year, s.wMonth, s.wDay, s.wDayOfWeek, 0, s.wHour, s.wMinute, s.wSecond,
                             s.wMilliseconds);
            else
                call_cvtdate(0, 0, tb->tm_year, s.wMonth, 0, 0, s.wDay, s.wHour, s.wMinute, s.wSecond, s.wMilliseconds);
        } else {
            // the VC6 US rules: first Sunday in April / last Sunday in October, 02:00
            call_cvtdate(1, 1, yr, 4, 1, 0, 0, 2, 0, 0, 0);
            call_cvtdate(0, 1, tb->tm_year, 10, 5, 0, 0, 2, 0, 0, 0);
        }
    }
    const i32 start = g_dststart().yd, end = g_dstend().yd, yday = tb->tm_yday;
    if (start < end) {  // northern hemisphere
        if (yday < start || yday > end) return 0;
        if (yday > start && yday < end) return 1;
    } else {  // southern hemisphere
        if (yday < end || yday > start) return 1;
        if (yday > end && yday < start) return 0;
    }
    const i32 ms = wrap(((w(tb->tm_hour) * 60u + w(tb->tm_min)) * 60u + w(tb->tm_sec)) * 1000u);
    if (yday == start) return ms >= g_dststart().ms ? 1 : 0;
    return ms < g_dstend().ms ? 1 : 0;
}

// ------------------------------------------------------------------------ 0x007bd73c _isindst
i32 JOAN_CDECL isindst(Tm* tb) {
    lock(0xb);
    const i32 r = call_isindst_lk(tb);
    unlock(0xb);
    return r;
}

// ------------------------------------------------------------------------ 0x007bda49 gmtime
Tm* JOAN_CDECL crt_gmtime(const i32* timp) {
    c_gmtime.hit();
    i32 caltim = *timp;
    bool islpyr = false;
    u8* ptd = getptd();
    if (caltim < 0) return nullptr;
    // _tiddata::_gmtimebuf (lib/crt/mt.cpp Tiddata::gmtimebuf, through crt.hpp: no raw offset)
    Tm*& buf = reinterpret_cast<Tm*&>(joan::crt::ptd_gmtimebuf(ptd));
    Tm* ptm;
    if (buf != nullptr) {
        ptm = buf;
    } else {
        buf = static_cast<Tm*>(crt_malloc(sizeof(Tm)));
        ptm = buf != nullptr ? buf : &img::global<Tm>(kStaticTm);
    }
    i32 tmptim = caltim / 126230400;  // _FOUR_YEAR_SEC
    caltim -= tmptim * 126230400;
    tmptim = tmptim * 4 + 70;
    if (caltim >= 31536000) {  // _YEAR_SEC
        ++tmptim;
        caltim -= 31536000;
        if (caltim >= 31536000) {
            ++tmptim;
            caltim -= 31536000;
            if (caltim >= 31622400) {  // _YEAR_SEC + _DAY_SEC
                ++tmptim;
                caltim -= 31622400;
            } else {
                islpyr = true;
            }
        }
    }
    ptm->tm_year = tmptim;
    ptm->tm_yday = caltim / 86400;
    caltim -= ptm->tm_yday * 86400;
    const img::Addr mdays = islpyr ? kLpDays : kDays;
    i32 i = 1;
    while (daytab(mdays, i) < ptm->tm_yday) ++i;
    ptm->tm_mon = --i;
    ptm->tm_mday = ptm->tm_yday - daytab(mdays, i);
    ptm->tm_wday = (*timp / 86400 + 4) % 7;  // *timp read again, as the original does
    ptm->tm_hour = caltim / 3600;
    caltim -= ptm->tm_hour * 3600;
    ptm->tm_min = caltim / 60;
    ptm->tm_isdst = 0;
    ptm->tm_sec = caltim - ptm->tm_min * 60;
    return ptm;
}

// ------------------------------------------------------------------------ 0x007b466f localtime
Tm* JOAN_CDECL crt_localtime(const i32* ptime) {
    c_localtime.hit();
    if (*ptime < 0) return nullptr;
    call_tzset();
    i32 ltime = *ptime;
    Tm* ptm;
    if (ltime > 3 * 86400 && ltime < 0x7ffc0b7f) {
        ltime = wrap(w(ltime) - w(g_timezone()));
        ptm = call_gmtime(&ltime);
        if (g_daylight() != 0 && call_isindst(ptm) != 0) {
            ltime = wrap(w(ltime) - w(g_dstbias()));
            ptm = call_gmtime(&ltime);
            ptm->tm_isdst = 1;
        }
        return ptm;
    }
    ptm = call_gmtime(ptime);
    const bool dst = call_isindst(ptm) != 0;
    u32 v = w(ptm->tm_sec);
    if (dst) v -= w(g_dstbias());
    ltime = wrap(v - w(g_timezone()));

    ptm->tm_sec = ltime % 60;
    if (ptm->tm_sec < 0) {
        ptm->tm_sec += 60;
        ltime -= 60;
    }
    ltime = wrap(w(ltime / 60) + w(ptm->tm_min));
    ptm->tm_min = ltime % 60;
    if (ptm->tm_min < 0) {
        ptm->tm_min += 60;
        ltime -= 60;
    }
    ltime = wrap(w(ltime / 60) + w(ptm->tm_hour));
    ptm->tm_hour = ltime % 24;
    if (ptm->tm_hour < 0) {
        ptm->tm_hour += 24;
        ltime -= 24;
    }
    ltime /= 24;
    if (ltime > 0) {
        ptm->tm_wday = (ptm->tm_wday + ltime) % 7;
        ptm->tm_mday += ltime;
        ptm->tm_yday += ltime;
    } else if (ltime < 0) {
        ptm->tm_wday = (ptm->tm_wday + ltime + 7) % 7;
        ptm->tm_mday += ltime;
        if (ptm->tm_mday > 0) {
            ptm->tm_yday += ltime;
        } else {
            ptm->tm_mday += 31;
            ptm->tm_yday = 364;
            ptm->tm_mon = 11;
            --ptm->tm_year;
        }
    }
    return ptm;
}

// ------------------------------------------------------------------------ 0x007bbfc7 __loctotime_t
// fix/uninit: the original leaves tb.tm_min / tb.tm_sec (read by _isindst_lk on DST transition
// days) uninitialised; they are mn / sc here.
i32 JOAN_CDECL loctotime(i32 yr, i32 mo, i32 dy, i32 hr, i32 mn, i32 sc, i32 dstflag) {
    c_loctotime.hit();
    yr -= 1900;
    if (yr < 70 || yr > 138) return -1;
    i32 tmpdays = wrap(w(daytab(kDays - 4, mo)) + w(dy));
    if ((yr & 3) == 0 && mo > 2) ++tmpdays;
    call_tzset();
    const u32 days = w(yr) * 365u + w(tmpdays) + w((yr - 1) >> 2);
    u32 t = (days * 24u + w(hr)) * 60u + w(mn);
    t = t * 60u + w(g_timezone());
    t = t + w(sc) + 0x7c558180u;  // - (70 * 365 + 17) * 86400
    Tm tb{};
    tb.tm_yday = tmpdays;
    tb.tm_year = yr;
    tb.tm_mon = mo - 1;
    tb.tm_hour = hr;
    tb.tm_min = mn;
    tb.tm_sec = sc;
    if (dstflag == 1 || (dstflag == -1 && g_daylight() != 0 && call_isindst(&tb) != 0)) t += w(g_dstbias());
    return wrap(t);
}

}  // namespace

#define CRT_TIME(name, addr, fn) JOAN_REPLACE("crt/time", name, addr, fn)
CRT_TIME("crt::remove", 0x007b338a, &crt_remove);
CRT_TIME("crt::_unlink", 0x007b33b4, &crt_unlink);
CRT_TIME("crt::time", 0x007b33bf, &crt_time);
CRT_TIME("crt::_mkdir", 0x007b349b, &crt_mkdir);
CRT_TIME("crt::_findfirst", 0x007b3719, &crt_findfirst);
CRT_TIME("crt::_findnext", 0x007b37e6, &crt_findnext);
CRT_TIME("crt::_findclose", 0x007b38ae, &crt_findclose);
CRT_TIME("crt::__timet_from_ft", 0x007b38ce, &timet_from_ft);
CRT_TIME("crt::_chdir", 0x007b3ff3, &crt_chdir);
CRT_TIME("crt::_getcwd", 0x007b4079, &crt_getcwd);
CRT_TIME("crt::_getdcwd_lk", 0x007b40c9, &getdcwd_lk);
CRT_TIME("crt::_validdrive", 0x007b419e, &validdrive);
CRT_TIME("crt::rename", 0x007b41d5, &crt_rename);
CRT_TIME("crt::localtime", 0x007b466f, &crt_localtime);
CRT_TIME("crt::clock", 0x007b6ac5, &crt_clock);
CRT_TIME("crt::__inittime", 0x007b6b08, &inittime);
CRT_TIME("crt::__tzset", 0x007bd471, &tzset_once);
CRT_TIME("crt::_tzset_lk", 0x007bd4b5, &tzset_lk);
CRT_TIME("crt::_isindst", 0x007bd73c, &isindst);
CRT_TIME("crt::_isindst_lk", 0x007bd75d, &isindst_lk);
CRT_TIME("crt::cvtdate", 0x007bd909, &cvtdate);
CRT_TIME("crt::gmtime", 0x007bda49, &crt_gmtime);
#undef CRT_TIME
JOAN_REPLACE("fix/uninit", "crt::__loctotime_t", 0x007bbfc7, &loctotime);

// ---------------------------------------- direct calls of the facades (re/bind/gen_bridges.py)
auto joan::orig::libc::remove(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_remove>(a0)); }  // 0x7b338a
auto joan::orig::libc::time(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_time>(a0)); }  // 0x7b33bf
auto joan::orig::libc::_mkdir(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_mkdir>(a0)); }  // 0x7b349b
auto joan::orig::libc::_findfirst(const char* a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_findfirst>(a0, a1)); }  // 0x7b3719
auto joan::orig::libc::_findnext(std::int32_t a0, void* a1) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_findnext>(a0, a1)); }  // 0x7b37e6
auto joan::orig::libc::_findclose(std::int32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_findclose>(a0)); }  // 0x7b38ae
auto joan::orig::libc::__timet_from_ft(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::timet_from_ft>(a0)); }  // 0x7b38ce
auto joan::orig::libc::_chdir(const char* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_chdir>(a0)); }  // 0x7b3ff3
auto joan::orig::libc::_getcwd(const char* a0, std::int32_t a1) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::crt_getcwd>(a0, a1)); }  // 0x7b4079
auto joan::orig::libc::_getdcwd_lk(std::int32_t a0, const char* a1, std::int32_t a2) -> const char* { return joan::bind::conv<const char*>(joan::bind::free<&::getdcwd_lk>(a0, a1, a2)); }  // 0x7b40c9
auto joan::orig::libc::_validdrive(std::uint32_t a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::validdrive>(a0)); }  // 0x7b419e
auto joan::orig::libc::clock() -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::crt_clock>()); }  // 0x7b6ac5
auto joan::orig::libc::__loctotime_t(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::loctotime>(a0, a1, a2, a3, a4, a5, a6)); }  // 0x7bbfc7
auto joan::orig::libc::__tzset() -> void { joan::bind::free<&::tzset_once>(); }  // 0x7bd471
auto joan::orig::libc::_tzset_lk() -> void { joan::bind::free<&::tzset_lk>(); }  // 0x7bd4b5
auto joan::orig::libc::_isindst(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::isindst>(a0)); }  // 0x7bd73c
auto joan::orig::libc::_isindst_lk(void* a0) -> std::int32_t { return joan::bind::conv<std::int32_t>(joan::bind::free<&::isindst_lk>(a0)); }  // 0x7bd75d
auto joan::orig::libc::cvtdate(std::int32_t a0, std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4, std::int32_t a5, std::int32_t a6, std::int32_t a7, std::int32_t a8, std::int32_t a9, std::int32_t a10) -> void { joan::bind::free<&::cvtdate>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }  // 0x7bd909
auto joan::orig::libc::gmtime(void* a0) -> void* { return joan::bind::conv<void*>(joan::bind::free<&::crt_gmtime>(a0)); }  // 0x7bda49
