// The game window on SDL3 (phase C4 round P1, src/platform/README.md): joan.exe's USER32 window and message imports are
// answered here, so the window the game draws into is an SDL window and the game's message loop runs on SDL events.
//
// What the game does with its window (src/game/unknown_mainwnd.cpp, syswnd.cpp, d3dsys.cpp, misc_004994b0.cpp):
// - Window::create 0x004d6e00: RegisterClassA("JoanOfArc", wndproc 0x004d7d60, icon 0x66), AdjustWindowRectEx,
//   CreateWindowExA(0x40000, style 0x12ca0000 = visible captioned window, client = the menu resolution at 0,0),
//   UpdateWindow, SetFocus.
// - start_d3d (d3dsys.cpp): SetWindowLongA style 0x92000000 (popup) / ex 0x40008, SetWindowPos(TOPMOST, 0, 0, mode)
//   - then the GL device makes it borderless full screen (gl/device.cpp, cover_monitor below).
// - Game::run: PeekMessageA(PM_NOREMOVE) -> GetMessageA / TranslateMessage / DispatchMessageA, otherwise the frame
//   wait MsgWaitForMultipleObjectsEx(0, -, ms, QS_ALLINPUT, MWMO_ALERTABLE): only WAIT_TIMEOUT runs a frame (the
//   alertable wait also runs the ReadFileEx completions of the streaming code).
// - The window procedure (0x004d7d60 -> 0x004d7ef0 -> 0x00603e20 -> 0x005a1e20) reacts to WM_CREATE (keeps the
//   HWND), WM_DESTROY (PostQuitMessage), WM_PAINT (redraw flag), WM_ACTIVATEAPP (activate / suspend: DirectInput,
//   sound, the device), WM_CLOSE (quit flag), WM_SYSCOMMAND SC_MONITORPOWER (minimise), WM_INPUTLANGCHANGEREQUEST.
//   Keyboard and mouse come from DirectInput (sdl_input.cpp answers it from SDL events), never from messages.
//
// So the window here: SDL owns it (its own window class and procedure); the game's procedure gets the messages it
// knows, made from SDL events:
//   SDL_EVENT_QUIT / WINDOW_CLOSE_REQUESTED -> WM_CLOSE (posted)
//   WINDOW_FOCUS_GAINED / LOST              -> WM_ACTIVATEAPP 1 / 0 (sent: delivered inside PeekMessageA /
//                                              GetMessageA, as Windows delivers sent messages)
//   WINDOW_EXPOSED                          -> WM_PAINT (posted, one at a time)
//   keyboard / mouse                        -> the DirectInput devices of sdl_input.cpp (round P2)
// plus WM_CREATE (and WM_ACTIVATEAPP 1 when the new window has the focus) inside CreateWindowExA, WM_DESTROY inside
// DestroyWindow, WM_QUIT from PostQuitMessage. DefWindowProcA of the game window does nothing (SDL's procedure
// already handled what Windows sent). The HWND the game keeps is SDL's real window handle (the options dialog uses
// it as before). The window is an OpenGL window: the GL device's context is SDL's (gl_create_context, round P3).
//
// Window::create's class resources: LoadIconA(instance, 0x66) gives the icon's image as an SDL surface (shown with
// SDL_SetWindowIcon, as Windows' WM_SETICON showed the HICON) and GetStockObject(BLACK_BRUSH) a handle that only
// goes into WNDCLASSA.hbrBackground (SDL's window class paints its own background). The developer key 0x79
// (misc_52f930.cpp load_world_dialog, cheat mode) asks for two files with GetOpenFileNameA: SDL's file dialog here.
//
// The replay layer (src/replay/imports.cpp) wraps these imports on top of this file exactly as it wrapped USER32:
// the recording holds the messages the game got and what its procedure reacted to.
#include "win32_types.hpp"
#if !defined(JOAN_WIN32_OWN_TYPES)
#include <commdlg.h>  // OPENFILENAMEA (after windows.h); win32_types.hpp has its own elsewhere
#endif

#include <SDL3/SDL.h>

#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <filesystem>
#include <string>
#include <vector>

#include "../runtime/loader/import_overrides.hpp"
#include "../registry.hpp"
#include "files.hpp"
#include "sdl_internal.hpp"
#include "text.hpp"
#include "window.hpp"
#include "threads.hpp"
#include "ini.hpp"

