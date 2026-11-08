// The options menu's "joystick" control type when a game pad is plugged in / out while the game runs ([Platform]
// Input=sdl pads, ilib/mousecr.cpp Input::poll_joysticks): see interfac/optmenui.cpp.
#pragma once

namespace joan::optmenu {

/// Joystick slot 0 got (true) / lost (false) a pad. Adds / removes the joystick entry of the control-type selector
/// (built once by OptMenu::build_controls 0x007435e0, with the entry only if a joystick was there); losing the pad
/// while the joystick control type is active switches to the keyboard type, as the options menu would.
void joystick_presence_changed(bool present);

/// The input method follows the device last used ([Platform] PadAutoInput, platform/sdl_input.cpp): `pad` = a game
/// pad was used -> the joystick type, else the keyboard type used before. Applied as the options menu applies it.
void auto_input_type(bool pad);

}  // namespace joan::optmenu
