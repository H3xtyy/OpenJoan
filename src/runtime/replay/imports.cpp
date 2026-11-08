// Record / replay: wrappers for joan.exe's imports (the IAT slots inside joan_image). Overview: replay.hpp.
//
// Kinds of wrapper (each says which in its comment):
//   value  - a query: replay returns the recorded result and out parameters WITHOUT calling the system (time, input,
//            system state, waits and sleeps - so replays run as fast as the game code allows);
//   real   - the system call is made in both modes (it has effects we need: windows, files, devices); its results are
//            recorded and, on replay, replaced by the recorded ones (or compared: a difference is a divergence);
//   skip   - record calls it, replay does not (effects outside the game: cursor moves).
// Imports that nothing reaches any more (src/loader/dead_imports.inc, re/gen_dead_imports.py: the loader traps them)
// have no wrapper; their ids stay in ids.hpp as placeholders.
// Only calls on the game thread are logged; other threads always get the real function (counted in the report).
#include "../../callconv.hpp"
#include "../../compiler.hpp"
#if defined(_WIN32)
#include <windows.h>
// After windows.h (WIN32_LEAN_AND_MEAN leaves these out):
#include <commdlg.h>
#include <mmsystem.h>
#endif

#include <cstdint>
#include <cstring>
#include <map>
#include <type_traits>
#include <string>

#include "../image.hpp"
#include <unordered_map>

#include "../loader/data_image.hpp"
#include "../../platform/memory.hpp"
#include "internal.hpp"
#include "sys.hpp"

