#include "context.hpp"

#include "glapi.hpp"

#include "../platform/window.hpp"

namespace joan::gl {

// The context is SDL's, on the SDL game window (platform/sdl_window.cpp); the WGL path for a Win32 window went with
// [Platform] Window=win32 (cleanup C1).
bool create_context(HWND window, bool debug, int swap_interval, Context& out, std::string& error) {
    out.window = window;
    out.sdl = platform::gl_create_context(window, debug, out.major, out.minor, error);
    if (!out.sdl) return false;
    if (const char* missing = load_functions(&platform::gl_proc)) {
        error = std::string("missing OpenGL function ") + missing;
        return false;
    }
    platform::gl_set_swap_interval(swap_interval);
    return true;
}

void destroy_context(Context& ctx) {
    if (ctx.sdl) platform::gl_destroy_context(ctx.sdl);
    if (ctx.sdl) unload_functions();
    ctx = Context{};
}

void make_current(const Context& ctx) {
    if (ctx.sdl) platform::gl_make_current(ctx.window, ctx.sdl);
}

void swap_buffers(const Context& ctx) {
    if (ctx.sdl) platform::gl_swap(ctx.window);
}

}  // namespace joan::gl
