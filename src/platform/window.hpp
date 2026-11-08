// The platform layer (phase C4, SDL3): the game window (sdl_window.cpp), keyboard / mouse (sdl_input.cpp), time
// (sdl_time.cpp) - what our own code (the GL device, the developer menu) asks of it.
#pragma once

#include <filesystem>
#include <string>
#include <vector>

/// The platform layer: game window, display queries, settings file location, time, threads, code pages (SDL3 based).
namespace joan::platform {

/// The folder of the executable (openjoan.ini, the logs; SDL_GetBasePath, also before SDL_Init), with its separator.
const std::filesystem::path& exe_dir();
/// The settings file (Phase 1 rename): `dir`/openjoan.ini, or `dir`/joan_re.ini - the name before the rename - when
/// only that one exists; openjoan.ini when neither does (every setting at its default). Looks at the folder each call.
std::filesystem::path settings_path(const std::filesystem::path& dir);
/// settings_path(exe_dir()): the settings file next to the executable.
const std::filesystem::path& settings_file();
/// The settings file names: the current one and the fallback.
inline constexpr const char* kSettingsName = "openjoan.ini";
/// The name of the settings file before the Phase 1 rename, read when only that one exists.
inline constexpr const char* kSettingsFallbackName = "joan_re.ini";
/// An error message box (before or after the game window exists; in front of it when it does).
void error_box(const char* title, const char* text);

/// SDL's video subsystem, once (the game window, and the display queries below that may come before it).
bool video_init();
/// A display mode: size in pixels, refresh rate in whole Hz as Windows lists it (59 for 59.94, 0 = unknown), bits
/// per pixel.
struct ScreenMode {
    int width = 0;    ///< Width in pixels.
    int height = 0;   ///< Height in pixels.
    int refresh = 0;  ///< Refresh rate in whole Hz (59 for 59.94); 0 = unknown.
    int bits = 0;     ///< Bits per pixel.
};
/// Displays in Direct3D's adapter order: the primary display first, then the others.
int display_count();
/// Every full-screen mode of display `index` (SDL's list).
std::vector<ScreenMode> display_modes(int index);
/// The mode display `index` shows now / its desktop mode.
bool current_display_mode(int index, ScreenMode& out);
/// The desktop mode of display `index` (see current_display_mode).
bool desktop_display_mode(int index, ScreenMode& out);
/// The top-left corner of display `index` on the desktop.
bool display_origin(int index, int& x, int& y);

/// Full-screen request of the GL device (Direct3D would switch the display mode): the game window covers its monitor
/// as a borderless window. True when the SDL window did it; false when the window is not ours or SDL failed.
bool cover_monitor(void* hwnd);

/// Is `hwnd` the SDL game window (the only window the GL device draws into)?
bool is_sdl_window(void* hwnd);
/// OpenGL 4.6 (or 4.5) core context on the SDL game window, made current; nullptr + `error` on failure.
void* gl_create_context(void* hwnd, bool debug, int& major, int& minor, std::string& error);
/// Makes the OpenGL context `ctx` current on the SDL game window `hwnd`; false on failure.
bool gl_make_current(void* hwnd, void* ctx);
/// Destroys the OpenGL context `ctx`.
void gl_destroy_context(void* ctx);
/// Presents (swaps) the back buffer of the SDL game window `hwnd`.
void gl_swap(void* hwnd);
/// 0 = off, 1 = vsync, -1 = adaptive (plain vsync when the driver has no adaptive vsync).
bool gl_set_swap_interval(int interval);
/// A GL function of the current context (OpenGL 1.1 ones included).
void* gl_proc(const char* name);

/// Client area size in pixels of the SDL game window (false for any other window).
bool client_size(void* hwnd, int& w, int& h);
/// Has the SDL game window the keyboard focus?
bool window_focused(void* hwnd);
/// Refresh rate of the SDL game window's display in Hz, 0 if unknown.
int refresh_rate(void* hwnd);
/// Is the key with Windows virtual-key code `vk` down (the developer menu / screenshot keys; the key position of a
/// US layout for the punctuation keys, e.g. 0xC0 = the key left of 1)? SDL's keyboard state when the SDL window
/// exists, else GetAsyncKeyState.
bool key_down(int vk);

/// The game's clocks (sdl_time.cpp): timeGetTime's milliseconds, the performance counter and its frequency.
unsigned long time_ms();
/// The performance counter of the game (high-resolution clock), in ticks of perf_frequency() per second.
long long perf_counter();
/// Ticks per second of perf_counter().
long long perf_frequency();
/// Sleeps `ms` milliseconds (0 = yield the rest of the time slice).
void sleep_ms(unsigned long ms);

}  // namespace joan::platform
