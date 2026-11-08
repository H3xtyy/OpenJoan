// SPDX-License-Identifier: GPL-3.0-or-later
// [Fixes] MouseOptions: see mouse_options.hpp.
#include "mouse_options.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "../platform/ini.hpp"
#include "../registry.hpp"
#include "../runtime/replay/replay.hpp"

namespace joan::optmenu::mouse {

namespace {
// The cursor's speed in % per slider position: 25% .. 400%, doubling every five positions.
constexpr int kCursorPercent[kCursorMax + 1] = {25,  29,  33,  38,  44,  50,  57,  66,  76,  87, 100,
                                                115, 132, 152, 174, 200, 230, 264, 303, 348, 400};
void* g_slider = nullptr;
void* g_label = nullptr;
std::int64_t g_rest[2] = {0, 0};  // fractions of a count, in hundredths
}  // namespace

bool on() { return joan::hook::settings().fix_mouse_options; }

int row_shift() { return on() ? 0x34 : 0; }

int cursor_position() { return std::clamp(joan::hook::settings().cursor_sensitivity, 0, kCursorMax); }

void set_cursor_position(int position) {
    position = std::clamp(position, 0, kCursorMax);
    joan::hook::mutable_settings().cursor_sensitivity = position;
    if (joan::replay::active()) return;
    joan::platform::ini::write_string("Platform", "CursorSensitivity", std::to_string(position).c_str(),
                                      joan::hook::ini_path());
}

std::int32_t cursor_delta(int axis, std::int32_t delta) {
    if (!on()) return delta;
    const int percent = kCursorPercent[cursor_position()];
    if (percent == 100) return delta;
    const std::int64_t total = g_rest[axis] + static_cast<std::int64_t>(delta) * percent;
    const std::int64_t whole = total / 100;
    g_rest[axis] = total - whole * 100;
    return static_cast<std::int32_t>(whole);
}

float low_look_option(float v) {
    const float base = 0.2f * std::pow(10.0f, std::max(v, -2.0f) + 1.0f);
    return -1.0f + (base - 0.2f) / 0.7f;
}

void*& cursor_slider() { return g_slider; }
void*& cursor_label() { return g_label; }

}  // namespace joan::optmenu::mouse