namespace joan::replay {
namespace {

// OpenAL (OpenAL32.dll exports are cdecl).
using ALuint = unsigned int;
using ALint = int;
using ALenum = int;
using ALsizei = int;
using ALboolean = char;

// ---------------------------------------------------------------- helpers

/// value: the recorded T on replay, else `real()` (recorded when recording).
template <class T, class F>
T value(Id id, std::uint32_t check, F&& real) {
    T r{};
    if (replaying() && play(id, check, r)) return r;
    r = real();
    rec(id, check, r);
    return r;
}

/// real: `real()` always runs; replay returns the recorded T instead of its result.
template <class T, class F>
T real_then(Id id, std::uint32_t check, F&& real) {
    const T r = real();
    if (replaying()) {
        T recorded{};
        if (play(id, check, recorded)) return recorded;
        return r;
    }
    rec(id, check, r);
    return r;
}

/// An event without results (Sleep, ...): true on replay when the recording had it (skip the real call).
bool play_empty(Id id, std::uint32_t check) {
    std::uint32_t n = 0;
    return play_raw(id, check, n) != nullptr;
}

/// Variable-size results: the recorded payload on replay (nullptr: call the real function).
const std::uint8_t* play_var(Id id, std::uint32_t check, std::uint32_t& n) { return play_raw(id, check, n); }


// Payloads keep the x86 sizes, so x86 and x64 builds replay each other's recordings (docs/X64.md: the replay gate).
// LRESULT is 8 bytes on x64: recorded as its low 32 bits (x86 builds: LRESULT itself, the payload as before). The
// results recorded this way (DispatchMessageA, DefWindowProcA of the messages the game reacts to) are small numbers.
using LResult32 = std::conditional_t<sizeof(LRESULT) == 4, LRESULT, std::int32_t>;
template <class T>
std::uint32_t u32v(T v) {
    return static_cast<std::uint32_t>(v);
}

#define GAME_THREAD_OR(id, real_call) \
    do {                              \
        if (!on_main()) {             \
            other_thread(Id::id);     \
            return real_call;         \
        }                             \
    } while (0)

// ---------------------------------------------------------------- originals

#define ORIGINAL(ret, cc, name, params) ret(cc* o_##name) params = nullptr

// KERNEL32
ORIGINAL(BOOL, WINAPI, QueryPerformanceCounter, (LARGE_INTEGER*));
ORIGINAL(LPWCH, WINAPI, GetEnvironmentStringsW, ());
ORIGINAL(BOOL, WINAPI, FreeEnvironmentStringsW, (LPWCH));
ORIGINAL(BOOL, WINAPI, QueryPerformanceFrequency, (LARGE_INTEGER*));
ORIGINAL(void, WINAPI, GetLocalTime, (SYSTEMTIME*));
ORIGINAL(void, WINAPI, GetSystemTime, (SYSTEMTIME*));
ORIGINAL(void, WINAPI, GetSystemTimeAsFileTime, (FILETIME*));
ORIGINAL(DWORD, WINAPI, GetTimeZoneInformation, (TIME_ZONE_INFORMATION*));
ORIGINAL(void, WINAPI, Sleep, (DWORD));
ORIGINAL(DWORD, WINAPI, SleepEx, (DWORD, BOOL));
ORIGINAL(DWORD, WINAPI, WaitForSingleObject, (HANDLE, DWORD));
ORIGINAL(DWORD, WINAPI, WaitForMultipleObjectsEx, (DWORD, const HANDLE*, BOOL, DWORD, BOOL));
ORIGINAL(BOOL, WINAPI, ReadFileEx, (HANDLE, LPVOID, DWORD, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE));
ORIGINAL(void, WINAPI, GlobalMemoryStatus, (LPMEMORYSTATUS));
ORIGINAL(BOOL, WINAPI, GetDiskFreeSpaceA, (LPCSTR, LPDWORD, LPDWORD, LPDWORD, LPDWORD));
ORIGINAL(UINT, WINAPI, GetDriveTypeA, (LPCSTR));
ORIGINAL(DWORD, WINAPI, GetVersion, ());
ORIGINAL(BOOL, WINAPI, GetVersionExA, (LPOSVERSIONINFOA));
ORIGINAL(DWORD, WINAPI, GetLastError, ());
ORIGINAL(LPSTR, WINAPI, GetCommandLineA, ());
ORIGINAL(HANDLE, WINAPI, CreateFileA, (LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE));
ORIGINAL(BOOL, WINAPI, ReadFile, (HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED));
ORIGINAL(HANDLE, WINAPI, FindFirstFileA, (LPCSTR, LPWIN32_FIND_DATAA));
ORIGINAL(BOOL, WINAPI, FindNextFileA, (HANDLE, LPWIN32_FIND_DATAA));
ORIGINAL(BOOL, WINAPI, FindClose, (HANDLE));
ORIGINAL(HANDLE, WINAPI, CreateEventA, (LPSECURITY_ATTRIBUTES, BOOL, BOOL, LPCSTR));
ORIGINAL(BOOL, WINAPI, CloseHandle, (HANDLE));
ORIGINAL(void, WINAPI, ExitProcess, (UINT));
// USER32
ORIGINAL(BOOL, WINAPI, PeekMessageA, (LPMSG, HWND, UINT, UINT, UINT));
ORIGINAL(BOOL, WINAPI, GetMessageA, (LPMSG, HWND, UINT, UINT));
ORIGINAL(BOOL, WINAPI, TranslateMessage, (const MSG*));
ORIGINAL(LRESULT, WINAPI, DispatchMessageA, (const MSG*));
ORIGINAL(DWORD, WINAPI, MsgWaitForMultipleObjectsEx, (DWORD, const HANDLE*, DWORD, DWORD, DWORD));
ORIGINAL(BOOL, WINAPI, GetCursorPos, (LPPOINT));
ORIGINAL(SHORT, WINAPI, GetKeyState, (int));
ORIGINAL(BOOL, WINAPI, AdjustWindowRectEx, (LPRECT, DWORD, BOOL, DWORD));
ORIGINAL(int, WINAPI, ShowCursor, (BOOL));
ORIGINAL(BOOL, WINAPI, SetCursorPos, (int, int));
ORIGINAL(ATOM, WINAPI, RegisterClassA, (const WNDCLASSA*));
ORIGINAL(HWND, WINAPI, CreateWindowExA, (DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID));
ORIGINAL(BOOL, WINAPI, ShowWindow, (HWND, int));
ORIGINAL(BOOL, WINAPI, UpdateWindow, (HWND));
ORIGINAL(HWND, WINAPI, SetFocus, (HWND));
ORIGINAL(BOOL, WINAPI, SetWindowPos, (HWND, HWND, int, int, int, int, UINT));
ORIGINAL(LONG, WINAPI, SetWindowLongA, (HWND, int, LONG));
ORIGINAL(BOOL, WINAPI, DestroyWindow, (HWND));
ORIGINAL(LRESULT, WINAPI, DefWindowProcA, (HWND, UINT, WPARAM, LPARAM));
ORIGINAL(int, WINAPI, MessageBoxA, (HWND, LPCSTR, LPCSTR, UINT));
// comdlg32
ORIGINAL(BOOL, WINAPI, GetOpenFileNameA, (LPOPENFILENAMEA));
// WINMM
ORIGINAL(DWORD, WINAPI, timeGetTime, ());
// OpenAL32
ORIGINAL(void, JOAN_CDECL, alGetSourcei, (ALuint, ALenum, ALint*));
ORIGINAL(ALenum, JOAN_CDECL, alGetError, ());
ORIGINAL(void, JOAN_CDECL, alGenSources, (ALsizei, ALuint*));
ORIGINAL(void, JOAN_CDECL, alGenBuffers, (ALsizei, ALuint*));
ORIGINAL(ALboolean, JOAN_CDECL, alIsSource, (ALuint));
ORIGINAL(ALboolean, JOAN_CDECL, alIsBuffer, (ALuint));
ORIGINAL(void, JOAN_CDECL, alSourceUnqueueBuffers, (ALuint, ALsizei, ALuint*));
// name translation only (replay): every OpenAL call that takes a source / buffer name from the game
ORIGINAL(void, JOAN_CDECL, alDeleteSources, (ALsizei, const ALuint*));
ORIGINAL(void, JOAN_CDECL, alDeleteBuffers, (ALsizei, const ALuint*));
ORIGINAL(void, JOAN_CDECL, alBufferData, (ALuint, ALenum, const void*, ALsizei, ALsizei));
ORIGINAL(void, JOAN_CDECL, alSourcef, (ALuint, ALenum, float));
ORIGINAL(void, JOAN_CDECL, alSource3f, (ALuint, ALenum, float, float, float));
ORIGINAL(void, JOAN_CDECL, alSourcei, (ALuint, ALenum, ALint));
ORIGINAL(void, JOAN_CDECL, alSourcePlay, (ALuint));
ORIGINAL(void, JOAN_CDECL, alSourcePause, (ALuint));
ORIGINAL(void, JOAN_CDECL, alSourceStop, (ALuint));
ORIGINAL(void, JOAN_CDECL, alSourceRewind, (ALuint));
ORIGINAL(void, JOAN_CDECL, alSourceQueueBuffers, (ALuint, ALsizei, const ALuint*));
ORIGINAL(void*, JOAN_CDECL, alcOpenDevice, (const char*));
ORIGINAL(void*, JOAN_CDECL, alcCreateContext, (void*, const ALint*));
// factories
ORIGINAL(void*, WINAPI, Direct3DCreate8, (UINT));

// ---------------------------------------------------------------- KERNEL32: time

// value
BOOL WINAPI w_QueryPerformanceCounter(LARGE_INTEGER* p) {
    GAME_THREAD_OR(QueryPerformanceCounter, o_QueryPerformanceCounter(p));
#pragma pack(push, 1)  // the record is 12 bytes on every build (LARGE_INTEGER is 8-aligned on x64: 16 bytes unpacked)
    struct R {
        BOOL r;
        LARGE_INTEGER v;
    };
#pragma pack(pop)
    const R x = value<R>(Id::QueryPerformanceCounter, 0, [&] {
        R y{};
        y.r = o_QueryPerformanceCounter(&y.v);
        return y;
    });
    if (p) *p = x.v;
    return x.r;
}

// value
BOOL WINAPI w_QueryPerformanceFrequency(LARGE_INTEGER* p) {
    GAME_THREAD_OR(QueryPerformanceFrequency, o_QueryPerformanceFrequency(p));
#pragma pack(push, 1)  // the record is 12 bytes on every build (LARGE_INTEGER is 8-aligned on x64: 16 bytes unpacked)
    struct R {
        BOOL r;
        LARGE_INTEGER v;
    };
#pragma pack(pop)
    const R x = value<R>(Id::QueryPerformanceFrequency, 0, [&] {
        R y{};
        y.r = o_QueryPerformanceFrequency(&y.v);
        return y;
    });
    if (p) *p = x.v;
    return x.r;
}

// value
void WINAPI w_GetLocalTime(SYSTEMTIME* p) {
    GAME_THREAD_OR(GetLocalTime, o_GetLocalTime(p));
    *p = value<SYSTEMTIME>(Id::GetLocalTime, 0, [&] {
        SYSTEMTIME t{};
        o_GetLocalTime(&t);
        return t;
    });
}

// value
void WINAPI w_GetSystemTime(SYSTEMTIME* p) {
    GAME_THREAD_OR(GetSystemTime, o_GetSystemTime(p));
    *p = value<SYSTEMTIME>(Id::GetSystemTime, 0, [&] {
        SYSTEMTIME t{};
        o_GetSystemTime(&t);
        return t;
    });
}

// value
void WINAPI w_GetSystemTimeAsFileTime(FILETIME* p) {
    GAME_THREAD_OR(GetSystemTimeAsFileTime, o_GetSystemTimeAsFileTime(p));
    *p = value<FILETIME>(Id::GetSystemTimeAsFileTime, 0, [&] {
        FILETIME t{};
        o_GetSystemTimeAsFileTime(&t);
        return t;
    });
}

// value
DWORD WINAPI w_GetTimeZoneInformation(TIME_ZONE_INFORMATION* p) {
    GAME_THREAD_OR(GetTimeZoneInformation, o_GetTimeZoneInformation(p));
    struct R {
        DWORD r;
        TIME_ZONE_INFORMATION tz;
    };
    const R x = value<R>(Id::GetTimeZoneInformation, 0, [&] {
        R y{};
        y.r = o_GetTimeZoneInformation(&y.tz);
        return y;
    });
    if (p) *p = x.tz;
    return x.r;
}

// value (timeGetTime is the game's main clock: 0x00591050 Misc::get_time)
DWORD WINAPI w_timeGetTime() {
    GAME_THREAD_OR(timeGetTime, o_timeGetTime());
    const DWORD t = value<DWORD>(Id::timeGetTime, 0, [] { return o_timeGetTime(); });
    if (replaying()) pace_to(t);
    return t;
}

// ---------------------------------------------------------------- KERNEL32: sleeps, waits, asynchronous reads

// skip (a replay does not wait)
void WINAPI w_Sleep(DWORD ms) {
    GAME_THREAD_OR(Sleep, o_Sleep(ms));
    if (replaying() && play_empty(Id::Sleep, ms)) return;
    o_Sleep(ms);
    record_call(Id::Sleep, ms, nullptr, 0);
}

// value (alertable: ReadFileEx completions run inside; replay injects the recorded ones here)
DWORD WINAPI w_SleepEx(DWORD ms, BOOL alertable) {
    GAME_THREAD_OR(SleepEx, o_SleepEx(ms, alertable));
    return value<DWORD>(Id::SleepEx, check_of({ms, u32v(alertable)}), [&] { return o_SleepEx(ms, alertable); });
}

// value
DWORD WINAPI w_WaitForSingleObject(HANDLE h, DWORD ms) {
    GAME_THREAD_OR(WaitForSingleObject, o_WaitForSingleObject(h, ms));
    return value<DWORD>(Id::WaitForSingleObject, ms, [&] { return o_WaitForSingleObject(h, ms); });
}

// value
DWORD WINAPI w_WaitForMultipleObjectsEx(DWORD n, const HANDLE* hs, BOOL all, DWORD ms, BOOL alertable) {
    GAME_THREAD_OR(WaitForMultipleObjectsEx, o_WaitForMultipleObjectsEx(n, hs, all, ms, alertable));
    return value<DWORD>(Id::WaitForMultipleObjectsEx, check_of({n, u32v(all), ms, u32v(alertable)}),
                        [&] { return o_WaitForMultipleObjectsEx(n, hs, all, ms, alertable); });
}

// ReadFileEx (the palette / resource streamer, 0x00578d70 PalStream::init and FileLoader): recording runs the real
// read with our completion routine, which writes an Apc event when Windows delivers it (inside an alertable wait) and
// calls the game's routine. Replay reads the data at once (synchronously) and runs the game's routine where the
// recording had the Apc event, with the recorded error / byte count.
struct ApcRecording {
    LPOVERLAPPED_COMPLETION_ROUTINE routine;
    std::uint32_t seq;
};
struct ApcReplay {
    LPOVERLAPPED_COMPLETION_ROUTINE routine;
    LPOVERLAPPED ov;
    DWORD got;
};
std::map<LPOVERLAPPED, ApcRecording> g_apc_recording;
std::map<std::uint32_t, ApcReplay> g_apc_replay;
std::uint32_t g_apc_seq = 0;

VOID CALLBACK apc_thunk(DWORD error, DWORD bytes, LPOVERLAPPED ov) {
    const auto it = g_apc_recording.find(ov);
    if (it == g_apc_recording.end()) return;  // cannot happen: only our ReadFileEx passes this routine
    const ApcRecording a = it->second;
    g_apc_recording.erase(it);
    if (on_main()) record_apc(a.seq, error, bytes);
    a.routine(error, bytes, ov);
}

BOOL WINAPI w_ReadFileEx(HANDLE h, LPVOID buf, DWORD n, LPOVERLAPPED ov, LPOVERLAPPED_COMPLETION_ROUTINE routine) {
    GAME_THREAD_OR(ReadFileEx, o_ReadFileEx(h, buf, n, ov, routine));
    struct R {
        BOOL r;
        std::uint32_t seq;
    };
    const std::uint32_t check = check_of({n, ov ? u32v(ov->Offset) : 0u});
    if (replaying()) {
        R x{};
        if (play(Id::ReadFileEx, check, x)) {
            if (x.r && ov) {
                OVERLAPPED tmp{};
                tmp.Offset = ov->Offset;
                tmp.OffsetHigh = ov->OffsetHigh;
                tmp.hEvent = o_CreateEventA ? o_CreateEventA(nullptr, TRUE, FALSE, nullptr) : nullptr;
                DWORD got = 0;
                // through the game's ReadFile target (platform/files.cpp: h is one of its portable handles)
                if (!o_ReadFile(h, buf, n, &got, &tmp)) {
#if defined(_WIN32)
                    if (o_GetLastError() == ERROR_IO_PENDING)
                        GetOverlappedResult(h, &tmp, &got, TRUE);
                    else
                        got = 0;
#else
                    got = 0;  // platform/files.cpp reads at once: a failure is a failure
#endif
                }
                if (tmp.hEvent && o_CloseHandle) o_CloseHandle(tmp.hEvent);
                ov->Internal = 0x103;  // STATUS_PENDING until the recorded completion is injected
                g_apc_replay[x.seq] = {routine, ov, got};
            }
            return x.r;
        }
        return o_ReadFileEx(h, buf, n, ov, routine);
    }
    const std::uint32_t seq = g_apc_seq++;
    if (ov) g_apc_recording[ov] = {routine, seq};
    const BOOL r = o_ReadFileEx(h, buf, n, ov, ov ? &apc_thunk : routine);
    if (!r && ov) g_apc_recording.erase(ov);
    rec(Id::ReadFileEx, check, R{r, seq});
    return r;
}

// ---------------------------------------------------------------- KERNEL32: system queries

// value
void WINAPI w_GlobalMemoryStatus(LPMEMORYSTATUS p) {
    GAME_THREAD_OR(GlobalMemoryStatus, o_GlobalMemoryStatus(p));
#if UINTPTR_MAX == 0xffffffffu
    *p = value<MEMORYSTATUS>(Id::GlobalMemoryStatus, 0, [&] {
        MEMORYSTATUS m{};
        m.dwLength = sizeof(m);
        o_GlobalMemoryStatus(&m);
        return m;
    });
#else
    // x64: MEMORYSTATUS has SIZE_T fields (56 bytes); recorded in the x86 form (8 DWORDs, 32 bytes; a value above
    // 4 GB as 0xffffffff, as 32-bit Windows reports it)
    struct Status32 {
        DWORD length, load, total_phys, avail_phys, total_page_file, avail_page_file, total_virtual, avail_virtual;
    };
    static_assert(sizeof(Status32) == 32);
    const Status32 s = value<Status32>(Id::GlobalMemoryStatus, 0, [&] {
        MEMORYSTATUS m{};
        m.dwLength = sizeof(m);
        o_GlobalMemoryStatus(&m);
        auto d = [](SIZE_T v) { return static_cast<DWORD>(v > 0xffffffffu ? 0xffffffffu : v); };
        return Status32{32u, m.dwMemoryLoad, d(m.dwTotalPhys), d(m.dwAvailPhys), d(m.dwTotalPageFile),
                        d(m.dwAvailPageFile), d(m.dwTotalVirtual), d(m.dwAvailVirtual)};
    });
    p->dwLength = sizeof(MEMORYSTATUS);
    p->dwMemoryLoad = s.load;
    p->dwTotalPhys = s.total_phys;
    p->dwAvailPhys = s.avail_phys;
    p->dwTotalPageFile = s.total_page_file;
    p->dwAvailPageFile = s.avail_page_file;
    p->dwTotalVirtual = s.total_virtual;
    p->dwAvailVirtual = s.avail_virtual;
#endif
}

// value
BOOL WINAPI w_GetDiskFreeSpaceA(LPCSTR root, LPDWORD a, LPDWORD b, LPDWORD c, LPDWORD d) {
    GAME_THREAD_OR(GetDiskFreeSpaceA, o_GetDiskFreeSpaceA(root, a, b, c, d));
    struct R {
        BOOL r;
        DWORD v[4];
    };
    const R x = value<R>(Id::GetDiskFreeSpaceA, check_str(root), [&] {
        R y{};
        y.r = o_GetDiskFreeSpaceA(root, &y.v[0], &y.v[1], &y.v[2], &y.v[3]);
        return y;
    });
    if (a) *a = x.v[0];
    if (b) *b = x.v[1];
    if (c) *c = x.v[2];
    if (d) *d = x.v[3];
    return x.r;
}

// value
UINT WINAPI w_GetDriveTypeA(LPCSTR root) {
    GAME_THREAD_OR(GetDriveTypeA, o_GetDriveTypeA(root));
    return value<UINT>(Id::GetDriveTypeA, check_str(root), [&] { return o_GetDriveTypeA(root); });
}

// value
DWORD WINAPI w_GetVersion() {
    GAME_THREAD_OR(GetVersion, o_GetVersion());
    return value<DWORD>(Id::GetVersion, 0, [] { return o_GetVersion(); });
}

// value
BOOL WINAPI w_GetVersionExA(LPOSVERSIONINFOA p) {
    GAME_THREAD_OR(GetVersionExA, o_GetVersionExA(p));
    const DWORD size = p ? p->dwOSVersionInfoSize : 0;
    const DWORD n = size < sizeof(OSVERSIONINFOEXA) ? size : static_cast<DWORD>(sizeof(OSVERSIONINFOEXA));
    std::uint32_t got = 0;
    if (replaying()) {
        if (const std::uint8_t* d = play_var(Id::GetVersionExA, size, got)) {
            Cursor c(d, got);
            const BOOL r = c.get<BOOL>();
            c.get_blob(p, n);
            if (p) p->dwOSVersionInfoSize = size;
            return r;
        }
    }
    const BOOL r = o_GetVersionExA(p);
    Bytes b;
    b.put(r).put_blob(p, n);
    rec(Id::GetVersionExA, size, b);
    return r;
}

// value (the game asks after ReadFileEx / CreateFileA failures)
DWORD WINAPI w_GetLastError() {
    if (!on_main()) return o_GetLastError();  // not counted: the CRT asks on every thread
    return value<DWORD>(Id::GetLastError, 0, [] { return o_GetLastError(); });
}

// value (the CRT parses it at start-up: the recording's command line is what the game sees on replay)
LPSTR WINAPI w_GetCommandLineA() {
    GAME_THREAD_OR(GetCommandLineA, o_GetCommandLineA());
    static std::string replayed;
    std::uint32_t n = 0;
    if (replaying()) {
        if (const std::uint8_t* d = play_var(Id::GetCommandLineA, 0, n)) {
            if (replayed.empty()) replayed.assign(reinterpret_cast<const char*>(d), n);
            return replayed.data();
        }
    }
    LPSTR s = o_GetCommandLineA();
    record_call(Id::GetCommandLineA, 0, s, s ? static_cast<std::uint32_t>(std::strlen(s)) : 0u);
    return s;
}

// ---------------------------------------------------------------- KERNEL32: files

// real (the file is opened in both modes). Recorded: success, and for files opened for reading their size and a
// content hash; replay compares them (a file that opened before and does not now is a divergence, other contents a
// warning).
HANDLE WINAPI w_CreateFileA(LPCSTR name, DWORD access, DWORD share, LPSECURITY_ATTRIBUTES sa, DWORD disp, DWORD flags,
                            HANDLE tmpl) {
    GAME_THREAD_OR(CreateFileA, o_CreateFileA(name, access, share, sa, disp, flags, tmpl));
    const HANDLE h = o_CreateFileA(name, access, share, sa, disp, flags, tmpl);
    const DWORD err = o_GetLastError();
    track_handle(h);
    struct R {
        std::uint32_t ok;
        std::uint32_t error;
        std::uint64_t size;
        std::uint64_t hash;
    };
    R now{h != INVALID_HANDLE_VALUE ? 1u : 0u, err, 0, 0};
    const bool reading = (access & GENERIC_WRITE) == 0 && disp == OPEN_EXISTING;
    if (now.ok && reading) now.hash = file_content_hash(name, &now.size);
    const std::uint32_t check = check_str(name, access ^ (disp << 4));
    if (replaying()) {
        R x{};
        if (play(Id::CreateFileA, check, x)) {
            if (x.ok != now.ok)
                diverge(Id::CreateFileA, (std::string(name ? name : "?") +
                                          (x.ok ? ": opened while recording, fails now" : ": failed while recording, opens now"))
                                             .c_str());
            else if (now.ok && reading && hash_files() && x.hash != 0 && (x.size != now.size || x.hash != now.hash))
                warn("%s differs from the recording's file (size %llu / %llu)", name ? name : "?",
                     static_cast<unsigned long long>(x.size), static_cast<unsigned long long>(now.size));
        }
        sys::set_last_error(err);
        return h;
    }
    rec(Id::CreateFileA, check, now);
    sys::set_last_error(err);
    return h;
}

// real (synchronous reads only; the count must be the recorded one)
BOOL WINAPI w_ReadFile(HANDLE h, LPVOID buf, DWORD n, LPDWORD got, LPOVERLAPPED ov) {
    GAME_THREAD_OR(ReadFile, o_ReadFile(h, buf, n, got, ov));
    if (ov) return o_ReadFile(h, buf, n, got, ov);
    struct R {
        BOOL r;
        DWORD got;
    };
    DWORD count = 0;
    const R now{o_ReadFile(h, buf, n, &count, nullptr), count};
    if (got) *got = count;
    if (replaying()) {
        R x{};
        if (play(Id::ReadFile, n, x) && (x.r != now.r || x.got != now.got))
            diverge(Id::ReadFile, "the file read returned another byte count than while recording");
        return now.r;
    }
    rec(Id::ReadFile, n, now);
    return now.r;
}

// value: the directory listing is the recording's (save games, profiles); replay does not touch the disk. The find
// handle of a replay is the recorded number (only FindNextFileA / FindClose see it, both replayed): the same value in
// both runs, so it needs no normalisation in the state hash (likewise registry keys).
struct FindResult {
    std::uint32_t handle;
    WIN32_FIND_DATAA fd;
};
HANDLE WINAPI w_FindFirstFileA(LPCSTR pattern, LPWIN32_FIND_DATAA fd) {
    GAME_THREAD_OR(FindFirstFileA, o_FindFirstFileA(pattern, fd));
    const FindResult x = value<FindResult>(Id::FindFirstFileA, check_str(pattern), [&] {
        FindResult y{};
        y.handle = recorded_bits(o_FindFirstFileA(pattern, &y.fd));
        return y;
    });
    // The handle is recorded as 32 bits (the platform's handles have 32 significant bits); INVALID_HANDLE_VALUE is -1 at
    // the pointer's width, so a recorded 0xffffffff comes back as the 64-bit -1 on x64 (the CRT's _findfirst compares
    // against it; a zero-extended 0xffffffff skipped its error path and left the recorded GetLastError unconsumed).
    const HANDLE h = x.handle == 0xffffffffu ? INVALID_HANDLE_VALUE : reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(x.handle));
    if (fd) *fd = x.fd;
    return h;
}

// value
BOOL WINAPI w_FindNextFileA(HANDLE h, LPWIN32_FIND_DATAA fd) {
    GAME_THREAD_OR(FindNextFileA, o_FindNextFileA(h, fd));
    struct R {
        BOOL r;
        WIN32_FIND_DATAA fd;
    };
    const R x = value<R>(Id::FindNextFileA, 0, [&] {
        R y{};
        y.r = o_FindNextFileA(h, &y.fd);
        return y;
    });
    if (fd && x.r) *fd = x.fd;
    return x.r;
}

// value (the handle of a replayed FindFirstFileA is the recorded number, not a real handle)
BOOL WINAPI w_FindClose(HANDLE h) {
    GAME_THREAD_OR(FindClose, o_FindClose(h));
    return value<BOOL>(Id::FindClose, 0, [&] { return o_FindClose(h); });
}

// real (the handle is remembered for the state hash)
HANDLE WINAPI w_CreateEventA(LPSECURITY_ATTRIBUTES sa, BOOL manual, BOOL initial, LPCSTR name) {
    GAME_THREAD_OR(CreateEventA, o_CreateEventA(sa, manual, initial, name));
    const HANDLE h = o_CreateEventA(sa, manual, initial, name);
    track_handle(h);
    return h;
}

// real, not logged: a closed handle is no longer normalised in the state hash (handle values are reused)
BOOL WINAPI w_CloseHandle(HANDLE h) {
    if (on_main()) untrack_handle(h);
    return o_CloseHandle(h);
}

// End of the game: the recording's end event / the replay result, then the loader's ExitProcess (its reports).
void WINAPI w_ExitProcess(UINT code) {
    if (mode() != Mode::Off) {
        game_exit(code);
        code = replay_exit_code(code);
    }
    o_ExitProcess(code);
}

// ---------------------------------------------------------------- USER32: messages

struct MsgRecord {
    std::uint32_t hwnd, message, wparam, lparam, time;
    LONG x, y;
};
MsgRecord to_record(const MSG& m) {
    return {hwnd_code(m.hwnd), m.message, static_cast<std::uint32_t>(m.wParam), static_cast<std::uint32_t>(m.lParam),
            m.time, m.pt.x, m.pt.y};
}
void from_record(const MsgRecord& r, MSG* m) {
    m->hwnd = hwnd_from_code(r.hwnd);
    m->message = r.message;
    m->wParam = static_cast<WPARAM>(r.wparam);
    m->lParam = static_cast<LPARAM>(r.lparam);
    m->time = r.time;
    m->pt.x = r.x;
    m->pt.y = r.y;
}

// value (replay: the real queue is emptied and its messages go to DefWindowProc - the game sees only recorded ones)
BOOL WINAPI w_PeekMessageA(LPMSG m, HWND h, UINT lo, UINT hi, UINT remove) {
    GAME_THREAD_OR(PeekMessageA, o_PeekMessageA(m, h, lo, hi, remove));
    struct R {
        BOOL r;
        MsgRecord m;
    };
    const std::uint32_t check = check_of({hwnd_code(h), lo, hi, remove});
    if (replaying()) {
        pump_real_messages();
        R x{};
        if (play(Id::PeekMessageA, check, x)) {
            if (x.r) from_record(x.m, m);
            return x.r;
        }
    }
    R y{};
    y.r = o_PeekMessageA(m, h, lo, hi, remove);
    if (y.r) y.m = to_record(*m);
    rec(Id::PeekMessageA, check, y);
    return y.r;
}

// value
BOOL WINAPI w_GetMessageA(LPMSG m, HWND h, UINT lo, UINT hi) {
    GAME_THREAD_OR(GetMessageA, o_GetMessageA(m, h, lo, hi));
    struct R {
        BOOL r;
        MsgRecord m;
    };
    const std::uint32_t check = check_of({hwnd_code(h), lo, hi});
    if (replaying()) {
        pump_real_messages();
        R x{};
        if (play(Id::GetMessageA, check, x)) {
            from_record(x.m, m);
            return x.r;
        }
    }
    R y{};
    y.r = o_GetMessageA(m, h, lo, hi);
    y.m = to_record(*m);
    rec(Id::GetMessageA, check, y);
    return y.r;
}

// value
BOOL WINAPI w_TranslateMessage(const MSG* m) {
    GAME_THREAD_OR(TranslateMessage, o_TranslateMessage(m));
    return value<BOOL>(Id::TranslateMessage, m ? m->message : 0u, [&] { return o_TranslateMessage(m); });
}

// value (what the game's WndProc did with the message was recorded as WndProc events and is injected)
LRESULT WINAPI w_DispatchMessageA(const MSG* m) {
    GAME_THREAD_OR(DispatchMessageA, o_DispatchMessageA(m));
    return value<LResult32>(Id::DispatchMessageA, m ? m->message : 0u,
                            [&] { return static_cast<LResult32>(o_DispatchMessageA(m)); });
}

// value (the main loop's frame wait 0x00499960; alertable)
DWORD WINAPI w_MsgWaitForMultipleObjectsEx(DWORD n, const HANDLE* hs, DWORD ms, DWORD mask, DWORD flags) {
    GAME_THREAD_OR(MsgWaitForMultipleObjectsEx, o_MsgWaitForMultipleObjectsEx(n, hs, ms, mask, flags));
    return value<DWORD>(Id::MsgWaitForMultipleObjectsEx, check_of({n, ms, mask, flags}),
                        [&] { return o_MsgWaitForMultipleObjectsEx(n, hs, ms, mask, flags); });
}

// ---------------------------------------------------------------- USER32: input, window

// value
BOOL WINAPI w_GetCursorPos(LPPOINT p) {
    GAME_THREAD_OR(GetCursorPos, o_GetCursorPos(p));
    struct R {
        BOOL r;
        POINT p;
    };
    const R x = value<R>(Id::GetCursorPos, 0, [&] {
        R y{};
        y.r = o_GetCursorPos(&y.p);
        return y;
    });
    if (p && x.r) *p = x.p;
    return x.r;
}

// value
SHORT WINAPI w_GetKeyState(int vk) {
    GAME_THREAD_OR(GetKeyState, o_GetKeyState(vk));
    return value<SHORT>(Id::GetKeyState, u32v(vk), [&] { return o_GetKeyState(vk); });
}

// value
BOOL WINAPI w_AdjustWindowRectEx(LPRECT rc, DWORD style, BOOL menu, DWORD ex) {
    GAME_THREAD_OR(AdjustWindowRectEx, o_AdjustWindowRectEx(rc, style, menu, ex));
    struct R {
        BOOL r;
        RECT rc;
    };
    const RECT in = rc ? *rc : RECT{};
    const R x = value<R>(Id::AdjustWindowRectEx,
                         check_of({style, u32v(menu), ex, u32v(in.left), u32v(in.top), u32v(in.right), u32v(in.bottom)}),
                         [&] {
                             R y{};
                             y.rc = in;
                             y.r = o_AdjustWindowRectEx(&y.rc, style, menu, ex);
                             return y;
                         });
    if (rc) *rc = x.rc;
    return x.r;
}

// real (the display counter is the recorded one)
int WINAPI w_ShowCursor(BOOL show) {
    GAME_THREAD_OR(ShowCursor, o_ShowCursor(show));
    return real_then<int>(Id::ShowCursor, u32v(show), [&] { return o_ShowCursor(show); });
}

// value: rdtsc of the game (not an import; the hook's read_tsc calls this)
}  // namespace