namespace {

// ---------------------------------------------------------------- settings

struct PlatformSettings {
    bool gamepad = true;  // game pads ([Platform] Gamepad)
};

// Asked by the loader while it resolves joan.exe's imports, before host::start reads the settings: the ini is read
// here (next to openjoan.exe).
const PlatformSettings& settings() {
    static const PlatformSettings s = [] {
        PlatformSettings r;
        const std::filesystem::path& ini = joan::platform::settings_file();
        r.gamepad = joan::kReleaseBuild || joan::platform::ini::get_int("Platform", "Gamepad", 1, ini.c_str()) != 0;  // release: always on
        return r;
    }();
    return s;
}

// ---------------------------------------------------------------- the window

constexpr UINT kWmActivateApp = 0x1c;

struct State {
    WNDPROC proc = nullptr;  // the class's window procedure (the replay layer's proxy when it records / replays)
    SDL_Surface* icon = nullptr;  // the class's icon (j_LoadIconA)
    std::string class_name;
    SDL_Window* window = nullptr;
    HWND hwnd = nullptr;
    DWORD thread = 0;
    LONG style = 0;
    LONG ex_style = 0;
    bool active = false;         // what the game was last told by WM_ACTIVATEAPP
    bool close_posted = false;   // a WM_CLOSE is queued
    bool paint_posted = false;   // a WM_PAINT is queued
    bool paint_pending = false;  // UpdateWindow sends WM_PAINT (a new visible window has not been painted)
    std::deque<MSG> posted;      // what PeekMessageA / GetMessageA return
    std::deque<MSG> sent;        // delivered to the procedure inside PeekMessageA / GetMessageA
};
State g;

bool ours(HWND h) { return h != nullptr && h == g.hwnd; }
// The thread that created the game window (its messages stay ours after DestroyWindow: WM_QUIT).
bool on_window_thread() { return g.thread != 0 && joan::platform::thread_id() == g.thread; }

// JOAN_NOT_OURS's error (a window or message queue that is not ours: Windows' on Windows, none elsewhere).
[[maybe_unused]] void set_error(DWORD e) { joan::platform::set_last_error(e); }

MSG make_msg(HWND h, UINT message, WPARAM wp, LPARAM lp) {
    MSG m = {};
    m.hwnd = h;
    m.message = message;
    m.wParam = wp;
    m.lParam = lp;
#ifdef _WIN32
    m.time = GetTickCount();
    GetCursorPos(&m.pt);
#else
    m.time = static_cast<DWORD>(joan::platform::time_ms());
    float x = 0, y = 0;
    SDL_GetGlobalMouseState(&x, &y);
    int ox = 0, oy = 0;
    joan::platform::detail::screen_origin(ox, oy);
    m.pt = {static_cast<LONG>(x) - ox, static_cast<LONG>(y) - oy};
#endif
    return m;
}

LRESULT call_proc(HWND h, UINT message, WPARAM wp, LPARAM lp) {
    return g.proc ? g.proc(h, message, wp, lp) : 0;
}

// Tells the game when SDL's focus differs from what the game knows (Windows sends WM_ACTIVATEAPP while the call
// that activated the window runs: CreateWindowExA, ShowWindow, SetFocus - here too).
// What the game is told about the focus: always "active" for a co-op host (keep_active).
bool g_keep_active = false;
bool game_focus(bool focus) { return focus || g_keep_active; }

void sync_activation_now() {
    if (!g.window) return;
    const bool focus = game_focus((SDL_GetWindowFlags(g.window) & SDL_WINDOW_INPUT_FOCUS) != 0);
    if (focus == g.active) return;
    g.active = focus;
    call_proc(g.hwnd, kWmActivateApp, focus ? 1u : 0u, 0);
}

void post(UINT message, WPARAM wp = 0, LPARAM lp = 0) { g.posted.push_back(make_msg(g.hwnd, message, wp, lp)); }

void translate(const SDL_Event& e) {
    switch (e.type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if (!g.close_posted && g.hwnd) {
                g.close_posted = true;
                post(WM_CLOSE);
            }
            break;
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_FOCUS_LOST: {
            const bool focus = e.type == SDL_EVENT_WINDOW_FOCUS_GAINED;
            // Only the last of several changes counts (a modal dialog or message box of the game takes the focus
            // and gives it back: Windows does not deactivate the application for that); delivery compares with
            // what the game was told.
            for (auto it = g.sent.begin(); it != g.sent.end();)
                it = it->message == kWmActivateApp ? g.sent.erase(it) : it + 1;
            g.sent.push_back(make_msg(g.hwnd, kWmActivateApp, game_focus(focus) ? 1u : 0u, 0));
            joan::platform::detail::on_focus(focus);  // DirectInput: foreground devices are lost at once
            break;
        }
        case SDL_EVENT_WINDOW_EXPOSED:
            if (!g.paint_posted && g.hwnd) {
                g.paint_posted = true;
                post(WM_PAINT);
            }
            break;
        default:
            joan::platform::detail::on_input_event(e);  // keyboard / mouse: sdl_input.cpp
            break;
    }
}

void pump() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) translate(e);
}

void deliver_sent() {
    while (!g.sent.empty()) {
        const MSG m = g.sent.front();
        g.sent.pop_front();
        if (m.message == kWmActivateApp) {
            const bool focus = m.wParam != 0;
            if (focus == g.active) continue;
            g.active = focus;
        }
        if (g.hwnd) call_proc(g.hwnd, m.message, m.wParam, m.lParam);
    }
}

bool in_range(UINT message, UINT lo, UINT hi) {
    if (lo == 0 && hi == 0) return true;
    return message >= lo && message <= hi;
}

// Takes (remove) or copies the first posted message in [lo, hi].
bool take(MSG* out, UINT lo, UINT hi, bool remove) {
    for (auto it = g.posted.begin(); it != g.posted.end(); ++it) {
        if (!in_range(it->message, lo, hi) && it->message != WM_QUIT) continue;
        *out = *it;
        if (remove) {
            if (it->message == WM_CLOSE) g.close_posted = false;
            if (it->message == WM_PAINT) g.paint_posted = false;
            g.posted.erase(it);
        }
        return true;
    }
    return false;
}

// ---------------------------------------------------------------- class resources

// An icon resource (RT_GROUP_ICON + its largest RT_ICON image, a DIB of 1 / 4 / 8 / 24 / 32 bits per pixel with
// its AND mask) as an ARGB surface; nullptr when there is none or it is a PNG image.
SDL_Surface* decode_icon(HMODULE module, LPCSTR name) {
    auto bytes = [module](LPCSTR id, LPCSTR type, DWORD& size) -> const std::uint8_t* {
#ifdef _WIN32
        HRSRC r = FindResourceA(module, id, type);
        HGLOBAL h = r ? LoadResource(module, r) : nullptr;
        size = r ? SizeofResource(module, r) : 0;
        return h ? static_cast<const std::uint8_t*>(LockResource(h)) : nullptr;
#else
        (void)module, (void)id, (void)type;  // no PE resources elsewhere: icon 102 comes from embedded_icon()
        size = 0;
        return nullptr;
#endif
    };
    auto u16 = [](const std::uint8_t* p) { return static_cast<unsigned>(p[0] | (p[1] << 8)); };
    auto u32 = [](const std::uint8_t* p) {
        return static_cast<std::uint32_t>(p[0] | (p[1] << 8) | (p[2] << 16) | (static_cast<std::uint32_t>(p[3]) << 24));
    };
    DWORD n = 0;
    const std::uint8_t* dir = bytes(name, MAKEINTRESOURCEA(14), n);  // RT_GROUP_ICON
    if (!dir || n < 6) return nullptr;
    const unsigned count = u16(dir + 4);
    unsigned best_id = 0;
    unsigned best_score = 0;
    for (unsigned i = 0; i < count && 6 + 14 * (i + 1) <= n; ++i) {  // GRPICONDIRENTRY: w h colors - planes bits size id
        const std::uint8_t* e = dir + 6 + 14 * i;
        const unsigned w = e[0] ? e[0] : 256u;
        const unsigned score = w * 64 + u16(e + 6);
        if (score > best_score) {
            best_score = score;
            best_id = u16(e + 12);
        }
    }
    if (best_id == 0) return nullptr;
    const std::uint8_t* dib = bytes(MAKEINTRESOURCEA(best_id), MAKEINTRESOURCEA(3), n);  // RT_ICON
    if (!dib || n < 40 || u32(dib) < 40) return nullptr;  // a PNG image starts with 0x89 'P'
    const int w = static_cast<int>(u32(dib + 4));
    const int h = static_cast<int>(u32(dib + 8)) / 2;  // XOR image + AND mask
    const unsigned bits = u16(dib + 14);
    if (w <= 0 || h <= 0 || w > 256 || h > 256 || u32(dib + 16) != 0 /* BI_RGB */) return nullptr;
    if (bits != 1 && bits != 4 && bits != 8 && bits != 24 && bits != 32) return nullptr;
    const std::size_t colors = bits <= 8 ? (u32(dib + 32) ? u32(dib + 32) : (1u << bits)) : 0;
    const std::uint8_t* pal = dib + u32(dib);
    const std::uint8_t* xr = pal + 4 * colors;
    const std::size_t xor_stride = ((static_cast<std::size_t>(w) * bits + 31) / 32) * 4;
    const std::size_t and_stride = ((static_cast<std::size_t>(w) + 31) / 32) * 4;
    const std::uint8_t* an = xr + xor_stride * static_cast<std::size_t>(h);
    if (static_cast<std::size_t>(an + and_stride * static_cast<std::size_t>(h) - dib) > n) return nullptr;
    SDL_Surface* s = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_ARGB8888);
    if (!s) return nullptr;
    bool has_alpha = false;
    for (int y = 0; y < h; ++y) {
        const std::uint8_t* row = xr + xor_stride * static_cast<std::size_t>(h - 1 - y);  // bottom-up
        const std::uint8_t* mask = an + and_stride * static_cast<std::size_t>(h - 1 - y);
        auto* out = reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(s->pixels) + static_cast<std::size_t>(y) * s->pitch);
        for (int x = 0; x < w; ++x) {
            std::uint32_t b = 0, gr = 0, r = 0, a = 0xff;
            if (bits >= 24) {
                const std::uint8_t* p = row + static_cast<std::size_t>(x) * (bits / 8);
                b = p[0];
                gr = p[1];
                r = p[2];
                if (bits == 32) {
                    a = p[3];
                    has_alpha = has_alpha || a != 0;
                }
            } else {
                const std::size_t bit = static_cast<std::size_t>(x) * bits;
                const unsigned idx = (row[bit / 8] >> (8 - bits - bit % 8)) & ((1u << bits) - 1);
                if (idx < colors) {
                    b = pal[4 * idx];
                    gr = pal[4 * idx + 1];
                    r = pal[4 * idx + 2];
                }
            }
            const bool transparent = (mask[x / 8] >> (7 - x % 8)) & 1;
            if (bits != 32 && transparent) a = 0;
            out[x] = (a << 24) | (r << 16) | (gr << 8) | b;
        }
    }
    if (bits == 32 && !has_alpha) {  // an old 32-bit icon without alpha: the AND mask
        for (int y = 0; y < h; ++y) {
            const std::uint8_t* mask = an + and_stride * static_cast<std::size_t>(h - 1 - y);
            auto* out = reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(s->pixels) + static_cast<std::size_t>(y) * s->pitch);
            for (int x = 0; x < w; ++x)
                if (!((mask[x / 8] >> (7 - x % 8)) & 1)) out[x] |= 0xff000000u;
        }
    }
    return s;
}

