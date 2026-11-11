// The game's keyboard and mouse on SDL3 (phase C4, src/platform/README.md): the native input devices of input.hpp, fed
// by the SDL window's event pump, and the USER32 cursor / key-state / message-box imports answered by SDL.
//
// What the game does with its input (src/ilib/mousecr.cpp; the original did the same through DirectInput 8):
// - Input::init 0x00592380: the mouse (exclusive, buffer 200 items) and the keyboard (non-exclusive, 16), acquired.
// - Input::poll 0x00594f90, every frame: acquire both (an error = no input this frame), then twice read up to 200 /
//   16 items: mouse X / Y / Z deltas and buttons 0xc..0xe, keyboard keys (DIK code, 0x80 = down, time stamp for
//   the double-tap timing); the two streams merged by sequence number.
// - Input::unacquire_all 0x005929f0 on WM_ACTIVATEAPP 0, Input::reacquire 0x00592ae0 on WM_ACTIVATEAPP 1.
//
// Here: DirectInput's semantics without DirectInput - buffered items (sequence numbers shared by both devices,
// overflow reported), foreground devices lost with the focus (acquire fails while the window is in the background),
// the exclusive mouse as SDL's relative mouse mode (raw motion, cursor hidden and kept in the window), key repeats
// not reported. The replay layer records every call (joan::replay::input_call).
//
// USER32: GetCursorPos / SetCursorPos (desktop position), ScreenToClient / GetClientRect of the game
// window, ShowCursor (Windows' display counter on SDL_ShowCursor / SDL_HideCursor), GetKeyState (the lock keys of
// the key modifiers 0x00598bd0 reads), MessageBoxA (SDL's message box; the game's ANSI text converted to UTF-8).
#include "win32_types.hpp"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <deque>
#include <string>
#include <vector>

#include "../runtime/loader/import_overrides.hpp"
#include "../data_names.hpp"
#include "../display.hpp"
#include "../interfac/joystick_option.hpp"
#include "../registry.hpp"
#include "../runtime/replay/replay.hpp"
#include "input.hpp"
#include "sdl_internal.hpp"
#include "text.hpp"
#include "window.hpp"
#include "ini.hpp"
#include "files.hpp"
#include "threads.hpp"