unsigned long long cpu_tsc() { return joan::cpu::ticks(); }

unsigned long long tsc(unsigned long long real) {
    if ((!recording() && !replaying()) || !on_main()) return real;
    return value<std::uint64_t>(Id::rdtsc, 0, [&] { return static_cast<std::uint64_t>(real); });
}

namespace {

// value: the environment block (variables differ between two starts of the game - another shell, another
// terminal - and the CRT copies the block into the game heap, so its size shifts every later heap offset).
// Replay hands out a copy of the recorded block; FreeEnvironmentStringsW of that copy releases it here.
std::vector<WCHAR> g_env_copy;  // WCHAR: wchar_t on Windows, char16_t in platform/win32_types.hpp
LPWCH WINAPI w_GetEnvironmentStringsW() {
    GAME_THREAD_OR(GetEnvironmentStringsW, o_GetEnvironmentStringsW());
    if (replaying()) {
        std::uint32_t n = 0;
        if (const std::uint8_t* d = play_var(Id::GetEnvironmentStringsW, 0, n)) {
            g_env_copy.assign(n / 2 + 2, WCHAR{0});
            std::memcpy(g_env_copy.data(), d, n);
            return g_env_copy.data();
        }
        return o_GetEnvironmentStringsW();
    }
    LPWCH env = o_GetEnvironmentStringsW();
    std::uint32_t chars = 0;
    if (env)
        while (env[chars] != 0 || env[chars + 1] != 0) ++chars;
    Bytes b;
    b.put_raw(env, env ? (chars + 2) * 2u : 0u);
    rec(Id::GetEnvironmentStringsW, 0, b);
    return env;
}
BOOL WINAPI w_FreeEnvironmentStringsW(LPWCH env) {
    if (!g_env_copy.empty() && env == g_env_copy.data()) return TRUE;
    return o_FreeEnvironmentStringsW(env);
}

// skip (a replay does not move the mouse)
BOOL WINAPI w_SetCursorPos(int x, int y) {
    GAME_THREAD_OR(SetCursorPos, o_SetCursorPos(x, y));
    return value<BOOL>(Id::SetCursorPos, check_of({u32v(x), u32v(y)}), [&] { return o_SetCursorPos(x, y); });
}

// real: the game's window procedure is replaced by wndproc_proxy (replay.cpp), which records the messages it
// reacts to and, on replay, keeps real messages away from the game.
ATOM WINAPI w_RegisterClassA(const WNDCLASSA* wc) {
    GAME_THREAD_OR(RegisterClassA, o_RegisterClassA(wc));
    WNDCLASSA copy = *wc;
    if (!game_wndproc()) {
        set_game_wndproc(wc->lpfnWndProc);
        copy.lpfnWndProc = &wndproc_proxy;
    } else {
        log("RegisterClassA: a second window class (%s) - its messages are not recorded", wc->lpszClassName);
    }
    const ATOM a = o_RegisterClassA(&copy);
    const std::uint32_t ok = a != 0;
    if (replaying()) {
        std::uint32_t x = 0;
        if (play(Id::RegisterClassA, check_str(wc->lpszClassName), x) && x != ok)
            diverge(Id::RegisterClassA, "registering the window class failed in one run only");
        return a;
    }
    rec(Id::RegisterClassA, check_str(wc->lpszClassName), ok);
    return a;
}

// real; the first window is the game window (messages, HWND values in payloads and in the state hash)
HWND WINAPI w_CreateWindowExA(DWORD ex, LPCSTR cls, LPCSTR title, DWORD style, int x, int y, int w, int h, HWND parent,
                              HMENU menu, HINSTANCE inst, LPVOID param) {
    GAME_THREAD_OR(CreateWindowExA, o_CreateWindowExA(ex, cls, title, style, x, y, w, h, parent, menu, inst, param));
    const HWND wnd = o_CreateWindowExA(ex, cls, title, style, x, y, w, h, parent, menu, inst, param);
    if (wnd && !game_window()) set_game_window(wnd);
    const std::uint32_t ok = wnd != nullptr;
    const std::uint32_t check = check_of({ex, style, u32v(w), u32v(h)});
    if (replaying()) {
        std::uint32_t r = 0;
        if (play(Id::CreateWindowExA, check, r) && r != ok)
            diverge(Id::CreateWindowExA, "creating the window failed in one run only");
        return wnd;
    }
    rec(Id::CreateWindowExA, check, ok);
    return wnd;
}

// real (these anchor window messages Windows sends while they run: replay injects them at the same place)
BOOL WINAPI w_ShowWindow(HWND h, int cmd) {
    GAME_THREAD_OR(ShowWindow, o_ShowWindow(h, cmd));
    return real_then<BOOL>(Id::ShowWindow, check_of({hwnd_code(h), u32v(cmd)}), [&] { return o_ShowWindow(h, cmd); });
}
BOOL WINAPI w_UpdateWindow(HWND h) {
    GAME_THREAD_OR(UpdateWindow, o_UpdateWindow(h));
    return real_then<BOOL>(Id::UpdateWindow, hwnd_code(h), [&] { return o_UpdateWindow(h); });
}
HWND WINAPI w_SetFocus(HWND h) {
    GAME_THREAD_OR(SetFocus, o_SetFocus(h));
    return hwnd_from_code(real_then<std::uint32_t>(Id::SetFocus, hwnd_code(h), [&] { return hwnd_code(o_SetFocus(h)); }));
}
BOOL WINAPI w_SetWindowPos(HWND h, HWND after, int x, int y, int cx, int cy, UINT flags) {
    GAME_THREAD_OR(SetWindowPos, o_SetWindowPos(h, after, x, y, cx, cy, flags));
    return real_then<BOOL>(Id::SetWindowPos,
                           check_of({hwnd_code(h), hwnd_code(after), u32v(x), u32v(y), u32v(cx), u32v(cy), flags}),
                           [&] { return o_SetWindowPos(h, after, x, y, cx, cy, flags); });
}
LONG WINAPI w_SetWindowLongA(HWND h, int index, LONG v) {
    GAME_THREAD_OR(SetWindowLongA, o_SetWindowLongA(h, index, v));
    if (index == GWLP_WNDPROC && h == game_window() && game_wndproc()) {  // GWL_WNDPROC (-4; x86 name only)
        // The game replaces its window procedure: the proxy stays, it calls the new one.
        const WNDPROC old = game_wndproc();
        set_game_wndproc(reinterpret_cast<WNDPROC>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(v))));
        return static_cast<LONG>(reinterpret_cast<std::uintptr_t>(old));
    }
    return real_then<LONG>(Id::SetWindowLongA, check_of({hwnd_code(h), u32v(index), u32v(v)}),
                           [&] { return o_SetWindowLongA(h, index, v); });
}
BOOL WINAPI w_DestroyWindow(HWND h) {
    GAME_THREAD_OR(DestroyWindow, o_DestroyWindow(h));
    return real_then<BOOL>(Id::DestroyWindow, hwnd_code(h), [&] { return o_DestroyWindow(h); });
}