// OpenJoan's own icon (ADR-0002) for platforms whose executables carry no PE resources: the 32 x 32 image of
// src/loader/openjoan.ico as ARGB words (tools/make_icon.py), under the id the game asks for (0x66 = 102).
#include "openjoan_icon.inc"
constexpr std::uintptr_t kOpenJoanIconId = 102;

SDL_Surface* embedded_icon() {
    SDL_Surface* s = SDL_CreateSurface(kOpenJoanIconSize, kOpenJoanIconSize, SDL_PIXELFORMAT_ARGB8888);
    if (!s) return nullptr;
    for (int y = 0; y < kOpenJoanIconSize; ++y)
        std::memcpy(static_cast<std::uint8_t*>(s->pixels) + static_cast<std::size_t>(y) * s->pitch,
                    &kOpenJoanIcon[static_cast<std::size_t>(y) * kOpenJoanIconSize],
                    static_cast<std::size_t>(kOpenJoanIconSize) * 4);
    joan::hook::log_line("SDL window: icon %u is OpenJoan's own (embedded)", static_cast<unsigned>(kOpenJoanIconId));
    return s;
}

struct LoadedIcon {
    HINSTANCE instance;
    std::uintptr_t id;
    SDL_Surface* surface;
};
std::vector<LoadedIcon> g_icons;  // LoadIconA keeps what it loaded (Windows' shared icons are never freed either)

SDL_Surface* icon_surface(HICON h) {
    for (const LoadedIcon& i : g_icons)
        if (reinterpret_cast<HICON>(i.surface) == h) return i.surface;
    return nullptr;
}

// The game's only call: Window::create, its instance (this executable) and the icon id 0x66. The HICON is the
// surface's address (the game only puts it into the window class).
HICON WINAPI j_LoadIconA(HINSTANCE instance, LPCSTR name) {
    if (!instance || !IS_INTRESOURCE(name)) {
        joan::hook::log_line("SDL window: LoadIconA of a system / named icon - none");
        return nullptr;
    }
    const auto id = reinterpret_cast<std::uintptr_t>(name);
    for (const LoadedIcon& i : g_icons)
        if (i.instance == instance && i.id == id) return reinterpret_cast<HICON>(i.surface);
    SDL_Surface* s = decode_icon(instance, name);
    if (!s && id == kOpenJoanIconId) s = embedded_icon();
    if (!s) {
        joan::hook::log_line("SDL window: no icon %u in the module", static_cast<unsigned>(id));
        return nullptr;
    }
    g_icons.push_back(LoadedIcon{instance, id, s});
    return reinterpret_cast<HICON>(s);
}

// Window::create's GetStockObject(BLACK_BRUSH): only WNDCLASSA.hbrBackground gets it, which j_RegisterClassA does not
// use. A non-null handle per stock object (0 .. STOCK_LAST, 9 is not one: NULL as Windows).
constexpr std::uintptr_t kStockBase = 0x6B000004u;
HGDIOBJ WINAPI j_GetStockObject(int index) {
    if (index < 0 || index > 19 || index == 9) return nullptr;
    return reinterpret_cast<HGDIOBJ>(kStockBase + 4u * static_cast<unsigned>(index));
}
bool stock_object(const void* h) {
    const auto v = reinterpret_cast<std::uintptr_t>(h);
    return v >= kStockBase && v <= kStockBase + 4u * 19u;
}

// ---------------------------------------------------------------- window imports

ATOM WINAPI j_RegisterClassA(const WNDCLASSA* wc) {
    if (!wc || !wc->lpszClassName) return 0;
    if (g.proc) {
        joan::hook::log_line("SDL window: a second window class (%s) goes to Windows", wc->lpszClassName);
        WNDCLASSA copy = *wc;
        if (icon_surface(copy.hIcon)) copy.hIcon = nullptr;
        if (stock_object(copy.hbrBackground)) copy.hbrBackground = nullptr;
        JOAN_NOT_OURS(RegisterClassA(&copy), 0);
    }
    g.proc = wc->lpfnWndProc;
    g.icon = icon_surface(wc->hIcon);
    g.class_name = wc->lpszClassName;
    return 1;
}

