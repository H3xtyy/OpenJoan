// joan.exe's remaining small system imports on portable code (phase C5 round F2, src/platform/README.md):
//   - critical sections (KERNEL32 Initialize / Enter / Leave / DeleteCriticalSection: the streaming queue's lock,
//     the C runtime's locks): a recursive mutex per CRITICAL_SECTION address, kept in a table of ours - the game's
//     CRITICAL_SECTION bytes stay zero (deterministic for the replay's state hashes, where Windows wrote debug-info
//     pointers and thread ids);
//   - InterlockedIncrement / InterlockedDecrement (reference counts): atomic builtins;
//   - the clock and calendar: GetSystemTimeAsFileTime, GetSystemTime, GetLocalTime (std::chrono and the C library's
//     local time), GetTimeZoneInformation (the C runtime's tzset: the zone's current offset from UTC, reported as a zone
//     without daylight-saving rules - local times of today are exact; Windows would also shift times on the other side
//     of a DST change, which the game never shows);
//   - COM (ole32): CoInitialize / CoUninitialize - the movie and music players call them around their (formerly
//     Windows Media) decoders, FFmpeg needs no COM; CoCreateInstance has no caller left (the DirectMusic code went
//     in cleanup C1) and fails with REGDB_E_CLASSNOTREG. ole32.dll is then not loaded;
//   - the top-level exception filter (port round P2): SetUnhandledExceptionFilter (the C runtime's
//     __CxxSetUnhandledExceptionFilter / __CxxRestoreUnhandledExceptionFilter, .CRT$XI / XT) and UnhandledExceptionFilter
//     (_XcptFilter, the _threadstart filter: an exception no signal action takes). On Windows they are Windows' (the
//     filter is what Windows runs when an exception leaves a thread; crash.cpp's handler is the one chained behind the
//     C runtime's); elsewhere a filter of our own state that UnhandledExceptionFilter calls (there are no structured
//     exceptions to deliver to it: a fatal signal does not reach it).
#include "win32_types.hpp"

#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <chrono>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "../runtime/loader/import_overrides.hpp"

namespace {

// ---------------------------------------------------------------- critical sections

std::mutex g_cs_lock;
std::unordered_map<const void*, std::unique_ptr<std::recursive_mutex>>& sections() {
    static auto* m = new std::unordered_map<const void*, std::unique_ptr<std::recursive_mutex>>();  // never destroyed:
    return *m;  // the C runtime locks / unlocks during exit, after static destructors
}

std::recursive_mutex& mutex_of(const void* cs) {
    std::lock_guard<std::mutex> l(g_cs_lock);
    auto& m = sections()[cs];
    if (!m) m = std::make_unique<std::recursive_mutex>();  // Enter before Initialize: Windows would crash; we cope
    return *m;
}

void WINAPI j_InitializeCriticalSection(LPCRITICAL_SECTION cs) {
    std::memset(cs, 0, sizeof(*cs));
    std::lock_guard<std::mutex> l(g_cs_lock);
    sections()[cs] = std::make_unique<std::recursive_mutex>();
}
void WINAPI j_EnterCriticalSection(LPCRITICAL_SECTION cs) { mutex_of(cs).lock(); }
void WINAPI j_LeaveCriticalSection(LPCRITICAL_SECTION cs) { mutex_of(cs).unlock(); }
void WINAPI j_DeleteCriticalSection(LPCRITICAL_SECTION cs) {
    std::lock_guard<std::mutex> l(g_cs_lock);
    sections().erase(cs);
}

LONG WINAPI j_InterlockedIncrement(LONG volatile* p) {
#ifdef _MSC_VER
    return _InterlockedIncrement(p);
#else
    return __atomic_add_fetch(p, 1, __ATOMIC_SEQ_CST);
#endif
}
LONG WINAPI j_InterlockedDecrement(LONG volatile* p) {
#ifdef _MSC_VER
    return _InterlockedDecrement(p);
#else
    return __atomic_sub_fetch(p, 1, __ATOMIC_SEQ_CST);
#endif
}

// ---------------------------------------------------------------- time

constexpr std::int64_t kUnixEpoch100ns = 116444736000000000ll;  // 1601-01-01 .. 1970-01-01

// Now: seconds since 1970 and the milliseconds of that second.
void now(std::time_t& secs, unsigned& ms) {
    const auto t = std::chrono::system_clock::now().time_since_epoch();
    const auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t).count();
    secs = static_cast<std::time_t>(total_ms / 1000);
    ms = static_cast<unsigned>(total_ms % 1000);
}

void to_systemtime(const std::tm& t, unsigned ms, SYSTEMTIME* st) {
    st->wYear = static_cast<WORD>(t.tm_year + 1900);
    st->wMonth = static_cast<WORD>(t.tm_mon + 1);
    st->wDayOfWeek = static_cast<WORD>(t.tm_wday);
    st->wDay = static_cast<WORD>(t.tm_mday);
    st->wHour = static_cast<WORD>(t.tm_hour);
    st->wMinute = static_cast<WORD>(t.tm_min);
    st->wSecond = static_cast<WORD>(t.tm_sec);
    st->wMilliseconds = static_cast<WORD>(ms);
}