// real, logged for the messages the game reacts to (DefWindowProc of WM_SYSCOMMAND / WM_CLOSE sends further ones)
LRESULT WINAPI w_DefWindowProcA(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if (!on_main() || !relevant_message(msg, wp)) return o_DefWindowProcA(h, msg, wp, lp);
    return real_then<LResult32>(Id::DefWindowProcA, check_of({msg, static_cast<std::uint32_t>(wp)}),
                                [&] { return static_cast<LResult32>(o_DefWindowProcA(h, msg, wp, lp)); });
}

// value (a replay shows no message box: the recorded button is returned)
int WINAPI w_MessageBoxA(HWND h, LPCSTR text, LPCSTR caption, UINT type) {
    GAME_THREAD_OR(MessageBoxA, o_MessageBoxA(h, text, caption, type));
    return value<int>(Id::MessageBoxA, check_str(text, type), [&] { return o_MessageBoxA(h, text, caption, type); });
}

// ---------------------------------------------------------------- comdlg32

// value (a replay shows no file dialog)
BOOL WINAPI w_GetOpenFileNameA(LPOPENFILENAMEA ofn) {
    GAME_THREAD_OR(GetOpenFileNameA, o_GetOpenFileNameA(ofn));
    const std::uint32_t check = check_of({ofn->Flags, ofn->nMaxFile});
    std::uint32_t n = 0;
    if (replaying()) {
        if (const std::uint8_t* d = play_var(Id::GetOpenFileNameA, check, n)) {
            Cursor c(d, n);
            const BOOL r = c.get<BOOL>();
            ofn->nFileOffset = c.get<WORD>();
            ofn->nFileExtension = c.get<WORD>();
            c.get_blob(ofn->lpstrFile, ofn->nMaxFile);
            return r;
        }
    }
    const BOOL r = o_GetOpenFileNameA(ofn);
    Bytes b;
    b.put(r).put(ofn->nFileOffset).put(ofn->nFileExtension).put_blob(ofn->lpstrFile, ofn->lpstrFile ? ofn->nMaxFile : 0u);
    rec(Id::GetOpenFileNameA, check, b);
    return r;
}