BOOL WINAPI j_AdjustWindowRectEx(LPRECT, DWORD, BOOL, DWORD) {
    return TRUE;  // SDL sizes the client area: the game's client rectangle is the window size it asks for
}

HWND WINAPI j_CreateWindowExA(DWORD ex, LPCSTR cls, LPCSTR title, DWORD style, int x, int y, int w, int h,
                              HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    const bool class_atom = reinterpret_cast<std::uintptr_t>(cls) <= 0xffff;
    if (g.window || !g.proc || class_atom || g.class_name != cls)
        JOAN_NOT_OURS(CreateWindowExA(ex, cls, title, style, x, y, w, h, parent, menu, inst, param), nullptr);

    // An OpenGL window (the GL device's context: gl_create_context below). The pixel format is chosen when the
    // window is created: 32-bit RGBA, double-buffered, no depth / stencil (the game draws into framebuffer objects).
    // DPI: SDL declares per-monitor v2 awareness, as display.cpp's make_dpi_aware already did.
    if (!joan::platform::video_init()) return nullptr;
    if (settings().gamepad && !SDL_InitSubSystem(SDL_INIT_GAMEPAD))
        joan::hook::log_line("SDL window: no game pads (SDL_Init(GAMEPAD) failed: %s)", SDL_GetError());
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    const bool visible = (style & WS_VISIBLE) != 0;
    const bool bordered = (style & WS_CAPTION) == WS_CAPTION;
    const SDL_PropertiesID p = SDL_CreateProperties();
    SDL_SetStringProperty(p, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title ? title : "");
    constexpr Sint64 kCentered = static_cast<Sint64>(SDL_WINDOWPOS_CENTERED);
#ifdef _WIN32
    SDL_SetNumberProperty(p, SDL_PROP_WINDOW_CREATE_X_NUMBER, x == CW_USEDEFAULT ? kCentered : Sint64{x});
    SDL_SetNumberProperty(p, SDL_PROP_WINDOW_CREATE_Y_NUMBER, y == CW_USEDEFAULT ? kCentered : Sint64{y});
#else
    int ox = 0, oy = 0;
    joan::platform::detail::screen_origin(ox, oy);  // the game's 0,0: the primary display's top-left
    SDL_SetNumberProperty(p, SDL_PROP_WINDOW_CREATE_X_NUMBER, x == CW_USEDEFAULT ? kCentered : Sint64{x} + ox);
    SDL_SetNumberProperty(p, SDL_PROP_WINDOW_CREATE_Y_NUMBER, y == CW_USEDEFAULT ? kCentered : Sint64{y} + oy);
#endif
    SDL_SetNumberProperty(p, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, w > 0 ? w : 640);
    SDL_SetNumberProperty(p, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, h > 0 ? h : 480);
    SDL_SetBooleanProperty(p, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true);  // shown after WM_CREATE, as Windows
    SDL_SetBooleanProperty(p, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, !bordered);
    SDL_SetBooleanProperty(p, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
    SDL_Window* window = SDL_CreateWindowWithProperties(p);
    SDL_DestroyProperties(p);
    if (!window) {
        joan::hook::log_line("SDL window: SDL_CreateWindow failed: %s", SDL_GetError());
        return nullptr;
    }
#ifdef _WIN32
    const auto hwnd = static_cast<HWND>(
        SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
#else
    // No Windows handle elsewhere: the game only compares and passes its HWND back to us, so the SDL window stands
    // for it (unique while the window lives).
    const auto hwnd = reinterpret_cast<HWND>(window);
#endif
    if (!hwnd) {
        joan::hook::log_line("SDL window: no Win32 window handle");
        SDL_DestroyWindow(window);
        return nullptr;
    }
    g.window = window;
    g.hwnd = hwnd;
    g.thread = static_cast<DWORD>(joan::platform::thread_id());
    g.style = static_cast<LONG>(style);
    g.ex_style = static_cast<LONG>(ex);
    if (g.icon && !SDL_SetWindowIcon(window, g.icon))
        joan::hook::log_line("SDL window: SDL_SetWindowIcon failed: %s", SDL_GetError());

    CREATESTRUCTA cs = {};
    cs.lpCreateParams = param;
    cs.hInstance = inst;
    cs.hMenu = menu;
    cs.hwndParent = parent;
    cs.cy = h;
    cs.cx = w;
    cs.y = y;
    cs.x = x;
    cs.style = static_cast<LONG>(style);
    cs.lpszName = title;
    cs.lpszClass = cls;
    cs.dwExStyle = ex;
    if (call_proc(hwnd, WM_CREATE, 0, reinterpret_cast<LPARAM>(&cs)) == -1) {
        SDL_DestroyWindow(window);
        g.window = nullptr;
        g.hwnd = nullptr;
        return nullptr;
    }
    if (visible) {
        SDL_ShowWindow(window);
        SDL_RaiseWindow(window);
        g.paint_pending = true;
        sync_activation_now();
    }
    joan::hook::log_line("SDL window: %s, %dx%d at %d,%d, window %p (SDL %d.%d.%d)", bordered ? "captioned" : "borderless",
                         w, h, x, y, static_cast<void*>(hwnd), SDL_VERSIONNUM_MAJOR(SDL_GetVersion()),
                         SDL_VERSIONNUM_MINOR(SDL_GetVersion()), SDL_VERSIONNUM_MICRO(SDL_GetVersion()));
    return hwnd;
}

BOOL WINAPI j_ShowWindow(HWND h, int cmd) {
    if (!ours(h)) JOAN_NOT_OURS(ShowWindow(h, cmd), FALSE);
    const SDL_WindowFlags flags = SDL_GetWindowFlags(g.window);
    const BOOL was_visible = (flags & SDL_WINDOW_HIDDEN) == 0;
    switch (cmd) {
        case SW_HIDE:
            SDL_HideWindow(g.window);
            break;
        case SW_MINIMIZE:
        case SW_SHOWMINIMIZED:
        case SW_SHOWMINNOACTIVE:
        case SW_FORCEMINIMIZE:
            SDL_MinimizeWindow(g.window);
            break;
        case SW_MAXIMIZE:
            SDL_ShowWindow(g.window);
            SDL_MaximizeWindow(g.window);
            break;
        case SW_RESTORE:
            SDL_ShowWindow(g.window);
            SDL_RestoreWindow(g.window);
            break;
        default:
            SDL_ShowWindow(g.window);
            break;
    }
    sync_activation_now();
    return was_visible;
}

BOOL WINAPI j_UpdateWindow(HWND h) {
    if (!ours(h)) JOAN_NOT_OURS(UpdateWindow(h), FALSE);
    if (g.paint_pending) {
        g.paint_pending = false;
        call_proc(g.hwnd, WM_PAINT, 0, 0);
    }
    return TRUE;
}

HWND WINAPI j_SetFocus(HWND h) {
    if (!ours(h)) JOAN_NOT_OURS(SetFocus(h), nullptr);
    const bool had = (SDL_GetWindowFlags(g.window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    SDL_RaiseWindow(g.window);
    sync_activation_now();
    return had ? g.hwnd : nullptr;
}

LONG WINAPI j_SetWindowLongA(HWND h, int index, LONG v) {
    if (!ours(h)) JOAN_NOT_OURS(SetWindowLongA(h, index, v), 0);
    LONG old = 0;
    switch (index) {
        case GWL_STYLE:
            old = g.style;
            g.style = v;
            SDL_SetWindowBordered(g.window, (static_cast<DWORD>(v) & WS_CAPTION) == WS_CAPTION);
            if ((static_cast<DWORD>(v) & WS_VISIBLE) != 0) SDL_ShowWindow(g.window);
            break;
        case GWL_EXSTYLE:
            old = g.ex_style;
            g.ex_style = v;  // tool / topmost bits: SDL's window keeps its own (see SetWindowPos)
            break;
#if UINTPTR_MAX == 0xffffffffu
        case GWLP_WNDPROC:  // GWL_WNDPROC (-4; x86 name only)
            old = static_cast<LONG>(reinterpret_cast<std::uintptr_t>(g.proc));
            g.proc = reinterpret_cast<WNDPROC>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(v)));
            break;
#else
        case GWLP_WNDPROC:  // x64: a LONG cannot carry the procedure (Windows has no GWL_WNDPROC there either; the
                            // game never sets it: only GWL_STYLE / GWL_EXSTYLE, start_d3d)
            set_error(ERROR_INVALID_INDEX);
            break;
#endif
        default:
#ifdef _WIN32
            old = SetWindowLongA(h, index, v);
#else
            set_error(ERROR_INVALID_INDEX);
#endif
            break;
    }
    return old;
}

BOOL WINAPI j_SetWindowPos(HWND h, [[maybe_unused]] HWND after, int x, int y, int cx, int cy, UINT flags) {
    if (!ours(h)) JOAN_NOT_OURS(SetWindowPos(h, after, x, y, cx, cy, flags), FALSE);
    // HWND_TOPMOST of start_d3d is not kept: the full-screen window is SDL's borderless one (cover_monitor) and
    // stays an ordinary window for Alt+Tab.
    if (!(flags & SWP_NOSIZE) && cx > 0 && cy > 0) SDL_SetWindowSize(g.window, cx, cy);
#ifdef _WIN32
    if (!(flags & SWP_NOMOVE)) SDL_SetWindowPosition(g.window, x, y);
#else
    if (!(flags & SWP_NOMOVE)) {
        int ox = 0, oy = 0;
        joan::platform::detail::screen_origin(ox, oy);
        SDL_SetWindowPosition(g.window, x + ox, y + oy);
    }
#endif
    if (flags & SWP_HIDEWINDOW) SDL_HideWindow(g.window);
    if (flags & SWP_SHOWWINDOW) SDL_ShowWindow(g.window);
    SDL_SyncWindow(g.window);
    return TRUE;
}

BOOL WINAPI j_DestroyWindow(HWND h) {
    if (!ours(h)) JOAN_NOT_OURS(DestroyWindow(h), FALSE);
    call_proc(h, WM_DESTROY, 0, 0);
    SDL_DestroyWindow(g.window);
    g.window = nullptr;
    g.hwnd = nullptr;
    g.sent.clear();
    for (auto it = g.posted.begin(); it != g.posted.end();)
        it = it->hwnd == h ? g.posted.erase(it) : it + 1;
    g.close_posted = g.paint_posted = false;
    return TRUE;
}

LRESULT WINAPI j_DefWindowProcA(HWND h, [[maybe_unused]] UINT message, [[maybe_unused]] WPARAM wp, [[maybe_unused]] LPARAM lp) {
    // The game window's messages come from here, not from Windows: nothing to do for them (WM_CLOSE does not
    // destroy the window - the game's procedure never passes it on anyway).
    if (ours(h)) return 0;
#ifdef _WIN32
    return DefWindowProcA(h, message, wp, lp);  // a window of Windows'
#else
    return 0;  // there are no other windows
#endif
}

// ---------------------------------------------------------------- message imports

void WINAPI j_PostQuitMessage(int code) {
    if (!on_window_thread()) {
#ifdef _WIN32
        PostQuitMessage(code);  // a thread of Windows' own queues
#endif
        return;
    }
    g.posted.push_back(make_msg(nullptr, WM_QUIT, static_cast<WPARAM>(code), 0));
}

BOOL WINAPI j_PeekMessageA(LPMSG m, HWND h, UINT lo, UINT hi, UINT remove) {
    if (!on_window_thread()) JOAN_NOT_OURS(PeekMessageA(m, h, lo, hi, remove), FALSE);
    if (h != nullptr && !ours(h)) JOAN_NOT_OURS(PeekMessageA(m, h, lo, hi, remove), FALSE);
    if (g.window) pump();
    deliver_sent();
    return take(m, lo, hi, (remove & PM_REMOVE) != 0) ? TRUE : FALSE;
}

BOOL WINAPI j_GetMessageA(LPMSG m, HWND h, UINT lo, UINT hi) {
    if (!on_window_thread()) JOAN_NOT_OURS(GetMessageA(m, h, lo, hi), -1);
    if (h != nullptr && !ours(h)) JOAN_NOT_OURS(GetMessageA(m, h, lo, hi), -1);
    for (;;) {
        if (g.window) pump();
        deliver_sent();
        if (take(m, lo, hi, true)) return m->message == WM_QUIT ? FALSE : TRUE;
        if (!g.window) return -1;  // nothing can arrive any more
        SDL_WaitEvent(nullptr);
    }
}

BOOL WINAPI j_TranslateMessage(const MSG* m) {
    if (m && (m->hwnd == nullptr || ours(m->hwnd))) return FALSE;  // no key messages come from here
    JOAN_NOT_OURS(TranslateMessage(m), FALSE);
}

LRESULT WINAPI j_DispatchMessageA(const MSG* m) {
    if (!m) return 0;
    if (ours(m->hwnd)) return call_proc(m->hwnd, m->message, m->wParam, m->lParam);
    if (m->hwnd == nullptr) return 0;
    JOAN_NOT_OURS(DispatchMessageA(m), 0);
}

#ifndef _WIN32
// MsgWaitForMultipleObjectsEx without Windows' queue: the objects (process.cpp's events / threads; none for the
// frame wait) or an SDL event, whichever comes first, checked every few milliseconds; WAIT_OBJECT_0 + n for an event.
DWORD wait_objects_or_event(DWORD n, const HANDLE* hs, DWORD ms, DWORD flags) {
    const bool alertable = (flags & MWMO_ALERTABLE) != 0;
    const bool all = (flags & MWMO_WAITALL) != 0;
    const std::uint64_t start = SDL_GetTicks();
    for (;;) {
        if (alertable && joan::platform::files::run_pending_io()) return WAIT_IO_COMPLETION;
        DWORD r = WAIT_TIMEOUT;
        if (n > 0 && !joan::platform::files::sync_wait(n, hs, all, 0, false, r)) {
            set_error(ERROR_INVALID_HANDLE);
            return WAIT_FAILED;
        }
        if (r != WAIT_TIMEOUT) return r;
        const std::uint64_t spent = SDL_GetTicks() - start;
        if (ms != INFINITE && spent >= ms) return WAIT_TIMEOUT;
        const std::uint64_t slice = n > 0 || alertable ? 2u : 50u;
        const std::uint64_t left = ms == INFINITE ? slice : std::min<std::uint64_t>(slice, ms - spent);
        if (SDL_WaitEventTimeout(nullptr, static_cast<Sint32>(left))) return WAIT_OBJECT_0 + n;
    }
}
#endif

// The frame wait: the time and the alertable wait (APCs: WAIT_IO_COMPLETION) are Windows'; a message ends it only
// when one of ours is waiting (SDL's own window messages - mouse moves, raw input - just continue the wait).
DWORD WINAPI j_MsgWaitForMultipleObjectsEx(DWORD n, const HANDLE* hs, DWORD ms, [[maybe_unused]] DWORD mask, DWORD flags) {
    if (!on_window_thread()) JOAN_NOT_OURS(MsgWaitForMultipleObjectsEx(n, hs, ms, mask, flags), WAIT_FAILED);
    // ReadFileEx completions of the portable file layer (platform/files.cpp) run in alertable waits, as Windows' do.
    if ((flags & MWMO_ALERTABLE) && joan::platform::files::run_pending_io()) return WAIT_IO_COMPLETION;
    if (g.window) pump();
    if (!g.posted.empty() || !g.sent.empty()) return WAIT_OBJECT_0 + n;
    const std::uint64_t start = SDL_GetTicks();
    for (;;) {
        DWORD left = INFINITE;
        if (ms != INFINITE) {
            const std::uint64_t spent = SDL_GetTicks() - start;
            left = spent >= ms ? 0u : static_cast<DWORD>(ms - spent);
        }
#ifdef _WIN32
        const DWORD r = MsgWaitForMultipleObjectsEx(n, hs, left, mask, flags);
#else
        const DWORD r = wait_objects_or_event(n, hs, left, flags);
#endif
        if (r != WAIT_OBJECT_0 + n) return r;  // an object, WAIT_TIMEOUT, WAIT_IO_COMPLETION, WAIT_FAILED
        if (g.window) pump();
        if (!g.posted.empty() || !g.sent.empty()) return r;
        if (left == 0) return WAIT_TIMEOUT;
    }
}

// ---------------------------------------------------------------- GetOpenFileNameA

// The developer key 0x79's two file choices (lStructSize 0x4c, filter "WorldFile (*.wor)" / "ScriptFile (*.sce)",
// initial folder MAP\ / SCRIPT\SCENARIO\ relative to the current directory, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY):
// SDL's file dialog, waited for here (Windows' dialog is modal too). Results as Windows': TRUE with the full ANSI path
// in lpstrFile (nFileOffset / nFileExtension / lpstrFileTitle / nFilterIndex set, the current directory changed to
// the file's folder unless OFN_NOCHANGEDIR); FALSE when cancelled, on an error, or when lpstrFile is too small (then
// its first word holds the size needed, FNERR_BUFFERTOOSMALL).
struct FileDialog {
    std::atomic<bool> done{false};
    bool ok = false;
    std::string path;  // UTF-8
    int filter = -1;
};

void SDLCALL file_dialog_done(void* user, const char* const* files, int filter) {
    auto* d = static_cast<FileDialog*>(user);
    if (!files) joan::hook::log_line("SDL file dialog failed: %s", SDL_GetError());
    if (files && files[0]) {
        d->ok = true;
        d->path = files[0];
        d->filter = filter;
    }
    d->done.store(true);
}

// "MAP\" (relative to the current directory) as the absolute UTF-8 folder SDL's dialogs take.
std::string dialog_location(const char* ansi) {
    std::string s = ansi;
#ifndef _WIN32
    for (char& c : s)
        if (c == '\\') c = '/';
#endif
    std::error_code ec;
    std::filesystem::path p = std::filesystem::absolute(std::filesystem::path(s), ec);
    if (ec) return {};
    std::string a = p.lexically_normal().string();
    while (a.size() > 3 && (a.back() == '\\' || a.back() == '/')) a.pop_back();
    return joan::platform::text::ansi_to_utf8(a.c_str());
}

BOOL WINAPI j_GetOpenFileNameA(LPOPENFILENAMEA ofn) {
    if (!ofn || ofn->lStructSize < OPENFILENAME_SIZE_VERSION_400A || !ofn->lpstrFile || ofn->nMaxFile == 0) return FALSE;
    // lpstrFilter: "description\0*.ext;*.ext2\0...\0\0" -> SDL's "ext;ext2" ("*" for all files)
    std::vector<std::string> names, patterns;
    for (const char* f = ofn->lpstrFilter; f && *f;) {
        const char* pat = f + std::strlen(f) + 1;
        if (!*pat) break;
        std::string out;
        for (const char* q = pat; *q;) {
            const char* end = std::strchr(q, ';');
            std::string one(q, end ? static_cast<std::size_t>(end - q) : std::strlen(q));
            if (one.rfind("*.", 0) == 0) one.erase(0, 2);
            if (one.empty() || one == "*") one = "*";
            if (!out.empty()) out += ';';
            out += one;
            q = end ? end + 1 : q + std::strlen(q);
        }
        names.push_back(joan::platform::text::ansi_to_utf8(f));
        patterns.push_back(out);
        f = pat + std::strlen(pat) + 1;
    }
    std::vector<SDL_DialogFileFilter> filters;
    for (std::size_t i = 0; i < names.size(); ++i) filters.push_back(SDL_DialogFileFilter{names[i].c_str(), patterns[i].c_str()});
    const std::string location = ofn->lpstrFile[0] ? dialog_location(ofn->lpstrFile)
                                 : ofn->lpstrInitialDir ? dialog_location(ofn->lpstrInitialDir)
                                                        : std::string();
    FileDialog d;
    SDL_ShowOpenFileDialog(&file_dialog_done, &d, g.window, filters.empty() ? nullptr : filters.data(),
                           static_cast<int>(filters.size()), location.empty() ? nullptr : location.c_str(),
                           false);
    while (!d.done.load()) {  // the dialog runs on SDL's thread or on its event loop
        SDL_PumpEvents();
        SDL_Delay(10);
    }
    if (!d.ok) return FALSE;
    const std::string path = joan::platform::text::utf8_to_ansi(d.path.c_str());
    std::error_code ec;
    if ((ofn->Flags & OFN_FILEMUSTEXIST) && !std::filesystem::is_regular_file(std::filesystem::path(path), ec)) {
        joan::hook::log_line("SDL file dialog: %s does not exist", path.c_str());
        return FALSE;
    }
    if (path.size() + 1 > ofn->nMaxFile) {
        if (ofn->nMaxFile >= 2) {
            const auto need = static_cast<WORD>(path.size() + 1);
            std::memcpy(ofn->lpstrFile, &need, sizeof need);
        }
        return FALSE;
    }
    std::memcpy(ofn->lpstrFile, path.c_str(), path.size() + 1);
    const std::size_t slash = path.find_last_of("\\/");
    const std::size_t name = slash == std::string::npos ? 0 : slash + 1;
    const std::size_t dot = path.find_last_of('.');
    ofn->nFileOffset = static_cast<WORD>(name);
    ofn->nFileExtension = static_cast<WORD>(dot != std::string::npos && dot >= name ? dot + 1 : path.size());
    if (ofn->lpstrFileTitle && ofn->nMaxFileTitle > 0) {
        const std::string title = path.substr(name);
        const std::size_t n = title.size() < ofn->nMaxFileTitle ? title.size() : ofn->nMaxFileTitle - 1;
        std::memcpy(ofn->lpstrFileTitle, title.c_str(), n);
        ofn->lpstrFileTitle[n] = '\0';
    }
    if (d.filter >= 0 && !filters.empty()) ofn->nFilterIndex = static_cast<DWORD>(d.filter + 1);
    if (!(ofn->Flags & OFN_NOCHANGEDIR) && name > 0)
        std::filesystem::current_path(std::filesystem::path(path.substr(0, name)), ec);
    return TRUE;
}

}  // namespace

namespace joan::platform {

namespace detail {
SDL_Window* window() { return g.window; }
#ifndef _WIN32
void screen_origin(int& x, int& y) {
    SDL_Rect r{};
    const SDL_DisplayID primary = SDL_GetPrimaryDisplay();
    if (primary != 0 && SDL_GetDisplayBounds(primary, &r)) {
        x = r.x;
        y = r.y;
    } else {
        x = y = 0;
    }
}
#endif
bool focused() { return g.window && (SDL_GetWindowFlags(g.window) & SDL_WINDOW_INPUT_FOCUS) != 0; }
bool is_window(void* hwnd) { return ours(static_cast<HWND>(hwnd)); }
bool gamepads_enabled() { return settings().gamepad; }
}  // namespace detail

// ---------------------------------------------------------------- OpenGL (the GL device, gl/context.cpp)

void* gl_create_context(void* hwnd, bool debug, int& major, int& minor, std::string& error) {
    if (!ours(static_cast<HWND>(hwnd))) {
        error = "not the SDL game window";
        return nullptr;
    }
    // The game destroys its window while it exits and releases textures afterwards (static destructors): SDL would unload
    // the GL driver with its last GL window, leaving the GL functions pointing at unmapped code. A reference of our own
    // keeps the driver loaded until the process ends; a release without a context then does nothing.
    static const bool driver_kept = SDL_GL_LoadLibrary(nullptr);
    (void)driver_kept;
    for (const int m : {6, 5}) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, debug ? SDL_GL_CONTEXT_DEBUG_FLAG : 0);
        if (SDL_GLContext ctx = SDL_GL_CreateContext(g.window)) {  // and makes it current
            major = 4;
            minor = m;
            return ctx;
        }
        error = SDL_GetError();
    }
    error = "the driver offers neither OpenGL 4.6 nor 4.5 core (" + error + ")";
    return nullptr;
}