namespace {

// ---------------------------------------------------------------- the devices

using joan::platform::input::Item;
namespace in = joan::platform::input;

struct MouseState {  // what GetDeviceState would report (not used by the game; kept for diagnostics)
    LONG x, y, z;
    BYTE buttons[4];
};

struct DeviceState {
    bool open = false;
    bool exclusive = false;  // the mouse: SDL's relative mode while acquired
    std::uint32_t buffer_size = 0;
    bool acquired = false;
    bool overflow = false;
    std::deque<Item> items;
    MouseState mouse{};
    BYTE keys[256] = {};
};

DeviceState g_mouse_dev;
DeviceState g_key_dev;
DWORD g_sequence = 0;
std::deque<Item>* g_from_mouse = nullptr;  // co-op host: another player's devices (input::read_from)
std::deque<Item>* g_from_keys = nullptr;
in::Forward g_forward = nullptr;      // co-op player: this computer's items go to the host too (input::forward)
BYTE g_forward_keys[256] = {};        // the keys down as forwarded (key changes only)
DWORD g_forward_sequence = 0;
float g_rest_x = 0, g_rest_y = 0, g_rest_wheel = 0;

DeviceState& state(in::Device d) { return d == in::Device::Mouse ? g_mouse_dev : g_key_dev; }

// SDL event time (ns, SDL_GetTicksNS's clock) in the game's timeGetTime base (sdl_time.cpp): the game compares key
// time stamps with its timeGetTime clock (0x00591050).
DWORD stamp(Uint64 ns) {
    const std::int64_t age_ms = (static_cast<std::int64_t>(SDL_GetTicksNS()) - static_cast<std::int64_t>(ns)) / 1000000;
    return static_cast<DWORD>(joan::platform::time_ms() - static_cast<DWORD>(age_ms));
}

void push(DeviceState& d, DWORD ofs, DWORD data, DWORD time, DWORD seq) {
    if (!d.acquired || d.buffer_size == 0) return;
    if (d.items.size() >= d.buffer_size) {
        d.overflow = true;  // DirectInput keeps the oldest items
        return;
    }
    d.items.push_back(Item{ofs, data, time, seq, 0});
}

void update_relative_mode() {
    SDL_Window* w = joan::platform::detail::window();
    if (!w) return;
    const bool want = g_mouse_dev.acquired && g_mouse_dev.exclusive && joan::platform::detail::focused();
    if (SDL_GetWindowRelativeMouseMode(w) != want) SDL_SetWindowRelativeMouseMode(w, want);
}

void drop(DeviceState& d) {
    d.acquired = false;
    d.items.clear();
    d.overflow = false;
    std::memset(d.keys, 0, sizeof d.keys);
    d.mouse = MouseState{};
}

// The real devices (the replay layer calls these while recording / without a recording).
std::int32_t real_acquire(std::int32_t device) {
    if (g_from_mouse != nullptr) return in::kOk;  // another player's devices: always there
    DeviceState& d = state(static_cast<in::Device>(device));
    if (!d.open) return in::kNotAcquired;
    if (!joan::platform::detail::focused()) return in::kOtherAppHasPriority;  // foreground devices
    if (d.acquired) return in::kNoEffect;
    d.acquired = true;
    update_relative_mode();
    return in::kOk;
}

std::int32_t real_unacquire(std::int32_t device) {
    DeviceState& d = state(static_cast<in::Device>(device));
    if (!d.acquired) return in::kNoEffect;
    drop(d);
    update_relative_mode();
    return in::kOk;
}

void pad_menu_frame();  // the pad in the menus (below)

std::int32_t real_read(std::int32_t device, Item* items, std::uint32_t* count) {
    if (g_from_mouse != nullptr) {  // another player's devices
        std::deque<Item>& q = static_cast<in::Device>(device) == in::Device::Mouse ? *g_from_mouse : *g_from_keys;
        std::uint32_t n = 0;
        while (n < *count && !q.empty()) {
            items[n++] = q.front();
            q.pop_front();
        }
        *count = n;
        return in::kOk;
    }
    if (static_cast<in::Device>(device) == in::Device::Mouse) pad_menu_frame();  // once per Input::poll, before the items go out
    DeviceState& d = state(static_cast<in::Device>(device));
    if (!d.acquired) {
        *count = 0;
        return in::kNotAcquired;
    }
    std::uint32_t n = 0;
    while (n < *count && !d.items.empty()) {
        items[n++] = d.items.front();
        d.items.pop_front();
    }
    *count = n;
    const bool lost = d.overflow;
    d.overflow = false;
    return lost ? in::kNoEffect : in::kOk;  // DI_BUFFEROVERFLOW
}

// ---------------------------------------------------------------- game pads

SDL_Gamepad* g_pads[in::kMaxPads] = {};

void pad_added(SDL_JoystickID id) {
    for (SDL_Gamepad* p : g_pads)
        if (p && SDL_GetGamepadID(p) == id) return;  // already open
    for (int i = 0; i < in::kMaxPads; ++i) {
        if (g_pads[i]) continue;
        g_pads[i] = SDL_OpenGamepad(id);
        if (g_pads[i]) {
            const char* name = SDL_GetGamepadName(g_pads[i]);
            joan::hook::log_line("SDL input: game pad %d connected: %s", i + 1, name ? name : "?");
        } else {
            joan::hook::log_line("SDL input: game pad %u could not be opened: %s", static_cast<unsigned>(id), SDL_GetError());
        }
        return;
    }
}

// The pads behind move up: slot 0 is the game's joystick (its controls and options use only that one), so it holds
// a pad whenever one is connected.
void pad_removed(SDL_JoystickID id) {
    for (int i = 0; i < in::kMaxPads; ++i) {
        if (g_pads[i] && SDL_GetGamepadID(g_pads[i]) == id) {
            SDL_CloseGamepad(g_pads[i]);
            g_pads[i] = nullptr;
            joan::hook::log_line("SDL input: game pad %d disconnected", i + 1);
        }
    }
    int n = 0;
    for (int i = 0; i < in::kMaxPads; ++i)
        if (g_pads[i]) g_pads[n++] = g_pads[i];
    for (int i = n; i < in::kMaxPads; ++i) g_pads[i] = nullptr;
}

std::int32_t real_pads(std::int32_t, void* out, std::uint32_t*) {
    // The pads SDL knows now, not only those whose SDL_EVENT_GAMEPAD_ADDED was pumped already: the game asks at
    // Input::init (its DirectInput version enumerated the joysticks there) and builds its options menu - whose
    // joystick control type is offered only when a joystick is there - right after.
    if (joan::platform::detail::gamepads_enabled() && (SDL_WasInit(SDL_INIT_GAMEPAD) & SDL_INIT_GAMEPAD) != 0) {
        int n = 0;
        if (SDL_JoystickID* ids = SDL_GetGamepads(&n)) {
            for (int i = 0; i < n; ++i) pad_added(ids[i]);
            SDL_free(ids);
        }
    }
    std::uint32_t mask = 0;
    for (int i = 0; i < in::kMaxPads; ++i)
        if (g_pads[i] && SDL_GamepadConnected(g_pads[i])) mask |= 1u << i;
    std::memcpy(out, &mask, sizeof mask);
    return in::kOk;
}

// The pad options of openjoan.ini ([Platform], read once, next to openjoan.exe).
struct PadSettings {
    float deadzone = 0.15f;  // PadDeadzone (%): a stick inside it reads centred (no drift); the rest is rescaled to 0..1
    float curve = 1.0f;      // PadCurve: the stick's response, |v|^curve (1 = linear, 2 = finer near the centre)
    float trigger = 0.5f;    // PadTrigger (%): how far a trigger goes down before it counts as pressed
    float rumble = 1.0f;     // PadRumble (%): vibration strength, 0 = off
    float cursor = 1.0f;     // PadCursorSpeed (%): the menu cursor's speed on the left stick
    bool menu = true;        // PadMenu: the pad drives the menus' cursor (left stick, A, B, D-pad)
    float camera = 1.0f;     // PadCamera (%): the right stick turns the camera in the game as the mouse does, 0 = off
    bool camera_invert = false;  // PadCameraInvert: the right stick's up / down reversed
    bool auto_input = true;  // PadAutoInput: the options' input method follows the device last used (pad / keyboard)
};
const PadSettings& pad_settings() {
    static const PadSettings s = [] {
        PadSettings r;
        const std::filesystem::path& ini = joan::platform::settings_file();
        auto pct = [&](const char* key, int def, int lo, int hi) {
            return static_cast<float>(std::clamp(static_cast<int>(joan::platform::ini::get_int("Platform", key, def, ini.c_str())), lo, hi)) / 100.0f;
        };
        r.deadzone = pct("PadDeadzone", 15, 0, 90);
        r.trigger = pct("PadTrigger", 50, 5, 95);
        r.rumble = pct("PadRumble", 100, 0, 200);
        r.cursor = pct("PadCursorSpeed", 100, 10, 400);
        char curve[32] = {};
        joan::platform::ini::get_string("Platform", "PadCurve", "1.0", curve, sizeof curve, ini.c_str());
        r.curve = std::clamp(static_cast<float>(std::atof(curve)), 0.5f, 4.0f);
        r.menu = joan::platform::ini::get_int("Platform", "PadMenu", 1, ini.c_str()) != 0;
        r.camera = pct("PadCamera", 100, 0, 400);
        r.camera_invert = joan::platform::ini::get_int("Platform", "PadCameraInvert", 0, ini.c_str()) != 0;
        r.auto_input = joan::platform::ini::get_int("Platform", "PadAutoInput", 1, ini.c_str()) != 0;
        joan::hook::log_line("SDL input: pads - deadzone %.0f%%, curve %.2f, trigger %.0f%%, rumble %.0f%%, menu cursor %s (%.0f%%), "
                             "camera %.0f%%%s, automatic input method %s",
                             r.deadzone * 100.0f, r.curve, r.trigger * 100.0f, r.rumble * 100.0f, r.menu ? "on" : "off", r.cursor * 100.0f,
                             r.camera * 100.0f, r.camera_invert ? " (inverted)" : "", r.auto_input ? "on" : "off");
        return r;
    }();
    return s;
}

// A stick (two axes) after the radial deadzone and the response curve: -1..1 each, centred inside the deadzone.
void stick_xy(SDL_Gamepad* p, SDL_GamepadAxis ax, SDL_GamepadAxis ay, float& x, float& y) {
    x = std::clamp(static_cast<float>(SDL_GetGamepadAxis(p, ax)) / 32767.0f, -1.0f, 1.0f);
    y = std::clamp(static_cast<float>(SDL_GetGamepadAxis(p, ay)) / 32767.0f, -1.0f, 1.0f);
    const PadSettings& s = pad_settings();
    const float m = std::sqrt(x * x + y * y);
    if (m <= s.deadzone || m <= 0.0f) {
        x = y = 0.0f;
        return;
    }
    float r = std::min(1.0f, (m - s.deadzone) / (1.0f - s.deadzone));
    if (s.curve != 1.0f) r = std::pow(r, s.curve);
    x = x / m * r;
    y = y / m * r;
}
// -1..1 as DirectInput's default joystick range 0..65535 (centre 32768).
std::int32_t di_axis(float v) { return std::clamp(static_cast<std::int32_t>(std::lround(v * 32767.0f)) + 32768, 0, 65535); }
bool trigger_down(SDL_Gamepad* p, SDL_GamepadAxis a) {
    return static_cast<float>(SDL_GetGamepadAxis(p, a)) > pad_settings().trigger * 32767.0f;
}

std::int32_t real_read_pad(std::int32_t slot, void* out, std::uint32_t*) {
    in::PadState st{};
    st.pov = 0xffffffffu;
    std::memset(st.axes, 0, sizeof st.axes);
    SDL_Gamepad* p = slot >= 0 && slot < in::kMaxPads ? g_pads[slot] : nullptr;
    if (!p || !SDL_GamepadConnected(p) || !joan::platform::detail::focused()) {
        std::memcpy(out, &st, sizeof st);
        return in::kNotAcquired;
    }
    {
        float lx, ly, rx, ry;
        stick_xy(p, SDL_GAMEPAD_AXIS_LEFTX, SDL_GAMEPAD_AXIS_LEFTY, lx, ly);
        stick_xy(p, SDL_GAMEPAD_AXIS_RIGHTX, SDL_GAMEPAD_AXIS_RIGHTY, rx, ry);
        st.axes[0] = di_axis(lx);
        st.axes[1] = di_axis(ly);
        st.axes[2] = di_axis(rx);
        st.axes[3] = di_axis(ry);
    }
    const bool up = SDL_GetGamepadButton(p, SDL_GAMEPAD_BUTTON_DPAD_UP);
    const bool down = SDL_GetGamepadButton(p, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
    const bool left = SDL_GetGamepadButton(p, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
    const bool right = SDL_GetGamepadButton(p, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
    const int dx = (right ? 1 : 0) - (left ? 1 : 0);
    const int dy = (down ? 1 : 0) - (up ? 1 : 0);
    static const std::uint32_t kPov[3][3] = {  // [dy + 1][dx + 1]
        {31500, 0, 4500},
        {27000, 0xffffffffu, 9000},
        {22500, 18000, 13500},
    };
    st.pov = kPov[dy + 1][dx + 1];
    static const SDL_GamepadButton kButtons[10] = {
        SDL_GAMEPAD_BUTTON_SOUTH,      SDL_GAMEPAD_BUTTON_EAST,       SDL_GAMEPAD_BUTTON_WEST,
        SDL_GAMEPAD_BUTTON_NORTH,      SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
        SDL_GAMEPAD_BUTTON_BACK,       SDL_GAMEPAD_BUTTON_START,      SDL_GAMEPAD_BUTTON_LEFT_STICK,
        SDL_GAMEPAD_BUTTON_RIGHT_STICK,
    };
    for (int b = 0; b < 10; ++b) st.buttons[b] = SDL_GetGamepadButton(p, kButtons[b]) ? 0x80 : 0;
    // The triggers (0..32767) as buttons 10 / 11, pressed past PadTrigger.
    st.buttons[10] = trigger_down(p, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) ? 0x80 : 0;
    st.buttons[11] = trigger_down(p, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) ? 0x80 : 0;
    std::memcpy(out, &st, sizeof st);
    return in::kOk;
}

// ---------------------------------------------------------------- the pad in the menus

struct WidgetRect {
    std::int16_t x0, y0, x1, y1;
};
std::vector<WidgetRect> g_widgets_now;  // drawn since the last poll (Button::draw)
std::vector<WidgetRect> g_widgets;      // the last frame's: the D-pad's targets

struct PadMenu {
    bool active = false;
    Uint64 last_ns = 0;
    float rest_x = 0.0f, rest_y = 0.0f;  // the cursor's fractions of a pixel
    bool a = false, b = false;           // A / B held (their mouse button / Esc key items sent)
    int dpad = 0;                        // the D-pad direction held: 1 up, 2 down, 3 left, 4 right
    Uint64 dpad_next_ns = 0;             // its next repeat
    Uint64 wheel_next_ns = 0;            // the right stick's next wheel notch
};
PadMenu g_pad_menu;
Uint64 g_pad_menu_held_ns = 0;  // pad_menu_hold: the pad off the menu until then

void push_mouse(DWORD ofs, DWORD data) { push(g_mouse_dev, ofs, data, static_cast<DWORD>(joan::platform::time_ms()), g_sequence++); }
void push_key(DWORD dik, bool down) { push(g_key_dev, dik, down ? 0x80u : 0u, static_cast<DWORD>(joan::platform::time_ms()), g_sequence++); }

// The D-pad: the cursor jumps to the centre of the nearest button in that direction (the menu's layout, 1024x768).
void hop(int dir, int& mx, int& my) {
    const float dx = dir == 3 ? -1.0f : dir == 4 ? 1.0f : 0.0f, dy = dir == 1 ? -1.0f : dir == 2 ? 1.0f : 0.0f;
    float best = 1e9f;
    int bx = mx, by = my;
    for (const WidgetRect& w : g_widgets) {
        if (w.x1 - w.x0 < 8 || w.y1 - w.y0 < 8) continue;
        const int cx = std::clamp((w.x0 + w.x1) / 2, 8, 1015), cy = std::clamp((w.y0 + w.y1) / 2, 8, 759);
        if (mx >= w.x0 && mx <= w.x1 && my >= w.y0 && my <= w.y1) continue;  // the one under the cursor
        const float vx = static_cast<float>(cx - mx), vy = static_cast<float>(cy - my);
        const float along = vx * dx + vy * dy;                      // distance in the direction
        const float across = std::fabs(vx * dy - vy * dx);          // distance off it
        if (along < 4.0f || across > along * 2.0f) continue;        // ahead, within about 63 degrees
        const float score = along + across * 2.0f;
        if (score < best) {
            best = score;
            bx = cx;
            by = cy;
        }
    }
    mx = bx;
    my = by;
}

// The input method of the options (keyboard & mouse / joystick) follows the device last used ([Platform]
// PadAutoInput): a pad button pressed, a stick or trigger pushed -> joystick; a key, a mouse button or a real mouse
// movement (SDL's events, not the items the pad itself sends in the menus) -> the keyboard type used before.
bool g_kbm_used = false;
void pad_auto_input(SDL_Gamepad* p) {
    if (!pad_settings().auto_input) return;
    bool pad = false;
    for (int b = 0; b < SDL_GAMEPAD_BUTTON_COUNT && !pad; ++b) pad = SDL_GetGamepadButton(p, static_cast<SDL_GamepadButton>(b));
    if (!pad) {
        float x, y;
        stick_xy(p, SDL_GAMEPAD_AXIS_LEFTX, SDL_GAMEPAD_AXIS_LEFTY, x, y);
        pad = x * x + y * y > 0.25f;
        stick_xy(p, SDL_GAMEPAD_AXIS_RIGHTX, SDL_GAMEPAD_AXIS_RIGHTY, x, y);
        pad = pad || x * x + y * y > 0.25f || trigger_down(p, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) || trigger_down(p, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
    }
    if (pad)
        joan::optmenu::auto_input_type(true);
    else if (g_kbm_used)
        joan::optmenu::auto_input_type(false);
}

// The right stick in the game (no menu screen up): the mouse's movement - the camera / turning in action mode, the
// cursor in RTS mode - at up to ~1200 pixels a second ([Platform] PadCamera, PadCameraInvert).
float g_cam_rest_x = 0.0f, g_cam_rest_y = 0.0f;
Uint64 g_cam_last_ns = 0;
void pad_camera(SDL_Gamepad* p, Uint64 now) {
    const float dt = g_cam_last_ns != 0 ? std::min(0.1f, static_cast<float>(now - g_cam_last_ns) / 1e9f) : 0.0f;
    g_cam_last_ns = now;
    const PadSettings& s = pad_settings();
    if (!p || s.camera <= 0.0f) {
        g_cam_rest_x = g_cam_rest_y = 0.0f;
        return;
    }
    float rx, ry;
    stick_xy(p, SDL_GAMEPAD_AXIS_RIGHTX, SDL_GAMEPAD_AXIS_RIGHTY, rx, ry);
    if (rx == 0.0f && ry == 0.0f) return;
    const float speed = 1200.0f * s.camera;
    g_cam_rest_x += rx * speed * dt;
    g_cam_rest_y += (s.camera_invert ? -ry : ry) * speed * dt;
    const LONG dx = static_cast<LONG>(g_cam_rest_x), dy = static_cast<LONG>(g_cam_rest_y);
    g_cam_rest_x -= static_cast<float>(dx);
    g_cam_rest_y -= static_cast<float>(dy);
    const DWORD seq = g_sequence++;  // one report, as a mouse's
    if (dx) push(g_mouse_dev, 0, static_cast<DWORD>(dx), static_cast<DWORD>(joan::platform::time_ms()), seq);
    if (dy) push(g_mouse_dev, 4, static_cast<DWORD>(dy), static_cast<DWORD>(joan::platform::time_ms()), seq);
}

void pad_menu_frame() {
    // The game reads the mouse twice per Input::poll (0x00594f90, two rounds): this runs once a frame.
    const Uint64 now = SDL_GetTicksNS();
    static Uint64 last_run = 0;
    if (last_run != 0 && now - last_run < 2000000ull) return;
    last_run = now;
    g_widgets.swap(g_widgets_now);
    g_widgets_now.clear();
    PadMenu& m = g_pad_menu;
    SDL_Gamepad* const p = g_pads[0];
    const bool usable = p && SDL_GamepadConnected(p) && joan::platform::detail::focused();
    if (usable) pad_auto_input(p);
    g_kbm_used = false;
    const bool menu_screen = joan::display::canvas_armed();
    pad_camera(usable && !menu_screen ? p : nullptr, now);
    const bool on = usable && pad_settings().menu && menu_screen;
    if (!on) {
        if (m.a) push_mouse(0xc, 0);  // let go of what the pad held
        if (m.b) push_key(0x01, false);
        m = PadMenu{};
        return;
    }
    const bool a = SDL_GetGamepadButton(p, SDL_GAMEPAD_BUTTON_SOUTH), b = SDL_GetGamepadButton(p, SDL_GAMEPAD_BUTTON_EAST);
    if (now < g_pad_menu_held_ns) {
        // A screen reads the pad's buttons itself (binding one on the key bindings page): let go of A / B and take
        // what is held now as already handled, so nothing clicks or goes back when the menu gets the pad again.
        if (m.a && !a) push_mouse(0xc, 0);
        if (m.b && !b) push_key(0x01, false);
        m.a = a;
        m.b = b;
        m.active = false;
        return;
    }
    const float dt = m.active ? std::min(0.1f, static_cast<float>(now - m.last_ns) / 1e9f) : 0.0f;
    m.active = true;
    m.last_ns = now;
    int& mx = joan::hook::global<int>(d::g_mouse_x);
    int& my = joan::hook::global<int>(d::g_mouse_y);
    // Left stick: the cursor, up to ~900 layout pixels a second at full tilt (PadCursorSpeed).
    float lx, ly;
    stick_xy(p, SDL_GAMEPAD_AXIS_LEFTX, SDL_GAMEPAD_AXIS_LEFTY, lx, ly);
    if (lx != 0.0f || ly != 0.0f) {
        const float speed = 900.0f * pad_settings().cursor;
        m.rest_x += lx * speed * dt;
        m.rest_y += ly * speed * dt;
        const int ix = static_cast<int>(m.rest_x), iy = static_cast<int>(m.rest_y);
        m.rest_x -= static_cast<float>(ix);
        m.rest_y -= static_cast<float>(iy);
        mx = std::clamp(mx + ix, 0, 1023);
        my = std::clamp(my + iy, 0, 767);
    }
    // D-pad: hop to the next button (held: again after 0.4 s, then every 0.15 s).
    const int dir = SDL_GetGamepadButton(p, SDL_GAMEPAD_BUTTON_DPAD_UP)      ? 1
                    : SDL_GetGamepadButton(p, SDL_GAMEPAD_BUTTON_DPAD_DOWN)  ? 2
                    : SDL_GetGamepadButton(p, SDL_GAMEPAD_BUTTON_DPAD_LEFT)  ? 3
                    : SDL_GetGamepadButton(p, SDL_GAMEPAD_BUTTON_DPAD_RIGHT) ? 4
                                                                             : 0;
    if (dir != 0 && (dir != m.dpad || now >= m.dpad_next_ns)) {
        hop(dir, mx, my);
        m.dpad_next_ns = now + (dir != m.dpad ? 400000000ull : 150000000ull);
    }
    m.dpad = dir;
    // A: the left mouse button; B: Esc (back / close).
    if (a != m.a) push_mouse(0xc, a ? 0x80u : 0u);
    if (b != m.b) push_key(0x01, b);
    m.a = a;
    m.b = b;
    // Right stick up / down: the mouse wheel (lists), a notch every 0.12 s.
    float rx, ry;
    stick_xy(p, SDL_GAMEPAD_AXIS_RIGHTX, SDL_GAMEPAD_AXIS_RIGHTY, rx, ry);
    if (std::fabs(ry) > 0.5f && now >= m.wheel_next_ns) {
        push_mouse(8, static_cast<DWORD>(ry < 0.0f ? 120 : -120));
        m.wheel_next_ns = now + 120000000ull;
    }
}

// SDL_Scancode -> DIK_* (the PC set-1 scan code, 0x80 for the extended keys).
BYTE dik_of(SDL_Scancode sc) {
    if (sc >= SDL_SCANCODE_F1 && sc <= SDL_SCANCODE_F10) return static_cast<BYTE>(0x3b + (sc - SDL_SCANCODE_F1));
    switch (sc) {
        case SDL_SCANCODE_ESCAPE: return 0x01;
        case SDL_SCANCODE_1: return 0x02;
        case SDL_SCANCODE_2: return 0x03;
        case SDL_SCANCODE_3: return 0x04;
        case SDL_SCANCODE_4: return 0x05;
        case SDL_SCANCODE_5: return 0x06;
        case SDL_SCANCODE_6: return 0x07;
        case SDL_SCANCODE_7: return 0x08;
        case SDL_SCANCODE_8: return 0x09;
        case SDL_SCANCODE_9: return 0x0a;
        case SDL_SCANCODE_0: return 0x0b;
        case SDL_SCANCODE_MINUS: return 0x0c;
        case SDL_SCANCODE_EQUALS: return 0x0d;
        case SDL_SCANCODE_BACKSPACE: return 0x0e;
        case SDL_SCANCODE_TAB: return 0x0f;
        case SDL_SCANCODE_Q: return 0x10;
        case SDL_SCANCODE_W: return 0x11;
        case SDL_SCANCODE_E: return 0x12;
        case SDL_SCANCODE_R: return 0x13;
        case SDL_SCANCODE_T: return 0x14;
        case SDL_SCANCODE_Y: return 0x15;
        case SDL_SCANCODE_U: return 0x16;
        case SDL_SCANCODE_I: return 0x17;
        case SDL_SCANCODE_O: return 0x18;
        case SDL_SCANCODE_P: return 0x19;
        case SDL_SCANCODE_LEFTBRACKET: return 0x1a;
        case SDL_SCANCODE_RIGHTBRACKET: return 0x1b;
        case SDL_SCANCODE_RETURN: return 0x1c;
        case SDL_SCANCODE_LCTRL: return 0x1d;
        case SDL_SCANCODE_A: return 0x1e;
        case SDL_SCANCODE_S: return 0x1f;
        case SDL_SCANCODE_D: return 0x20;
        case SDL_SCANCODE_F: return 0x21;
        case SDL_SCANCODE_G: return 0x22;
        case SDL_SCANCODE_H: return 0x23;
        case SDL_SCANCODE_J: return 0x24;
        case SDL_SCANCODE_K: return 0x25;
        case SDL_SCANCODE_L: return 0x26;
        case SDL_SCANCODE_SEMICOLON: return 0x27;
        case SDL_SCANCODE_APOSTROPHE: return 0x28;
        case SDL_SCANCODE_GRAVE: return 0x29;
        case SDL_SCANCODE_LSHIFT: return 0x2a;
        case SDL_SCANCODE_BACKSLASH: return 0x2b;
        case SDL_SCANCODE_NONUSHASH: return 0x2b;  // the same key position on ISO keyboards
        case SDL_SCANCODE_Z: return 0x2c;
        case SDL_SCANCODE_X: return 0x2d;
        case SDL_SCANCODE_C: return 0x2e;
        case SDL_SCANCODE_V: return 0x2f;
        case SDL_SCANCODE_B: return 0x30;
        case SDL_SCANCODE_N: return 0x31;
        case SDL_SCANCODE_M: return 0x32;
        case SDL_SCANCODE_COMMA: return 0x33;
        case SDL_SCANCODE_PERIOD: return 0x34;
        case SDL_SCANCODE_SLASH: return 0x35;
        case SDL_SCANCODE_RSHIFT: return 0x36;
        case SDL_SCANCODE_KP_MULTIPLY: return 0x37;
        case SDL_SCANCODE_LALT: return 0x38;
        case SDL_SCANCODE_SPACE: return 0x39;
        case SDL_SCANCODE_CAPSLOCK: return 0x3a;
        case SDL_SCANCODE_NUMLOCKCLEAR: return 0x45;
        case SDL_SCANCODE_SCROLLLOCK: return 0x46;
        case SDL_SCANCODE_KP_7: return 0x47;
        case SDL_SCANCODE_KP_8: return 0x48;
        case SDL_SCANCODE_KP_9: return 0x49;
        case SDL_SCANCODE_KP_MINUS: return 0x4a;
        case SDL_SCANCODE_KP_4: return 0x4b;
        case SDL_SCANCODE_KP_5: return 0x4c;
        case SDL_SCANCODE_KP_6: return 0x4d;
        case SDL_SCANCODE_KP_PLUS: return 0x4e;
        case SDL_SCANCODE_KP_1: return 0x4f;
        case SDL_SCANCODE_KP_2: return 0x50;
        case SDL_SCANCODE_KP_3: return 0x51;
        case SDL_SCANCODE_KP_0: return 0x52;
        case SDL_SCANCODE_KP_PERIOD: return 0x53;
        case SDL_SCANCODE_NONUSBACKSLASH: return 0x56;  // DIK_OEM_102
        case SDL_SCANCODE_F11: return 0x57;
        case SDL_SCANCODE_F12: return 0x58;
        case SDL_SCANCODE_F13: return 0x64;
        case SDL_SCANCODE_F14: return 0x65;
        case SDL_SCANCODE_F15: return 0x66;
        case SDL_SCANCODE_INTERNATIONAL2: return 0x70;  // DIK_KANA
        case SDL_SCANCODE_INTERNATIONAL1: return 0x73;  // DIK_ABNT_C1
        case SDL_SCANCODE_INTERNATIONAL4: return 0x79;  // DIK_CONVERT
        case SDL_SCANCODE_INTERNATIONAL5: return 0x7b;  // DIK_NOCONVERT
        case SDL_SCANCODE_INTERNATIONAL3: return 0x7d;  // DIK_YEN
        case SDL_SCANCODE_KP_EQUALS: return 0x8d;
        case SDL_SCANCODE_KP_ENTER: return 0x9c;
        case SDL_SCANCODE_RCTRL: return 0x9d;
        case SDL_SCANCODE_MUTE: return 0xa0;
        case SDL_SCANCODE_VOLUMEDOWN: return 0xae;
        case SDL_SCANCODE_VOLUMEUP: return 0xb0;
        case SDL_SCANCODE_KP_COMMA: return 0xb3;
        case SDL_SCANCODE_KP_DIVIDE: return 0xb5;
        case SDL_SCANCODE_PRINTSCREEN: return 0xb7;  // DIK_SYSRQ
        case SDL_SCANCODE_RALT: return 0xb8;
        case SDL_SCANCODE_PAUSE: return 0xc5;
        case SDL_SCANCODE_HOME: return 0xc7;
        case SDL_SCANCODE_UP: return 0xc8;
        case SDL_SCANCODE_PAGEUP: return 0xc9;
        case SDL_SCANCODE_LEFT: return 0xcb;
        case SDL_SCANCODE_RIGHT: return 0xcd;
        case SDL_SCANCODE_END: return 0xcf;
        case SDL_SCANCODE_DOWN: return 0xd0;
        case SDL_SCANCODE_PAGEDOWN: return 0xd1;
        case SDL_SCANCODE_INSERT: return 0xd2;
        case SDL_SCANCODE_DELETE: return 0xd3;
        case SDL_SCANCODE_LGUI: return 0xdb;
        case SDL_SCANCODE_RGUI: return 0xdc;
        case SDL_SCANCODE_APPLICATION: return 0xdd;
        case SDL_SCANCODE_POWER: return 0xde;
        case SDL_SCANCODE_SLEEP: return 0xdf;
        default: return 0;
    }
}

// SDL mouse button -> DirectInput button (DIMOFS_BUTTON0 + n): left, right, middle, X1.
int di_button(Uint8 b) {
    switch (b) {
        case SDL_BUTTON_LEFT: return 0;
        case SDL_BUTTON_RIGHT: return 1;
        case SDL_BUTTON_MIDDLE: return 2;
        case SDL_BUTTON_X1: return 3;
        default: return -1;
    }
}

// Whole units of a float delta, the rest kept for the next event.
LONG take_whole(float& rest, float delta) {
    rest += delta;
    const float whole = std::trunc(rest);
    rest -= whole;
    return static_cast<LONG>(whole);
}

// ---------------------------------------------------------------- key_down (our own keys)

// Windows virtual-key code -> SDL scancode (the key position of a US layout for the punctuation keys).
SDL_Scancode scancode_of_vk(int vk) {
    if (vk >= 'A' && vk <= 'Z') return static_cast<SDL_Scancode>(SDL_SCANCODE_A + (vk - 'A'));
    if (vk >= '1' && vk <= '9') return static_cast<SDL_Scancode>(SDL_SCANCODE_1 + (vk - '1'));
    if (vk == '0') return SDL_SCANCODE_0;
    if (vk >= VK_F1 && vk <= VK_F12) return static_cast<SDL_Scancode>(SDL_SCANCODE_F1 + (vk - VK_F1));
    if (vk >= VK_F13 && vk <= VK_F24) return static_cast<SDL_Scancode>(SDL_SCANCODE_F13 + (vk - VK_F13));
    if (vk >= VK_NUMPAD1 && vk <= VK_NUMPAD9) return static_cast<SDL_Scancode>(SDL_SCANCODE_KP_1 + (vk - VK_NUMPAD1));
    switch (vk) {
        case VK_NUMPAD0: return SDL_SCANCODE_KP_0;
        case VK_BACK: return SDL_SCANCODE_BACKSPACE;
        case VK_TAB: return SDL_SCANCODE_TAB;
        case VK_RETURN: return SDL_SCANCODE_RETURN;
        case VK_PAUSE: return SDL_SCANCODE_PAUSE;
        case VK_CAPITAL: return SDL_SCANCODE_CAPSLOCK;
        case VK_ESCAPE: return SDL_SCANCODE_ESCAPE;
        case VK_SPACE: return SDL_SCANCODE_SPACE;
        case VK_PRIOR: return SDL_SCANCODE_PAGEUP;
        case VK_NEXT: return SDL_SCANCODE_PAGEDOWN;
        case VK_END: return SDL_SCANCODE_END;
        case VK_HOME: return SDL_SCANCODE_HOME;
        case VK_LEFT: return SDL_SCANCODE_LEFT;
        case VK_UP: return SDL_SCANCODE_UP;
        case VK_RIGHT: return SDL_SCANCODE_RIGHT;
        case VK_DOWN: return SDL_SCANCODE_DOWN;
        case VK_SNAPSHOT: return SDL_SCANCODE_PRINTSCREEN;
        case VK_INSERT: return SDL_SCANCODE_INSERT;
        case VK_DELETE: return SDL_SCANCODE_DELETE;
        case VK_MULTIPLY: return SDL_SCANCODE_KP_MULTIPLY;
        case VK_ADD: return SDL_SCANCODE_KP_PLUS;
        case VK_SUBTRACT: return SDL_SCANCODE_KP_MINUS;
        case VK_DECIMAL: return SDL_SCANCODE_KP_PERIOD;
        case VK_DIVIDE: return SDL_SCANCODE_KP_DIVIDE;
        case VK_NUMLOCK: return SDL_SCANCODE_NUMLOCKCLEAR;
        case VK_SCROLL: return SDL_SCANCODE_SCROLLLOCK;
        case VK_LSHIFT: return SDL_SCANCODE_LSHIFT;
        case VK_RSHIFT: return SDL_SCANCODE_RSHIFT;
        case VK_LCONTROL: return SDL_SCANCODE_LCTRL;
        case VK_RCONTROL: return SDL_SCANCODE_RCTRL;
        case VK_LMENU: return SDL_SCANCODE_LALT;
        case VK_RMENU: return SDL_SCANCODE_RALT;
        case VK_OEM_1: return SDL_SCANCODE_SEMICOLON;
        case VK_OEM_PLUS: return SDL_SCANCODE_EQUALS;
        case VK_OEM_COMMA: return SDL_SCANCODE_COMMA;
        case VK_OEM_MINUS: return SDL_SCANCODE_MINUS;
        case VK_OEM_PERIOD: return SDL_SCANCODE_PERIOD;
        case VK_OEM_2: return SDL_SCANCODE_SLASH;
        case VK_OEM_3: return SDL_SCANCODE_GRAVE;
        case VK_OEM_4: return SDL_SCANCODE_LEFTBRACKET;
        case VK_OEM_5: return SDL_SCANCODE_BACKSLASH;
        case VK_OEM_6: return SDL_SCANCODE_RIGHTBRACKET;
        case VK_OEM_7: return SDL_SCANCODE_APOSTROPHE;
        case VK_OEM_102: return SDL_SCANCODE_NONUSBACKSLASH;
        default: return SDL_SCANCODE_UNKNOWN;
    }
}

// ---------------------------------------------------------------- USER32

bool video_ready() { return (SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO) != 0; }

// JOAN_NOT_OURS's error (before SDL's video is up, or a window that is not ours: Windows' on Windows, none elsewhere).
[[maybe_unused]] void set_error(DWORD e) { joan::platform::set_last_error(e); }

BOOL WINAPI j_GetCursorPos(LPPOINT p) {
    if (!p) return FALSE;
    if (!video_ready()) JOAN_NOT_OURS(GetCursorPos(p), FALSE);
    float x = 0, y = 0;
    SDL_GetGlobalMouseState(&x, &y);
    p->x = static_cast<LONG>(std::floor(x));
    p->y = static_cast<LONG>(std::floor(y));
#ifndef _WIN32
    int ox = 0, oy = 0;
    joan::platform::detail::screen_origin(ox, oy);  // the game's screen: the primary display's top-left is 0,0
    p->x -= ox;
    p->y -= oy;
#endif
    return TRUE;
}

BOOL WINAPI j_SetCursorPos(int x, int y) {
    if (!video_ready()) JOAN_NOT_OURS(SetCursorPos(x, y), FALSE);
#ifndef _WIN32
    int ox = 0, oy = 0;
    joan::platform::detail::screen_origin(ox, oy);
    x += ox;
    y += oy;
#endif
    return SDL_WarpMouseGlobal(static_cast<float>(x), static_cast<float>(y)) ? TRUE : FALSE;
}

BOOL WINAPI j_GetClientRect(HWND h, LPRECT rc) {
    SDL_Window* w = joan::platform::detail::window();
    if (!w || !joan::platform::detail::is_window(h)) JOAN_NOT_OURS(GetClientRect(h, rc), FALSE);
    if (!rc) return FALSE;
    int cw = 0, ch = 0;
    SDL_GetWindowSizeInPixels(w, &cw, &ch);
    *rc = RECT{0, 0, cw, ch};
    return TRUE;
}

BOOL WINAPI j_ScreenToClient(HWND h, LPPOINT p) {
    SDL_Window* w = joan::platform::detail::window();
    if (!w || !joan::platform::detail::is_window(h)) JOAN_NOT_OURS(ScreenToClient(h, p), FALSE);
    if (!p) return FALSE;
    int x = 0, y = 0;
    SDL_GetWindowPosition(w, &x, &y);  // of the client area
#ifndef _WIN32
    int ox = 0, oy = 0;
    joan::platform::detail::screen_origin(ox, oy);
    x -= ox;
    y -= oy;
#endif
    p->x -= x;
    p->y -= y;
    return TRUE;
}

// Windows' cursor display counter: shown while >= 0.
int g_cursor_count = 0;
int WINAPI j_ShowCursor(BOOL show) {
    if (!video_ready()) JOAN_NOT_OURS(ShowCursor(show), 0);
    g_cursor_count += show ? 1 : -1;
    if (g_cursor_count >= 0) SDL_ShowCursor();
    else SDL_HideCursor();
    return g_cursor_count;
}

SHORT WINAPI j_GetKeyState(int vk) {
    if (!video_ready()) JOAN_NOT_OURS(GetKeyState(vk), 0);
    const SDL_Keymod mods = SDL_GetModState();
    SDL_Scancode sc = SDL_SCANCODE_UNKNOWN;
    SDL_Keymod lock = SDL_KMOD_NONE;
    switch (vk) {
        case VK_CAPITAL: sc = SDL_SCANCODE_CAPSLOCK; lock = SDL_KMOD_CAPS; break;
        case VK_NUMLOCK: sc = SDL_SCANCODE_NUMLOCKCLEAR; lock = SDL_KMOD_NUM; break;
        case VK_SCROLL: sc = SDL_SCANCODE_SCROLLLOCK; lock = SDL_KMOD_SCROLL; break;
        default: JOAN_NOT_OURS(GetKeyState(vk), 0);  // the game asks only for the lock keys
    }
    int n = 0;
    const bool* keys = SDL_GetKeyboardState(&n);
    const bool down = keys && sc < n && keys[sc];
    return static_cast<SHORT>((down ? 0x8000 : 0) | ((mods & lock) != 0 ? 1 : 0));
}

// The game's ANSI text as UTF-8 (platform/text.cpp: the [Platform] CodePage).
std::string utf8(const char* s) { return joan::platform::text::ansi_to_utf8(s); }

int WINAPI j_MessageBoxA(HWND h, LPCSTR text, LPCSTR caption, UINT type) {
    struct Button {
        int id;
        const char* label;
    };
    static const Button kOk[] = {{IDOK, "OK"}};
    static const Button kOkCancel[] = {{IDOK, "OK"}, {IDCANCEL, "Cancel"}};
    static const Button kAbortRetryIgnore[] = {{IDABORT, "Abort"}, {IDRETRY, "Retry"}, {IDIGNORE, "Ignore"}};
    static const Button kYesNoCancel[] = {{IDYES, "Yes"}, {IDNO, "No"}, {IDCANCEL, "Cancel"}};
    static const Button kYesNo[] = {{IDYES, "Yes"}, {IDNO, "No"}};
    static const Button kRetryCancel[] = {{IDRETRY, "Retry"}, {IDCANCEL, "Cancel"}};
    const Button* set = kOk;
    int count = 1;
    switch (type & MB_TYPEMASK) {
        case MB_OKCANCEL: set = kOkCancel; count = 2; break;
        case MB_ABORTRETRYIGNORE: set = kAbortRetryIgnore; count = 3; break;
        case MB_YESNOCANCEL: set = kYesNoCancel; count = 3; break;
        case MB_YESNO: set = kYesNo; count = 2; break;
        case MB_RETRYCANCEL: set = kRetryCancel; count = 2; break;
        case MB_OK: break;
        default: JOAN_NOT_OURS(MessageBoxA(h, text, caption, type), 0);
    }
    const int def = static_cast<int>((type & MB_DEFMASK) >> 8);  // MB_DEFBUTTON1..3
    SDL_MessageBoxButtonData buttons[3] = {};
    for (int i = 0; i < count; ++i) {
        buttons[i].buttonID = set[i].id;
        buttons[i].text = set[i].label;
        buttons[i].flags = 0;
        if (i == (def < count ? def : 0)) buttons[i].flags |= SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
        if (set[i].id == IDCANCEL || (count == 1) || (set[i].id == IDNO && count == 2))
            buttons[i].flags |= SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
    }
    SDL_MessageBoxFlags flags = SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
    switch (type & MB_ICONMASK) {
        case MB_ICONERROR: flags |= SDL_MESSAGEBOX_ERROR; break;
        case MB_ICONWARNING: flags |= SDL_MESSAGEBOX_WARNING; break;
        case MB_ICONINFORMATION:
        case MB_ICONQUESTION: flags |= SDL_MESSAGEBOX_INFORMATION; break;
        default: break;
    }
    const std::string t = utf8(text);
    const std::string c = utf8(caption);
    SDL_MessageBoxData box = {};
    box.flags = flags;
    box.window = joan::platform::detail::is_window(h) ? joan::platform::detail::window() : nullptr;
    box.title = c.c_str();
    box.message = t.c_str();
    box.numbuttons = count;
    box.buttons = buttons;
    int pressed = -1;
    if (!SDL_ShowMessageBox(&box, &pressed)) {
        joan::hook::log_line("SDL message box failed (%s): Windows' MessageBoxA", SDL_GetError());
        JOAN_NOT_OURS(MessageBoxA(h, text, caption, type), 0);
    }
    if (pressed < 0) pressed = (count == 1) ? IDOK : IDCANCEL;  // closed without a button
    return pressed;
}

}  // namespace

namespace joan::platform {

namespace input {

void open(std::uint32_t mouse_buffer, std::uint32_t keyboard_buffer) {
    g_mouse_dev = DeviceState{};
    g_key_dev = DeviceState{};
    g_mouse_dev.open = g_key_dev.open = true;
    g_mouse_dev.exclusive = true;
    g_mouse_dev.buffer_size = mouse_buffer;
    g_key_dev.buffer_size = keyboard_buffer;
    hook::log_line("SDL input: keyboard and mouse read natively (buffers %u / %u items)", static_cast<unsigned>(mouse_buffer),
                   static_cast<unsigned>(keyboard_buffer));
}

void close() {
    drop(g_mouse_dev);
    drop(g_key_dev);
    g_mouse_dev.open = g_key_dev.open = false;
    update_relative_mode();
}

std::int32_t acquire(Device d) {
    return replay::input_call(replay::InputOp::Acquire, static_cast<std::int32_t>(d), nullptr, nullptr,
                              [](std::int32_t dev, void*, std::uint32_t*) { return real_acquire(dev); });
}
std::int32_t unacquire(Device d) {
    return replay::input_call(replay::InputOp::Unacquire, static_cast<std::int32_t>(d), nullptr, nullptr,
                              [](std::int32_t dev, void*, std::uint32_t*) { return real_unacquire(dev); });
}
std::int32_t read(Device d, Item* items, std::uint32_t* count) {
    return replay::input_call(replay::InputOp::Read, static_cast<std::int32_t>(d), items, count,
                              [](std::int32_t dev, void* p, std::uint32_t* n) { return real_read(dev, static_cast<Item*>(p), n); });
}

void read_from(std::deque<Item>* mouse, std::deque<Item>* keyboard) {
    g_from_mouse = mouse != nullptr && keyboard != nullptr ? mouse : nullptr;
    g_from_keys = g_from_mouse != nullptr ? keyboard : nullptr;
}

void forward(Forward to) {
    g_forward = to;
    std::memset(g_forward_keys, 0, sizeof g_forward_keys);
}

std::uint32_t pads() {
    std::uint32_t mask = 0;
    std::uint32_t size = sizeof mask;
    replay::input_call(replay::InputOp::PadMask, 0, &mask, &size, &real_pads);
    return mask;
}

std::int32_t read_pad(int slot, PadState* out) {
    std::uint32_t size = sizeof(PadState);
    return replay::input_call(replay::InputOp::PadRead, slot, out, &size, &real_read_pad);
}

void menu_widget(int x0, int y0, int x1, int y1) {
    if (g_widgets_now.size() < 1024)
        g_widgets_now.push_back({static_cast<std::int16_t>(x0), static_cast<std::int16_t>(y0), static_cast<std::int16_t>(x1), static_cast<std::int16_t>(y1)});
}

void pad_menu_hold() { g_pad_menu_held_ns = SDL_GetTicksNS() + 150000000ull; }

bool pad_look(float& x, float& y) {
    x = y = 0.0f;
    SDL_Gamepad* const p = g_pads[0];
    const PadSettings& s = pad_settings();
    if (!p || !SDL_GamepadConnected(p) || s.camera <= 0.0f) return false;
    if (!joan::platform::detail::focused()) return true;
    stick_xy(p, SDL_GAMEPAD_AXIS_RIGHTX, SDL_GAMEPAD_AXIS_RIGHTY, x, y);
    x *= s.camera;
    y *= s.camera_invert ? -s.camera : s.camera;
    return true;
}

const char* pad_button_name(int n) {
    SDL_Gamepad* const p = g_pads[0];
    if (!p || n < 1 || n > 12) return nullptr;
    // JOYPADn is the game's button slot n - 1, filled in Input::poll_joysticks (ilib/mousecr.cpp pad::kButtonSlots)
    // from the buttons south, east, west, north, LB, RB, back, start, left stick, right stick, LT, RT:
    // slots 0..11 = south, east, west, north, LB, RB, right stick, left stick, back, start, LT, RT.
    static const char* const kXbox[12] = {"A", "B", "X", "Y", "LB", "RB", "RS", "LS", "View", "Menu", "LT", "RT"};
    static const char* const kPs4[12] = {"Cross", "Circle", "Square", "Triangle", "L1", "R1", "R3", "L3", "Share", "Options", "L2", "R2"};
    static const char* const kPs5[12] = {"Cross", "Circle", "Square", "Triangle", "L1", "R1", "R3", "L3", "Create", "Options", "L2", "R2"};
    static const char* const kNintendo[12] = {"B", "A", "Y", "X", "L", "R", "RS", "LS", "-", "+", "ZL", "ZR"};  // by position (south = B)
    switch (SDL_GetGamepadType(p)) {
        case SDL_GAMEPAD_TYPE_PS3:
        case SDL_GAMEPAD_TYPE_PS4: return kPs4[n - 1];
        case SDL_GAMEPAD_TYPE_PS5: return kPs5[n - 1];
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_PRO:
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
        case SDL_GAMEPAD_TYPE_NINTENDO_SWITCH_JOYCON_PAIR: return kNintendo[n - 1];
        default: return kXbox[n - 1];
    }
}

void rumble(float low, float high, unsigned ms) {
    SDL_Gamepad* const p = g_pads[0];
    const float k = pad_settings().rumble;
    if (!p || k <= 0.0f || !joan::platform::detail::focused()) return;
    auto motor = [&](float v) { return static_cast<Uint16>(std::clamp(v * k, 0.0f, 1.0f) * 65535.0f); };
    SDL_RumbleGamepad(p, motor(low), motor(high), ms);
}

}  // namespace input

bool key_down(int vk) {
#ifdef _WIN32
#ifdef _WIN32
    if (!detail::window()) return (GetAsyncKeyState(vk) & 0x8000) != 0;
#else
    if (!detail::window()) return false;  // no keyboard before the game window
#endif
#else
    if (!detail::window()) return false;
#endif
    int n = 0;
    const bool* keys = SDL_GetKeyboardState(&n);
    if (!keys) return false;
    auto down = [&](SDL_Scancode sc) { return sc != SDL_SCANCODE_UNKNOWN && sc < n && keys[sc]; };
    switch (vk) {  // either side
        case VK_SHIFT: return down(SDL_SCANCODE_LSHIFT) || down(SDL_SCANCODE_RSHIFT);
        case VK_CONTROL: return down(SDL_SCANCODE_LCTRL) || down(SDL_SCANCODE_RCTRL);
        case VK_MENU: return down(SDL_SCANCODE_LALT) || down(SDL_SCANCODE_RALT);
        default: return down(scancode_of_vk(vk));
    }
}

namespace detail {

void on_focus(bool focused) {
    if (focused) return;  // the game acquires again (Input::poll, every frame)
    drop(g_mouse_dev);  // lost: acquire() again (Input::poll, every frame)
    drop(g_key_dev);
    update_relative_mode();
    g_rest_x = g_rest_y = g_rest_wheel = 0;
}

namespace {

// A co-op player's item for the host (input::forward), whatever the game itself does with the devices.
void forward_item(in::Device device, DWORD ofs, DWORD data, DWORD time, DWORD seq) {
    if (g_forward == nullptr || !joan::platform::detail::focused()) return;
    g_forward(device, Item{ofs, data, time, seq, 0});
}

void forward_event(const SDL_Event& e) {
    if (g_forward == nullptr) return;
    switch (e.type) {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            const BYTE dik = e.key.repeat ? 0 : dik_of(e.key.scancode);
            const bool down = e.type == SDL_EVENT_KEY_DOWN;
            if (dik == 0 || (g_forward_keys[dik] != 0) == down) return;
            g_forward_keys[dik] = down ? 0x80 : 0;
            forward_item(in::Device::Keyboard, dik, down ? 0x80u : 0u, stamp(e.key.timestamp), ++g_forward_sequence);
            return;
        }
        case SDL_EVENT_MOUSE_MOTION: {
            const auto dx = static_cast<LONG>(std::lround(e.motion.xrel)), dy = static_cast<LONG>(std::lround(e.motion.yrel));
            const DWORD t = stamp(e.motion.timestamp), seq = ++g_forward_sequence;
            if (dx) forward_item(in::Device::Mouse, 0, static_cast<DWORD>(dx), t, seq);
            if (dy) forward_item(in::Device::Mouse, 4, static_cast<DWORD>(dy), t, seq);
            return;
        }
        case SDL_EVENT_MOUSE_WHEEL: {
            const float y = e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -e.wheel.y : e.wheel.y;
            const auto dz = static_cast<LONG>(std::lround(y * WHEEL_DELTA));
            if (dz) forward_item(in::Device::Mouse, 8, static_cast<DWORD>(dz), stamp(e.wheel.timestamp), ++g_forward_sequence);
            return;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            const int b = di_button(e.button.button);
            if (b < 0) return;
            forward_item(in::Device::Mouse, static_cast<DWORD>(0xc + b), e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? 0x80u : 0u,
                         stamp(e.button.timestamp), ++g_forward_sequence);
            return;
        }
        default:
            return;
    }
}

}  // namespace

void on_input_event(const SDL_Event& e) {
    forward_event(e);
    switch (e.type) {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            if (e.key.repeat) return;  // DirectInput reports changes only
            const BYTE dik = dik_of(e.key.scancode);
            if (dik == 0) return;
            DeviceState& k = g_key_dev;
            if (!k.acquired) return;
            const bool down = e.type == SDL_EVENT_KEY_DOWN;
            if ((k.keys[dik] != 0) == down) return;
            if (down) g_kbm_used = true;  // the input method follows (pad_auto_input)
            k.keys[dik] = down ? 0x80 : 0;
            push(k, dik, down ? 0x80u : 0u, stamp(e.key.timestamp), ++g_sequence);
            return;
        }
        case SDL_EVENT_MOUSE_MOTION: {
            DeviceState& m = g_mouse_dev;
            if (!m.acquired) return;
            if (std::fabs(e.motion.xrel) + std::fabs(e.motion.yrel) > 6.0f) g_kbm_used = true;  // a real movement, not a jitter
            const LONG dx = take_whole(g_rest_x, e.motion.xrel);
            const LONG dy = take_whole(g_rest_y, e.motion.yrel);
            const DWORD t = stamp(e.motion.timestamp);
            const DWORD seq = ++g_sequence;  // one report: X and Y share it, as DirectInput's
            if (dx) push(m, 0, static_cast<DWORD>(dx), t, seq);
            if (dy) push(m, 4, static_cast<DWORD>(dy), t, seq);
            m.mouse.x += dx;
            m.mouse.y += dy;
            return;
        }
        case SDL_EVENT_MOUSE_WHEEL: {
            DeviceState& m = g_mouse_dev;
            if (!m.acquired) return;
            float y = e.wheel.y;
            if (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) y = -y;
            const LONG dz = take_whole(g_rest_wheel, y * WHEEL_DELTA);
            if (!dz) return;
            push(m, 8, static_cast<DWORD>(dz), stamp(e.wheel.timestamp), ++g_sequence);
            m.mouse.z += dz;
            return;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            DeviceState& m = g_mouse_dev;
            if (!m.acquired) return;
            const int b = di_button(e.button.button);
            if (b < 0) return;
            const bool down = e.type == SDL_EVENT_MOUSE_BUTTON_DOWN;
            m.mouse.buttons[b] = down ? 0x80 : 0;
            if (down) g_kbm_used = true;
            push(m, static_cast<DWORD>(0xc + b), down ? 0x80u : 0u, stamp(e.button.timestamp), ++g_sequence);
            return;
        }
        case SDL_EVENT_GAMEPAD_ADDED:
            pad_added(e.gdevice.which);
            return;
        case SDL_EVENT_GAMEPAD_REMOVED:
            pad_removed(e.gdevice.which);
            return;
        default:
            return;
    }
}

}  // namespace detail
}  // namespace joan::platform

#define U(name) JOAN_IMPORT_OVERRIDE("USER32.dll", #name, &j_##name, "SDL " #name)
U(GetCursorPos);
U(SetCursorPos);
U(GetClientRect);
U(ScreenToClient);
U(ShowCursor);
U(GetKeyState);
U(MessageBoxA);
#undef U