// ---------------------------------------------------------------- OpenAL32

// value (AL_SOURCE_STATE / AL_BUFFERS_QUEUED / AL_BUFFERS_PROCESSED drive the streams; the source name is not part of
// the check: another OpenAL may number sources differently)
void JOAN_CDECL w_alGetSourcei(ALuint src, ALenum param, ALint* out) {
    GAME_THREAD_OR(alGetSourcei, o_alGetSourcei(src, param, out));
    // the check includes the source name (the recorded one in both runs since patch 148), so a replay that asks
    // about another source diverges right there instead of later
    const ALint v = value<ALint>(Id::alGetSourcei, check_of({u32v(param), src}), [&] {
        ALint y = 0;
        o_alGetSourcei(src, param, &y);
        return y;
    });
    if (out) *out = v;
}

// Sources / buffers: the game always sees the RECORDED names (they end up in its state); on replay the real OpenAL
// names can differ (real playback timing frees / reuses names differently), so every call that hands a name to
// OpenAL translates recorded -> real (patch 148: without it a replay was silent - the game fed data to names that
// did not exist).
std::unordered_map<ALuint, ALuint> g_al_sources, g_al_buffers;  // recorded name -> real name (replay only)

ALuint real_name(const std::unordered_map<ALuint, ALuint>& m, ALuint recorded) {
    const auto it = m.find(recorded);
    return it == m.end() ? recorded : it->second;
}