bool gl_make_current(void* hwnd, void* ctx) {
    return ours(static_cast<HWND>(hwnd)) && SDL_GL_MakeCurrent(g.window, static_cast<SDL_GLContext>(ctx));
}

void gl_destroy_context(void* ctx) {
    if (!ctx) return;
    if (g.window) SDL_GL_MakeCurrent(g.window, nullptr);
    SDL_GL_DestroyContext(static_cast<SDL_GLContext>(ctx));
}

void gl_swap(void* hwnd) {
    if (ours(static_cast<HWND>(hwnd))) SDL_GL_SwapWindow(g.window);
}

bool gl_set_swap_interval(int interval) {
    if (SDL_GL_SetSwapInterval(interval)) return true;
    return interval == -1 && SDL_GL_SetSwapInterval(1);  // no adaptive vsync: plain vsync
}

void* gl_proc(const char* name) { return reinterpret_cast<void*>(SDL_GL_GetProcAddress(name)); }

// ---------------------------------------------------------------- queries of our own code

const std::filesystem::path& exe_dir() {
    static const std::filesystem::path dir = [] {
#ifndef _WIN32
        // Started from an AppImage (docs/INSTALL.md): the executable runs from a temporary mount; the game folder is the
        // one the .AppImage file is in. The runtime sets APPIMAGE to the file's absolute path.
        if (const char* image = std::getenv("APPIMAGE"); image != nullptr && *image != '\0' && std::getenv("APPDIR") != nullptr) {
            const std::filesystem::path folder = std::filesystem::path(image).parent_path();
            if (!folder.empty()) return folder / "";
        }
#endif
        const char* base = SDL_GetBasePath();  // UTF-8, ends with a separator; SDL keeps the string
        if (base == nullptr || *base == '\0') return std::filesystem::current_path() / "";
        return std::filesystem::path(std::u8string(reinterpret_cast<const char8_t*>(base)));
    }();
    return dir;
}

