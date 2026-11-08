// OpenGL 4.6 core context on the game's window: SDL's (platform/sdl_window.cpp) on the SDL game window, WGL on a
// Win32 one ([Platform] Window=win32).
#pragma once

#include "../platform/win32_types.hpp"

#include <string>

/// OpenGL 4.6 core context on the game window and the function loader.
namespace joan::gl {

/// An OpenGL context bound to the game window; filled by create_context().
struct Context {
    HWND window = nullptr;  ///< The game window the context draws into (set by the caller of create_context).
    HDC dc = nullptr;      ///< WGL: the window's device context; null with the SDL window.
    HGLRC rc = nullptr;    ///< WGL: the GL rendering context; null with the SDL window.
    void* sdl = nullptr;   ///< SDL_GLContext of the SDL window; null with a Win32 (WGL) window.
    int major = 0 /**< OpenGL major version the created context reports */, minor = 0 /**< minor version (4.6, or 4.5 as the fallback) */;
    /// True once a context exists (WGL rendering context or SDL context).
    bool valid() const { return rc != nullptr || sdl != nullptr; }
};

/// Creates a 4.6 (or 4.5) core context on `window`, makes it current and loads
/// the GL functions. `debug` requests a debug context. On failure returns false
/// and describes the problem in `error`.
bool create_context(HWND window, bool debug, int swap_interval, Context& out, std::string& error);
/// Destroys the OpenGL context `ctx` (WGL or SDL).
void destroy_context(Context& ctx);
/// Makes `ctx` current on its window again.
void make_current(const Context& ctx);
/// Presents the back buffer of the window.
void swap_buffers(const Context& ctx);

}  // namespace joan::gl
