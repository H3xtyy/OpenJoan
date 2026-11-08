#include "data_names.hpp"
#include "platform/memory.hpp"
#include "display.hpp"

#include "platform/threads.hpp"
#include "platform/win32_types.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "canvas_api.hpp"
#include "runtime/patch.hpp"
#include "runtime/profiler.hpp"
#include "registry.hpp"
#include "platform/window.hpp"

namespace joan::display {
namespace {

Size g_size;  // what the game runs at
Canvas g_canvas;
int g_canvas_depth = 0;
Layout g_outer_layout = Layout::screen;  // the outermost layout scope being drawn
std::int32_t g_layout_size[2] = {1024, 768};  // read instead of the screen size by code laid out for 1024x768
std::int32_t g_box_size[2] = {1024, 768};     // message-box centring: 1024x768 while such a screen is shown
int g_canvas_sprites = 0;     // sprites a 1024x768 screen queued during this frame
bool g_canvas_armed = false;
int g_canvas_idle = 0;
int g_scene_bottom = 0;       // lowest edge of the last frame's 3D viewports (0 = none)
int g_logged_viewports = 0;
int g_bar_top = 0;            // top of last frame's bottom cinematic bar (full-width sprite at the bottom), 0 = none
int g_bar_top_frame = 0;
int g_ui_depth = 0;           // inside the HUD draw function
int g_ui_off_depth = 0;       // inside a HUD part tied to the 3D world
int g_ui_center_depth = 0;    // inside a centred panel (scaled about the screen centre)
int g_ui_corner_depth = 0;    // inside a panel in the top-left corner (scaled about that corner)
int g_ui_cursor_depth = 0;    // drawing something that follows the game's mouse (dragged item)
int g_unit_depth = 0;         // inside a text-string draw
bool g_unit_placed = false;   // the current string's placement is chosen
float g_unit_ax = 0.0f, g_unit_ay = 0.0f;
float g_ui_scale = 1.0f;
bool g_opaque_frame = false;  // a full 1024x768 screen (not an overlay like the quit popup) drew this frame
bool g_opaque_shown = false;  // ... the last frame
int g_logged_bars = 0;
// Dialogue portrait + text block in the 1024x768 layout (y 689..755): its centre sits in the
// middle of the original cinematic bar.
constexpr float kDialogueCentre = 722.0f;        // frames without a canvas draw while armed

// Mouse (ilib Mouse at 0x00a516b0): cur_x / cur_y, clamped by the game to its boundary
// (x1, y1, x2, y2 at +0x636; set by Mouse::set_boundary 0x00593af0 / reset_boundary 0x00593f60).
constexpr img::Addr kMouseX = d::g_mouse_x, kMouseY = d::g_mouse_y, kMouseBounds = d::g_mouse + 0x636;
int g_saved_bounds[4] = {};
constexpr int kLayoutWidth = 1024, kLayoutHeight = 768;

struct Patch {
    const char* name;
    std::uintptr_t address;  // where the 32-bit value lives (data: img::addr(d::X), the current address)
    std::uint32_t expected;  // original value (checked before anything is written)
    std::uint32_t value;
    std::uintptr_t function = 0;  // code patch: the function holding it (0 = data)
};
// Code immediates as apply_patches set them (joan::canvas::resolution_imm), including the ones
// not written because their function has a C++ replacement.
struct CodeValue {
    std::uintptr_t address;
    std::uint32_t value;
};
CodeValue g_code_values[8];
int g_code_value_count = 0;

// Data only (joan_image; .rdata is still writable while host::start runs, the loader protects it afterwards). No
// code is patched any more: never make a data page executable (stage S7d: DEP is on).
bool write32(std::uintptr_t address, std::uint32_t value) {
    auto* p = reinterpret_cast<void*>(address);
    const platform::memory::Writable writable(p, 4);  // keeps the page's other access: never adds execute
    if (!writable.ok()) return false;
    std::memcpy(p, &value, 4);
    return true;
}

std::uint32_t float_bits(float f) {
    std::uint32_t v = 0;
    std::memcpy(&v, &f, 4);
    return v;
}

std::uint32_t read32(std::uintptr_t address) {
    std::uint32_t v = 0;
    std::memcpy(&v, reinterpret_cast<const void*>(address), 4);
    return v;
}

void make_dpi_aware() {
#ifdef _WIN32
    // Real pixels on scaled desktops (125% / 150%): otherwise Windows reports and stretches a smaller screen.
    using SetContextFn = BOOL(WINAPI*)(HANDLE);
    HMODULE user32 = GetModuleHandleA("user32.dll");
    if (const auto set = reinterpret_cast<SetContextFn>(
            reinterpret_cast<void*>(user32 ? GetProcAddress(user32, "SetProcessDpiAwarenessContext") : nullptr))) {
        if (set(reinterpret_cast<HANDLE>(static_cast<std::intptr_t>(-4)))) return;  // PER_MONITOR_AWARE_V2
    }
    SetProcessDPIAware();
#endif
}

// The primary display's desktop size in pixels (SDL; may come before the game window: platform::video_init).
Size desktop_size() {
    platform::ScreenMode sm;
    if (platform::desktop_display_mode(0, sm) && sm.width > 0 && sm.height > 0) return {sm.width, sm.height};
    return {kLayoutWidth, kLayoutHeight};
}

bool community_patch_installed() {
    // Any d3d8.dll next to joan.exe is a wrapper; the widescreen one patches the same bytes.
    const std::string path = hook::data_dir() + "d3d8.dll";
#ifdef _WIN32
    return GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES;
#else
    return false;  // a Windows DLL: not on other systems
#endif
}

}  // namespace

Size game_size() { return g_size; }
const Canvas& canvas() { return g_canvas; }
bool canvas_drawing() { return g_canvas.enabled && g_canvas_depth > 0 && g_outer_layout == Layout::screen; }
bool canvas_armed() { return g_canvas_armed; }
bool g_opaque_before = false;  // ... the frame before that
// The closing frame of a menu still shows it (its pieces queued outside its draw function), so the
// bars stay black for one more frame.
bool opaque_screen_shown() { return g_opaque_shown || g_opaque_before; }
bool opaque_screen_active() { return g_opaque_shown; }
bool g_loading_drawing = false;  // inside Game::draw_loading_screen
bool g_loading_frame = false;    // ... and it queued a sprite this frame
bool g_after_loading = false;    // the loading screen was shown; the level not yet (loading_done)
void loading_screen(bool drawing) { g_loading_drawing = drawing; }
bool loading_drawing() { return g_loading_drawing; }
bool loading_just_ended() { return g_after_loading && !g_opaque_shown; }
void loading_done() { g_after_loading = false; }
std::int32_t* layout_size() { return g_layout_size; }
std::int32_t* box_size() { return g_box_size; }

namespace {
void update_box_size() {
    const bool layout = g_canvas_armed || canvas_drawing();
    g_box_size[0] = layout ? kLayoutWidth : g_size.width;
    g_box_size[1] = layout ? kLayoutHeight : g_size.height;
}
}  // namespace

bool g_outer_opaque = false;

void layout_enter(Layout layout, bool opaque) {
    // Some of these draw functions are called every frame and draw nothing while their screen is
    // closed, so a screen counts as shown only once it queues a sprite (canvas_sprite).
    if (g_canvas_depth++ == 0) {
        g_outer_layout = layout;
        g_outer_opaque = opaque;
    }
    update_box_size();
}
void layout_leave() {
    if (g_canvas_depth > 0) --g_canvas_depth;
    update_box_size();
}
Layout swap_layout(Layout layout) {
    const Layout previous = g_outer_layout;
    if (g_canvas_depth > 0) g_outer_layout = layout;
    return previous;
}

void ui_enter(Layout kind) {
    if (kind == Layout::ui) ++g_ui_depth;
    else if (kind == Layout::ui_center) {
        ++g_ui_depth;
        ++g_ui_center_depth;
    } else if (kind == Layout::ui_corner) {
        ++g_ui_depth;
        ++g_ui_corner_depth;
    } else if (kind == Layout::ui_off) ++g_ui_off_depth;
    else if (g_unit_depth++ == 0) g_unit_placed = false;
}
int g_self_scaled_depth = 0;  // inside a draw that scales its own pieces (lens flares, archer icon)
void self_scaled_enter() { ++g_self_scaled_depth; }
void self_scaled_leave() {
    if (g_self_scaled_depth > 0) --g_self_scaled_depth;
}
void ui_cursor_enter() { ++g_ui_cursor_depth; }
void ui_cursor_leave() {
    if (g_ui_cursor_depth > 0) --g_ui_cursor_depth;
}
void ui_leave(Layout kind) {
    if (kind == Layout::ui) {
        if (g_ui_depth > 0) --g_ui_depth;
    } else if (kind == Layout::ui_center) {
        if (g_ui_depth > 0) --g_ui_depth;
        if (g_ui_center_depth > 0) --g_ui_center_depth;
    } else if (kind == Layout::ui_corner) {
        if (g_ui_depth > 0) --g_ui_depth;
        if (g_ui_corner_depth > 0) --g_ui_corner_depth;
    } else if (kind == Layout::ui_off) {
        if (g_ui_off_depth > 0) --g_ui_off_depth;
    } else if (g_unit_depth > 0) {
        --g_unit_depth;
    }
}
float ui_scale() { return g_ui_scale; }

namespace {
// HUD pieces as shown in the last frame (for remapping clicks) and the real cursor position.
struct UiRect {
    int x1, y1, x2, y2;
    float ax, ay;
};
std::vector<UiRect> g_ui_rects_frame, g_ui_rects;
bool g_ui_mouse_valid = false;
int g_real_mx = 0, g_real_my = 0, g_written_mx = 0, g_written_my = 0;

bool ui_active() { return g_ui_scale != 1.0f && !g_canvas_armed && g_ui_depth > 0 && g_ui_off_depth == 0; }

float ui_anchor(float v, float size) { return v < size / 3.0f ? 0.0f : v > size * 2.0f / 3.0f ? size : size * 0.5f; }

// UiScale for a HUD piece: scaled about the screen edge / corner / centre nearest to it (by
// thirds of the screen); a text string keeps the anchor of its first glyph.
void place_ui_sprite(std::int32_t* args) {
    const float s = g_ui_scale, w = static_cast<float>(g_size.width), h = static_cast<float>(g_size.height);
    const int x1 = static_cast<std::int16_t>(args[0]), y1 = static_cast<std::int16_t>(args[1]);
    const int x2 = static_cast<std::int16_t>(args[2]), y2 = static_cast<std::int16_t>(args[3]);
    if (x1 <= 0 && x2 >= g_size.width - 1) return;  // full-width pieces (cinematic bars, fades)
    float ax = 0.0f, ay = 0.0f;
    const bool follows_cursor = g_ui_cursor_depth > 0 && g_ui_mouse_valid;
    if (follows_cursor) {
        // A piece placed at the game's mouse (G) plus an offset: keep the offset (scaled) from the
        // real cursor (R). Scaling about a with a + (G - a) * s = R does exactly that.
        ax = (static_cast<float>(g_real_mx) - static_cast<float>(g_written_mx) * s) / (1.0f - s);
        ay = (static_cast<float>(g_real_my) - static_cast<float>(g_written_my) * s) / (1.0f - s);
    } else if (g_ui_center_depth > 0) {
        ax = w * 0.5f;
        ay = h * 0.5f;
    } else if (g_ui_corner_depth > 0) {
        ax = 0.0f;  // the whole panel about one point: a piece crossing a third of the screen must not take another anchor
        ay = 0.0f;
    } else if (g_unit_depth > 0 && g_unit_placed) {
        ax = g_unit_ax;
        ay = g_unit_ay;
    } else {
        const float cx = (static_cast<float>(x1) + static_cast<float>(x2)) * 0.5f;
        const float cy = (static_cast<float>(y1) + static_cast<float>(y2)) * 0.5f;
        ax = ui_anchor(cx, w);
        ay = ui_anchor(cy, h);
        if (g_unit_depth > 0) {
            g_unit_ax = ax;
            g_unit_ay = ay;
            g_unit_placed = true;
        }
    }
    auto lo = [&](int v, float a) { return static_cast<std::int32_t>(std::lround(a + (static_cast<float>(v) - a) * s)); };
    auto hi = [&](int v, float a) { return static_cast<std::int32_t>(std::lround(a + (static_cast<float>(v) + 1.0f - a) * s)) - 1; };
    args[0] = lo(x1, ax);
    args[1] = lo(y1, ay);
    args[2] = hi(x2, ax);
    args[3] = hi(y2, ay);
    if (!follows_cursor && g_ui_rects_frame.size() < 4096)
        g_ui_rects_frame.push_back({static_cast<std::int16_t>(args[0]), static_cast<std::int16_t>(args[1]),
                                    static_cast<std::int16_t>(args[2]), static_cast<std::int16_t>(args[3]), ax, ay});
}

// Diagnostics: where 2D pieces drawn during missions outside the scaled HUD come from (the
// game keeps frame pointers: [ebp] = caller's ebp, [ebp+4] = return address).
int g_logged_chains = 0;
std::vector<std::uint32_t> g_chain_keys;
void log_unscaled(const std::int32_t* args, std::uintptr_t frame) {
    if (kReleaseBuild || g_logged_chains >= 60 || frame == 0) return;  // release: no log to write it to
    std::uintptr_t low = 0, high = 0;
    if (!platform::stack_range(low, high)) return;
    std::uintptr_t ret[8] = {};
    int n = 0;
    // [f] = caller's frame pointer, [f + one word] = return address (4 / 8 bytes: x86 / x64 frame chains)
    constexpr std::uintptr_t kWord = sizeof(std::uintptr_t);
    for (std::uintptr_t f = frame; n < 8 && f >= low && f + 2 * kWord <= high && (f & (kWord - 1)) == 0;) {
        ret[n++] = *reinterpret_cast<const std::uintptr_t*>(f + kWord);
        const std::uintptr_t next = *reinterpret_cast<const std::uintptr_t*>(f);
        if (next <= f) break;
        f = next;
    }
    std::uint32_t key = 2166136261u;
    for (int i = 0; i < n && i < 5; ++i) key = (key ^ static_cast<std::uint32_t>(ret[i])) * 16777619u;
    if (std::find(g_chain_keys.begin(), g_chain_keys.end(), key) != g_chain_keys.end()) return;
    g_chain_keys.push_back(key);
    ++g_logged_chains;
    std::string chain;  // the callers by name (openjoan.pdb): the load address changes from run to run
    for (int i = 0; i < n; ++i) chain += (i ? " < " : "") + prof::symbol_at(ret[i]);
    hook::log_line("[Display] unscaled 2D (%d,%d)-(%d,%d) from %s", static_cast<std::int16_t>(args[0]),
                   static_cast<std::int16_t>(args[1]), static_cast<std::int16_t>(args[2]), static_cast<std::int16_t>(args[3]),
                   chain.c_str());
}
}  // namespace

void canvas_sprite_from(std::int32_t* args, std::uintptr_t frame) {
    canvas_sprite(args);
    if (g_canvas.enabled && g_ui_scale != 1.0f && !g_canvas_armed && g_canvas_depth == 0 && g_ui_depth == 0 &&
        !g_opaque_shown && g_self_scaled_depth == 0) {
        const int x1 = static_cast<std::int16_t>(args[0]), x2 = static_cast<std::int16_t>(args[2]);
        if (!(x1 <= 0 && x2 >= g_size.width - 1)) log_unscaled(args, frame);
    }
}

void camera_viewport(std::int32_t* args, std::uintptr_t caller) {
    // The shop / blacksmith cameras (item and portrait views) are set up at load time by
    // 0x006c2120 around the screen centre: scale their viewports with UiScale like the panel.
    if (g_ui_scale == 1.0f || caller < 0x006c2120 || caller >= 0x006c27c0) return;
    const float s = g_ui_scale, cx = static_cast<float>(g_size.width) * 0.5f, cy = static_cast<float>(g_size.height) * 0.5f;
    auto lo = [&](std::int32_t v, float c) { return static_cast<std::int32_t>(std::lround(c + (static_cast<float>(v) - c) * s)); };
    auto hi = [&](std::int32_t v, float c) { return static_cast<std::int32_t>(std::lround(c + (static_cast<float>(v) + 1.0f - c) * s)) - 1; };
    args[2] = lo(args[2], cx);
    args[3] = lo(args[3], cy);
    args[4] = hi(args[4], cx);
    args[5] = hi(args[5], cy);
}

bool ui_cursor(int& x, int& y) {
    if (!g_ui_mouse_valid) return false;
    x = g_real_mx;
    y = g_real_my;
    return true;
}

void canvas_sprite(std::int32_t* args) {
    // Sprite queue (0x00632a70): args[0..3] = x1, y1, x2, y2 (inclusive screen pixels, shorts).
    if (!g_canvas.enabled) return;
    Layout layout = g_outer_layout;
    if (g_canvas_depth == 0 && g_opaque_shown) {
        // Pieces of a full 1024x768 screen queued outside its draw function (e.g. the frame around
        // the mission-brief commander portrait).
        layout = Layout::screen;
    } else if (g_canvas_depth == 0) {
        // A cinematic bar: a full-width sprite reaching the bottom edge, starting in the lower half.
        const int x1 = static_cast<std::int16_t>(args[0]), y1 = static_cast<std::int16_t>(args[1]);
        const int x2 = static_cast<std::int16_t>(args[2]), y2 = static_cast<std::int16_t>(args[3]);
        if (x1 <= 0 && x2 >= g_size.width - 1 && y2 >= g_size.height - 1 && y1 > g_size.height / 2 && y1 < g_size.height - 8)
            g_bar_top_frame = std::max(g_bar_top_frame, y1);
        if (ui_active()) place_ui_sprite(args);
        return;
    }
    float s = 1.0f, ox = 0.0f, oy = 0.0f;
    if (layout == Layout::screen) {
        // Full-screen fades (0, 0, W-1, H-1) keep covering the whole screen - and do not count as the
        // 1024x768 screen being shown: the loading screen fades the level's first frames in with one,
        // and the world behind it is drawn full screen (not clipped to the 1024x768 screen between bars).
        if (static_cast<std::int16_t>(args[0]) == 0 && static_cast<std::int16_t>(args[1]) == 0 &&
            static_cast<std::int16_t>(args[2]) == g_size.width - 1 && static_cast<std::int16_t>(args[3]) == g_size.height - 1)
            return;
        if (g_canvas_depth > 0) {
            ++g_canvas_sprites;
            if (g_outer_opaque) g_opaque_frame = true;
            if (g_loading_drawing) g_loading_frame = true;
        }
        s = g_canvas.scale;
        ox = g_canvas.x;
        oy = g_canvas.y;
    } else {
        // HUD pieces laid out for 1024x768 at UiScale: messages bottom-centred, dialogue bottom-left.
        s = g_ui_scale;
        if (layout == Layout::hud_top_right)
            ox = std::round(static_cast<float>(g_size.width) - kLayoutWidth * s);
        else if (layout == Layout::hud || layout == Layout::hud_top)
            ox = std::floor((static_cast<float>(g_size.width) - kLayoutWidth * s) * 0.5f);
        oy = layout == Layout::hud_top || layout == Layout::hud_top_right ? 0.0f : std::round(static_cast<float>(g_size.height) - kLayoutHeight * s);
        // Dialogue during cutscenes: centred in the cinematic bar under the 3D view.
        int bar = g_bar_top;
        if (bar == 0 && g_scene_bottom > g_size.height / 2 && g_scene_bottom < g_size.height - 8) bar = g_scene_bottom;
        if (layout == Layout::hud_left && bar > 0)
            oy = std::round((static_cast<float>(bar) + static_cast<float>(g_size.height)) * 0.5f - kDialogueCentre * s);
    }
    auto lo = [&](std::int32_t v, float offset) { return static_cast<std::int32_t>(std::lround(static_cast<std::int16_t>(v) * s + offset)); };
    auto hi = [&](std::int32_t v, float offset) {
        return static_cast<std::int32_t>(std::lround((static_cast<std::int16_t>(v) + 1) * s + offset)) - 1;
    };
    args[0] = lo(args[0], ox);
    args[1] = lo(args[1], oy);
    args[2] = hi(args[2], ox);
    args[3] = hi(args[3], oy);
}

void canvas_model(std::int32_t* args) {
    if (g_canvas_depth == 0 && ui_active()) {
        // A HUD model (minimap player arrow, camp icons): its point placed like the HUD's sprites.
        const float s = g_ui_scale, w = static_cast<float>(g_size.width), h = static_cast<float>(g_size.height);
        const float x = static_cast<std::int16_t>(args[0]), y = static_cast<std::int16_t>(args[1]);
        const float ax = g_ui_center_depth > 0 ? w * 0.5f : g_ui_corner_depth > 0 ? 0.0f : ui_anchor(x, w);
        const float ay = g_ui_center_depth > 0 ? h * 0.5f : g_ui_corner_depth > 0 ? 0.0f : ui_anchor(y, h);
        args[0] = static_cast<std::int32_t>(std::lround(ax + (x - ax) * s));
        args[1] = static_cast<std::int32_t>(std::lround(ay + (y - ay) * s));
        float scale = 0.0f;
        std::memcpy(&scale, &args[7], 4);
        scale *= s;
        std::memcpy(&args[7], &scale, 4);
        return;
    }
    if (!canvas_drawing()) return;
    const float s = g_canvas.scale;
    args[0] = static_cast<std::int32_t>(std::lround(static_cast<std::int16_t>(args[0]) * s + g_canvas.x));
    args[1] = static_cast<std::int32_t>(std::lround(static_cast<std::int16_t>(args[1]) * s + g_canvas.y));
    float scale = 0.0f;
    std::memcpy(&scale, &args[7], 4);
    scale *= s;
    std::memcpy(&args[7], &scale, 4);
}

void set_scene_bottom(int bottom) {
    if (bottom != g_scene_bottom && bottom != 0 && g_logged_viewports < 30) {
        ++g_logged_viewports;
        hook::log_line("[Display] 3D view ends at y=%d of %d%s", bottom, g_size.height, g_canvas_armed ? " (1024x768 screen shown)" : "");
    }
    g_scene_bottom = bottom;
}

void canvas_end_frame() {
    if (!g_canvas.enabled) return;
    if (g_bar_top_frame != g_bar_top && g_bar_top_frame != 0 && g_logged_bars < 20) {
        ++g_logged_bars;
        hook::log_line("[Display] cinematic bar sprite: bottom bar starts at y=%d of %d", g_bar_top_frame, g_size.height);
    }
    g_bar_top = std::exchange(g_bar_top_frame, 0);
    g_opaque_before = g_opaque_shown;
    g_opaque_shown = std::exchange(g_opaque_frame, false);
    if (std::exchange(g_loading_frame, false)) g_after_loading = true;
    int& mx = hook::global<int>(kMouseX);
    int& my = hook::global<int>(kMouseY);
    // UiScale: the game's mouse position was remapped onto the unscaled HUD last frame; take the
    // real position back (plus this frame's movement) first.
    if (g_ui_mouse_valid) {
        g_real_mx = std::clamp(g_real_mx + mx - g_written_mx, 0, g_size.width - 1);
        g_real_my = std::clamp(g_real_my + my - g_written_my, 0, g_size.height - 1);
        mx = g_real_mx;
        my = g_real_my;
    }
    g_ui_rects.swap(g_ui_rects_frame);
    g_ui_rects_frame.clear();
    auto* bounds = &img::global<int>(kMouseBounds);
    const bool used = std::exchange(g_canvas_sprites, 0) > 0;
    if (used) {
        g_canvas_idle = 0;
        if (!g_canvas_armed) {
            // Keep the cursor where it is on screen: screen -> layout coordinates.
            mx = static_cast<int>((static_cast<float>(mx) - g_canvas.x) / g_canvas.scale);
            my = static_cast<int>((static_cast<float>(my) - g_canvas.y) / g_canvas.scale);
            std::memcpy(g_saved_bounds, bounds, sizeof g_saved_bounds);
            g_canvas_armed = true;
            hook::log_line("[Display] 1024x768 screen shown - mouse in layout coordinates (%d,%d)", mx, my);
        }
    } else if (g_canvas_armed && ++g_canvas_idle >= 3) {
        std::memcpy(bounds, g_saved_bounds, sizeof g_saved_bounds);
        mx = std::min(static_cast<int>(static_cast<float>(mx) * g_canvas.scale + g_canvas.x), g_size.width - 1);
        my = std::min(static_cast<int>(static_cast<float>(my) * g_canvas.scale + g_canvas.y), g_size.height - 1);
        g_canvas_armed = false;
        hook::log_line("[Display] back to the full screen - mouse at (%d,%d)", mx, my);
    }
    update_box_size();
    if (!g_canvas_armed && g_ui_scale != 1.0f) {
        // Over a scaled HUD piece the game gets the matching point of the unscaled HUD, so its
        // clicks (character portraits, inventory) land; the cursor is drawn at the real position.
        if (!g_ui_mouse_valid) {
            g_real_mx = mx;
            g_real_my = my;
            g_ui_mouse_valid = true;
        }
        int gx = g_real_mx, gy = g_real_my;
        for (auto r = g_ui_rects.rbegin(); r != g_ui_rects.rend(); ++r) {
            if (g_real_mx < r->x1 || g_real_mx > r->x2 || g_real_my < r->y1 || g_real_my > r->y2) continue;
            gx = static_cast<int>(std::lround(r->ax + (static_cast<float>(g_real_mx) - r->ax) / g_ui_scale));
            gy = static_cast<int>(std::lround(r->ay + (static_cast<float>(g_real_my) - r->ay) / g_ui_scale));
            gx = std::clamp(gx, 0, g_size.width - 1);
            gy = std::clamp(gy, 0, g_size.height - 1);
            break;
        }
        mx = g_written_mx = gx;
        my = g_written_my = gy;
    } else {
        g_ui_mouse_valid = false;
    }
    if (g_canvas_armed) {
        // The game clamps the mouse to its boundary: keep it inside the 1024x768 layout.
        const int layout[4] = {0, 0, kLayoutWidth - 1, kLayoutHeight - 1};
        std::memcpy(bounds, layout, sizeof layout);
        mx = std::clamp(mx, 0, kLayoutWidth - 1);
        my = std::clamp(my, 0, kLayoutHeight - 1);
    }
}

void apply_patches() {
    const auto& s = hook::settings();
    if (s.dpi_aware) make_dpi_aware();
    g_ui_scale = 1.0f;

    const std::string& r = s.resolution;
    if (r == "original" || r.empty()) {
        hook::log_line("[Display] Resolution=original: the game runs at 1024x768");
        return;
    }
    Size want;
    if (r == "desktop") {
        want = desktop_size();
    } else if (std::sscanf(r.c_str(), "%dx%d", &want.width, &want.height) != 2) {
        hook::log_line("[Display] Resolution=%s not understood (use desktop, original or WIDTHxHEIGHT) - 1024x768", r.c_str());
        return;
    }
    if (want.width < 640 || want.height < 480 || want.width > 16384 || want.height > 16384) {
        hook::log_line("[Display] Resolution %dx%d out of range - 1024x768", want.width, want.height);
        return;
    }
    if (community_patch_installed()) {
        hook::log_line("[Display] a d3d8.dll (community widescreen patch) is installed next to joan.exe - "
                       "resolution left to it; rename it to use [Display] Resolution");
        return;
    }

    const auto w = static_cast<std::uint32_t>(want.width), h = static_cast<std::uint32_t>(want.height);
    const Patch patches[] = {
        // Direct3D start-up: the display mode searched in the adapter's mode list (mov [ebp-0xc], 400h / [ebp-0x10], 300h).
        {"requested mode width", 0x005f6c79 + 3, 1024, w, 0x005f6c00},
        {"requested mode height", 0x005f6c80 + 3, 768, h, 0x005f6c00},
        // Screen description table copied to 0x00a48858 when the display starts.
        {"screen width", img::addr(d::g_a016a4), 1024, w},
        {"screen height", img::addr(d::g_a016a8), 768, h},
        {"screen rect right", img::addr(d::g_a016b8), 1023, w - 1},
        {"screen rect bottom", img::addr(d::g_a016bc), 767, h - 1},
        {"second rect right", img::addr(d::g_a016d0), 1023, w - 1},
        {"second rect bottom", img::addr(d::g_a016d4), 767, h - 1},
        // Combo counter (0x00702740): right-aligned to a hard-coded 1024 (mov reg, 400h).
        {"combo x 1", 0x007028c1 + 1, 1024, w, 0x00702740},
        {"combo x 2", 0x00702a5b + 1, 1024, w, 0x00702740},
        {"combo x 3", 0x00702bea + 1, 1024, w, 0x00702740},
        {"combo x 4", 0x00703001 + 1, 1024, w, 0x00702740},
        {"combo x 5", 0x00703245 + 1, 1024, w, 0x00702740},
        // ... and its digits, placed leftwards from x = 864.0f (float constant, read only at 0x00702d72).
        {"combo digits x", img::addr(d::flt_864), float_bits(864.0f), float_bits(864.0f + static_cast<float>(want.width - 1024))},
    };
    // joan.exe's code no longer exists: the code immediates (rows with a function) are not checked or written; their
    // functions are replaced and read the value through resolution_imm. The data rows are checked, then written.
    for (const Patch& p : patches) {
        if (p.function != 0) continue;
        if (read32(p.address) != p.expected) {
            hook::log_line("[Display] '%s' at 0x%08llx is %u, expected %u - resolution patches not applied", p.name,
                           static_cast<unsigned long long>(p.address), read32(p.address), p.expected);
            return;
        }
    }
    // The checks above passed: from here on the resolution counts as applied.
    g_code_value_count = 0;
    for (const Patch& p : patches) {
        if (p.function != 0) {
            if (g_code_value_count < static_cast<int>(sizeof g_code_values / sizeof g_code_values[0]))
                g_code_values[g_code_value_count++] = {p.address, p.value};
            hook::log_line("[Display] '%s' at 0x%08llx: function 0x%08x %s - value %u read by the replacement", p.name,
                           static_cast<unsigned long long>(p.address), static_cast<unsigned>(p.function),
                           canvas::original_bypassed(p.function) ? "replaced" : "NOT replaced (traps)", p.value);
            continue;
        }
        if (!write32(p.address, p.value)) {
            // Stopped here as before: the patches already written stay; this one keeps its value.
            hook::log_line("[Display] cannot write '%s' at 0x%08llx", p.name, static_cast<unsigned long long>(p.address));
            return;
        }
    }
    g_size = want;
    update_box_size();
    hook::log_line("[Display] the game runs at %dx%d (Resolution=%s)", want.width, want.height, r.c_str());
    if (want.width > kLayoutWidth || want.height > kLayoutHeight) {
        const float scale = std::min(static_cast<float>(want.width) / kLayoutWidth,
                                     static_cast<float>(want.height) / kLayoutHeight);
        g_canvas.enabled = true;
        g_canvas.scale = scale;
        g_canvas.x = std::floor((static_cast<float>(want.width) - kLayoutWidth * scale) * 0.5f);
        g_canvas.y = std::floor((static_cast<float>(want.height) - kLayoutHeight * scale) * 0.5f);
        hook::log_line("[Display] 1024x768 screens are drawn scaled by %.4f at (%.0f,%.0f)", scale, g_canvas.x, g_canvas.y);
        set_ui_scale(s.ui_scale);
    }
}

namespace {
void (*g_ui_scale_listener)() = nullptr;
}
void set_ui_scale_listener(void (*listener)()) { g_ui_scale_listener = listener; }

void set_ui_scale(float setting) {
    if (!g_canvas.enabled) return;  // 1024x768 or smaller: the HUD is drawn as the original
    // UiScale: auto = the largest half step not above the screens' scale (1080p 1, 1440p 1.5, 4K 2.5).
    float ui = setting;
    if (ui <= 0.0f) ui = std::max(1.0f, std::floor(g_canvas.scale * 2.0f) / 2.0f);
    g_ui_scale = std::clamp(ui, 1.0f, 4.0f);
    hook::log_line("[Display] HUD drawn at UiScale %.2f%s", g_ui_scale, setting <= 0.0f ? " (auto)" : "");
    if (g_ui_scale_listener != nullptr) g_ui_scale_listener();
}

}  // namespace joan::display

namespace joan::canvas {
std::uint32_t resolution_imm(std::uintptr_t operand) {
    for (int i = 0; i < display::g_code_value_count; ++i)
        if (display::g_code_values[i].address == operand) return display::g_code_values[i].value;
    // Not applied: the original immediate (joan.exe's code bytes no longer exist).
    return operand == kModeHeightImm ? 768u : 1024u;
}
float combo_digits_x() { return hook::global<float>(d::flt_864); }
}  // namespace joan::canvas