void WINAPI j_GetSystemTimeAsFileTime(LPFILETIME ft) {
    const auto t = std::chrono::system_clock::now().time_since_epoch();
    const std::int64_t v = std::chrono::duration_cast<std::chrono::nanoseconds>(t).count() / 100 + kUnixEpoch100ns;
    ft->dwLowDateTime = static_cast<DWORD>(static_cast<std::uint64_t>(v) & 0xffffffffu);
    ft->dwHighDateTime = static_cast<DWORD>(static_cast<std::uint64_t>(v) >> 32);
}

void WINAPI j_GetSystemTime(LPSYSTEMTIME st) {
    std::time_t s = 0;
    unsigned ms = 0;
    now(s, ms);
    std::tm t{};
#ifdef _WIN32
    gmtime_s(&t, &s);
#else
    gmtime_r(&s, &t);
#endif
    to_systemtime(t, ms, st);
}

void WINAPI j_GetLocalTime(LPSYSTEMTIME st) {
    std::time_t s = 0;
    unsigned ms = 0;
    now(s, ms);
    std::tm t{};
#ifdef _WIN32
    localtime_s(&t, &s);
#else
    localtime_r(&s, &t);
#endif
    to_systemtime(t, ms, st);
}

// The local zone's offset from UTC right now, in minutes (east positive).
LONG offset_minutes() {
    const std::time_t s = std::time(nullptr);
    std::tm l{};
    std::tm u{};
#ifdef _WIN32
    localtime_s(&l, &s);
    gmtime_s(&u, &s);
#else
    localtime_r(&s, &l);
    gmtime_r(&s, &u);
#endif
    int days = l.tm_yday - u.tm_yday;
    if (l.tm_year != u.tm_year) days = l.tm_year > u.tm_year ? 1 : -1;
    return static_cast<LONG>(days * 1440 + (l.tm_hour - u.tm_hour) * 60 + (l.tm_min - u.tm_min));
}

DWORD WINAPI j_GetTimeZoneInformation(LPTIME_ZONE_INFORMATION tz) {
    std::memset(tz, 0, sizeof(*tz));
    tz->Bias = -offset_minutes();  // UTC = local + Bias
    // The C library's zone name, widened (ASCII in practice: "CET", "UTC", ...).
    std::time_t s = std::time(nullptr);
    std::tm l{};
#ifdef _WIN32
    localtime_s(&l, &s);
#else
    localtime_r(&s, &l);
#endif
    char name[64] = {};
    std::strftime(name, sizeof(name), "%Z", &l);
    for (int i = 0; i < 31 && name[i]; ++i) tz->StandardName[i] = static_cast<WCHAR>(static_cast<unsigned char>(name[i]));
    std::memcpy(tz->DaylightName, tz->StandardName, sizeof(tz->DaylightName));
    return TIME_ZONE_ID_UNKNOWN;  // no daylight-saving rules: the current offset throughout
}

// ---------------------------------------------------------------- COM

thread_local int t_com = 0;

HRESULT WINAPI j_CoInitialize(LPVOID) { return t_com++ == 0 ? S_OK : S_FALSE; }
void WINAPI j_CoUninitialize() {
    if (t_com > 0) --t_com;
}
HRESULT WINAPI j_CoCreateInstance(const GUID*, void*, DWORD, const GUID*, LPVOID* out) {
    if (out) *out = nullptr;
    return static_cast<HRESULT>(0x80040154u);  // REGDB_E_CLASSNOTREG
}

// ---------------------------------------------------------------- top-level exception filter

#ifndef _WIN32
LPTOP_LEVEL_EXCEPTION_FILTER g_filter = nullptr;
#endif

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI j_SetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER filter) {
#ifdef _WIN32
    return ::SetUnhandledExceptionFilter(filter);
#else
    LPTOP_LEVEL_EXCEPTION_FILTER previous = g_filter;
    g_filter = filter;
    return previous;
#endif
}

LONG WINAPI j_UnhandledExceptionFilter(EXCEPTION_POINTERS* info) {
#ifdef _WIN32
    return ::UnhandledExceptionFilter(info);
#else
    return g_filter ? g_filter(info) : EXCEPTION_CONTINUE_SEARCH;
#endif
}

}  // namespace

#define K(name) JOAN_IMPORT_OVERRIDE("KERNEL32.dll", #name, &j_##name, "portable " #name)
K(InitializeCriticalSection);
K(EnterCriticalSection);
K(LeaveCriticalSection);
K(DeleteCriticalSection);
K(InterlockedIncrement);
K(InterlockedDecrement);
K(GetSystemTimeAsFileTime);
K(GetSystemTime);
K(GetLocalTime);
K(GetTimeZoneInformation);
K(SetUnhandledExceptionFilter);
K(UnhandledExceptionFilter);
#undef K
#define O(name) JOAN_IMPORT_OVERRIDE("ole32.dll", #name, &j_##name, "no COM " #name)
O(CoInitialize);
O(CoUninitialize);
O(CoCreateInstance);
#undef O
