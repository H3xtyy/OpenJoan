// Record / replay internals shared by replay.cpp (state, injection, frame hashes, reports), imports.cpp (IAT wrappers)
// and com.cpp (COM method wrappers). See replay.hpp for the overview.
#pragma once

#if defined(_WIN32)
#include <windows.h>
#else
#include "../../platform/win32_types.hpp"
#endif

#include <bit>
#include <cstdint>
#include <cstring>

// Window messages the recorder records / injects (platform/win32_types.hpp has the ones the platform layer uses).
#ifndef WM_ACTIVATEAPP
#define WM_ACTIVATEAPP 0x001C
#endif
#ifndef WM_INPUTLANGCHANGEREQUEST
#define WM_INPUTLANGCHANGEREQUEST 0x0050
#endif
#ifndef WM_SYSCOMMAND
#define WM_SYSCOMMAND 0x0112
#endif

#include "ids.hpp"
#include "stream.hpp"

namespace joan::replay {

/// Whether nothing is installed (Off), the session is recorded (Record) or a recording is replayed (Replay).
enum class Mode : std::uint8_t { Off, Record, Replay };

/// Current mode (Off when nothing is installed, and after a divergence in ReplayOnDivergence=continue).
Mode mode();
/// True when the calling thread is the game thread and recording / replay is active: only those calls are logged
/// (others go straight to the real function and are counted).
bool on_main();
/// True while recording.
inline bool recording() { return mode() == Mode::Record; }
/// True while replaying.
inline bool replaying() { return mode() == Mode::Replay; }
/// A handle as a recording stores it: 32 bits, the x86 format that x86 and x64 builds share (stream.hpp). The replay
/// only compares and looks up these bits (the window, tracked handles); a replayed call that hands a handle back gives
/// the recorded bits, which the game passes on to further replayed calls and never dereferences.
inline std::uint32_t recorded_bits(const void* h) noexcept {
    return static_cast<std::uint32_t>(std::bit_cast<std::uintptr_t>(h));
}
/// Counts a call of `id` made on another thread (reported at the end).
void other_thread(Id id);

// ---------------------------------------------------------------- record
/// Appends a Call event for `id` with the argument check value and `n` payload bytes to the recording (record mode).
void record_call(Id id, std::uint32_t check, const void* payload, std::uint32_t n);
template <class T>
/// record_call with the bytes of `v` as the payload.
void rec(Id id, std::uint32_t check, const T& v) {
    record_call(id, check, &v, sizeof(T));
}
/// record_call with the bytes of `b` as the payload.
inline void rec(Id id, std::uint32_t check, const Bytes& b) {
    record_call(id, check, b.data.data(), static_cast<std::uint32_t>(b.data.size()));
}
/// A window message the game's WndProc is about to handle (record mode, game thread).
void record_wndproc(std::uint32_t msg, std::uint32_t wparam, std::uint32_t lparam);
/// A ReadFileEx completion routine is about to run (record mode, game thread).
void record_apc(std::uint32_t seq, std::uint32_t error, std::uint32_t bytes);

// ---------------------------------------------------------------- replay
/// Injects the window messages / I/O completions recorded before the next call, then reads that call, which must be
/// `id` with the same check value. Returns its payload (size in n), or nullptr when replay is no longer active (the
/// recording diverged with ReplayOnDivergence=continue, or ended): the caller then calls the real function.
const std::uint8_t* play_raw(Id id, std::uint32_t check, std::uint32_t& n);
/// Reports a payload whose size differs from what this build expects; returns false (or does not return).
bool payload_size_mismatch(Id id, std::uint32_t recorded, std::uint32_t expected);
/// play_raw() for a fixed-size payload (a separate name: an overload taking std::uint32_t& made
/// play(id, check, some_uint32) read the payload SIZE, e.g. UINT / BOOL-as-u32 results - fixed in patch 134).
/// play() for a fixed-size payload. A size different from the recording's is a divergence.
template <class T>
bool play(Id id, std::uint32_t check, T& out) {
    std::uint32_t n = 0;
    const std::uint8_t* p = play_raw(id, check, n);
    if (!p) return false;
    if (n != sizeof(T)) return payload_size_mismatch(id, n, static_cast<std::uint32_t>(sizeof(T)));
    std::memcpy(&out, p, sizeof(T));
    return true;
}
/// Reports a divergence that a wrapper detected itself (a result that must be equal, e.g. a file that opened in the
/// recording but not now). Stops the game (ReplayOnDivergence=stop) or switches replay off.
void diverge(Id id, const char* what);
/// A difference that does not stop the replay (it is listed in the report).
void warn(const char* fmt, ...);

/// Injection callbacks (replay): imports.cpp delivers a recorded ReadFileEx completion.
void inject_apc(std::uint32_t seq, std::uint32_t error, std::uint32_t bytes);

// ---------------------------------------------------------------- window
/// The game window as the replay layer knows it.
HWND game_window();
/// Remembers `h` as the game window.
void set_game_window(HWND h);
/// The game's window procedure (RegisterClassA wrapper puts wndproc_proxy in its place).
WNDPROC game_wndproc();
/// Remembers the game's window procedure `p` (see game_wndproc).
void set_game_wndproc(WNDPROC p);
/// The window procedure the replay layer puts in place of the game's: records or injects the messages the game reacts to, then calls the game's.
LRESULT CALLBACK wndproc_proxy(HWND h, UINT msg, WPARAM wp, LPARAM lp);
/// Messages whose handling by the game is recorded / injected (the ones its WndProc reacts to).
bool relevant_message(UINT msg, WPARAM wp);
/// HWND in a payload: 0 = null, 1 = the game window, else the raw value.
std::uint32_t hwnd_code(HWND h);
/// The window handle for a payload code (see hwnd_code).
HWND hwnd_from_code(std::uint32_t code);
/// The real DefWindowProcA / PeekMessageA / DispatchMessageA (not logged).
LRESULT real_def_window_proc(HWND h, UINT msg, WPARAM wp, LPARAM lp);
/// Pumps the real window messages (PeekMessageA / DispatchMessageA, not logged).
void pump_real_messages();

// ---------------------------------------------------------------- frames, handles
/// Present: hashes the frame and writes (record) or compares (replay) it. `device` is the IDirect3DDevice8.
void frame_boundary(void* device);
/// Number of the frame being hashed (the count of Present calls so far).
std::uint32_t frame_number();
/// A handle value the game may store (files, events, find handles, registry keys): normalised in the state hash.
void track_handle(const void* h);
/// Stops tracking the handle `h` (see track_handle).
void untrack_handle(const void* h);
/// File content hash of a file opened for reading (cached per path; 0 when hashing is off or the file is unreadable).
std::uint64_t file_content_hash(const char* path, std::uint64_t* size);
/// Whether file content hashing is on (see file_content_hash).
bool hash_files();
/// Replay: waits until as much real time has passed since the start as the recording's clock (timeGetTime) says,
/// so a replay plays at the recorded speed ([Debug] ReplayPace=1, default; 0 = as fast as possible).
void pace_to(std::uint32_t recorded_ms);

// ---------------------------------------------------------------- end
/// ExitProcess of the game: writes the end of the recording / checks it on replay and writes the report.
void game_exit(UINT code);
/// Ends the process now through the game's ExitProcess import target (reports are written first).
[[noreturn]] void exit_now(UINT code);
/// The ExitProcess the game's import slot held before install (the loader's: end-of-run reports, then exit).
[[noreturn]] void real_exit_process(UINT code);
/// Exit code of the process: the game's when recording, the replay result when replaying (0 identical, 3 divergence,
/// 4 state hash mismatches).
UINT replay_exit_code(UINT game_code);

// ---------------------------------------------------------------- installation (imports.cpp / com.cpp)
/// Installs the wrappers over the game's imports (IAT slots) for the session's mode.
void install_imports();
/// Wraps the methods of the Direct3D 8 object `d3d` and the objects it creates (com.cpp).
void wrap_direct3d(void* d3d);

/// openjoan.log line with a "replay: " prefix.
void log(const char* fmt, ...);

}  // namespace joan::replay