std::filesystem::path settings_path(const std::filesystem::path& dir) {
    const std::filesystem::path current = dir / kSettingsName;
    std::error_code ec;
    if (std::filesystem::exists(current, ec)) return current;
    const std::filesystem::path fallback = dir / kSettingsFallbackName;
    if (std::filesystem::exists(fallback, ec)) return fallback;
    return current;
}

const std::filesystem::path& settings_file() {
    static const std::filesystem::path file = settings_path(exe_dir());
    return file;
}

void error_box(const char* title, const char* text) {
    // In front of the game window when there is one (a full-screen window would hide a box of its own).
    if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, g.window))
        joan::hook::log_line("message box failed (%s): %s", SDL_GetError(), text);
}

bool video_init() {
    static const bool ok = [] {
        SDL_SetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, "0");     // the game decides (WM_CLOSE)
        SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");  // a borderless window, not a display mode
        if (SDL_InitSubSystem(SDL_INIT_VIDEO)) return true;
        joan::hook::log_line("SDL: SDL_Init(VIDEO) failed: %s", SDL_GetError());
        return false;
    }();
    return ok;
}

namespace {

// The displays, primary first (Direct3D's adapter 0), then SDL's order.
std::vector<SDL_DisplayID> displays_in_order() {
    std::vector<SDL_DisplayID> out;
    if (!video_init()) return out;
    const SDL_DisplayID primary = SDL_GetPrimaryDisplay();
    if (primary != 0) out.push_back(primary);
    int n = 0;
    if (SDL_DisplayID* ids = SDL_GetDisplays(&n)) {
        for (int i = 0; i < n; ++i)
            if (ids[i] != primary) out.push_back(ids[i]);
        SDL_free(ids);
    }
    return out;
}
SDL_DisplayID display_at(int index) {
    const std::vector<SDL_DisplayID> list = displays_in_order();
    return index >= 0 && index < static_cast<int>(list.size()) ? list[static_cast<std::size_t>(index)] : 0;
}
// SDL's mode as Windows lists it: pixels, whole Hz (59.94 -> 59, as EnumDisplaySettings; 143.998 -> 144).
ScreenMode to_screen_mode(const SDL_DisplayMode& m) {
    ScreenMode s;
    const float density = m.pixel_density > 0.0f ? m.pixel_density : 1.0f;
    s.width = static_cast<int>(static_cast<float>(m.w) * density + 0.5f);
    s.height = static_cast<int>(static_cast<float>(m.h) * density + 0.5f);
    if (m.refresh_rate_numerator > 0 && m.refresh_rate_denominator > 0) {
        const double hz = static_cast<double>(m.refresh_rate_numerator) / m.refresh_rate_denominator;
        const double whole = std::floor(hz + 0.5);
        s.refresh = static_cast<int>(std::fabs(hz - whole) < 0.01 ? whole : std::floor(hz));
    } else {
        s.refresh = static_cast<int>(m.refresh_rate + 0.5f);
    }
    s.bits = SDL_BYTESPERPIXEL(m.format) * 8;
    return s;
}

}  // namespace

