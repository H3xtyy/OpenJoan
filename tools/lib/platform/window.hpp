// SDL3 window + OpenGL context. Replaces the original's Win32 window,
// DirectDraw/Direct3D 8 device setup (en3d/sysen.cpp, en3d/vgabuf.cpp) and
// the resolution patching that the community d3d8.dll wrapper had to do:
// the window is resizable, high-DPI aware and supports borderless full screen.
#pragma once

#include <functional>
#include <memory>
#include <string>

struct SDL_Window;
union SDL_Event;

namespace joan::platform {

struct WindowConfig {
    std::string title = "Wars and Warriors: Joan of Arc";
    int width = 1280;
    int height = 720;
    bool fullscreen = false;  // borderless desktop full screen
    bool vsync = true;
};

class Window {
public:
    explicit Window(const WindowConfig& config);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    /// Pumps events; returns false once the user asked to quit. Every event is
    /// also passed to `on_event` (after the window's own handling: Alt+Enter / F11).
    bool poll_events(const std::function<void(const SDL_Event&)>& on_event = {});
    void set_title(const std::string& title);
    void swap();
    void toggle_fullscreen();

    /// Drawable size in physical pixels (for glViewport).
    void drawable_size(int& w, int& h) const;
    [[nodiscard]] SDL_Window* sdl() const { return window_; }

private:
    SDL_Window* window_ = nullptr;
    void* gl_context_ = nullptr;
    bool fullscreen_ = false;
};

/// Directory containing the running executable (SDL_GetBasePath).
std::string executable_dir();

}  // namespace joan::platform