void al_names(Id id, void(JOAN_CDECL* real)(ALsizei, ALuint*), std::unordered_map<ALuint, ALuint>& map, ALsizei n,
              ALuint* out) {
    if (n <= 0 || !out) return real(n, out);
    real(n, out);
    const std::uint32_t bytes = static_cast<std::uint32_t>(n) * 4u;
    if (replaying()) {
        std::uint32_t got = 0;
        if (const std::uint8_t* d = play_var(id, static_cast<std::uint32_t>(n), got)) {
            for (ALsizei i = 0; i < n && static_cast<std::uint32_t>(i) * 4u + 4u <= got; ++i) {
                ALuint recorded = 0;
                std::memcpy(&recorded, d + 4 * i, 4);
                map[recorded] = out[i];
                out[i] = recorded;
            }
        }
        return;
    }
    Bytes b;
    b.put_raw(out, bytes);
    rec(id, static_cast<std::uint32_t>(n), b);
}
void JOAN_CDECL w_alGenSources(ALsizei n, ALuint* out) {
    GAME_THREAD_OR(alGenSources, o_alGenSources(n, out));
    al_names(Id::alGenSources, o_alGenSources, g_al_sources, n, out);
}
void JOAN_CDECL w_alGenBuffers(ALsizei n, ALuint* out) {
    GAME_THREAD_OR(alGenBuffers, o_alGenBuffers(n, out));
    al_names(Id::alGenBuffers, o_alGenBuffers, g_al_buffers, n, out);
}