int display_count() { return static_cast<int>(displays_in_order().size()); }

std::vector<ScreenMode> display_modes(int index) {
    std::vector<ScreenMode> out;
    const SDL_DisplayID id = display_at(index);
    if (id == 0) return out;
    int n = 0;
    if (SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(id, &n)) {
        for (int i = 0; i < n; ++i)
            if (modes[i]) out.push_back(to_screen_mode(*modes[i]));
        SDL_free(modes);
    }
    return out;
}

bool current_display_mode(int index, ScreenMode& out) {
    const SDL_DisplayID id = display_at(index);
    const SDL_DisplayMode* m = id != 0 ? SDL_GetCurrentDisplayMode(id) : nullptr;
    if (m == nullptr) return false;
    out = to_screen_mode(*m);
    return true;
}

bool desktop_display_mode(int index, ScreenMode& out) {
    const SDL_DisplayID id = display_at(index);
    const SDL_DisplayMode* m = id != 0 ? SDL_GetDesktopDisplayMode(id) : nullptr;
    if (m == nullptr) return false;
    out = to_screen_mode(*m);
    return true;
}

bool display_origin(int index, int& x, int& y) {
    const SDL_DisplayID id = display_at(index);
    SDL_Rect r{};
    if (id == 0 || !SDL_GetDisplayBounds(id, &r)) return false;
#ifndef _WIN32
    int ox = 0, oy = 0;
    detail::screen_origin(ox, oy);  // relative to the primary display, as Windows reports it
    r.x -= ox;
    r.y -= oy;
#endif
    x = r.x;
    y = r.y;
    return true;
}

