// [Debug] FreecamKey - see freecam.hpp.
#include "freecam.hpp"

#include "platform/win32_types.hpp"

#include <chrono>
#include <cmath>
#include <string>
#ifndef _WIN32
#include <SDL3/SDL.h>
#endif

#include "platform/window.hpp"
#include "registry.hpp"
#include "platform/ini.hpp"

namespace joan::freecam {
namespace {

struct State {
    bool loaded = false, on = false, start_pending = false, prev_toggle = false, looking = false, toggled = false;
    int key = 0x78;  // F9
    float speed = 1000.0f;  // world units per second (Joan is ~79 tall)
    Pose pose{};
    // This frame's input: movement along forward / right / world up (world units), turn (radians).
    float forward = 0.0f, right = 0.0f, up = 0.0f, turn_yaw = 0.0f, turn_pitch = 0.0f;
    POINT anchor{};
    std::chrono::steady_clock::time_point last{};
    // Mouse / key directions in the game's angle terms (worked out from the axes on the first pose).
    float yaw_sign = 1.0f, pitch_sign = 1.0f;
    bool signs_known = false;
} g;

bool down(int vk) { return platform::key_down(vk); }

}  // namespace

bool active() { return g.on; }
bool takes_input() { return g.on || g.toggled; }

void frame(bool focused, bool menu_open) {
    if (!g.loaded) {
        g.loaded = true;
        const std::string ini = hook::ini_path();
        g.key = static_cast<int>(joan::platform::ini::get_int("Debug", "FreecamKey", 0x78, ini.c_str()));
        if (g.key <= 0 || g.key > 255) g.key = 0x78;
        g.speed = static_cast<float>(joan::platform::ini::get_int("Debug", "FreecamSpeed", 1000, ini.c_str()));
        if (g.speed <= 0.0f) g.speed = 1000.0f;
    }
    const auto now = std::chrono::steady_clock::now();
    float dt = g.last.time_since_epoch().count() ? std::chrono::duration<float>(now - g.last).count() : 0.0f;
    g.last = now;
    if (dt > 0.1f) dt = 0.1f;  // a long frame (loading) does not throw the camera away

    const bool toggle = focused && down(g.key);
    g.toggled = toggle && !g.prev_toggle;
    if (toggle && !g.prev_toggle) {
        g.on = !g.on;
        g.start_pending = g.on;
        g.looking = false;
        hook::log_line("[Freecam] %s", g.on ? "on" : "off");
    }
    g.prev_toggle = toggle;
    if (!g.on || !focused || menu_open) {
        g.looking = false;
        return;
    }

    float speed = g.speed * dt;
    if (down(VK_SHIFT)) speed *= 5.0f;
    if (down(VK_CONTROL)) speed *= 0.2f;
    g.forward += ((down('W') ? 1.0f : 0.0f) - (down('S') ? 1.0f : 0.0f)) * speed;
    g.right += ((down('D') ? 1.0f : 0.0f) - (down('A') ? 1.0f : 0.0f)) * speed;
    g.up += ((down('E') ? 1.0f : 0.0f) - (down('Q') ? 1.0f : 0.0f)) * speed;

    // Turning: the arrow keys, or the mouse while its right button is held (the cursor stays put).
    const float key_turn = 1.5f * dt;  // radians per second
    g.turn_yaw += ((down(VK_RIGHT) ? 1.0f : 0.0f) - (down(VK_LEFT) ? 1.0f : 0.0f)) * key_turn;
    g.turn_pitch += ((down(VK_UP) ? 1.0f : 0.0f) - (down(VK_DOWN) ? 1.0f : 0.0f)) * key_turn;
#ifdef _WIN32
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {  // a mouse button: not in SDL's keyboard state
        POINT p{};
        GetCursorPos(&p);
#else
    float mx = 0.0f, my = 0.0f;
    if (SDL_GetGlobalMouseState(&mx, &my) & SDL_BUTTON_RMASK) {
        const POINT p{static_cast<LONG>(mx), static_cast<LONG>(my)};
#endif
        if (g.looking) {
            constexpr float kRadiansPerPixel = 0.003f;
            g.turn_yaw += static_cast<float>(p.x - g.anchor.x) * kRadiansPerPixel;
            g.turn_pitch -= static_cast<float>(p.y - g.anchor.y) * kRadiansPerPixel;
#ifdef _WIN32
            SetCursorPos(g.anchor.x, g.anchor.y);
#else
            SDL_WarpMouseGlobal(static_cast<float>(g.anchor.x), static_cast<float>(g.anchor.y));
#endif
        } else {
            g.anchor = p;
            g.looking = true;
        }
    } else {
        g.looking = false;
    }
}

bool pose(const Pose& start, Axes axes, Pose& out) {
    if (!g.on) return false;
    if (g.start_pending) {
        g.start_pending = false;
        g.pose = start;
        g.forward = g.right = g.up = g.turn_yaw = g.turn_pitch = 0.0f;
    }
    if (!g.signs_known) {
        // Which way the angles turn the view: right (+yaw) should move the forward axis towards the right
        // axis, up (+pitch) should raise it.
        g.signs_known = true;
        float f0[3], r0[3], f1[3], r1[3];
        axes(0.0f, 0.0f, f0, r0);
        axes(0.1f, 0.0f, f1, r1);
        g.yaw_sign = (f1[0] - f0[0]) * r0[0] + (f1[1] - f0[1]) * r0[1] + (f1[2] - f0[2]) * r0[2] >= 0.0f ? 1.0f : -1.0f;
        axes(0.0f, 0.1f, f1, r1);
        g.pitch_sign = f1[2] - f0[2] >= 0.0f ? 1.0f : -1.0f;
    }
    Pose& p = g.pose;
    if (g.turn_yaw != 0.0f || g.turn_pitch != 0.0f) {
        p.yaw += g.yaw_sign * g.turn_yaw;
        p.pitch += g.pitch_sign * g.turn_pitch;
        // Not over the top: about +-85 degrees from level.
        float f[3], r[3];
        axes(p.yaw, p.pitch, f, r);
        if (std::fabs(f[2]) > 0.996f) p.pitch -= g.pitch_sign * g.turn_pitch;
        g.turn_yaw = g.turn_pitch = 0.0f;
    }
    if (g.forward != 0.0f || g.right != 0.0f || g.up != 0.0f) {
        float f[3], r[3];
        axes(p.yaw, p.pitch, f, r);
        for (int i = 0; i < 3; ++i) p.position[i] += f[i] * g.forward + r[i] * g.right;
        p.position[2] += g.up;  // world z is up
        g.forward = g.right = g.up = 0.0f;
    }
    out = p;
    return true;
}

}  // namespace joan::freecam