// translation wrappers (no events)
void al_delete(void(JOAN_CDECL* real)(ALsizei, const ALuint*), std::unordered_map<ALuint, ALuint>& map, ALsizei n,
               const ALuint* names) {
    if (!replaying() || n <= 0 || !names || n > 256) return real(n, names);
    ALuint r[256];
    for (ALsizei i = 0; i < n; ++i) {
        r[i] = real_name(map, names[i]);
        map.erase(names[i]);
    }
    real(n, r);
}
void JOAN_CDECL w_alDeleteSources(ALsizei n, const ALuint* names) { al_delete(o_alDeleteSources, g_al_sources, n, names); }
void JOAN_CDECL w_alDeleteBuffers(ALsizei n, const ALuint* names) { al_delete(o_alDeleteBuffers, g_al_buffers, n, names); }
void JOAN_CDECL w_alBufferData(ALuint b, ALenum f, const void* d, ALsizei size, ALsizei freq) {
    o_alBufferData(replaying() ? real_name(g_al_buffers, b) : b, f, d, size, freq);
}
ALuint src_of(ALuint s) { return replaying() ? real_name(g_al_sources, s) : s; }
void JOAN_CDECL w_alSourcef(ALuint s, ALenum p, float v) { o_alSourcef(src_of(s), p, v); }
void JOAN_CDECL w_alSource3f(ALuint s, ALenum p, float x, float y, float z) { o_alSource3f(src_of(s), p, x, y, z); }
void JOAN_CDECL w_alSourcei(ALuint s, ALenum p, ALint v) {
    if (replaying() && p == 0x1009 /* AL_BUFFER */ && v != 0)
        v = static_cast<ALint>(real_name(g_al_buffers, static_cast<ALuint>(v)));
    o_alSourcei(src_of(s), p, v);
}
void JOAN_CDECL w_alSourcePlay(ALuint s) { o_alSourcePlay(src_of(s)); }
void JOAN_CDECL w_alSourcePause(ALuint s) { o_alSourcePause(src_of(s)); }
void JOAN_CDECL w_alSourceStop(ALuint s) { o_alSourceStop(src_of(s)); }
void JOAN_CDECL w_alSourceRewind(ALuint s) { o_alSourceRewind(src_of(s)); }
void JOAN_CDECL w_alSourceQueueBuffers(ALuint s, ALsizei n, const ALuint* bufs) {
    if (!replaying() || n <= 0 || !bufs || n > 256) return o_alSourceQueueBuffers(src_of(s), n, bufs);
    ALuint r[256];
    for (ALsizei i = 0; i < n; ++i) r[i] = real_name(g_al_buffers, bufs[i]);
    o_alSourceQueueBuffers(src_of(s), n, r);
}

// value (the game asks whether its sources / buffers still exist)
ALboolean JOAN_CDECL w_alIsSource(ALuint name) {
    GAME_THREAD_OR(alIsSource, o_alIsSource(name));
    return value<ALboolean>(Id::alIsSource, name, [&] { return o_alIsSource(name); });
}
ALboolean JOAN_CDECL w_alIsBuffer(ALuint name) {
    GAME_THREAD_OR(alIsBuffer, o_alIsBuffer(name));
    return value<ALboolean>(Id::alIsBuffer, name, [&] { return o_alIsBuffer(name); });
}

// real, the game gets the recorded buffer names (which buffers finished playing depends on real time)
void JOAN_CDECL w_alSourceUnqueueBuffers(ALuint src, ALsizei n, ALuint* out) {
    GAME_THREAD_OR(alSourceUnqueueBuffers, o_alSourceUnqueueBuffers(src, n, out));
    if (n > 0 && out) {
        // replay: unqueue only what is really processed (a source must not lose queued buffers it still plays);
        // the game sees the recorded names
        ALuint real_out[64] = {};
        const ALsizei m = n < 64 ? n : 64;
        if (replaying()) {
            ALint processed = 0;
            o_alGetSourcei(src_of(src), 0x1016 /* AL_BUFFERS_PROCESSED */, &processed);
            if (processed > 0) o_alSourceUnqueueBuffers(src_of(src), processed < m ? processed : m, real_out);
            std::uint32_t got = 0;
            if (const std::uint8_t* d = play_var(Id::alSourceUnqueueBuffers, static_cast<std::uint32_t>(n), got))
                std::memcpy(out, d, std::min<std::uint32_t>(got, static_cast<std::uint32_t>(n) * 4u));
            return;
        }
        o_alSourceUnqueueBuffers(src, n, out);
        Bytes b;
        b.put_raw(out, static_cast<std::uint32_t>(n) * 4u);
        rec(Id::alSourceUnqueueBuffers, static_cast<std::uint32_t>(n), b);
        return;
    }
    o_alSourceUnqueueBuffers(src, n, out);
}

// value
ALenum JOAN_CDECL w_alGetError() {
    GAME_THREAD_OR(alGetError, o_alGetError());
    return value<ALenum>(Id::alGetError, 0, [] { return o_alGetError(); });
}

// real (a replay needs an audio device whenever the recording had one)
void* JOAN_CDECL w_alcOpenDevice(const char* name) {
    GAME_THREAD_OR(alcOpenDevice, o_alcOpenDevice(name));
    void* const dev = o_alcOpenDevice(name);
    const std::uint32_t ok = dev != nullptr;
    if (replaying()) {
        std::uint32_t x = 0;
        if (play(Id::alcOpenDevice, 0, x) && x != ok)
            diverge(Id::alcOpenDevice, ok ? "no OpenAL device while recording, one now" : "no OpenAL device now");
        return dev;
    }
    rec(Id::alcOpenDevice, 0, ok);
    return dev;
}

// real
void* JOAN_CDECL w_alcCreateContext(void* dev, const ALint* attrs) {
    GAME_THREAD_OR(alcCreateContext, o_alcCreateContext(dev, attrs));
    void* const ctx = o_alcCreateContext(dev, attrs);
    const std::uint32_t ok = ctx != nullptr;
    if (replaying()) {
        std::uint32_t x = 0;
        if (play(Id::alcCreateContext, 0, x) && x != ok) diverge(Id::alcCreateContext, "the OpenAL context differs");
        return ctx;
    }
    rec(Id::alcCreateContext, 0, ok);
    return ctx;
}

// ---------------------------------------------------------------- factories (COM objects: com.cpp)

