// The game's clocks and sleeps on SDL3 (phase C4 round P3, src/platform/README.md): joan.exe's timeGetTime (WINMM),
// QueryPerformanceCounter / QueryPerformanceFrequency and Sleep (KERNEL32) answered by SDL's timer functions.
//
// - timeGetTime: milliseconds of SDL_GetTicks (the performance counter, so finer than timeGetTime's default 1 ms /
//   15.6 ms tick) plus a fixed offset: the game sees a running system clock as before (no 0 values near the start,
//   which some of its "time since" checks would read as "never"). Wraps at 2^32 ms like timeGetTime.
// - QueryPerformanceCounter / Frequency: SDL_GetPerformanceCounter / Frequency (on Windows the same counter).
// - Sleep: SDL_Delay (on Windows a high-resolution waitable timer: the 1 ms waits of the frame loop are 1 ms without
//   timeBeginPeriod); Sleep(0) yields the time slice as before.
// SleepEx is platform/files.cpp's: it is the alertable wait of the streaming code's ReadFileEx completions.
#include "win32_types.hpp"

#include <SDL3/SDL.h>

#include <cstdint>
#include <thread>

#include "../runtime/loader/import_overrides.hpp"
#include "window.hpp"

namespace {

constexpr std::uint64_t kClockBase = 10000000;  // ms (~2.8 hours of "uptime")

DWORD WINAPI j_timeGetTime() { return static_cast<DWORD>(SDL_GetTicks() + kClockBase); }

BOOL WINAPI j_QueryPerformanceCounter(LARGE_INTEGER* out) {
    if (!out) return FALSE;
    out->QuadPart = static_cast<LONGLONG>(SDL_GetPerformanceCounter());
    return TRUE;
}

BOOL WINAPI j_QueryPerformanceFrequency(LARGE_INTEGER* out) {
    if (!out) return FALSE;
    out->QuadPart = static_cast<LONGLONG>(SDL_GetPerformanceFrequency());
    return TRUE;
}

void WINAPI j_Sleep(DWORD ms) {
    if (ms == 0) {
        std::this_thread::yield();
        return;
    }
    SDL_Delay(static_cast<Uint32>(ms));
}

}  // namespace

namespace joan::platform {

unsigned long time_ms() { return j_timeGetTime(); }
long long perf_counter() { return static_cast<long long>(SDL_GetPerformanceCounter()); }
long long perf_frequency() { return static_cast<long long>(SDL_GetPerformanceFrequency()); }
void sleep_ms(unsigned long ms) { j_Sleep(static_cast<DWORD>(ms)); }

}  // namespace joan::platform

JOAN_IMPORT_OVERRIDE("WINMM.dll", "timeGetTime", &j_timeGetTime, "SDL timeGetTime");
JOAN_IMPORT_OVERRIDE("KERNEL32.dll", "QueryPerformanceCounter", &j_QueryPerformanceCounter, "SDL QueryPerformanceCounter");
JOAN_IMPORT_OVERRIDE("KERNEL32.dll", "QueryPerformanceFrequency", &j_QueryPerformanceFrequency,
                     "SDL QueryPerformanceFrequency");
JOAN_IMPORT_OVERRIDE("KERNEL32.dll", "Sleep", &j_Sleep, "SDL Sleep");
