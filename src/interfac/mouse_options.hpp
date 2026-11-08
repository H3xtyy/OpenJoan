// SPDX-License-Identifier: GPL-3.0-or-later
// [Fixes] MouseOptions: the INPUT OPTIONS page's camera and cursor sensitivity (interfac/optmenui.cpp).
//
// In the original the "Mouse Sensitivity" slider only turns the camera (Player::look_sensitivity 0x004322d0, -1..1 in
// steps of 0.1); the cursor of the menus and of the RTS mode follows the raw mouse counts 1:1 (Input::poll,
// ilib/mousecr.cpp). With the switch on, the camera slider has ten more steps below the original's minimum and a new
// slider sets the cursor's speed. A recording stores the switch and the cursor setting (stream.hpp
// kCursorSensitivityShift): recordings made without them replay with the switch off.
#pragma once

#include <cstdint>

namespace joan::optmenu::mouse {

inline constexpr int kCursorDefault = 10;  ///< cursor slider position of the original's speed (100%)
inline constexpr int kCursorMax = 20;      ///< last cursor slider position (positions are 0..kCursorMax)

/// [Fixes] MouseOptions is on.
bool on();
/// Vertical shift of the rows below the new cursor row, in menu pixels (0 with the switch off).
int row_shift();
/// The cursor slider's position, 0..kCursorMax.
int cursor_position();
/// Takes a new cursor slider position and keeps it in openjoan.ini (not while recording or replaying).
void set_cursor_position(int position);
/// The cursor's movement for `delta` mouse counts on `axis` (0 = x, 1 = y). The fractions left over carry to the next
/// call. Returns `delta` unchanged with the switch off or at 100%.
std::int32_t cursor_delta(int axis, std::int32_t delta);
/// The value to store as Player::look_sensitivity for a slider value below -1 (`v` in -2..-1): the original's formula
/// `(option + 1) * 0.7 + 0.2` then gives 0.2 * 10^(v + 1), down to a tenth of the original's minimum at v = -2.
float low_look_option(float v);
/// The cursor row's slider widget, null while the page is not built.
void*& cursor_slider();
/// The cursor row's label widget, null while the page is not built.
void*& cursor_label();

}  // namespace joan::optmenu::mouse