void* WINAPI w_Direct3DCreate8(UINT sdk) {
    GAME_THREAD_OR(Direct3DCreate8, o_Direct3DCreate8(sdk));
    void* const d3d = o_Direct3DCreate8(sdk);
    const std::uint32_t ok = d3d != nullptr;
    if (replaying()) {
        std::uint32_t x = 0;
        if (play(Id::Direct3DCreate8, sdk, x) && x != ok) diverge(Id::Direct3DCreate8, "Direct3D 8 differs");
    } else {
        rec(Id::Direct3DCreate8, sdk, ok);
    }
    if (d3d && mode() != Mode::Off) wrap_direct3d(d3d);
    return d3d;
}

// ---------------------------------------------------------------- the table

struct Hook {
    const char* dll;
    const char* name;
    void* wrapper;
    void** original;
};
#define H(dll, name) {dll, #name, reinterpret_cast<void*>(&w_##name), reinterpret_cast<void**>(&o_##name)}
const Hook kHooks[] = {
    H("KERNEL32.dll", QueryPerformanceCounter), H("KERNEL32.dll", QueryPerformanceFrequency),
    H("KERNEL32.dll", GetEnvironmentStringsW), H("KERNEL32.dll", FreeEnvironmentStringsW),
    H("KERNEL32.dll", GetLocalTime), H("KERNEL32.dll", GetSystemTime), H("KERNEL32.dll", GetSystemTimeAsFileTime),
    H("KERNEL32.dll", GetTimeZoneInformation), H("KERNEL32.dll", Sleep), H("KERNEL32.dll", SleepEx),
    H("KERNEL32.dll", WaitForSingleObject), H("KERNEL32.dll", WaitForMultipleObjectsEx), H("KERNEL32.dll", ReadFileEx),
    H("KERNEL32.dll", GlobalMemoryStatus), H("KERNEL32.dll", GetDiskFreeSpaceA), H("KERNEL32.dll", GetDriveTypeA),
    H("KERNEL32.dll", GetVersion), H("KERNEL32.dll", GetVersionExA), H("KERNEL32.dll", GetLastError),
    H("KERNEL32.dll", GetCommandLineA), H("KERNEL32.dll", CreateFileA), H("KERNEL32.dll", ReadFile),
    H("KERNEL32.dll", FindFirstFileA), H("KERNEL32.dll", FindNextFileA), H("KERNEL32.dll", FindClose),
    H("KERNEL32.dll", CreateEventA), H("KERNEL32.dll", CloseHandle), H("KERNEL32.dll", ExitProcess),
    H("USER32.dll", PeekMessageA), H("USER32.dll", GetMessageA), H("USER32.dll", TranslateMessage),
    H("USER32.dll", DispatchMessageA), H("USER32.dll", MsgWaitForMultipleObjectsEx), H("USER32.dll", GetCursorPos),
    H("USER32.dll", GetKeyState), H("USER32.dll", AdjustWindowRectEx), H("USER32.dll", ShowCursor),
    H("USER32.dll", SetCursorPos), H("USER32.dll", RegisterClassA), H("USER32.dll", CreateWindowExA),
    H("USER32.dll", ShowWindow), H("USER32.dll", UpdateWindow), H("USER32.dll", SetFocus),
    H("USER32.dll", SetWindowPos), H("USER32.dll", SetWindowLongA), H("USER32.dll", DestroyWindow),
    H("USER32.dll", DefWindowProcA), H("USER32.dll", MessageBoxA),
    H("comdlg32.dll", GetOpenFileNameA),
    H("WINMM.dll", timeGetTime),
    H("OpenAL32.dll", alGetSourcei), H("OpenAL32.dll", alGetError), H("OpenAL32.dll", alcOpenDevice),
    H("OpenAL32.dll", alcCreateContext), H("OpenAL32.dll", alGenSources), H("OpenAL32.dll", alGenBuffers),
    H("OpenAL32.dll", alIsSource), H("OpenAL32.dll", alIsBuffer), H("OpenAL32.dll", alSourceUnqueueBuffers),
    H("OpenAL32.dll", alDeleteSources), H("OpenAL32.dll", alDeleteBuffers), H("OpenAL32.dll", alBufferData),
    H("OpenAL32.dll", alSourcef), H("OpenAL32.dll", alSource3f), H("OpenAL32.dll", alSourcei),
    H("OpenAL32.dll", alSourcePlay), H("OpenAL32.dll", alSourcePause), H("OpenAL32.dll", alSourceStop),
    H("OpenAL32.dll", alSourceRewind), H("OpenAL32.dll", alSourceQueueBuffers),
    H("d3d8.dll", Direct3DCreate8),
};
#undef H

bool same_name(const char* a, const char* b) {
    if (!a || !b) return false;
    for (;; ++a, ++b) {
        const int x = (*a >= 'A' && *a <= 'Z') ? *a - 'A' + 'a' : *a, y = (*b >= 'A' && *b <= 'Z') ? *b - 'A' + 'a' : *b;
        if (x != y) return false;
        if (!*a) return true;
    }
}

std::uintptr_t* slot_of(const Hook& h) {
    const auto dlls = data_image::import_dlls();
    for (const data_image::Import& im : data_image::imports()) {
        if (!same_name(dlls[im.dll], h.dll)) continue;
        if (im.name && std::strcmp(im.name, h.name) == 0)
            return &img::global<std::uintptr_t>(img::Addr(im.slot));
    }
    return nullptr;
}

bool hook_slot(const Hook& h, std::uintptr_t* slot) {
    if (!slot || *slot == 0) return false;
    if (*slot == reinterpret_cast<std::uintptr_t>(h.wrapper)) return true;
    const platform::memory::Writable w(slot, sizeof(*slot));
    if (!w.ok()) return false;
    *h.original = reinterpret_cast<void*>(*slot);
    *slot = reinterpret_cast<std::uintptr_t>(h.wrapper);
    return true;
}

}  // namespace

// ---------------------------------------------------------------- internal API

void install_imports() {
    int n = 0;
    for (const Hook& h : kHooks) {
        std::uintptr_t* slot = slot_of(h);
        if (!slot) {
            log("import %s!%s not found in joan.exe's import table", h.dll, h.name);
            continue;
        }
        if (hook_slot(h, slot))
            ++n;
        else
            log("import %s!%s has no target yet - not wrapped", h.dll, h.name);
    }
    log("%d imports wrapped", n);
}

void inject_apc(std::uint32_t seq, std::uint32_t error, std::uint32_t bytes) {
    const auto it = g_apc_replay.find(seq);
    if (it == g_apc_replay.end()) {
        diverge(Id::ReadFileEx, "a recorded I/O completion has no read request in this run");
        return;
    }
    const ApcReplay a = it->second;
    g_apc_replay.erase(it);
    if (a.got != bytes) warn("ReadFileEx request %u: read %u bytes now, %u while recording", static_cast<unsigned>(seq),
                             static_cast<unsigned>(a.got), static_cast<unsigned>(bytes));
    a.ov->Internal = error == 0 ? 0 : 0xc0000001;  // STATUS_SUCCESS / STATUS_UNSUCCESSFUL
    a.ov->InternalHigh = bytes;
    a.routine(error, bytes, a.ov);
}

LRESULT real_def_window_proc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    if (o_DefWindowProcA) return o_DefWindowProcA(h, msg, wp, lp);
#if defined(_WIN32)
    return DefWindowProcA(h, msg, wp, lp);
#else
    return 0;  // the platform layer's DefWindowProcA is the import's target: wrapped before any call reaches here
#endif
}

void pump_real_messages() {
    if (!o_PeekMessageA || !o_DispatchMessageA) return;
    MSG m;
    while (o_PeekMessageA(&m, nullptr, 0, 0, PM_REMOVE))
        if (m.message != WM_QUIT) o_DispatchMessageA(&m);  // to wndproc_proxy: DefWindowProc
}

void real_exit_process(UINT code) {
    if (o_ExitProcess) o_ExitProcess(code);
    sys::end_process(code);
}

}  // namespace joan::replay