bool is_sdl_window(void* hwnd) { return ours(static_cast<HWND>(hwnd)); }

bool client_size(void* hwnd, int& w, int& h) {
    return ours(static_cast<HWND>(hwnd)) && SDL_GetWindowSizeInPixels(g.window, &w, &h);
}

bool window_focused(void* hwnd) {
    return ours(static_cast<HWND>(hwnd)) && (SDL_GetWindowFlags(g.window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}

void keep_active() { g_keep_active = true; }

int refresh_rate(void* hwnd) {
    if (!ours(static_cast<HWND>(hwnd))) return 0;
    if (const SDL_DisplayMode* m = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(g.window)))
        if (m->refresh_rate > 1.0f) return static_cast<int>(m->refresh_rate + 0.5f);
    return 0;
}

bool cover_monitor(void* hwnd) {
    if (!ours(static_cast<HWND>(hwnd))) return false;
    SDL_SetWindowFullscreenMode(g.window, nullptr);  // desktop mode: a borderless window over the monitor
    if (!SDL_SetWindowFullscreen(g.window, true)) {
        joan::hook::log_line("SDL window: full screen failed: %s", SDL_GetError());
        return false;
    }
    SDL_SyncWindow(g.window);
    int w = 0;
    int h = 0;
    SDL_GetWindowSizeInPixels(g.window, &w, &h);
    joan::hook::log_line("SDL window: full screen requested - borderless window %dx%d", w, h);
    return true;
}

}  // namespace joan::platform

#define W(name) JOAN_IMPORT_OVERRIDE("USER32.dll", #name, &j_##name, "SDL " #name)
W(RegisterClassA);
W(AdjustWindowRectEx);
W(CreateWindowExA);
W(ShowWindow);
W(UpdateWindow);
W(SetFocus);
W(SetWindowLongA);
W(SetWindowPos);
W(DestroyWindow);
W(DefWindowProcA);
W(PostQuitMessage);
W(PeekMessageA);
W(GetMessageA);
W(TranslateMessage);
W(DispatchMessageA);
W(MsgWaitForMultipleObjectsEx);
W(LoadIconA);
#undef W
JOAN_IMPORT_OVERRIDE("GDI32.dll", "GetStockObject", &j_GetStockObject, "SDL GetStockObject");
JOAN_IMPORT_OVERRIDE("comdlg32.dll", "GetOpenFileNameA", &j_GetOpenFileNameA, "SDL GetOpenFileNameA");
