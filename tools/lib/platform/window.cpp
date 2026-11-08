#include "platform/window.hpp"

#include <SDL3/SDL.h>

#include <stdexcept>

#include "core/log.hpp"

namespace joan::platform {

Window::Window(const WindowConfig& config) : fullscreen_(config.fullscreen) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());

    // OpenGL 3.3 core everywhere except GLES platforms; macOS needs forward-compat.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_WindowFlags flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    if (config.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
    window_ = SDL_CreateWindow(config.title.c_str(), config.width, config.height, flags);
    if (!window_) throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());

    gl_context_ = SDL_GL_CreateContext(window_);
    if (!gl_context_) throw std::runtime_error(std::string("SDL_GL_CreateContext failed: ") + SDL_GetError());
    SDL_GL_MakeCurrent(window_, static_cast<SDL_GLContext>(gl_context_));
    if (config.vsync && !SDL_GL_SetSwapInterval(-1)) SDL_GL_SetSwapInterval(1);  // adaptive, else normal vsync

    log::info("window created ({}x{}, video driver {})", config.width, config.height, SDL_GetCurrentVideoDriver());
}

Window::~Window() {
    if (gl_context_) SDL_GL_DestroyContext(static_cast<SDL_GLContext>(gl_context_));
    if (window_) SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Window::poll_events(const std::function<void(const SDL_Event&)>& on_event) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (on_event) on_event(ev);
        switch (ev.type) {
            case SDL_EVENT_QUIT:
                return false;
            case SDL_EVENT_KEY_DOWN:
                if (ev.key.key == SDLK_RETURN && (ev.key.mod & SDL_KMOD_ALT)) toggle_fullscreen();
                if (ev.key.key == SDLK_F11) toggle_fullscreen();
                break;
            default:
                break;
        }
    }
    return true;
}

void Window::set_title(const std::string& title) { SDL_SetWindowTitle(window_, title.c_str()); }

void Window::swap() { SDL_GL_SwapWindow(window_); }

void Window::toggle_fullscreen() {
    fullscreen_ = !fullscreen_;
    SDL_SetWindowFullscreen(window_, fullscreen_);
}

void Window::drawable_size(int& w, int& h) const { SDL_GetWindowSizeInPixels(window_, &w, &h); }

std::string executable_dir() {
    const char* base = SDL_GetBasePath();
    return base ? std::string(base) : std::string();
}

}  // namespace joan::platform
